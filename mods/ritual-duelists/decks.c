#include "ritual_duelists.h"
#include <string.h>

/* The package is a weighted VIRTUAL entry (weight 30), not a real card.
 * Draw selection reserves all four real cards atomically before considering
 * the next weighted slot. An invalid package is removed from that draw only.
 * The actual summoning flow is delegated to the active Ritual MOD. */

static int rd_eligible(const RdOpponentProfile *p,const RdRitualRecipe *recipe,
  RdMaterialMatch match,void *context,u8 *held,int id,int role,int cap)
{
    if(id<=0||id>cap||id==recipe->result_card||held[id]>=RD_COPY_CAP)return 0;
    /* Weevil's Time Wizard is a normal weighted draw, NEVER a Javelin
       package material even if a broad recipe provider allows any monster. */
    if(p->recipe.ritual_magic==696 && id==16)return 0;
    return match(p,id,&recipe->materials[role],(unsigned)role,context)!=0;
}

static int rd_assign(const RdOpponentProfile *p,const RdRitualRecipe *recipe,
  RdMaterialMatch match,void *context,u8 *held,int cap,int role,u16 selected[3],int *budget)
{
    int i,id;
    const RdRitualRequirement *req;
    if(role==3)return 1;
    if(--(*budget)<0)return 0; /* cap adversarial custom-recipe search */
    req=&recipe->materials[role];
    /* Exact requirements MUST use their exact card, even if outside the pool. */
    if(req->exact_card>0){
        id=req->exact_card;
        if(!rd_eligible(p,recipe,match,context,held,id,role,cap))return 0;
        held[id]++;selected[role]=(u16)id;
        if(rd_assign(p,recipe,match,context,held,cap,role+1,selected,budget))return 1;
        held[id]--;return 0;
    }
    /* The approved per-duelist preferred materials take precedence. */
    for(i=0;i<p->baseline_material_count;i++){
        id=p->baseline_materials[i];
        if(!rd_eligible(p,recipe,match,context,held,id,role,cap))continue;
        held[id]++;selected[role]=(u16)id;
        if(rd_assign(p,recipe,match,context,held,cap,role+1,selected,budget))return 1;
        held[id]--;
    }
    for(i=0;i<p->pool_count;i++){
        id=p->pool[i].id;
        if(!rd_eligible(p,recipe,match,context,held,id,role,cap))continue;
        held[id]++;selected[role]=(u16)id;
        if(rd_assign(p,recipe,match,context,held,cap,role+1,selected,budget))return 1;
        held[id]--;
    }
    /* Only after the pool fails do we search the loaded card catalog. */
    for(id=1;id<=cap;id++){
        if(!rd_eligible(p,recipe,match,context,held,id,role,cap))continue;
        held[id]++;selected[role]=(u16)id;
        if(rd_assign(p,recipe,match,context,held,cap,role+1,selected,budget))return 1;
        held[id]--;
    }
    return 0;
}

static int rd_find_package(const RdOpponentProfile *p,const RdRitualRecipe *r,
  RdMaterialMatch match,void *ctx,u8 *held,int cap,u16 cards[4])
{
    u16 trio[3]={0};int i,role,budget=12000,found;
    if(r->material_count!=3||!r->materials||r->ritual_magic<1||
       r->ritual_magic>cap||held[r->ritual_magic]>=RD_COPY_CAP)return 0;
    /* Reservation of the Ritual Magic participates in the same copy cap. */
    held[r->ritual_magic]++;
    /* Preflight catches impossible generic requirements before backtracking,
       avoiding pathological O(catalog^3) provider lookups. */
    for(role=0;role<3;role++){
        found=0;
        if(r->materials[role].exact_card>0){
            found=rd_eligible(p,r,match,ctx,held,r->materials[role].exact_card,role,cap);
        } else for(i=1;i<=cap;i++)if(rd_eligible(p,r,match,ctx,held,i,role,cap)){
            found=1;break;
        }
        if(!found){held[r->ritual_magic]--;return 0;}
    }
    if(!rd_assign(p,r,match,ctx,held,cap,0,trio,&budget)){
        held[r->ritual_magic]--;return 0;
    }
    cards[0]=r->ritual_magic;
    for(i=0;i<3;i++)cards[i+1]=trio[i];
    for(i=0;i<4;i++)held[cards[i]]--; /* trial-only; commit is atomic */
    return 1;
}

