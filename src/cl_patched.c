#include "constants.h"

#define WS_STR2(x) #x
#define WS_STR(x) WS_STR2(x)

unsigned short *defines[] = {
    L"-D__eax=" WS_STR(WS_EAX),
    L"-D__ebx=" WS_STR(WS_EBX),
    L"-D__ecx=" WS_STR(WS_ECX),
    L"-D__edx=" WS_STR(WS_EDX),
    L"-D__esi=" WS_STR(WS_ESI),
    L"-D__edi=" WS_STR(WS_EDI),
    L"-D__usercall=__fastcall " WS_STR(VAL_USERCALL),
    L"-D__userpurge=__fastcall " WS_STR(VAL_USERPURGE)
};

#undef WS_STR
#undef WS_STR2

__declspec(dllexport, naked) void prep(void) {
    __asm {
        call DEFINE_STUFF
        push LENGTH defines
        mov ebx, OFFSET defines
        call DEFINE_STUFF
        add esp, 0x10
        jmp PREP_END
    }
}
