#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "hand_play.h"

static void expected_set(int source, int dest)
{
    unsigned char action[12], snapshot[12]={0};
    int i;
    memset(action, 0xA5, sizeof action);
    Rd_SelectFaceDownHandPlay(action,source,dest);
    snapshot[0]=(unsigned char)(source+11);
    snapshot[6]=(unsigned char)dest;
    snapshot[8]=1;
    assert(memcmp(action,snapshot,sizeof action)==0);
    for(i=0;i<12;i++)if(i!=0&&i!=6&&i!=8)assert(action[i]==0);
}
int main(void)
{
    /* Material to an empty monster field slot: prevents reported first-turn reveal. */
    expected_set(0,1);
    expected_set(4,5);
    /* Alternative to consuming a protected material: also set face-down. */
    expected_set(2,3);
    /* Ritual Magic keeps its already-correct face-down setup in the back row. */
    expected_set(1,6);
    expected_set(3,10);
    puts("PASS: all forced hand selections explicitly set face-down; clear stale flags");
    return 0;
}
