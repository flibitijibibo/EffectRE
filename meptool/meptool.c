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

static void print_effect(const char *fname, const MOJOSHADER_effect *effect,
                         const unsigned int indent)
{
    INDENT(); printf("PROFILE: %s\n", effect->profile);
    printf("\n");
    if (effect->error_count > 0)
    {
        int i;
        for (i = 0; i < effect->error_count; i++)
        {
            const MOJOSHADER_error *err = &effect->errors[i];
            INDENT();
            printf("%s:%d: ERROR: %s\n",
                    err->filename ? err->filename : fname,
                    err->error_position, err->error);
        } // for
    } // if
    else
    {
        int i, j, k;
        const MOJOSHADER_effectTechnique *technique = effect->techniques;
        const MOJOSHADER_effectTexture *texture = effect->textures;
        const MOJOSHADER_effectShader *shader = effect->shaders;
        const MOJOSHADER_effectParam *param = effect->params;

        for (i = 0; i < effect->param_count; i++, param++)
        {
            INDENT();
            printf("PARAM #%d '%s' -> '%s'\n", i, param->name, param->semantic);
            INDENT();
            printf("    CLASS: %d\n", param->param_class);
            INDENT();
            printf("    TYPE: %d\n", param->param_type);
            INDENT();
            if (param->param_type == MOJOSHADER_SYMTYPE_SAMPLER
             || param->param_type == MOJOSHADER_SYMTYPE_SAMPLER1D
             || param->param_type == MOJOSHADER_SYMTYPE_SAMPLER2D
             || param->param_type == MOJOSHADER_SYMTYPE_SAMPLER3D
             || param->param_type == MOJOSHADER_SYMTYPE_SAMPLERCUBE)
            {
                printf("    SAMPLER VALUES:\n");
                for (j = 0; j < param->value_count; j++)
                {
                    MOJOSHADER_effectSamplerState *states = (MOJOSHADER_effectSamplerState *) param->values;
                    if (states[j].type == MOJOSHADER_SAMP_TEXTURE)
                    {
                        printf("            TEXTURE #%d\n", states[j].valueI);
                    }
                    else
                    {
                        printf("            TYPE: %d -> ", states[j].type);
                        if (states[j].type == MOJOSHADER_SAMP_MIPMAPLODBIAS)
                        {
                            /* float types */
                            printf("FLOAT: %.2f\n", states[j].valueF);
                        }
                        else
                        {
                            /* int/enum types */
                            printf("INT: %d\n", states[j].valueI);
                        }
                    }
                }
            }
            else if (param->param_type == MOJOSHADER_SYMTYPE_BOOL)
            {
                printf("    BOOL VALUES:");
                int *bArray = (int *) param->values;
                for (j = 0; j < param->value_count; j++)
                {
                    printf(" %d", bArray[j]);
                }
                printf("\n");
            }
            else if (param->param_type == MOJOSHADER_SYMTYPE_INT)
            {
                printf("    INT VALUES:");
                int *iArray = (int *) param->values;
                for (j = 0; j < param->value_count; j++)
                {
                    printf(" %d", iArray[j]);
                }
                printf("\n");
            }
            else if (param->param_type == MOJOSHADER_SYMTYPE_FLOAT)
            {
                printf("    FLOAT VALUES:");
                float *fArray = (float *) param->values;
                for (j = 0; j < param->value_count; j++)
                {
                    printf(" %.2f", fArray[j]);
                }
                printf("\n");
            }
            else if (param->param_type == MOJOSHADER_SYMTYPE_TEXTURE)
            {
                printf("    TODO: TEXTURE VALUES -flibit\n");
            }
            else
            {
                printf("    TODO: PARSE THIS TYPE -flibit\n");
            }

            if (param->annotation_count > 0)
            {
                INDENT();
                printf("    ANNOTATIONS:\n");
            } // if
            for (j = 0; j < param->annotation_count; j++)
            {
                INDENT();
                printf("        %s\n", param->annotations[j].name);
            } // for
        } // for

        printf("\n");

        for (i = 0; i < effect->technique_count; i++, technique++)
        {
            const MOJOSHADER_effectPass *pass = technique->passes;
            INDENT(); printf("TECHNIQUE #%d ('%s'):\n", i, technique->name);
            for (j = 0; j < technique->pass_count; j++, pass++)
            {
                const MOJOSHADER_effectState *state = pass->states;
                INDENT(); printf("    PASS #%d ('%s'):\n", j, pass->name);
                for (k = 0; k < pass->state_count; k++, state++)
                {
                    INDENT(); printf("        STATE %d: %d\n", state->type, state->value);
                } // for
            } // for
            printf("\n");
        } // for

        for (i = 0; i < effect->texture_count; i++, texture++)
        {
            INDENT();
            printf("TEXTURE #%d ('%s'): %u\n", i,
                    texture->name, texture->param);
        } // for

        printf("\n");

        for (i = 0; i < effect->shader_count; i++, shader++)
        {
            INDENT();
            printf("SHADER #%d: technique %u, pass %u\n", i,
                    shader->technique, shader->pass);
            print_shader(fname, shader->shader, indent + 1);
        } // for
    } // else
} // print_effect

static int do_parse(const char *fname, const unsigned char *buf,
                    const int len, const char *prof)
{
    /* flibit modified this function, that's why it's not in testparsemini! */
    int retval = 0;
    const MOJOSHADER_effect *effect;

    if ( (buf[0] != 0x01) || (buf[1] != 0x09) ||
         (buf[2] != 0xFF) || (buf[3] != 0xFE) )
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
	long size = 0;
	unsigned int shaderOffset = 0;
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
			continue;
		}
		fread(&size, 4, 1, fileIn);
		if (size != 0xFEFF0901) /* This is a raw effect! */
		{
			/* TODO: WTF is in here...? -flibit */
			fseek(fileIn, 4, SEEK_CUR); /* Skip "first" uint. */
			fread(&shaderOffset, 4, 1, fileIn);
			fseek(fileIn, shaderOffset + 4, SEEK_SET);
		}
		else
		{
			fseek(fileIn, 0, SEEK_END);
			size = ftell(fileIn);
			fseek(fileIn, 0, SEEK_SET);
		}
		shader = (unsigned char*) malloc(size - shaderOffset);
		fread(shader, 1, size - shaderOffset, fileIn);
		fclose(fileIn);
		if (!do_parse(argv[i], shader, size, profile))
		{
			printf("Effect parsing error!\n");
		}
		free(shader);
	}

	/* Clean up. We out. */
	MOJOSHADER_glMakeContextCurrent(NULL);
	MOJOSHADER_glDestroyContext(shaderContext);
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
