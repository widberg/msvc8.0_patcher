#include "constants.h"
#include "structs.h"

__declspec(dllexport, naked) void usercall_allo_reg(void) {
    __asm {
        mov ecx, DWORD PTR [esp+0x18]
        test ecx, ecx
        jz fn_zero
        mov ecx, DWORD PTR [ecx+FUNCTION.attributes]
        jmp do_stuff
    fn_zero:
        mov ecx, DWORD PTR [esp+0x24]
        mov ecx, DWORD PTR [ecx+0x28]
        mov ecx, DWORD PTR [ecx+0x18]
    do_stuff:
        mov eax, DWORD PTR [ecx+FUNCTION_ATTRIBUTES.regs_data]
        test eax, eax
        jne handle_usercall
        mov edx, OFFSET FASTCALL_REGS
        mov edi, 2
        jmp END_PTR
    handle_usercall:
        lea edx, [eax+REGS_DATA.regs]
        mov edi, DWORD PTR [eax+REGS_DATA.size]
        jmp END_PTR
    }
}

__declspec(dllexport, naked) void read_extra(void) {
    __asm {
        mov eax, DWORD PTR [esi+0x20]
        and eax, 0x1C
        cmp eax, 4
        jne skip
        call LOREAD
        mov DWORD PTR [esi+FUNCTION_ATTRIBUTES.regs_data], eax
    skip:
        cmp LONG_TYPE_INDEX, 0
        jmp READ_EXTRA_END
    }
}

__declspec(dllexport, naked) void callee_clean(void) {
    __asm {
        mov edi, DWORD PTR [eax+FUNCTION_ATTRIBUTES.cc_and_other_bits]
        and edi, 0x1C
        cmp edi, 4
        jne callee_clean_skip
        mov edi, DWORD PTR [eax+FUNCTION_ATTRIBUTES.regs_data]
        test edi, edi
        jz callee_clean_skip
        mov edi, DWORD PTR [edi+REGS_DATA.flags]
        test edi, REGS_DATA_FLAG_CALLEECLEAN
        jz callee_clean_skip
        mov edi, 0
        jmp CALLEE_CLEAN_END
    callee_clean_skip:
        mov edi, DWORD PTR [eax+0x44]
        jmp CALLEE_CLEAN_END
    }
}

__declspec(dllexport, naked) void scratch_regs(void) {
    __asm {
        mov ecx, esi
        and ecx, 0x1F
        mov edx, 1
        shl edx, cl
        mov ecx, DWORD PTR [esp+0x24]
        mov ecx, DWORD PTR [ecx+0x28]
        mov ecx, DWORD PTR [ecx+0x18]
        test ecx, ecx
        jz cont
        mov ecx, DWORD PTR [ecx+FUNCTION_ATTRIBUTES.regs_data]
        test ecx, ecx
        jz cont
        test DWORD PTR [ecx+REGS_DATA.flags], REGS_DATA_FLAG_HASSPOILS
        jz cont
        mov ecx, DWORD PTR [ecx+REGS_DATA.spoils]
        test edx, ecx
        jz SCRATCH_REGS_SKIP
    cont:
        mov eax, DWORD PTR [esp+0x24]
        call TUP_FIND_DST_REG_INTERF
        jmp SCRATCH_REGS_END
    }
}