static void rd_commit(u8 *held,u16 out[RD_DECK_SIZE],int *len,
                      RdPackageStats *stats,const u16 cards[4])
{
    int i,p=stats->packages;
    for(i=0;i<4;i++){
        out[(*len)++]=cards[i];held[cards[i]]++;
        stats->included[p][i]=cards[i];
    }
    stats->packages++;
}

int Rd_GeneratePackageDeck(const RdOpponentProfile *p,
  const RdRitualRecipe *recipe,RdMaterialMatch match,void *context,
  int catalog_max_id,int mode,int free_guarantee,
  u16 out[RD_DECK_SIZE],RdRandom rng,void *rng_context,RdPackageStats *stats)
{
    static u8 held[RD_CARD_ID_MAX+1];
    RdPackageStats result={0};
    u16 pending[4];
    int n=0,i,j,total,weight,roll,selected,chosen,need_first;
    if(!stats)return 0;
    memset(stats,0,sizeof(*stats));
    if(!p||!p->pool||!recipe||!match||!out||!rng||
       catalog_max_id<=0||catalog_max_id>RD_CARD_ID_MAX||
       (mode!=2&&mode!=6)||recipe->material_count!=3||
       !recipe->materials||recipe->ritual_magic<=0||
       recipe->ritual_magic>catalog_max_id||
       p->pool_count==0){stats->error=1;return 0;}
    total=0;
    for(i=0;i<p->pool_count;i++){
        const RdWeightedCard *w=&p->pool[i];
        if(w->id<1||w->id>catalog_max_id||w->weight==0){stats->error=2;return 0;}
        for(j=0;j<i;j++)if(p->pool[j].id==w->id){stats->error=2;return 0;}
        total+=w->weight;
    }
    if(total+RD_PACKAGE_WEIGHT!=2048){stats->error=2;return 0;}
    memset(held,0,(size_t)catalog_max_id+1);
    need_first=(mode==2 || (mode==6 && free_guarantee!=0));
    if(need_first){
        if(!rd_find_package(p,recipe,match,context,held,catalog_max_id,pending)){
            stats->error=1;return 0;
        }
        rd_commit(held,out,&n,&result,pending);
    }
    while(n<RD_DECK_SIZE){
        int valid_package=(result.packages<RD_MAX_PACKAGES && n<=RD_DECK_SIZE-4 &&
          rd_find_package(p,recipe,match,context,held,catalog_max_id,pending));
        total=valid_package?RD_PACKAGE_WEIGHT:0;
        for(i=0;i<p->pool_count;i++)if(held[p->pool[i].id]<RD_COPY_CAP)
            total+=p->pool[i].weight;
        if(total<=0){stats->error=3;return 0;}
        roll=rng(total,rng_context);
        if(roll<0||roll>=total){stats->error=3;return 0;}
        if(valid_package && roll<RD_PACKAGE_WEIGHT){
            rd_commit(held,out,&n,&result,pending);
            continue;
        }
        if(valid_package)roll-=RD_PACKAGE_WEIGHT;
        selected=-1;
        for(i=0;i<p->pool_count;i++){
            weight=held[p->pool[i].id]<RD_COPY_CAP?p->pool[i].weight:0;
            if(roll<weight){selected=i;break;}
            roll-=weight;
        }
        if(selected<0){stats->error=3;return 0;}
        chosen=p->pool[selected].id;
        out[n++]=(u16)chosen;held[chosen]++;
    }
    for(i=RD_DECK_SIZE-1;i>0;i--){
        j=rng(i+1,rng_context);
        if(j<0||j>i){stats->error=3;return 0;}
        chosen=out[i];out[i]=out[j];out[j]=(u16)chosen;
    }
    *stats=result;
    return RD_DECK_SIZE;
}
