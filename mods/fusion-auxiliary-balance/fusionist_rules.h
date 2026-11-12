#ifndef AUX_FUSIONIST_RULES_H
#define AUX_FUSIONIST_RULES_H
/* Engine-independent rules. The caller supplies the CURRENT recipe resolver,
 * original stats and a uniform bounded draw. No recipes or partners live here. */
#define AUX_FUSIONIST 189
#define AUX_EGOTIST 318
typedef struct {
    int id, fused, metal, egotist, created, unlock_new;
    int real_a, real_b, virtual_material;
} AuxChainCard;
typedef struct {
    int count;
    int (*recipe)(int,int);
    int (*monster)(int);
    int (*stat)(int,int);
    unsigned (*draw)(unsigned);
} AuxFusionCatalog;
static int aux_fusionist_pick(const AuxFusionCatalog *c,int partner,int strong,
                              unsigned char *seen,int *virtual_material)
{
    int material,order,result,best=0,atk=0,def=0;
    unsigned ties=0;
    for(material=0;material<=c->count;material++)seen[material]=0;
    *virtual_material=0;
    for(material=1;material<=c->count;material++) for(order=0;order<2;order++) {
        int a,d,better;
        /* Recipe callback bypasses the wildcard itself. Query both orders
         * because a code mod can expose an ordered recipe. */
        result=order?c->recipe(material,partner):c->recipe(partner,material);
        if(result<=0 || result>c->count || !c->monster(result) || seen[result])continue;
        seen[result]=1;
        a=c->stat(result,0);d=c->stat(result,1);
        better=!best || (strong?(a>atk || (a==atk&&d>def)):
                                (a<atk || (a==atk&&d<def)));
        if(better) {best=result;atk=a;def=d;ties=1;*virtual_material=material;}
        else if(a==atk&&d==def) {
            ++ties;
            if(c->draw(ties)==0){best=result;*virtual_material=material;}
        }
    }
    return best;
}
static AuxChainCard aux_chain_fused(int a,int b,int result,int virtual_material,
                                   int metal_family)
{
    AuxChainCard s={0};
    s.id=result;s.fused=result>0 && a!=658 && b!=658 && a!=AUX_EGOTIST && b!=AUX_EGOTIST;
    s.created=result>0;s.real_a=a;s.real_b=b;
    s.virtual_material=virtual_material;
    /* A new monster never inherits the old monster's borrowed unlock. */
    s.metal=result>0 && metal_family && (a==658 || b==658);
    return s;
}
#endif
