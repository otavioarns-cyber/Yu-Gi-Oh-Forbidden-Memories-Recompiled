/* Test bridge to the unmodified window's staging and Apply routines.
 * No UI source edit, no mouse/gameplay claim; external restart is observed. */
#include "../../src/pc/platform/mods_window.c"
static int restart_calls, restart_failure;
int Platform_RestartGame(void){++restart_calls;return restart_failure?-1:0;}
int Hal22_WindowApply(int h,int d,int h_on,int d_on,int h_rank,int d_rank,int confirm,int fail){
 for(int i=0;i<Mods_Count();i++)adopt(i);
 pending=0;wanted[h]=h_on;wanted[d]=d_on;
 if(h_rank!=-999){ranks[h]=h_rank;ranks[d]=d_rank;}
 restart_failure=fail;int before=restart_calls,needs=needs_restart();
 apply();printf("WINDOW first needs_restart=%d pending=%d restarts=%d status=%s\n",needs,pending,restart_calls-before,status);
 if(confirm&&pending==1){apply();printf("WINDOW confirmed pending=%d restarts=%d status=%s\n",pending,restart_calls-before,status);}
 return needs*100+(restart_calls-before)*10+pending;
}

const char*Hal22_WindowStatus(void){return status;}
