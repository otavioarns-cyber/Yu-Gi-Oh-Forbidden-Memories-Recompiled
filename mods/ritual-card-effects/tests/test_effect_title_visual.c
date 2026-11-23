#include <assert.h>
#include <stdio.h>
#include <string.h>
#define DUEL_EFFECT_ENTRY_TOTAL 620
#define DUEL_EFFECT_ENTRY_FLAG_ACTIVE 0x80
/* Only the fields accessed by the isolated native-entry color helper. */
typedef struct {unsigned short code_00;short y_0E;unsigned char flags_11,field_16;} DuelEffectEntry;
/* minimal native channel object model to exercise the routing helper */
#define DUEL_EFFECT_CHANNEL_FLAG_ACTIVE 0x8000
#define DUEL_EFFECT_CHANNEL_COUNT 4
typedef struct DisplayObject { unsigned char field_67; } DisplayObject;
/* This test's channel struct includes the native routing fields. */
typedef struct {DuelEffectEntry *entry_head_24,*entry_end_20;DisplayObject *field_28;unsigned flags_34;} DuelEffectChannel;
DuelEffectChannel D_800EB0F8[DUEL_EFFECT_CHANNEL_COUNT];
#include "../effect_title_visual.h"

/* Regression: all four real viewer categories can be delivered in WHITE
 * outside a duel. Each is recolored individually; body and neighboring
 * category headings are never modified. Also test active vs consumed in duel. */
static unsigned short test_sjis(unsigned char ch)
{
    if(ch>='A' && ch<='Z')return (unsigned short)(0x8260+ch-'A');
    if(ch>='a' && ch<='z')return (unsigned short)(0x8281+ch-'a');
    if(ch==' ')return 0x8140;
    if(ch=='<')return 0x8183;
    if(ch=='>')return 0x8184;
    return ch;
}
static void test_deck_titles(void)
{
    static const char *names[]={"<Effect>","<Fusion Effect>","<Metalmorph Effect>","<Ritual Effect>"};
    static const unsigned char colors[]={5,7,4,2};
    DuelEffectEntry e[112];
    DuelEffectChannel channel={.entry_head_24=e,.entry_end_20=e+112};
    FmrTitleTint snap={0};
    int positions[4],lengths[4],i,j,pos=0,k;
    memset(e,0,sizeof(e));
    for(i=0;i<4;i++){
        positions[i]=pos;
        lengths[i]=(int)strlen(names[i]);
        for(j=0;j<lengths[i];j++,pos++){
            e[pos].code_00=test_sjis((unsigned char)names[i][j]);
            e[pos].y_0E=(short)(i*12);
            e[pos].flags_11=0x80;
            e[pos].field_16=0; /* the bug observed in Build Deck */
        }
        /* body text, also white; must remain white */
        for(j=0;j<4;j++,pos++){
            e[pos].code_00=test_sjis('b');
            e[pos].y_0E=(short)(i*12+6);
            e[pos].flags_11=0x80;
            e[pos].field_16=0;
        }
    }
    channel.entry_end_20=e+pos;
    for(k=0;k<4;k++){
        fmr_title_tint_named(&snap,&channel,names[k],colors[k],1);
        assert(snap.count==lengths[k]);
        for(i=0;i<4;i++)for(j=0;j<lengths[i];j++)
            assert(e[positions[i]+j].field_16==(i==k?colors[k]:0));
        for(i=0;i<4;i++)for(j=0;j<4;j++)
            assert(e[positions[i]+lengths[i]+j].field_16==0);
        fmr_title_restore(&snap);
        for(i=0;i<pos;i++)assert(e[i].field_16==0);
    }
    /* Already-colored render and consumed-effect state in a real duel. */
    for(j=0;j<lengths[2];j++)e[positions[2]+j].field_16=4;
    fmr_title_tint_named(&snap,&channel,names[2],4,0);
    for(j=0;j<lengths[2];j++)assert(e[positions[2]+j].field_16==0);
    fmr_title_restore(&snap);
    for(j=0;j<lengths[2];j++)assert(e[positions[2]+j].field_16==4);
    /* Uncolored ASCII also accepted (non-fullwidth font pathways). */
    for(j=0;j<lengths[3];j++)e[positions[3]+j].code_00=(unsigned short)names[3][j];
    fmr_title_tint_named(&snap,&channel,names[3],2,1);
    assert(snap.count==lengths[3]);
    for(j=0;j<lengths[3];j++)assert(e[positions[3]+j].field_16==2);
    fmr_title_restore(&snap);
}


