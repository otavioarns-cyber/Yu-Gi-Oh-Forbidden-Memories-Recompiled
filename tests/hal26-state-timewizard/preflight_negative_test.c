/* Typed proposal only, never wired into native load or canonical MODs. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../experiments/hal26/owner_preflight_v1.h"
static void valid(QsPayload*q,FlPayload*f,H26View*v){
 memset(q,0,sizeof*q);memset(f,0,sizeof*f);memset(v,0,30*sizeof*v);
 q->magic=0x51535031;q->version=q->eligible=q->owner=1;q->mask=1u<<5;q->row[5]=(QsRow){0,9,1};
 f->magic=0x464c5031;f->version=f->eligible=f->owner=1;f->serial_floor=77;f->row[6]=(FlRow){1,8,495,77};
 v[5]=(H26View){708,0,9,1,708};v[6]=(H26View){495,1,8,1,495};
}
int main(void){QsPayload q;FlPayload f;H26View v[30];unsigned checks=0;
 valid(&q,&f,v);assert(H26_Preflight(&q,&f,v,30,0)==H26_READY);++checks;
 assert(H26_Preflight(&q,&f,v,30,1)==H26_INCOMPLETE);++checks;
 assert(H26_Preflight(NULL,&f,v,30,0)==H26_BAD_ABI);assert(H26_Preflight(&q,NULL,v,30,0)==H26_BAD_ABI);assert(H26_Preflight(&q,&f,NULL,30,0)==H26_BAD_ABI);assert(H26_Preflight(&q,&f,v,29,0)==H26_BAD_ABI);checks+=4;
 for(unsigned k=0;k<23;k++){
  valid(&q,&f,v);
  switch(k){case 0:q.version++;break;case 1:f.version++;break;case 2:q.magic=0;break;case 3:f.magic=0;break;
   case 4:q.owner=0;break;case 5:f.owner=0;break;case 6:q.eligible=0;break;case 7:f.eligible=0;break;
   case 8:v[5].deck=2;break;case 9:v[6].deck=3;break;case 10:v[5].block++;break;case 11:v[6].data_id=12;break;
   case 12:f.row[6].serial=78;break;case 13:f.serial_floor=UINT32_MAX;break;case 14:q.mask|=1u<<31;break;
   case 15:q.fulfilled[0]=2;break;case 16:q.row[5].female=0;break;case 17:f.row[7]=f.row[6];break;
   case 18:v[7]=v[6];break;case 19:q.pad=1;break;case 20:v[0]=v[5];v[5].occupied=0;q.mask=1;q.row[0]=q.row[5];memset(&q.row[5],0,sizeof q.row[5]);break;
   case 21:f.row[0].card=1;break;case 22:q.row[0].female=1;break;
  }
  QsPayload before=q;FlPayload fb=f;H26View vb[30];memcpy(vb,v,sizeof v);
  assert(H26_Preflight(&q,&f,v,30,0)!=H26_READY);assert(!memcmp(&q,&before,sizeof q)&&!memcmp(&f,&fb,sizeof f)&&!memcmp(v,vb,sizeof v));++checks;
 }
 printf("PASS %u typed preflight cases, all invalid views/capsules refused without writes; complete collection always INCOMPLETE\n",checks);return 0;
}
