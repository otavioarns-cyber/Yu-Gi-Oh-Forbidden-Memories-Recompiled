/* Own portable control, no game/ROM/private MOD. Windows build must be PE32. */
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include "pc/mods/state_policy.h"
#ifdef _WIN32
#include <windows.h>
#endif
int main(void)
{
    static const char *const ids[]={"melhoria-dos-rituais","ritual-card-effects-test","ritual-duelists","fusion-auxiliary-balance","field-effects-secondary-types","ai-hard-mode","dual-play","time-wizard-time-magic","drop-missing-cards","foreign-unqualified",NULL};
    unsigned i,g,a,r,checks=0;
#ifdef _WIN32
    const unsigned char *base=(const unsigned char*)GetModuleHandleW(NULL);
    const IMAGE_DOS_HEADER *dos=(const IMAGE_DOS_HEADER*)base;
    const IMAGE_NT_HEADERS32 *pe=(const IMAGE_NT_HEADERS32*)(base+dos->e_lfanew);
    assert(sizeof(void*)==4 && dos->e_magic==IMAGE_DOS_SIGNATURE);
    assert(pe->Signature==IMAGE_NT_SIGNATURE && pe->FileHeader.Machine==IMAGE_FILE_MACHINE_I386);
    printf("WINDOWS_PE32_CONTROL machine=%04x pointer_bytes=%u\n",pe->FileHeader.Machine,(unsigned)sizeof(void*));
#endif
    for(g=0;g<2;g++)for(i=0;i<sizeof(ids)/sizeof(ids[0]);i++)for(a=0;a<2;a++)for(r=0;r<2;r++){
        /* Guard g is deliberately irrelevant to the data-safety predicate. */
        assert(Hal26_StateLoadIncomplete(ids[i],a,r)==(i<8 && (a||r)));
        ++checks;
    }
    printf("PASS HAL26 portable policy %u checks: active/resident OFF, cold OFF, unknown/data-only; no state restore executed\n",checks);
    return 0;
}
