/* The following code was ganked from utils/testparse.c in MojoShader. */

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

#if MOJOSHADER_DEBUG_MALLOC
static void *Malloc(int len)
{
    void *ptr = malloc(len + sizeof (int));
    int *store = (int *) ptr;
    printf("malloc() %d bytes (%p)\n", len, ptr);
    if (ptr == NULL) return NULL;
    *store = len;
    return (void *) (store + 1);
} // Malloc


static void Free(void *_ptr)
{
    int *ptr = (((int *) _ptr) - 1);
    int len = *ptr;
    printf("free() %d bytes (%p)\n", len, ptr);
    free(ptr);
} // Free
#else
#define Malloc NULL
#define Free NULL
#endif

static inline void do_indent(const unsigned int indent)
{
    unsigned int i;
    for (i = 0; i < indent; i++)
        printf("    ");
}

#define INDENT() do { if (indent) { do_indent(indent); } } while (0)


static const char *shader_type(const MOJOSHADER_shaderType s)
{
    switch (s)
    {
        case MOJOSHADER_TYPE_UNKNOWN: return "unknown";
        case MOJOSHADER_TYPE_PIXEL: return "pixel";
        case MOJOSHADER_TYPE_VERTEX: return "vertex";
        case MOJOSHADER_TYPE_GEOMETRY: return "geometry";
        default: return "(bogus value?)";
    } // switch

    return NULL;  // shouldn't hit this.
} // shader_type


static void print_typeinfo(const MOJOSHADER_symbolTypeInfo *info,
                           unsigned int indent)
{
    static const char *symclasses[] = {
        "scalar", "vector", "row-major matrix",
        "column-major matrix", "object", "struct"
    };

    static const char *symtypes[] = {
        "void", "bool", "int", "float", "string", "texture",
        "texture1d", "texture2d", "texture3d", "texturecube",
        "sampler", "sampler1d", "sampler2d", "sampler3d",
        "samplercube", "pixelshader", "vertexshader", "unsupported"
    };

    INDENT();
    printf("      symbol class %s\n", symclasses[info->parameter_class]);
    INDENT();
    printf("      symbol type %s\n", symtypes[info->parameter_type]);
    INDENT();
    printf("      rows %u\n", info->rows);
    INDENT();
    printf("      columns %u\n", info->columns);
    INDENT();
    printf("      elements %u\n", info->elements);

    if (info->member_count > 0)
    {
        int i;
        INDENT(); printf("      MEMBERS:\n");
        for (i = 0; i < info->member_count; i++)
        {
            const MOJOSHADER_symbolStructMember *member = &info->members[i];
            INDENT(); printf("      MEMBERS:\n");
            indent++;
            INDENT(); printf("      * %d: \"%s\"\n", i, member->name);
            print_typeinfo(&member->info, indent);
            indent--;
        } // for
    } // if
} // print_typeinfo


static void print_symbols(const MOJOSHADER_symbol *sym,
                          const unsigned int symbol_count,
                          const unsigned int indent)
{
    INDENT(); printf("SYMBOLS:");
    if (symbol_count == 0)
        printf(" (none.)\n");
    else
    {
        int i;
        printf("\n");
        for (i = 0; i < symbol_count; i++, sym++)
        {
            static const char *regsets[] = {
                "bool", "int4", "float4", "sampler"
            };

            INDENT(); printf("    * %d: \"%s\"\n", i, sym->name);
            INDENT(); printf("      register set %s\n", regsets[sym->register_set]);
            INDENT(); printf("      register index %u\n", sym->register_index);
            INDENT(); printf("      register count %u\n", sym->register_count);
            print_typeinfo(&sym->info, indent);
        } // for
        printf("\n");
    } // else
} // print_symbols


