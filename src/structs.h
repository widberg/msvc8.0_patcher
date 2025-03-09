typedef struct TOKEN_ {
    unsigned id;
    unsigned x;
    unsigned y;
    unsigned value;
    unsigned value2;
} TOKEN;

typedef struct REGS_DATA_ {
    unsigned flags;
    unsigned size;
    unsigned regs[REGS_DATA_MAX_REGS];
    unsigned spoils;
} REGS_DATA;

typedef struct FUNCTION_ATTRIBUTES_ {
    unsigned a;
    unsigned b;
    unsigned c;
    unsigned d;
    unsigned regs_data;
    unsigned g;
    unsigned h;
    unsigned i;
    unsigned cc_and_other_bits;
} FUNCTION_ATTRIBUTES;

typedef struct FUNCTION_ {
    FUNCTION_ATTRIBUTES *attributes;
} FUNCTION;
