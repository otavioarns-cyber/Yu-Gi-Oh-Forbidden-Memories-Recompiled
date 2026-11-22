#ifndef FMR_EFFECT_TITLE_VISUAL_H
#define FMR_EFFECT_TITLE_VISUAL_H
/* Native viewer palette, no custom font or new page: dim ONLY a complete
 * color-delimited title run. The source description and body are untouched.
 * A snapshot is restored immediately after the original renderer returns.
 * Color commands: 0=white (inactive), 2=blue, 4=silver, 5=orange, 7=custom fusion lilac. */
#define FMR_TITLE_MAX 96
typedef struct {
    DuelEffectEntry *entry[FMR_TITLE_MAX];
    unsigned char old[FMR_TITLE_MAX];
    int count;
} FmrTitleTint;
static void fmr_title_tint(FmrTitleTint *snap,DuelEffectChannel *box,
                           int original_color,int highlighted)
{
    DuelEffectEntry *e,*end;
    int wanted=highlighted?original_color:0;
    if(!snap || !box || !box->entry_head_24 || !box->entry_end_20)return;
    e=box->entry_head_24;end=box->entry_end_20;
    if(end<=e || end-e>DUEL_EFFECT_ENTRY_TOTAL)return;
    while(e<end && snap->count<FMR_TITLE_MAX) {
        DuelEffectEntry *first=e,*last=e;
        int y=e->y_0E,c=0,opened=0;
        if(!(e->flags_11&DUEL_EFFECT_ENTRY_FLAG_ACTIVE) ||
           e->field_16!=original_color){++e;continue;}
        while(last<end && (last->flags_11&DUEL_EFFECT_ENTRY_FLAG_ACTIVE) &&
              last->y_0E==y && last->field_16==original_color){
            if(last->code_00==0x8183 || last->code_00==0x003c)opened=1;
            ++c;++last;
        }
        /* Title lines in these mods use one long colored run, unlike the
         * highlighted +/- numbers inside description text.  Bracket glyph
         * is preferred but some glyph translations encode it differently. */
        if(c>=8 && (opened || c>=12)) {
            for(;first<last && snap->count<FMR_TITLE_MAX;++first) {
                snap->entry[snap->count]=first;
                snap->old[snap->count]=first->field_16;
                ++snap->count;
                first->field_16=(unsigned char)wanted;
            }
        }
        e=last;
    }
}
/* Backup matcher for the native card panel: translated text sometimes
 * splits or aliases the glyph stream, so an exact character-by-character
 * title match cannot be the only path for disabling a colored heading.
 * This scans a single native-color run (same Y, 8+ glyphs, preferably
 * beginning with the left bracket) and never changes white body text.
 * Only call on a *known* effect title category and only if the exact
 * heading match found no glyphs. */
static void fmr_title_tint_color_run(FmrTitleTint *snap,DuelEffectChannel *box,
                                     int native_color,int active)
{
    if(snap && snap->count==0 && !active)
        fmr_title_tint(snap,box,native_color,0);
}

static void fmr_title_restore(FmrTitleTint *snap)
{
    while(snap && snap->count>0) {
        --snap->count;
        snap->entry[snap->count]->field_16=snap->old[snap->count];
    }
}

/* In the Build Deck / Library viewer, a text channel may hand us a white
 * title even though the card description contains its native color command.
 * Match the WHOLE requested heading (instead of assuming the source color)
 * so that off-duel previews always use the category color, and other headings
 * and body lines remain completely untouched. Native fullwidth Shift-JIS and
 * literal ASCII glyph codes are accepted. */
