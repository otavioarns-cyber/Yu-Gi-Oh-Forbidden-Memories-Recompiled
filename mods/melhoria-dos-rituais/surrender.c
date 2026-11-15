#include "types.h"
#include "pc/mods/modapi.h"
#include "game/ai_opponent_data.h"
#include "game/input.h"
#include "game/duel_check_quit_input.h"
#include "game/duel_scene_update.h"
#include "game/duel_scene_state.h"
#include "game/duel_side_state.h"
#include "surrender.h"

/* This is a deliberately small adapter to the retail two-player quit prompt.
 * Duel_CheckQuitInput normally permits Select only when opponent ID < 0.
 * DuelScene_Update manages the native confirmation textbox and sets bit
 * 0x2000 in D_8009B16C if the player chose YES. In the single-player case,
 * that bit normally means "leave duel immediately", without a loss. We
 * translate just our own confirmed request into scene phase 0xC, which is
 * the native win/lose outro followed by the normal result/reward phase.
 * No custom textbox, no global input substitution, no altered opponents. */
static const MemoriesModHost *surrender_host;
static s32 (*original_quit_input)(void);
static void (*original_side_update)(void);
static int surrender_dialog_pending;

void Surrender_Reset(void)
{
    surrender_dialog_pending = 0;
}

static s32 quit_input_hook(void)
{
    /* Preserve the native two-player path, and do not hijack CPU turns. */
    if (gDuel_bOpponentID < 0 || D_8009B1D5 != 0 ||
        !surrender_host || !surrender_host->applied(surrender_host) ||
        !(gInput_wPad1Pressed & PAD_BUTTON_SELECT) ||
        gDuel_bQuitDialogState != 0 || surrender_dialog_pending) {
        return original_quit_input();
    }

    /* The game's own dialog defaults to NO, so accidental presses are safe. */
    gDuel_bQuitDialogState = 1;
    surrender_dialog_pending = 1;
    return 1;
}

static void side_update_hook(void)
{
    original_side_update();

    if (!surrender_dialog_pending)
        return;
    /* The native dialog has not finished handling YES/NO yet. */
    if (gDuel_bQuitDialogState != 0)
        return;

    surrender_dialog_pending = 0;
    if ((D_8009B16C & 0x2000u) == 0)
        return; /* NO or Cancel: resume the duel without changing anything. */

    /* YES: prevent Main_RunDuel from treating the request as a bare quit.
     * A normal scene phase 0xC records the CPU as winner and plays LOSE.
     * Keep the other duel flags (notably 0x1000) unchanged. */
    D_8009B16C &= (u16)~0x2000u;
    gDuel_bWinnerSide = 1;
    gDuel_wSceneStateFlags = 0x0Cu;
}

int Surrender_Init(const MemoriesModHost *host)
{
    surrender_host = host;
    Surrender_Reset();
    if (!host->hook(host, (void *)Duel_CheckQuitInput,
                    (void *)quit_input_hook, (void **)&original_quit_input))
        return 0;
    if (!host->hook(host, (void *)DuelScene_UpdateWithSideInput,
                    (void *)side_update_hook, (void **)&original_side_update))
        return 0;
    return 1;
}
