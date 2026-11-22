#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")/.."
SDK_INCLUDE="${SDK_INCLUDE:?set SDK_INCLUDE to official v0.3.0 SDK/include}"
CFLAGS=(-DRD_TEST_PROVIDER -O2 -std=gnu11 -Itests/mocks -I. -I"$SDK_INCLUDE")
work="${TMPDIR:-/tmp}/ritual_beta_test"
mkdir -p "$work"
compile() {
 local target="$1"; shift
 echo "=== $target ==="
 gcc "${CFLAGS[@]}" "tests/test_${target}.c" "$@" -o "$work/$target"
 "$work/$target"
}
compile packages decks.c profiles.c
compile rex_ready_distribution decks.c profiles.c
compile beta_profiles profiles.c
compile new_profiles profiles.c
compile tea_ocean_secmeton profiles.c
compile v3_forest_anubisius_mountain profiles.c
compile v3_recipes decks.c profiles.c
compile v3_atenza_meadow_kepura decks.c profiles.c
compile v3_labyrinth_sebek_neku_isis decks.c profiles.c
compile v3_seto_darknite_nitemare decks.c profiles.c
compile v4_round decks.c profiles.c
compile new_lot_recipes decks.c profiles.c
compile core planner.c profiles.c decks.c
compile recipe_decks decks.c profiles.c
compile v030_adapter recipe_adapter.c decks.c profiles.c planner.c
compile provider_adapter recipe_adapter.c decks.c profiles.c planner.c
compile diagnostics diagnostic_core.c
compile ritual_strategy planner.c profiles.c decks.c
compile activation_priority planner.c profiles.c
compile bakura_emergency adaptive_rules.c planner.c profiles.c
compile adaptive_rules adaptive_rules.c planner.c profiles.c decks.c
compile field_guard field_guard.c planner.c profiles.c decks.c
compile effect_intelligence profiles.c
compile ai_policy ai_policy.c
compile hand_play hand_play.c
compile deck_access deck_access.c hand_play.c
compile tribute_fetch tribute_fetch.c profiles.c
compile post_ritual post_ritual.c profiles.c
gcc -std=gnu11 -O2 -Wall -Wextra -I"$SDK_INCLUDE" -I. tests/test_drop_rewards.c -o "$work/test_drop_rewards"
"$work/test_drop_rewards"
python3 tests/audit_pools.py
printf '\nALL CURRENT BETA SUITES PASS\n'
