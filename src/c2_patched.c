__declspec(dllexport, naked) void usercall_allo_reg(void) {
    __asm {
        mov ecx, DWORD PTR [esp+0x18]
        test ecx, ecx
        jz fn_zero
        mov ecx, DWORD PTR [ecx]
        jmp do_stuff
    fn_zero:
        mov ecx, DWORD PTR [esp+0x24]
        mov ecx, DWORD PTR [ecx+0x28]
        mov ecx, DWORD PTR [ecx+0x18]
    do_stuff:
        mov eax, DWORD PTR [ecx+0x10]
        test eax, eax
        jne handle_usercall
        mov edx, OFFSET FASTCALL_REGS
        mov edi, 2
        jmp END_PTR
    handle_usercall:
        lea edx, [eax+0x8]
        mov edi, DWORD PTR [eax+0x4]
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
        mov DWORD PTR [esi+0x10], eax
    skip:
        cmp LONG_TYPE_INDEX, 0
        jmp READ_EXTRA_END
    }
}

__declspec(dllexport, naked) void callee_clean(void) {
    __asm {
        mov edi, DWORD PTR [eax+0x20]
        and edi, 0x1C
        cmp edi, 4
        jne callee_clean_skip
        mov edi, DWORD PTR [eax+0x10]
        test edi, edi
        jz callee_clean_skip
        mov edi, DWORD PTR [edi]
        test edi, 1
        jz callee_clean_skip
        mov edi, 0
        jmp CALLEE_CLEAN_END
    callee_clean_skip:
        mov edi, DWORD PTR [eax+0x44]
        jmp CALLEE_CLEAN_END
    }
}