static void print_preshader_operand(const MOJOSHADER_preshader *preshader,
                                    const int instidx, const int opidx)
{
    static char mask[] = { 'x', 'y', 'z', 'w' };
    const MOJOSHADER_preshaderInstruction *inst = &preshader->instructions[instidx];
    const MOJOSHADER_preshaderOperand *operand = &inst->operands[opidx];
    const int elems = inst->element_count;
    const int isscalarop = (inst->opcode >= MOJOSHADER_PRESHADEROP_SCALAR_OPS);
    const int isscalar = ((isscalarop) && (opidx == 0)); // probably wrong.
    int i;

    switch (operand->type)
    {
        case MOJOSHADER_PRESHADEROPERAND_LITERAL:
        {
            const double *lit = &preshader->literals[operand->index];
            printf("(");
            if (isscalar)
            {
                const double val = *lit;
                for (i = 0; i < elems-1; i++)
                    printf("%g, ", val);
                printf("%g)", val);
            } // if
            else
            {
                for (i = 0; i < elems-1; i++, lit++)
                    printf("%g, ", *lit);
                printf("%g)", *lit);
            } // else
            break;
        } // case

        case MOJOSHADER_PRESHADEROPERAND_INPUT:
        case MOJOSHADER_PRESHADEROPERAND_OUTPUT:
        case MOJOSHADER_PRESHADEROPERAND_TEMP:
        {
            int idx = operand->index % 4;
            char regch = 'c';
            if (operand->type == MOJOSHADER_PRESHADEROPERAND_TEMP)
                regch = 'r';

            // Begin flibitChanges!
            if (operand->array_register_count > 0)
            {
                for (i = operand->array_register_count - 1; i >= 0; i--)
                    printf("c%d[", operand->array_registers[i]);
                printf("%c%d.%c", regch, operand->index / 4, mask[idx]);
                for (i = 0; i < operand->array_register_count; i++)
                    printf("]");
                break;
            } // if
            // End flibitChanges!
            printf("%c%d", regch, operand->index / 4);
            if (isscalar)
                printf(".%c", mask[idx]);
            else if (elems != 4)
            {
                printf(".");
                for (i = 0; i < elems; i++)
                    printf("%c", mask[idx++]);
            } // else if
            break;
        } // case

        default:
            printf("[???{%d, %u}???]", (int) operand->type, operand->index);
            break;
    } // switch
} // print_preshader_operand


static void print_preshader(const MOJOSHADER_preshader *preshader,
                            const int indent)
{
    MOJOSHADER_preshaderInstruction *inst = preshader->instructions;
    int i, j;

    static const char *opcodestr[] = {
        "nop", "mov", "neg", "rcp", "frc", "exp", "log", "rsq", "sin", "cos",
        "asin", "acos", "atan", "min", "max", "lt", "ge", "add", "mul",
        "atan2", "div", "cmp", "movc", "dot", "noise", "min", "max", "lt",
        "ge", "add", "mul", "atan2", "div", "dot", "noise"
    };

    INDENT(); printf("PRESHADER:\n");

    print_symbols(preshader->symbols, preshader->symbol_count, indent + 1);

    for (i = 0; i < preshader->instruction_count; i++, inst++)
    {
        INDENT(); printf("    %s ", opcodestr[inst->opcode]);

        // print dest register first...
        print_preshader_operand(preshader, i, inst->operand_count - 1);

        // ...then the source registers.
        for (j = 0; j < inst->operand_count - 1; j++)
        {
            printf(", ");
            print_preshader_operand(preshader, i, j);
        } // for

        printf("\n");
    } // for

    printf("\n");
} // print_preshader


static void print_attrs(const char *category, const int count,
                        const MOJOSHADER_attribute *attributes,
                        const int indent)
{
    INDENT(); printf("%s:", category);
    if (count == 0)
        printf(" (none.)\n");
    else
    {
        int i;
        printf("\n");
        for (i = 0; i < count; i++)
        {
            static const char *usagenames[] = {
                "position", "blendweight", "blendindices", "normal",
                "psize", "texcoord", "tangent", "binormal", "tessfactor",
                "positiont", "color", "fog", "depth", "sample"
            };
            const MOJOSHADER_attribute *a = &attributes[i];
            char numstr[16] = { 0 };
            if (a->index != 0)
                snprintf(numstr, sizeof (numstr), "%d", a->index);
            INDENT();
            printf("    * %s%s", usagenames[(int) a->usage], numstr);
            if (a->name != NULL)
                printf(" (\"%s\")", a->name);
            printf("\n");
        } // for
    } // else
} // print_attrs


