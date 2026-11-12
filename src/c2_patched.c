#include "constants.h"
#include "structs.h"

// The regs data of every __usercall/__userpurge function, keyed by the function's symbol. It used to live in
// FUNCTION_ATTRIBUTES+0x10, which is the symbol's link to its back-end symbol: SymMakeUserVar only makes a
// back-end symbol when that field is zero and SymTerm/SymReset write it.
#pragma section(".widberg.data", read, write)
__declspec(allocate(".widberg.data")) REGS_ENTRY regs_table[REGS_TABLE_SIZE];

static REGS_ENTRY *regs_table_slot(FUNCTION_ATTRIBUTES *attributes) {
    unsigned i = ((unsigned)attributes >> 3) & (REGS_TABLE_SIZE - 1);
    unsigned n;
    for (n = 0; n < REGS_TABLE_SIZE; n++) {
        REGS_ENTRY *entry = &regs_table[i];
        if (entry->attributes == attributes || entry->attributes == 0) {
            return entry;
        }
        i = (i + 1) & (REGS_TABLE_SIZE - 1);
    }
    return 0;
}

// Called for every fastcall function symbol read from the IL, so an entry left by a freed symbol at the same
// address is overwritten
__declspec(dllexport) void __fastcall regs_table_set(FUNCTION_ATTRIBUTES *attributes, REGS_DATA *regs_data) {
    REGS_ENTRY *entry = regs_table_slot(attributes);
    if (entry == 0 || (entry->attributes == 0 && regs_data == 0)) {
        return;
    }
    entry->attributes = attributes;
    entry->regs_data = regs_data;
    entry->spoils.first = &entry->spoils_node;
    entry->spoils_node.base = 0;
    entry->spoils_node.bits = regs_data ? regs_data->spoils : 0;
    entry->spoils_node.next = 0;
}

__declspec(dllexport) REGS_DATA *__fastcall regs_table_get(FUNCTION_ATTRIBUTES *attributes) {
    REGS_ENTRY *entry;
    if (attributes == 0 || (attributes->cc_and_other_bits & IL_CC_MASK) != IL_CC_FASTCALL) {
        return 0;
    }
    entry = regs_table_slot(attributes);
    if (entry == 0 || entry->attributes != attributes) {
        return 0;
    }
    return entry->regs_data;
}

// The registers a call to this function overwrites, in the form of the used registers c2 records for a
// function compiled earlier in the same module (MscRecordInterProcRegInfo); 0 without __spoils
__declspec(dllexport) SBV *__fastcall regs_table_spoils(FUNCTION_ATTRIBUTES *attributes) {
    REGS_DATA *regs_data = regs_table_get(attributes);
    if (regs_data == 0 || (regs_data->flags & REGS_DATA_FLAG_HASSPOILS) == 0) {
        return 0;
    }
    return &regs_table_slot(attributes)->spoils;
}

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
        cmp BYTE PTR [ecx+0x8], 4
        jne no_usercall
        mov ecx, DWORD PTR [ecx+0x18]
    do_stuff:
        call regs_table_get
        test eax, eax
        jne handle_usercall
    no_usercall:
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
        mov eax, DWORD PTR [esi+FUNCTION_ATTRIBUTES.cc_and_other_bits]
        and eax, IL_CC_MASK
        cmp eax, IL_CC_FASTCALL
        jne skip
        push ecx
        push edx
        call LOREAD
        mov edx, eax
        mov ecx, esi
        call regs_table_set
        pop edx
        pop ecx
    skip:
        cmp LONG_TYPE_INDEX, 0
        jmp READ_EXTRA_END
    }
}

__declspec(dllexport, naked) void callee_clean(void) {
    __asm {
        push eax
        push ecx
        push edx
        mov ecx, eax
        call regs_table_get
        mov edi, eax
        pop edx
        pop ecx
        pop eax
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

// LoAddCallScratchRegs kills each volatile register the callee's used set contains, or every one when it has
// none. Give it the spoils list as that set.
__declspec(dllexport, naked) void scratch_regs(void) {
    __asm {
        mov ecx, DWORD PTR [esp+0x18]
        call regs_table_spoils
        test eax, eax
        jz no_spoils
        mov DWORD PTR [esp+0x10], eax
    no_spoils:
        mov ebp, DWORD PTR [esp+0x10]
        mov esi, 1
        jmp SCRATCH_REGS_END
    }
}
