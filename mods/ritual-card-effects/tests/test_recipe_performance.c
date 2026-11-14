/* Appended to the original provider fixture by test_recipe_performance.py.
 * The tested descriptors, matcher and provider come from each actual build. */
#include <stdint.h>
#include <time.h>
#include <limits.h>

static uint64_t digest=1469598103934665603ULL;
static unsigned checks;
static void record_bytes(const void *p,size_t n)
{
    const unsigned char *b=p;
    while(n--){digest^=*b++;digest*=1099511628211ULL;}
    ++checks;
}
static void record_int(int value){record_bytes(&value,sizeof(value));}
static uint32_t random_state=0x183975ac;
static unsigned next_random(void)
{
    random_state^=random_state<<13;random_state^=random_state>>17;
    random_state^=random_state<<5;return random_state;
}
static void catalog(unsigned world)
{
    unsigned id;
    for(id=1;id<=800;++id){
        unsigned atk=next_random()%401,def=next_random()%401;
        unsigned type=next_random()%(CARD_TYPE_MAGIC+2);
        gDuel_adwCardStats[id-1]=(int)(atk|(def<<CARD_STAT_DEFENSE_SHIFT)|
            (type<<CARD_STAT_TYPE_SHIFT));
        test_types[id]=(int)type;test_levels[id]=(int)(next_random()%13);
        test_base[id]=(world%3==0 && id>722)?(int)(next_random()%722+1):0;
        test_fusion[id]=(int)(next_random()&1);
    }
}
int main(int argc,char **argv)
{
    const int invalid[]={-1,0,801,INT_MIN,INT_MAX};
    RitualRecipeRequirementV1 roles[4];
    unsigned world,i,role;int id,result,n;
    (void)argv;
    for(world=0;world<24;++world){
        catalog(world);recipe_export_active=(world!=23);
        for(i=0;i<18;++i)recipe_settings[i]=(world%4!=1)||(i%2);
        for(i=0;i<18;++i){
            int ritual=ritual_options[i].ritual_id;
            memset(roles,0xA5,sizeof(roles));result=-1234;
            n=recipe_export_get(ritual,roles,3,&result);
            record_int(n);record_int(result);record_bytes(roles,sizeof(roles));
            for(role=0;role<5;++role){
                for(id=0;id<=801;++id)record_int(recipe_export_matches(ritual,role,id));
                for(unsigned k=0;k<sizeof(invalid)/sizeof(invalid[0]);++k)
                    record_int(recipe_export_matches(ritual,role,invalid[k]));
            }
            for(unsigned cap=0;cap<3;++cap){
                memset(roles,0xA5,sizeof(roles));result=-1234;
                record_int(recipe_export_get(ritual,roles,cap,&result));
                record_int(result);record_bytes(roles,sizeof(roles));
            }
            record_int(recipe_export_get(ritual,0,3,&result));
            record_int(recipe_export_get(ritual,roles,3,0));
        }
        /* Raw getters also serve the existing Field/Hand/Deck oracle. */
        for(id=-1;id<=801;++id){
            TablesRitualRequirement raw[3];unsigned short row[6],card=0xBEEF;
            memset(raw,0xA5,sizeof(raw));memset(row,0xA5,sizeof(row));
            record_int(compat_private_requirements(id,raw,&card));
            record_bytes(raw,sizeof(raw));record_bytes(&card,sizeof(card));
            record_int(compat_private_recipe(id,row));record_bytes(row,sizeof(row));
            record_int(recipe_export_get(id,roles,3,&result));
            record_int(recipe_export_matches(id,0,1));
        }
    }
    printf("RESULT checks=%u digest=%016llx\n",checks,(unsigned long long)digest);
    if(argc>1){
        volatile unsigned sink=0;
        const unsigned iterations=3000000;
        recipe_export_active=1;for(i=0;i<18;++i)recipe_settings[i]=1;
        catalog(2);
        clock_t start=clock();
        for(i=0;i<iterations;++i){
            int ritual=ritual_options[i%18].ritual_id;
            sink+=(unsigned)recipe_export_matches(ritual,(i/18)%3,(int)(i%800+1));
        }
        double ns=(double)(clock()-start)*1e9/CLOCKS_PER_SEC/iterations;
        printf("BENCH ns/query=%.3f sink=%u\n",ns,sink);
    }
    return 0;
}