static int fmr_heading_glyph(unsigned short code,unsigned char ascii)
{
    unsigned short sjis=0;
    if(ascii>='A' && ascii<='Z')sjis=(unsigned short)(0x8260+ascii-'A');
    else if(ascii>='a' && ascii<='z')sjis=(unsigned short)(0x8281+ascii-'a');
    else if(ascii==' ')sjis=0x8140;
    else if(ascii=='<')sjis=0x8183;
    else if(ascii=='>')sjis=0x8184;
    /* Retail text can encode the visible angle brackets four ways. The
     * native renderer canonicalizes 0x8171/0x8173 -> '<' and
     * 0x8172/0x8174 -> '>' immediately before drawing. Match the same
     * spellings, otherwise a correctly colored heading is never found. */
    if(ascii=='<' && (code==0x8171 || code==0x8173))return 1;
    if(ascii=='>' && (code==0x8172 || code==0x8174))return 1;
    return code==ascii || (sjis && code==sjis);
}
/* IMPORTANT: Exactly match the native draw callback func_80035E20
 * (v0.3.1-preview.1):
 *     D_800EB0F8[obj->field_67].entry_head_24
 * The prior versions looked for channels whose field_28==obj, but the
 * renderer does NOT use that relationship to choose a text channel. It
 * can be a different DisplayObject, including the card-information pane.
 * Never depend on the global selected-viewer pointer or channel ACTIVE flag
 * when choosing a channel for a currently DRAWING object. */
static DuelEffectChannel *fmr_title_draw_channel(DisplayObject *obj,
                                                  DuelEffectChannel *preferred)
{
    DuelEffectChannel *box;
    unsigned n;
    (void)preferred;
    if(!obj)return 0;
    n=(unsigned)obj->field_67;
    if(n>=DUEL_EFFECT_CHANNEL_COUNT)return 0;
    box=&D_800EB0F8[n];
    if(!box->entry_head_24 || !box->entry_end_20)return 0;
    return box;
}
static void fmr_title_tint_named(FmrTitleTint *snap,DuelEffectChannel *box,
                                 const char *heading,int color,int active)
{
    DuelEffectEntry *e,*end;
    int len=0;
    if(!snap || !box || !heading || !box->entry_head_24 || !box->entry_end_20)return;
    while(heading[len] && len<32)++len;
    if(len<8 || len>=32 || heading[0]!='<' || heading[len-1]!='>')return;
    e=box->entry_head_24;end=box->entry_end_20;
    if(end<=e || end-e>DUEL_EFFECT_ENTRY_TOTAL)return;
    for(;e+len<=end;++e){
        int j,ok=1;
        for(j=0;j<len;++j){
            DuelEffectEntry *g=e+j;
            if(!(g->flags_11&DUEL_EFFECT_ENTRY_FLAG_ACTIVE) ||
               g->y_0E!=e->y_0E ||
               !fmr_heading_glyph(g->code_00,(unsigned char)heading[j])){
                ok=0;break;
            }
        }
        if(!ok)continue;
        /* Color only this exact heading; never the next text run. */
        for(j=0;j<len && snap->count<FMR_TITLE_MAX;++j){
            DuelEffectEntry *g=e+j;
            snap->entry[snap->count]=g;
            snap->old[snap->count]=g->field_16;
            ++snap->count;
            g->field_16=(unsigned char)(active?color:0);
        }
        e+=len-1;
    }
}

/* Exact named match first; gracefully fall back to the color-delimited
 * heading, since real in-duel descriptions need not use the mock glyph
 * codes generated by the unit tests. */
/* For a card with a single effect category, the description starts with
 * its heading. Some Build Deck language/font paths do not publish the
 * heading in the same Shift-JIS form as the duel screen. If the exact
 * spelling does not match, allow re-coloring the FIRST description line
 * ONLY outside the duel, never a later body line. */
static void fmr_title_tint_effect(FmrTitleTint *snap,DuelEffectChannel *box,
                                  const char *name,int color,int active)
{
    int before=snap?snap->count:0;
    fmr_title_tint_named(snap,box,name,color,active);
    /* Each title is an independent match: when a card has BOTH <Effect>
     * and <Fusion Effect>, successful matching of the first must NOT
     * suppress the fallback for the second. Beta.29 shared snap->count
     * across categories and accidentally did exactly that. */
    if(snap && snap->count==before && !active)
        fmr_title_tint(snap,box,color,0);
}

#endif