static void test_field_channel_routing(void)
{
    /* The renderer uses obj->field_67, NOT ch->field_28. Here the old
     * heuristic would choose the WRONG channel; native must choose 2. */
    DisplayObject actual={2},other={1},invalid={4};
    DuelEffectEntry entries[24];
    DuelEffectEntry other_entries[24];
    DuelEffectChannel stale={0};
    FmrTitleTint snap={0};
    const char *name="<Fusion Effect>";
    int n=(int)strlen(name),i;
    memset(entries,0,sizeof(entries));
    memset(other_entries,0,sizeof(other_entries));
    memset(D_800EB0F8,0,sizeof(D_800EB0F8));
    stale.field_28=&actual;
    stale.flags_34=DUEL_EFFECT_CHANNEL_FLAG_ACTIVE;
    D_800EB0F8[1].field_28=&actual; /* Deliberately misleading! */
    D_800EB0F8[1].flags_34=DUEL_EFFECT_CHANNEL_FLAG_ACTIVE;
    D_800EB0F8[1].entry_head_24=other_entries;
    D_800EB0F8[1].entry_end_20=other_entries+n;
    D_800EB0F8[2].field_28=&other;
    D_800EB0F8[2].flags_34=0;  /* Not part of native draw routing. */
    D_800EB0F8[2].entry_head_24=entries;
    D_800EB0F8[2].entry_end_20=entries+n;
    for(i=0;i<n;i++){
        entries[i].code_00=test_sjis((unsigned char)name[i]);
        entries[i].flags_11=DUEL_EFFECT_ENTRY_FLAG_ACTIVE;
        entries[i].field_16=7;
        other_entries[i].field_16=7;
    }
    /* Both aliases are normalized to the same bracket by native drawing. */
    entries[0].code_00=0x8171;
    entries[n-1].code_00=0x8172;
    assert(fmr_title_draw_channel(&actual,&stale)==&D_800EB0F8[2]);
    fmr_title_tint_named(&snap,&D_800EB0F8[2],name,7,0);
    assert(snap.count==n);
    for(i=0;i<n;i++){
        assert(entries[i].field_16==0);
        assert(other_entries[i].field_16==7);
    }
    fmr_title_restore(&snap);
    for(i=0;i<n;i++)assert(entries[i].field_16==7);
    entries[0].code_00=0x8173;
    entries[n-1].code_00=0x8174;
    fmr_title_tint_named(&snap,&D_800EB0F8[2],name,7,0);
    assert(snap.count==n);
    fmr_title_restore(&snap);
    assert(fmr_title_draw_channel(&other,&stale)==&D_800EB0F8[1]);
    assert(fmr_title_draw_channel(&invalid,&stale)==0);
    D_800EB0F8[2].entry_end_20=0;
    assert(fmr_title_draw_channel(&actual,&stale)==0);
}

/* Runtime fallback: title glyph IDs may differ from the synthetic SJIS
 * string, even though its gold/blue/lilac color run is already present. */
static void test_color_run_fallback(void)
{
    DuelEffectEntry e[28];
    DuelEffectChannel ch={.entry_head_24=e,.entry_end_20=e+28};
    FmrTitleTint s={0};
    int i;
    memset(e,0,sizeof(e));
    for(i=0;i<15;i++) {
        e[i].code_00=(unsigned short)(0x9000+i); /* deliberately unknown */
        e[i].flags_11=DUEL_EFFECT_ENTRY_FLAG_ACTIVE;
        e[i].field_16=2;
        e[i].y_0E=3;
    }
    for(i=15;i<28;i++) {
        e[i].code_00=(unsigned short)(0x9100+i);
        e[i].flags_11=DUEL_EFFECT_ENTRY_FLAG_ACTIVE;
        e[i].field_16=0;
        e[i].y_0E=17;
    }
    fmr_title_tint_effect(&s,&ch,"<Ritual Effect>",2,0);
    assert(s.count==15);
    for(i=0;i<15;i++)assert(e[i].field_16==0);
    for(i=15;i<28;i++)assert(e[i].field_16==0);
    fmr_title_restore(&s);
    for(i=0;i<15;i++)assert(e[i].field_16==2);
    for(i=15;i<28;i++)assert(e[i].field_16==0);
    /* The same test for lilac Fusion Effect. */
    for(i=0;i<15;i++)e[i].field_16=7;
    fmr_title_tint_effect(&s,&ch,"<Fusion Effect>",7,0);
    assert(s.count==15);
    fmr_title_restore(&s);
    /* Fail-closed: a white heading doesn't become colorful in duel. */
    for(i=0;i<15;i++)e[i].field_16=0;
    fmr_title_tint_effect(&s,&ch,"<Fusion Effect>",7,0);
    assert(s.count==0);
}