static void print_shader(const char *fname, const MOJOSHADER_parseData *pd,
                         unsigned int indent)
{
    INDENT(); printf("PROFILE: %s\n", pd->profile);
    if (pd->error_count > 0)
    {
        int i;
        for (i = 0; i < pd->error_count; i++)
        {
            const MOJOSHADER_error *err = &pd->errors[i];
            INDENT();
            printf("%s:%d: ERROR: %s\n",
                    err->filename ? err->filename : fname,
                    err->error_position, err->error);
        } // for
    } // if
    else
    {
        INDENT(); printf("SHADER TYPE: %s\n", shader_type(pd->shader_type));
        INDENT(); printf("VERSION: %d.%d\n", pd->major_ver, pd->minor_ver);
        INDENT(); printf("INSTRUCTION COUNT: %d\n", (int) pd->instruction_count);
        print_attrs("INPUTS", pd->attribute_count, pd->attributes, indent);
        print_attrs("OUTPUTS", pd->output_count, pd->outputs, indent);

        INDENT(); printf("CONSTANTS:");
        if (pd->constant_count == 0)
            printf(" (none.)\n");
        else
        {
            int i;
            printf("\n");
            for (i = 0; i < pd->constant_count; i++)
            {
                static const char *typenames[] = { "float", "int", "bool" };
                const MOJOSHADER_constant *c = &pd->constants[i];
                INDENT(); 
                printf("    * %d: %s (", c->index, typenames[(int) c->type]);
                if (c->type == MOJOSHADER_UNIFORM_FLOAT)
                {
                    printf("%f %f %f %f", c->value.f[0], c->value.f[1],
                                          c->value.f[2], c->value.f[3]);
                } // if
                else if (c->type == MOJOSHADER_UNIFORM_INT)
                {
                    printf("%d %d %d %d", c->value.i[0], c->value.i[1],
                                          c->value.i[2], c->value.i[3]);
                } // else if
                else if (c->type == MOJOSHADER_UNIFORM_BOOL)
                {
                    printf("%s", c->value.b ? "true" : "false");
                } // else if
                else
                {
                    printf("???");
                } // else
                printf(")\n");
            } // for
        } // else

        INDENT(); printf("UNIFORMS:");
        if (pd->uniform_count == 0)
            printf(" (none.)\n");
        else
        {
            int i;
            printf("\n");
            for (i = 0; i < pd->uniform_count; i++)
            {
                static const char *typenames[] = { "float", "int", "bool" };
                const MOJOSHADER_uniform *u = &pd->uniforms[i];
                const char *arrayof = "";
                const char *constant = u->constant ? "const " : "";
                char arrayrange[64] = { '\0' };
                if (u->array_count > 0)
                {
                    arrayof = "array[";
                    snprintf(arrayrange, sizeof (arrayrange), "%d] ",
                             u->array_count);
                } // if

                INDENT();
                printf("    * %d: %s%s%s%s", u->index, constant, arrayof,
                        arrayrange, typenames[(int) u->type]);
                if (u->name != NULL)
                    printf(" (\"%s\")", u->name);
                printf("\n");
            } // for
        } // else

        INDENT(); printf("SAMPLERS:");
        if (pd->sampler_count == 0)
            printf(" (none.)\n");
        else
        {
            int i;
            printf("\n");
            for (i = 0; i < pd->sampler_count; i++)
            {
                static const char *typenames[] = { "2d", "cube", "volume" };
                const MOJOSHADER_sampler *s = &pd->samplers[i];
                INDENT();
                printf("    * %d: %s", s->index, typenames[(int) s->type]);
                if (s->name != NULL)
                    printf(" (\"%s\")", s->name);
                if (s->texbem)
                    printf(" [TEXBEM]");
                printf("\n");
            } // for
        } // else

        print_symbols(pd->symbols, pd->symbol_count, indent);

        if (pd->preshader != NULL)
            print_preshader(pd->preshader, indent);

        if (pd->output != NULL)
        {
            int i;
            INDENT();
            printf("OUTPUT:\n");
            indent++;
            INDENT();
            for (i = 0; i < pd->output_len; i++)
            {
                putchar((int) pd->output[i]);
                if (pd->output[i] == '\n')
                    INDENT();
            } // for
            printf("\n");
            indent--;
        } // if
    } // else

    printf("\n\n");
} // print_shader


static int do_parse(const char *fname, const unsigned char *buf,
                    const int len, const char *prof)
{
    int retval = 0;

    // magic for an effects file (!!! FIXME: I _think_).
    if ( ((buf[0] == 0x01) && (buf[1] == 0x09) &&
          (buf[2] == 0xFF) && (buf[3] == 0xFE)) ||
         ((buf[0] == 0xCF) && (buf[1] == 0x0B) &&
          (buf[2] == 0xF0) && (buf[3] == 0xBC)) )
    {
        const MOJOSHADER_effect *effect;
        effect = MOJOSHADER_parseEffect(prof, buf, len, NULL, 0,
                                        NULL, 0, Malloc, Free, 0);
        retval = (effect->error_count == 0);
        printf("EFFECT: %s\n", fname);
        print_effect(fname, effect, 1);
        MOJOSHADER_freeEffect(effect);
    } // if

    else  // do it as a regular compiled shader.
    {
        const MOJOSHADER_parseData *pd;
        pd = MOJOSHADER_parse(prof, buf, len, NULL, 0,
                              NULL, 0, Malloc, Free, NULL);
        retval = (pd->error_count == 0);
        printf("SHADER: %s\n", fname);
        print_shader(fname, pd, 1);
        MOJOSHADER_freeParseData(pd);
    } // else

    return retval;
} // do_parse
