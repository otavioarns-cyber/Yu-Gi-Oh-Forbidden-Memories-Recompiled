#include "../../src/pc/platform/mods_window.c"
static int calls,fail_restart;
int Platform_RestartGame(void){++calls;return fail_restart?-1:0;}
int Hal2224_WindowApply(int h,int r,int d,int wh,int wr,int wd,int rh,int rr,int rd,int confirm,int fail){
 for(int i=0;i<Mods_Count();i++)adopt(i);pending=0;wanted[h]=wh;wanted[r]=wr;wanted[d]=wd;
 if(rh!=-999){ranks[h]=rh;ranks[r]=rr;ranks[d]=rd;}
 int before=calls,needs=needs_restart();fail_restart=fail;apply();printf("WINDOW first needs_restart=%d pending=%d restarts=%d status=%s\n",needs,pending,calls-before,status);
 if(confirm&&pending==1){apply();printf("WINDOW confirm pending=%d restarts=%d status=%s\n",pending,calls-before,status);}return needs*100+(calls-before)*10+pending;
}
const char*Hal2224_WindowStatus(void){return status;}
