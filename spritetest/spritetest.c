/* Sprite Test - SpriteEffect Test Program for MojoShader Effect Support
 * Written by Ethan "flibitijibibo" Lee
 * http://www.flibitijibibo.com/
 *
 * Released under public domain.
 * No warranty implied; use at your own risk.
 */

#include <SDL2/SDL.h>
#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL_opengl.h>
#include "mojoshader.h"

#define SCREEN_WIDTH	800
#define SCREEN_HEIGHT	600

#define MOJOSHADER_PROFILE "glsl120"

typedef struct vertexstruct_s
{
	float x;
	float y;
	float z;
	Uint32 color;
	float tx;
	float ty;
} vertexstruct_t;
static const vertexstruct_t vertex_array[] =
{
	{
		0.0f,
		0.0f,
		0.0f,
		0xFFFFFFFF,
		0.0f,
		0.0f
	},
	{
		800.0f,
		0.0f,
		0.0f,
		0xFFFFFFFF,
		1.0f,
		0.0f
	},
	{
		0.0f,
		600.0f,
		0.0f,
		0xFFFFFFFF,
		0.0f,
		1.0f
	},
	{
		800.0f,
		600.0f,
		0.0f,
		0xFFFFFFFF,
		1.0f,
		1.0f
	}
};
static const Uint16 index_array[] =
{
	0, 1, 2,
	3, 2, 1
};
static const float transform_matrix[] =
{
	/* FIXME: Hardcoded for 800x600! */
	0.0025f, 0.0f,        0.0f, -1.0f,
	0.0f,   -0.003333333, 0.0f,  1.0f,
	0.0f,    0.0f,       -1.0f,  0.0f,
	0.0f,    0.0f,        0.0f,  1.0f
};

static void *GetGLProcAddress(const char *name, void *d)
{
	return SDL_GL_GetProcAddress(name);
}

int main(int argc, char **argv)
{
	SDL_Window *window;
	SDL_GLContext context;
	SDL_Event evt;
	MOJOSHADER_glContext *shaderContext;
	MOJOSHADER_effect *effect;
	MOJOSHADER_glEffect *glEffect;
	SDL_Surface *bitmap;
	GLuint texture;
	GLuint buffers[2];
	FILE *fileIn;
	unsigned int fileLen;
	unsigned char *effectData;
	unsigned int passes;
	MOJOSHADER_effectStateChanges changes;
	Uint8 run = 1;

	/* Create the window and GL context */
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow(
		"Sprite Test",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		SDL_WINDOW_OPENGL
	);
	context = SDL_GL_CreateContext(window);
	shaderContext = MOJOSHADER_glCreateContext(
		MOJOSHADER_PROFILE,
		GetGLProcAddress,
		NULL,
		NULL,
		NULL,
		NULL
	);
	MOJOSHADER_glMakeContextCurrent(shaderContext);

	/* Set up the viewport, create the texture/buffer data */
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	bitmap = SDL_LoadBMP("../Sprite.bmp");
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGB,
		350,
		350,
		0,
		GL_BGR,
		GL_UNSIGNED_BYTE,
		bitmap->pixels
	);
	SDL_FreeSurface(bitmap);
	glGenBuffersARB(2, buffers);
	glBindBufferARB(GL_ARRAY_BUFFER, buffers[0]);
	glBufferDataARB(
		GL_ARRAY_BUFFER,
		sizeof(vertexstruct_t) * 4,
		vertex_array,
		GL_STATIC_DRAW
	);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
	glBufferDataARB(
		GL_ELEMENT_ARRAY_BUFFER,
		12,
		index_array,
		GL_STATIC_DRAW
	);

	/* Load and read the SpriteBatch effect file */
	fileIn = fopen("../../FNA/StockEffects/FXB/SpriteEffect.fxb", "rb");
	fseek(fileIn, 0, SEEK_END);
	fileLen = ftell(fileIn);
	fseek(fileIn, 0, SEEK_SET);
	effectData = (unsigned char*) malloc(fileLen);
	fread(effectData, 1, fileLen, fileIn);
	fclose(fileIn);
	effect = MOJOSHADER_parseEffect(
		MOJOSHADER_PROFILE,
		effectData,
		fileLen,
		NULL,
		0,
		NULL,
		0,
		NULL,
		NULL,
		NULL
	);
	free(effectData);
	glEffect = MOJOSHADER_glCompileEffect(effect);
	MOJOSHADER_effectSetRawValueName(
		effect,
		"MatrixTransform",
		transform_matrix,
		0,
		64
	);

	while (run)
	{
		while (SDL_PollEvent(&evt) > 0)
		{
			if (evt.type == SDL_QUIT)
			{
				run = 0;
			}
		}

		/* Clear the screen to black. */
		glClear(GL_COLOR_BUFFER_BIT);

		/* Bind the effect */
		MOJOSHADER_glEffectBegin(glEffect, &passes, 0, &changes);
		MOJOSHADER_glEffectBeginPass(glEffect, 0);

		/* Set the attrib pointers now, while the effect is bound */
		MOJOSHADER_glSetVertexAttribute(
			MOJOSHADER_USAGE_POSITION,
			0,
			3,
			MOJOSHADER_ATTRIBUTE_FLOAT,
			0,
			sizeof(vertexstruct_t),
			(void*) 0
		);
		MOJOSHADER_glSetVertexAttribute(
			MOJOSHADER_USAGE_COLOR,
			0,
			4,
			MOJOSHADER_ATTRIBUTE_UBYTE,
			1,
			sizeof(vertexstruct_t),
			(void*) 12
		);
		MOJOSHADER_glSetVertexAttribute(
			MOJOSHADER_USAGE_TEXCOORD,
			0,
			2,
			MOJOSHADER_ATTRIBUTE_FLOAT,
			0,
			sizeof(vertexstruct_t),
			(void*) 16
		);

		/* Flush all changes to the shader and constant buffers */
		MOJOSHADER_glProgramReady();

		/* Draw! */
		glDrawRangeElements(
			GL_TRIANGLES,
			0,
			3,
			1,
			GL_UNSIGNED_SHORT,
			NULL
		);

		/* We've finished drawing. Present what we've drawn. */
		MOJOSHADER_glEffectEndPass(glEffect);
		MOJOSHADER_glEffectEnd(glEffect);
		SDL_GL_SwapWindow(window);
	}

	/* Clean up. We out. */
	glDeleteBuffersARB(2, buffers);
	glDeleteTextures(1, &texture);
	MOJOSHADER_glDeleteEffect(glEffect);
	MOJOSHADER_freeEffect(effect);
	MOJOSHADER_glMakeContextCurrent(NULL);
	MOJOSHADER_glDestroyContext(shaderContext);
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
