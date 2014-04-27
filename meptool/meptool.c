/* MEPTool - MojoShader Effect Parsing Tool
 *
 * Copyright (c) 2013-2014 Ethan Lee.
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software in a
 * product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * Ethan "flibitijibibo" Lee <flibitijibibo@flibitijibibo.com>
 *
 */

#include <SDL2/SDL.h>
#include "mojoshader/mojoshader.h"

void *MojoShaderMalloc(int bytes, void *data)
{
	return malloc(bytes);
}

void MojoShaderFree(void *ptr, void *data)
{
	free(ptr);
}

static void *MojoShaderGLLookup(const char *fnname, void *data)
{
	return SDL_GL_GetProcAddress(fnname);
}

/* BEGIN GANKED CODE FROM utils/testparse.c! */

#include "testparsemini.h"

static int do_parse(const char *fname, const unsigned char *buf,
                    const int len, const char *prof)
{
    /* flibit modified this function, that's why it's not in testparsemini! */
    int retval = 0;
    const MOJOSHADER_effect *effect;

    if ( (buf[0] != 0x01) && (buf[1] != 0x09) &&
         (buf[2] != 0xFF) && (buf[3] != 0xFE) )
    {
        printf("%s is not an effect!\n", fname);
        return retval;
    } // if
    effect = MOJOSHADER_parseEffect(prof, buf, len, NULL, 0,
                                    NULL, 0, MojoShaderMalloc, MojoShaderFree, 0);
    retval = (effect->error_count == 0);
    printf("EFFECT: %s\n", fname);
    print_effect(fname, effect, 1);
    MOJOSHADER_freeEffect(effect);

    return retval;
} // do_parse

/* END GANKED CODE FROM utils/testparse.c! */

int main(int argc, char **argv)
{
	SDL_Window *window;
	SDL_GLContext context;
	const char *profile;
	MOJOSHADER_glContext *shaderContext;
	int i;
	long size;
	unsigned int shaderOffset;
	unsigned char *shader;

	if (argc < 2)
	{
		printf("Usage: %s effectFile.fxb\n", argv[0]);
		return 0;
	}

	/* Initialize SDL2, create OpenGL window/context */
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow(
		"MojoShader Effect Parsing Tool",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		320,
		240,
		SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN
	);
	context = SDL_GL_CreateContext(window);

	/* Initialize MojoShader with the best available profile */
	profile = MOJOSHADER_glBestProfile(
		MojoShaderGLLookup,
		NULL,
		MojoShaderMalloc,
		MojoShaderFree,
		NULL
	);
	printf("Running with MojoShader profile %s\n", profile);
	shaderContext = MOJOSHADER_glCreateContext(
		profile,
		MojoShaderGLLookup,
		NULL,
		MojoShaderMalloc,
		MojoShaderFree,
		NULL
	);
	MOJOSHADER_glMakeContextCurrent(shaderContext);

	/* Open up the file. This should have come from UnXNB! */
	for (i = 1; i < argc; i += 1)
	{
		FILE *fileIn = fopen(argv[i], "rb");
		if (fileIn == NULL)
		{
			printf("%s not opened. Skipping.\n", argv[i]);
		}
		else
		{
			fread(&size, 4, 1, fileIn);
			fseek(fileIn, 4, SEEK_CUR); /* Skip "first" uint. */
			fread(&shaderOffset, 4, 1, fileIn);
			fseek(fileIn, shaderOffset + 4, SEEK_SET);
			shader = (unsigned char*) malloc(size - shaderOffset);
			fread(shader, 1, size - shaderOffset, fileIn);
			fclose(fileIn);
			if (!do_parse(argv[i], shader, size, profile))
			{
				printf("Effect parsing error!\n");
			}
			free(shader);
		}
	}

	/* Clean up. We out. */
	MOJOSHADER_glMakeContextCurrent(NULL);
	MOJOSHADER_glDestroyContext(shaderContext);
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
