__declspec(naked) void *DoMalloc(unsigned size) {
    __asm {
        jmp [__imp__malloc]
    }
}

__declspec(naked) void * __fastcall TOKEN_INPUT_STACK_PEAK_TOKEN(void *tok_input_stack) {
    __asm {
        mov eax, ecx
        jmp real_TOKEN_INPUT_STACK_PEAK_TOKEN
    }
}

__declspec(dllexport, naked) void *real_yylex(void) {
    __asm {
        sub     esp, 0x58
        push    ebp
        push    esi
        jmp YYLEX_RESUME
    }
}

#include "constants.h"
#define TOK_FASTCALL 205
#include "structs.h"
#include "yylex.h"

__declspec(dllexport, naked) void parser_optype(void) {
    __asm {
        cmp ax, FE_CC_FASTCALL
        je PARSER_OPTYPE_END
        mov ebx, YYVAL
        mov dword ptr [ebx+4], 0
        mov ebx, 0
        jmp PARSER_OPTYPE_END

    }
}

__declspec(dllexport, naked) void write_extra(void) {
    __asm {
        call WRITE_ATTR2
        and edi, IL_CC_MASK
        cmp edi, IL_CC_FASTCALL
        jne WRITE_EXTRA_END
        mov eax, dword ptr [esp+0x10]
        mov ebx, dword ptr [esp+0x68]
        call LOWRITE
        jmp WRITE_EXTRA_END
    }
}

__declspec(dllexport, naked) void add_func(void) {
    __asm {
        and word ptr [edi+0xC], 0xFFF8
        cmp ax, FE_CC_FASTCALL
        jne ADD_FUNC_END
        mov eax, dword ptr [edi+0x10]
        mov dword ptr [esi+0xC], eax
        jmp ADD_FUNC_END
    }
}

__declspec(dllexport, naked) void save_extra(void) {
    __asm {
        movzx edx, byte ptr [eax+2]
        and edx, FE_CC_MASK
        cmp edx, FE_CC_FASTCALL
        jne skip_save_extra
        mov edx, dword ptr [eax+0xC]
        mov dword ptr [esp+0x68], edx
    skip_save_extra:
        movzx edx, byte ptr [eax+2]
        and edx, 0x1F
        jmp SAVE_EXTRA_END
    }
}

__declspec(dllexport, naked) void vaargs(void) {
    __asm {
        mov edx, eax
        mov eax, ebx
        and eax, FE_CC_MASK
        cmp ax, FE_CC_STDCALL
        jz VAARGS_BAD_END
        cmp ax, FE_CC_FASTCALL
        jne VAARGS_UGLY_END
        mov edx, dword ptr [edx+0xC]
        test edx, edx
        jz VAARGS_BAD_END
        mov edx, dword ptr [edx]
        test edx, FE_CC_CDECL
        jz VAARGS_BAD_END
        jmp VAARGS_GOOD_END
    }
}
