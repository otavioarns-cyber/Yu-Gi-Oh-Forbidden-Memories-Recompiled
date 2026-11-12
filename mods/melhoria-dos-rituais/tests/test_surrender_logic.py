#!/usr/bin/env python3
"""Compile the exact production surrender hooks into a native simulation."""
from pathlib import Path
import subprocess
import tempfile

p=Path(__file__).resolve().parent.parent
src=(p/'surrender.c').read_text()

def extract(signature):
    start=src.index(signature)
    brace=src.index('{',start)
    level=0
    for pos in range(brace,len(src)):
        if src[pos]=='{': level+=1
        elif src[pos]=='}':
            level-=1
            if level==0:return src[start:pos+1]
    raise ValueError(signature)

code=r'''
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int32_t s32;
#define PAD_BUTTON_SELECT 0x100
struct MemoriesModHost;
typedef struct MemoriesModHost {int (*applied)(const struct MemoriesModHost*);} MemoriesModHost;
static int mod_enabled, prev_called, update_called, answer;
static int enabled(const MemoriesModHost *host) {(void)host;return mod_enabled;}
static MemoriesModHost host={enabled};
static s32 gDuel_bOpponentID;
static u8 D_8009B1D5;
static u32 gInput_wPad1Pressed;
static u8 gDuel_bQuitDialogState;
static u16 D_8009B16C;
static u8 gDuel_bWinnerSide;
static u16 gDuel_wSceneStateFlags;
static int surrender_dialog_pending;
static const MemoriesModHost *surrender_host = &host;
static int original_quit(void){prev_called++; if(gDuel_bOpponentID<0 && (gInput_wPad1Pressed&PAD_BUTTON_SELECT)){gDuel_bQuitDialogState=1;return 1;}return 0;}
static s32 (*original_quit_input)(void) = original_quit;
/* Simulate native DuelScene_UpdateWithSideInput -> DuelScene_Update, which
 * sets D_8009B16C bit 0x2000 only on a confirmed YES. */
static void original_update(void){
    update_called++;
    if(gDuel_bQuitDialogState==1)gDuel_bQuitDialogState=0x81;
    else if(gDuel_bQuitDialogState==0x81){
        gDuel_bQuitDialogState=0;
        if(answer)D_8009B16C|=0x2000;
    }
}
static void (*original_side_update)(void) = original_update;
'''
code += '\n'+extract('void Surrender_Reset(void)')+'\n'
code += extract('static s32 quit_input_hook(void)')+'\n'
code += extract('static void side_update_hook(void)')+'\n'
code += r'''
static void setup(int enemy, int who, int enabled, int selected) {
  gDuel_bOpponentID=enemy;D_8009B1D5=who;mod_enabled=enabled;
  gInput_wPad1Pressed=selected;gDuel_bQuitDialogState=0;
  D_8009B16C=0x1000;gDuel_bWinnerSide=0;
  gDuel_wSceneStateFlags=4; prev_called=0;update_called=0;answer=0;
  Surrender_Reset();
}
int main(void){
  /* Select opens the retail dialog on the player's side against CPU. */
  setup(12,0,1,PAD_BUTTON_SELECT);
  assert(quit_input_hook()==1 && gDuel_bQuitDialogState==1 &&
         surrender_dialog_pending==1 && prev_called==0);
  side_update_hook(); assert(gDuel_bQuitDialogState==0x81 &&
         gDuel_wSceneStateFlags==4 && update_called==1);
  side_update_hook(); assert(gDuel_bQuitDialogState==0 &&
         gDuel_wSceneStateFlags==4 && D_8009B16C==0x1000 &&
         !surrender_dialog_pending); /* NO */

  setup(12,0,1,PAD_BUTTON_SELECT);answer=1;
  assert(quit_input_hook()==1);
  side_update_hook();side_update_hook();
  assert(D_8009B16C==0x1000 && !surrender_dialog_pending);
  assert(gDuel_wSceneStateFlags==0xC && gDuel_bWinnerSide==1); /* YES->LOSE */

  setup(12,0,1,0);assert(quit_input_hook()==0 && !surrender_dialog_pending);
  setup(12,0,0,PAD_BUTTON_SELECT);assert(quit_input_hook()==0 && !surrender_dialog_pending);
  setup(12,1,1,PAD_BUTTON_SELECT);assert(quit_input_hook()==0 && !surrender_dialog_pending);
  setup(-1,0,1,PAD_BUTTON_SELECT);assert(quit_input_hook()==1 &&
       prev_called==1 && !surrender_dialog_pending); /* two-player native */
  setup(12,0,1,PAD_BUTTON_SELECT);
  gDuel_bQuitDialogState=0x81;assert(quit_input_hook()==0 && !surrender_dialog_pending);
  setup(12,0,1,PAD_BUTTON_SELECT);assert(quit_input_hook()==1);
  Surrender_Reset();assert(!surrender_dialog_pending); /* next duel */
  setup(12,0,1,PAD_BUTTON_SELECT);assert(quit_input_hook()==1);
  assert(quit_input_hook()==0 && surrender_dialog_pending); /* no duplicate */
  puts("PASS: 9 surrender cases (exact production hook bodies)");
}
'''
with tempfile.TemporaryDirectory() as td:
    fn=Path(td)/'test.c';exe=Path(td)/'test';fn.write_text(code)
    subprocess.run(['gcc','-std=c11','-O2','-Wall','-Wextra','-Werror',
                    '-fsanitize=undefined','-fno-omit-frame-pointer',str(fn),'-o',str(exe)],check=True)
    subprocess.run([str(exe)],check=True)
