/* Optional, per-instance unlock of the existing Ritual controllers.
 * Resolved LP, generated cards, terrain changes and permanent actions are
 * never rolled back. Revocation retires only future/continuous eligibility. */
static int borrowed_effect_armed(int i)
{
    DuelCardRecord *r=&D_801A7AD8[i];
    switch(r->card_id) {
    case SHIELD_ID:return marks[i].ritual && marks[i].data==r->data;
    case YAMADRON_ID:return yamadron_marks[i].ritual && yamadron_marks[i].data==r->data;
    case HUNGRY_BURGER_ID:return hungry_marks[i].ritual && hungry_marks[i].data==r->data;
    case FIENDS_MIRROR_ID:return fiend_mirror_marks[i].ritual && fiend_mirror_marks[i].data==r->data;
    case JAVELIN_BEETLE_ID:return javelin_marks[i].ritual && javelin_marks[i].data==r->data;
    case GATE_GUARDIAN_ID:return gate_marks[i].ritual && gate_marks[i].data==r->data;
    case CRAB_TURTLE_ID:return crab_marks[i].ritual && crab_marks[i].data==r->data;
    case FORTRESS_WHALE_ID:return whale_marks[i].ritual && whale_marks[i].data==r->data;
    case GARMA_SWORD_ID:return garma_marks[i].ritual && garma_marks[i].data==r->data;
    case TRI_HORNED_ID:return tri_marks[i].ritual && tri_marks[i].data==r->data;
    case SERPENT_NIGHT_ID:return serpent_marks[i].ritual && serpent_marks[i].data==r->data;
    case PERFORMANCE_ID:return performance_marks[i].ritual && performance_marks[i].data==r->data;
    case CHAOS_ID:return chaos_marks[i].ritual && chaos_marks[i].data==r->data;
    case SUPER_WAR_LION_ID:return war_lion_marks[i].ritual && war_lion_marks[i].data==r->data;
    case SENGENJIN_ID:return sengenjin_marks[i].ritual && sengenjin_marks[i].data==r->data;
    case BLACK_LUSTER_SOLDIER_ID:return bls_marks[i].ritual && bls_marks[i].data==r->data;
    case BLUE_EYES_ULTIMATE_ID:return ultimate_marks[i].ritual && ultimate_marks[i].data==r->data;
    case COSMO_QUEEN_ID:return cosmo_marks[i].ritual && cosmo_marks[i].data==r->data;
    case DOKURORIDER_ID:return dokurorider_marks[i].ritual && dokurorider_marks[i].data==r->data;
    case CHAKRA_ID:return chakra_marks[i].ritual && chakra_marks[i].data==r->data;
    case ZERA_ID:return zera_marks[i].ritual && zera_marks[i].data==r->data;
    case SKULL_GUARDIAN_ID:return skull_marks[i].ritual && skull_marks[i].data==r->data;
    case MASK_ID:return limit_test_mask_armed(r->data);
    case PSYCHO_PUPPET_ID:return find_psycho_mark(r->data)!=0;
    default:return 0;
    }
}
static void borrowed_retire(int i,int id,void *data)
{
    if(D_801A7AD8[i].data!=data || D_801A7AD8[i].card_id!=id)return;
    switch(id) {
    case SHIELD_ID:marks[i].ritual=0;shield_live_mask&=~(1u<<i);break;
    case YAMADRON_ID:yamadron_marks[i].ritual=0;yamadron_live_mask&=~(1u<<i);break;
    case HUNGRY_BURGER_ID:hungry_marks[i].ritual=0;hungry_live_mask&=~(1u<<i);break;
    case FIENDS_MIRROR_ID:fiend_mirror_marks[i].ritual=0;fiend_mirror_live_mask&=~(1u<<i);break;
    case JAVELIN_BEETLE_ID:javelin_marks[i].ritual=0;javelin_live_mask&=~(1u<<i);break;
    case GATE_GUARDIAN_ID:gate_marks[i].ritual=0;gate_live_mask&=~(1u<<i);break;
    case CRAB_TURTLE_ID:crab_marks[i].ritual=0;crab_live_mask&=~(1u<<i);break;
    case FORTRESS_WHALE_ID:whale_marks[i].ritual=0;whale_live_mask&=~(1u<<i);break;
    case GARMA_SWORD_ID:garma_marks[i].ritual=0;garma_live_mask&=~(1u<<i);break;
    case TRI_HORNED_ID:tri_marks[i].ritual=0;tri_live_mask&=~(1u<<i);break;
    case SERPENT_NIGHT_ID:serpent_marks[i].ritual=0;serpent_live_mask&=~(1u<<i);break;
    case PERFORMANCE_ID:performance_marks[i].ritual=0;performance_live_mask&=~(1u<<i);break;
    case CHAOS_ID:chaos_marks[i].ritual=0;chaos_live_mask&=~(1u<<i);break;
    case SUPER_WAR_LION_ID:war_lion_marks[i].ritual=0;war_lion_live_mask&=~(1u<<i);break;
    case SENGENJIN_ID:sengenjin_marks[i].ritual=0;sengenjin_live_mask&=~(1u<<i);break;
    case BLACK_LUSTER_SOLDIER_ID:bls_marks[i].ritual=0;bls_live_mask&=~(1u<<i);break;
    case BLUE_EYES_ULTIMATE_ID:ultimate_marks[i].ritual=0;ultimate_live_mask&=~(1u<<i);break;
    case COSMO_QUEEN_ID:cosmo_marks[i].ritual=0;cosmo_live_mask&=~(1u<<i);break;
    case DOKURORIDER_ID:dokurorider_marks[i].ritual=0;dokurorider_live_mask&=~(1u<<i);break;
    case CHAKRA_ID:chakra_marks[i].ritual=0;chakra_live_mask&=~(1u<<i);break;
    case ZERA_ID:zera_marks[i].ritual=0;zera_live_mask&=~(1u<<i);break;
    case SKULL_GUARDIAN_ID:skull_marks[i].ritual=0;skull_live_mask&=~(1u<<i);break;
    case MASK_ID:mask_forget(data);break;
    case PSYCHO_PUPPET_ID: {
        PsychoMark *m=find_psycho_mark(data);
        if(m) {
            unsigned bit=1u<<(unsigned)(m-psycho_marks);
            m->ritual=0;m->pending_summon_trigger=0;
            if(psycho_pending_mask&bit){psycho_pending_mask&=~bit;if(psycho_pending_count)--psycho_pending_count;}
        }
        break;
    }
    }
    chakra_pending_mask&=~(1u<<i);
    bls_new_source_pending&=~(1u<<i);
    dokurorider_new_source_pending&=~(1u<<i);
}
static void borrowed_reconcile(void)
{
    int i,j;
    for(i=0;i<30;i++) {
        BorrowedOrigin *b=&borrowed_origin[i];
        if(!b->serial)continue;
        for(j=0;j<30;j++)if(monster_slot(j) && D_801A7AD8[j].data==b->data &&
                           D_801A7AD8[j].card_id==b->id)break;
        if(j==30){memset(b,0,sizeof(*b));continue;}
        if(!test_ignore_ritual_requirement && borrowed_lease(j)!=b->serial) {
            borrowed_retire(j,b->id,b->data);
            memset(b,0,sizeof(*b));
        }
    }
}
