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

// c2's front-end symbol (SSR)
typedef struct FUNCTION_ATTRIBUTES_ {
    unsigned a;
    unsigned b;
    unsigned c;
    unsigned d;
    unsigned sym; // The back-end symbol of the function being compiled, not ours to use
    unsigned g;
    unsigned h;
    unsigned i;
    unsigned cc_and_other_bits;
} FUNCTION_ATTRIBUTES;

// c2's back-end symbol (SYM); for a physical register, key is the register number
typedef struct SYM_ {
    void *ssr;
    unsigned kind_and_flags;
    struct SYM_ *base;
    unsigned a;
    unsigned b;
    unsigned c;
    unsigned d;
    unsigned key;
} SYM;

// c2's tuple: an instruction, or one of its operands
typedef struct TUPLE_ {
    struct TUPLE_ *next;
    unsigned opcode;
    unsigned char kind;
    unsigned char flags;
    unsigned short type;
    unsigned a;
    unsigned b;
    unsigned c;
    void *sym;
    SYM *reg;
    unsigned d;
    unsigned e;
    struct TUPLE_ *srcs;
    struct TUPLE_ *dsts;
} TUPLE;

typedef struct BLOCK_ {
    unsigned a[7];
    TUPLE *tuples;
} BLOCK;

typedef struct FLOW_GRAPH_ {
    BLOCK *entry;
} FLOW_GRAPH;

// c2's per-function state (FUNC)
typedef struct FUNCTION_ {
    FUNCTION_ATTRIBUTES *attributes;
    unsigned a;
    FLOW_GRAPH *flow_graph;
} FUNCTION;

// c2's sparse bit vector, as LoAddCallScratchRegs reads a callee's used registers
typedef struct SBV_NODE_ {
    unsigned base;
    unsigned bits;
    struct SBV_NODE_ *next;
} SBV_NODE;

typedef struct SBV_ {
    SBV_NODE *first;
    unsigned pool;
} SBV;

// One entry of the out of band regs data table, keyed by the function's symbol
typedef struct REGS_ENTRY_ {
    FUNCTION_ATTRIBUTES *attributes;
    REGS_DATA *regs_data;
    SBV spoils;
    SBV_NODE spoils_node;
} REGS_ENTRY;
