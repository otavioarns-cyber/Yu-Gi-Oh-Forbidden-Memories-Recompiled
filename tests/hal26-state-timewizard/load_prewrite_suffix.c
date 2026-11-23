#undef fseek
#undef fread
#undef malloc
#undef swapcontext
int main(int argc,char**argv){
 assert(argc==2);char file[4096];snprintf(file,sizeof file,"%s/probe.state",argv[1]);unsigned checks=0;
 for(unsigned profile=0;profile<16;profile++)for(unsigned corrupt=0;corrupt<4;corrupt++){
  unsigned char bytes[64]={0};memcpy(bytes,"YFMSTATE",8);bytes[8]=corrupt==0?3:corrupt==1?2:255;bytes[12]=corrupt==2?99:1;if(corrupt==3)memset(bytes,0x99,sizeof bytes);
  FILE*f=fopen(file,"wb");assert(f&&fwrite(bytes,1,sizeof bytes,f)==sizeof bytes);fclose(f);
  active=profile<8?1u<<profile:0;resident=profile>=8?1u<<(profile-8):0;
  seeks=reads=allocs=parses=contexts=0;reason[0]=0;assert(load(file)==-1&&tower_refused);
  assert(!seeks&&!reads&&!allocs&&!parses&&!contexts&&!pending_image&&!pending_size);
  assert(strstr(reason,"current game was kept")&&strstr(reason,"memory-card"));
  unsigned char after[64];f=fopen(file,"rb");assert(f&&fread(after,1,sizeof after,f)==sizeof after);fclose(f);assert(!memcmp(after,bytes,sizeof bytes));++checks;
 }
 /* Cold OFF reaches legacy validation, which rejects this corrupt file.
    It does not demonstrate support for an old or successful savestate. */
 active=resident=0;seeks=reads=allocs=parses=0;assert(load(file)==-1&&!tower_refused&&seeks&&reads&&allocs&&!pending_image);++checks;
 printf("PASS %u literal native load refusal/legacy-path cases; zero read/parse/overwrite for protected profiles, input unchanged. No success restore/memory-card gameplay claimed.\n",checks);remove(file);return 0;
}
