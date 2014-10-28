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

#define MEPTOOL_SHADER_PROFILE "glsl120"

#include <stdio.h>
#include <stdlib.h>
#include "mojoshader/mojoshader.h"

/* BEGIN GANKED CODE FROM utils/testparse.c! */

static void print_effect(const char *fname, const MOJOSHADER_effect *effect,
                         const unsigned int indent);

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
        int i, j, k, l;
        const MOJOSHADER_effectTechnique *technique = effect->techniques;
        const MOJOSHADER_effectString *string = effect->strings;
        const MOJOSHADER_effectObject *object = effect->objects;
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
                        /* FIXME: Multiple values? */
                        printf("            TEXTURE #%d\n", *((int*) states[j].valuesI));
                    }
                    else
                    {
                        printf("            TYPE: %d -> ", states[j].type);
                        if (states[j].type == MOJOSHADER_SAMP_MIPMAPLODBIAS)
                        {
                            /* float types */
                            /* FIXME: Multiple values? */
                            printf("FLOAT: %.2f\n", *((float*) states[j].valuesF));
                        }
                        else
                        {
                            /* int/enum types */
                            /* FIXME: Multiple values? */
                            printf("INT: %d\n", *((int*) states[j].valuesI));
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
            else
            {
                printf("    OBJECT VALUES:");
                int *iArray = (int *) param->values;
                for (j = 0; j < param->value_count; j++)
                {
                    printf(" %d", iArray[j]);
                }
                printf("\n");
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
                    INDENT(); printf("        STATE %d:", state->type);
                    for (l = 0; l < state->value_count; l++)
                    {
                        printf(" %d", *((int *) state->values));
                    } // for
                    printf("\n");
                } // for
            } // for
            printf("\n");
        } // for

        for (i = 0; i < effect->string_count; i++, string++)
        {
            INDENT();
            printf("STRING #%d: %s: %u\n", i,
                   string->string, string->index);
        } // for

        printf("\n");

        for (i = 0; i < effect->object_count; i++, object++)
        {
            INDENT();
            if (object->type == MOJOSHADER_OBJECTTYPE_SHADER)
            {
                printf("SHADER #%d: technique %u, pass %u\n", i,
                       object->shader.technique, object->shader.pass);
                print_shader(fname, object->shader.shader, indent + 1);
            } // if
            else if (object->type == MOJOSHADER_OBJECTTYPE_MAPPING)
            {
                printf("MAPPING #%d: name '%s', parameter %u\n", i,
                       object->mapping.name, object->mapping.param);
            } // else if
            else
            {
                printf("UNKNOWN OBJECT: #%d\n", i);
            } // else
        } // for
    } // else
} // print_effect

/* END GANKED CODE FROM utils/testparse.c! */

int main(int argc, char **argv)
{
	int i;
	long size = 0;
	unsigned int shaderOffset = 0;
	unsigned char *shader;

	if (argc < 2)
	{
		printf("Usage: %s effectFile.fxb\n", argv[0]);
		return 0;
	}

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
		if (!do_parse(argv[i], shader, size, MEPTOOL_SHADER_PROFILE))
		{
			printf("Effect parsing error!\n");
		}
		free(shader);
	}

	return 0;
}
