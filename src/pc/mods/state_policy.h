#ifndef MEMORIES_EXPERIMENTAL_HAL26_STATE_POLICY_H
#define MEMORIES_EXPERIMENTAL_HAL26_STATE_POLICY_H
#include <string.h>
/* HAL26 candidate only. Refusal is independent of the hook-order opt-in.
 * Resident libraries can retain effect state after OFF; resetting them is
 * not proof of complete restoration. These IDs mark missing coverage, never
 * qualify a library or grant permission to load a state. */
static int Hal26_StateLoadIncomplete(const char *id, int active, int resident)
{
    static const char *const owners[] = {
        "melhoria-dos-rituais", "ritual-card-effects-test", "ritual-duelists",
        "fusion-auxiliary-balance", "field-effects-secondary-types",
        "ai-hard-mode", "dual-play", "time-wizard-time-magic"
    };
    unsigned i;
    if (!id || (!active && !resident)) return 0;
    for (i = 0; i < sizeof(owners) / sizeof(owners[0]); ++i)
        if (!strcmp(id, owners[i])) return 1;
    return 0;
}
#endif
