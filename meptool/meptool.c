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

static void print_value(const MOJOSHADER_effectValue *value,
                        const unsigned int indent)
{
    int i;

    INDENT();
    printf("VALUE: %s -> %s\n", value->name, value->semantic);

    static const char *classes[] =
    {
        "SCALAR",
        "VECTOR",
        "ROW-MAJOR MATRIX",
        "COLUMN-MAJOR MATRIX",
        "OBJECT",
        "STRUCT"
    };
    static const char *types[] =
    {
        "VOID",
        "BOOL",
        "INT",
        "FLOAT",
        "STRING",
        "TEXTURE",
        "TEXTURE1D",
        "TEXTURE2D",
        "TEXTURE3D",
        "TEXTURECUBE",
        "SAMPLER",
        "SAMPLER1D",
        "SAMPLER2D",
        "SAMPLER3D",
        "SAMPLERCUBE",
        "PIXELSHADER",
        "VERTEXSHADER",
        "UNSUPPORTED"
    };
    do_indent(indent + 1);
    printf("CLASS: %s\n", classes[value->value_class]);
    do_indent(indent + 1);
    printf("TYPE: %s\n", types[value->value_type]);

    do_indent(indent + 1);
    printf("ROWS/COLUMNS/ELEMENTS: %d, %d, %d\n",
           value->row_count, value->column_count, value->element_count);
    do_indent(indent + 1);
    printf("TOTAL VALUES: %d\n", value->value_count);

    if (value->value_type == MOJOSHADER_SYMTYPE_SAMPLER
     || value->value_type == MOJOSHADER_SYMTYPE_SAMPLER1D
     || value->value_type == MOJOSHADER_SYMTYPE_SAMPLER2D
     || value->value_type == MOJOSHADER_SYMTYPE_SAMPLER3D
     || value->value_type == MOJOSHADER_SYMTYPE_SAMPLERCUBE)
    {
        do_indent(indent + 1);
        printf("SAMPLER VALUES:\n");
        for (i = 0; i < value->value_count; i++)
        {
            MOJOSHADER_effectSamplerState *state = &value->valuesSS[i];

            static const char *samplerstatetypes[] =
            {
                "UNKNOWN0",
                "UNKNOWN1",
                "UNKNOWN2",
                "UNKNOWN3",
                "TEXTURE",
                "ADDRESSU",
                "ADDRESSV",
                "ADDRESSW",
                "BORDERCOLOR",
                "MAGFILTER",
                "MINFILTER",
                "MIPFILTER",
                "MIPMAPLODBIAS",
                "MAXMIPLEVEL",
                "MAXANISOTROPY",
                "SRGBTEXTURE",
                "ELEMENTINDEX",
                "DMAPOFFSET",
            };
            do_indent(indent + 2);
            printf("TYPE: %s -> ", samplerstatetypes[state->type]);

            /* Assuming only one value per state! */
            if (state->type == MOJOSHADER_SAMP_MIPMAPLODBIAS)
            {
                /* float types */
                printf("%.2f\n", *state->value.valuesF);
            } // if
            else
            {
                /* int/enum types */
                printf("%d\n", *state->value.valuesI);
            } // else
        } // for
    } // if
    else
    {
        do_indent(indent + 1);
        printf("%s VALUES:\n", types[value->value_type]);
        for (i = 0; i < value->value_count; i++)
        {
            do_indent(indent + 2);
            static const char *prints[] =
            {
                "%X\n",
                "%d\n",
                "%d\n",
                "%.2f\n",
                "%d\n",
                "%d\n",
                "%d\n",
                "%d\n",
                "%d\n",
                "%d\n",
                "SAMPLER?!\n",
                "SAMPLER?!\n",
                "SAMPLER?!\n",
                "SAMPLER?!\n",
                "SAMPLER?!\n",
                "%d\n",
                "%d\n",
                "%X\n"
            };
            if (value->value_type == MOJOSHADER_SYMTYPE_FLOAT)
            {
                printf(prints[value->value_type], value->valuesF[i]);
            }
            else
            {
                printf(prints[value->value_type], value->valuesI[i]);
            }
        } // for
    } // else
} // print_value

static void print_effect(const char *fname, const MOJOSHADER_effect *effect,
                         const unsigned int indent)
{
    INDENT();
    printf("PROFILE: %s\n", effect->profile);
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
        const MOJOSHADER_effectObject *object = effect->objects;
        const MOJOSHADER_effectParam *param = effect->params;

        for (i = 0; i < effect->param_count; i++, param++)
        {
            INDENT();
            printf("PARAM #%d\n", i);
            print_value(&param->value, indent + 1);

            if (param->annotation_count > 0)
            {
                do_indent(indent + 1);
                printf("ANNOTATIONS:\n");
            } // if
            for (j = 0; j < param->annotation_count; j++)
            {
                print_value(&param->annotations[j], indent + 2);
            } // for
        } // for
        printf("\n");

        for (i = 0; i < effect->technique_count; i++, technique++)
        {
            const MOJOSHADER_effectPass *pass = technique->passes;
            INDENT();
            printf("TECHNIQUE #%d ('%s'):\n", i, technique->name);
            for (j = 0; j < technique->pass_count; j++, pass++)
            {
                const MOJOSHADER_effectState *state = pass->states;
                do_indent(indent + 1);
                printf("PASS #%d ('%s'):\n", j, pass->name);
                for (k = 0; k < pass->state_count; k++, state++)
                {
                    do_indent(indent + 2);
                    printf("STATE %d:\n", state->type);
                    print_value(&state->value, indent + 3);
                } // for
            } // for
        } // for
        printf("\n");

        /* Start at index 1, 0 is always empty (thanks Microsoft!) */
        object++;
        for (i = 1; i < effect->object_count; i++, object++)
        {
            INDENT();
            if (object->type == MOJOSHADER_OBJECTTYPE_SHADER)
            {
                printf("SHADER #%d: technique %u, pass %u\n", i,
                       object->shader.technique, object->shader.pass);
                print_shader(fname, object->shader.shader, indent + 1);
            } // if
            else if (object->type == MOJOSHADER_OBJECTTYPE_MAPPING)
                printf("MAPPING #%d: name '%s', parameter %u\n", i,
                       object->mapping.name, object->mapping.param);
            else if (object->type == MOJOSHADER_OBJECTTYPE_STRING)
                printf("STRING #%d: '%s', parameter %u\n", i,
                       object->string.string, object->string.index);
            else if (object->type == MOJOSHADER_OBJECTTYPE_TEXTURE)
                printf("TEXTURE #%d: register %d\n", i,
                       object->texture.tex_register);
            else
                printf("UNKNOWN OBJECT: #%d\n", i);
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
