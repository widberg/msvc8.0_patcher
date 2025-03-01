__declspec(dllexport, naked) void *real_yylex(void) {
    __asm {
        push ebp
        mov ebp, esp
        and esp, 0xFFFFFFF8
        jmp YYLEX_RESUME
    }
}

#include "constants.h"
#define TOK_FASTCALL 225
#include "yylex.h"

__declspec(dllexport, naked) void parser_optype(void) {
    __asm {
        cmp ax, FE_CC_FASTCALL
        je PARSER_OPTYPE_END
        mov DWORD PTR [ebx+0x34], 0
        jmp PARSER_OPTYPE_END
    }
}

__declspec(dllexport, naked) void add_func(void) {
    __asm {
        cmp ax, FE_CC_FASTCALL
        jne ADD_FUNC_END
        mov eax, DWORD PTR [edi+0x10]
        mov DWORD PTR [esi+0xC], eax
        jmp ADD_FUNC_END
    }
}

__declspec(dllexport, naked) void add_func2(void) {
    __asm {
        cmp ax, FE_CC_FASTCALL
        jne ADD_FUNC2_END
        mov eax, DWORD PTR [esi+0x10]
        mov DWORD PTR [edi+0xC], eax
        jmp ADD_FUNC2_END
    }
}

__declspec(dllexport, naked) void save_extra(void) {
    __asm {
        movzx edx, BYTE PTR [eax+2]
        and edx, FE_CC_MASK
        cmp edx, FE_CC_FASTCALL
        jne skip_save_extra
        mov edx, DWORD PTR [eax+0xC]
        mov DWORD PTR [esp+0x118], edx
    skip_save_extra:
        movzx edx, BYTE PTR [eax+2]
        mov al, BYTE PTR [edi]
        jmp SAVE_EXTRA_END
    }
}

__declspec(dllexport, naked) void write_extra(void) {
    __asm {
        mov esi, DWORD PTR [esp+0x10]
        and esi, IL_CC_MASK
        cmp esi, IL_CC_FASTCALL
        jne skip
        mov esi, edi
        mov ebx, DWORD PTR [esp+0x118]
        call LOWRITE
    skip:
        mov ebx, DWORD PTR [esp+0x18]
        jmp WRITE_EXTRA_END
    }
}

__declspec(dllexport, naked) void vaargs(void) {
    __asm {
        mov edx, eax
        mov eax, ebx
        and eax, FE_CC_MASK
        cmp ax, FE_CC_CDECL
        jz VAARGS_GOOD_END
        cmp ax, FE_CC_THISCALL
        jz VAARGS_BAD_END
        cmp ax, FE_CC_FASTCALL
        jne VAARGS_UGLY_END
        mov edx, DWORD PTR [edx+0xC]
        test edx, edx
        jz VAARGS_BAD_END
        mov edx, DWORD PTR [edx]
        test edx, REGS_DATA_FLAG_VAARGS
        jz VAARGS_BAD_END
        jmp VAARGS_GOOD_END
    }
}

__declspec(dllexport, naked) void vaargs2(void) {
    __asm {
        cmp di, FE_CC_FASTCALL
        jne VAARGS2_UGLY_END
        mov eax, DWORD PTR [esi+0xC]
        test eax, eax
        jz VAARGS2_BAD_END
        mov eax, DWORD PTR [eax]
        test eax, REGS_DATA_FLAG_VAARGS
        jz VAARGS2_BAD_END
        jmp VAARGS2_GOOD_END
    }
}
