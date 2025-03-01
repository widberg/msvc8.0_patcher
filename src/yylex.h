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
} REGS_DATA;

static const unsigned char REGS_LOOKUP[] = {
    [WS_EAX] = MS_EAX,
    [WS_EBX] = MS_EBX,
    [WS_ECX] = MS_ECX,
    [WS_EDX] = MS_EDX,
    [WS_ESI] = MS_ESI,
    [WS_EDI] = MS_EDI
};

__declspec(dllexport) TOKEN *wrap_yylex(void) {
    TOKEN *token = real_yylex();
    if (token->id != TOK_FASTCALL) {
      return token;
    }
    TOKEN *peaked_token = TOKEN_INPUT_STACK_PEAK_TOKEN(&TOKEN_INPUT_STACK);
    if (peaked_token->id != TOK_INT) {
      token->value = 0;
      return token;
    }
    unsigned value = peaked_token->value2;
    if (value != WS_USERCALL && value != WS_USERPURGE) {
        token->value = 0;
        return token;
    }
    TOKEN_INPUT_STACK_GET_TOKEN(&TOKEN_INPUT_STACK);
    REGS_DATA *regs_data = (REGS_DATA*)DoMalloc(sizeof(*regs_data));

    regs_data->flags = value == WS_USERCALL;

    unsigned regs_size = 0;
    while (1) {
      TOKEN *peaked_token = TOKEN_INPUT_STACK_PEAK_TOKEN(&TOKEN_INPUT_STACK);
      if (peaked_token->id != TOK_INT) {
        break;
      }
      unsigned value = peaked_token->value2;
      regs_data->regs[regs_size] = REGS_LOOKUP[value];
      regs_size++;
      TOKEN_INPUT_STACK_GET_TOKEN(&TOKEN_INPUT_STACK);
      if (regs_size >= REGS_DATA_MAX_REGS) {
        break;
      }
    }
    regs_data->size = regs_size;
    token->value = (unsigned)regs_data;
    return token;
}