static void run(void)
{
    DuelEffectEntry entries[62];
    DuelEffectChannel channel={.entry_head_24=entries,.entry_end_20=entries+62};
    FmrTitleTint tint={0};
    int i;
    memset(entries,0,sizeof(entries));
    for(i=0;i<15;i++) {
        entries[i].code_00=i==0?0x8183:0x8260;
        entries[i].y_0E=12;entries[i].flags_11=0x80;entries[i].field_16=2;
    }
    for(i=15;i<39;i++) {
        entries[i].code_00=0x8260;
        entries[i].y_0E=24;entries[i].flags_11=0x80;entries[i].field_16=0;
    }
    for(i=39;i<61;i++) {
        entries[i].code_00=i==39?0x8183:0x8260;
        entries[i].y_0E=36;entries[i].flags_11=0x80;entries[i].field_16=4;
    }
    fmr_title_tint(&tint,&channel,2,0);
    assert(tint.count==15);
    for(i=0;i<15;i++)assert(entries[i].field_16==0);
    for(i=15;i<39;i++)assert(entries[i].field_16==0);
    fmr_title_restore(&tint);
    for(i=0;i<15;i++)assert(entries[i].field_16==2);
    fmr_title_tint(&tint,&channel,4,1);
    assert(tint.count==22);
    for(i=39;i<61;i++)assert(entries[i].field_16==4);
    fmr_title_restore(&tint);
    for(i=39;i<61;i++)assert(entries[i].field_16==4);
    /* Fusion uses the independent, lilac-colored eighth native ramp.
       While inactive, its heading must be white like the description. */
    for(i=0;i<15;i++)entries[i].field_16=7;
    fmr_title_tint(&tint,&channel,7,0);
    assert(tint.count==15);
    for(i=0;i<15;i++)assert(entries[i].field_16==0);
    fmr_title_restore(&tint);
    for(i=0;i<15;i++)assert(entries[i].field_16==7);
    fmr_title_tint(&tint,&channel,7,1);
    assert(tint.count==15);
    for(i=0;i<15;i++)assert(entries[i].field_16==7);
    fmr_title_restore(&tint);
    /* Silver, blue and orange must also always preserve their active hue. */
    for(i=0;i<15;i++)entries[i].field_16=5;
    fmr_title_tint(&tint,&channel,5,1);
    for(i=0;i<15;i++)assert(entries[i].field_16==5);
    fmr_title_restore(&tint);
    puts("PASS: active silver/lilac/orange, inactive white, restore, untouched body");
}
/* Regression for Fusionist with TWO titles: success on the first one
 * must not suppress fallback for a second title with translated glyphs.
 * This was the Beta.29 bug observed on two cards in hand. */
static void test_two_titles_independent(void)
{
    DuelEffectEntry e[40];
    DuelEffectChannel ch={.entry_head_24=e,.entry_end_20=e+40};
    FmrTitleTint snap={0};
    const char *normal="<Effect>";
    int i;
    memset(e,0,sizeof(e));
    for(i=0;i<8;i++){
        e[i].code_00=test_sjis((unsigned char)normal[i]);
        e[i].field_16=5;e[i].flags_11=0x80;e[i].y_0E=0;
    }
    for(i=8;i<23;i++){
        e[i].code_00=(unsigned short)(0x9000+i);
        e[i].field_16=7;e[i].flags_11=0x80;e[i].y_0E=12;
    }
    for(i=23;i<40;i++){
        e[i].code_00=0x8260;e[i].field_16=0;
        e[i].flags_11=0x80;e[i].y_0E=24;
    }
    fmr_title_tint_effect(&snap,&ch,"<Effect>",5,1);
    assert(snap.count==8);
    fmr_title_tint_effect(&snap,&ch,"<Fusion Effect>",7,0);
    assert(snap.count==23);
    for(i=0;i<8;i++)assert(e[i].field_16==5);
    for(i=8;i<40;i++)assert(e[i].field_16==0);
    fmr_title_restore(&snap);
    for(i=0;i<8;i++)assert(e[i].field_16==5);
    for(i=8;i<23;i++)assert(e[i].field_16==7);
    for(i=23;i<40;i++)assert(e[i].field_16==0);
}

static void test_off_duel_white_fallback(void)
{
    DuelEffectEntry e[35];
    DuelEffectChannel ch={.entry_head_24=e,.entry_end_20=e+35};
    FmrTitleTint snap={0};
    int i;
    memset(e,0,sizeof(e));
    for(i=0;i<15;i++){
        e[i].code_00=(unsigned short)(0x9500+i);e[i].field_16=0;
        e[i].flags_11=0x80;e[i].y_0E=0;
    }
    for(i=15;i<35;i++){
        e[i].code_00=0x8260;e[i].field_16=0;
        e[i].flags_11=0x80;e[i].y_0E=12;
    }
    fmr_title_tint_effect(&snap,&ch,"<Ritual Effect>",2,1);
    assert(snap.count==0);
    fmr_title_tint_first_line(&snap,&ch,2);
    assert(snap.count==15);
    for(i=0;i<15;i++)assert(e[i].field_16==2);
    for(i=15;i<35;i++)assert(e[i].field_16==0);
    fmr_title_restore(&snap);
    for(i=0;i<35;i++)assert(e[i].field_16==0);
}

int main(void){run();test_deck_titles();test_field_channel_routing();test_color_run_fallback();test_two_titles_independent();test_off_duel_white_fallback();return 0;}
