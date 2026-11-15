#ifndef RITUAL_RULES_PROVIDER_V1_H
#define RITUAL_RULES_PROVIDER_V1_H
/* Read-only, optional runtime contract for Ritual planning assistants.
   Settings remain owned by Melhoria dos Rituais. All fields are integers
   to keep the ABI stable between ELF32 and Windows 64-bit targets. */
#define RITUAL_RULES_PROVIDER_ABI_V1 1u
#define RITUAL_RULES_SOURCES_FIELD 1u
#define RITUAL_RULES_SOURCES_HAND 2u
#define RITUAL_RULES_SOURCES_DECK 4u

typedef struct RitualRulesSnapshotV1 {
    unsigned abi_version;
    unsigned material_count;
    unsigned sources;
    int min_field;
    int min_hand;
    int require_result; /* 0 off, 1 Hand, 2 Hand or Deck */
    int ritual_only_player; /* has no effect on CPU planning */
} RitualRulesSnapshotV1;

typedef struct RitualRulesProviderV1 {
    unsigned abi_version;
    /* Return 0 when the publishing mod is not applied. Never mutate state. */
    int (*read)(RitualRulesSnapshotV1 *out, unsigned out_size);
} RitualRulesProviderV1;
#endif
