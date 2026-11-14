"""Check packaged artwork and production coin decoder; no game rendering."""
from pathlib import Path
import json,os,re,struct,subprocess,tempfile,zlib
from PIL import Image
mod=Path(__file__).resolve().parents[1]
manifest=json.loads((mod/'mod.json').read_text())
pngs=sorted((mod/'assets').rglob('*.png'))
for path in pngs:
    with Image.open(path) as image:
        image.load()
    data=path.read_bytes();assert data[:8]==b'\x89PNG\r\n\x1a\n';offset=8;ended=False
    while offset<len(data):
        size=struct.unpack_from('>I',data,offset)[0];tag=data[offset+4:offset+8]
        assert offset+12+size<=len(data)
        assert zlib.crc32(data[offset+4:offset+8+size])&0xffffffff==struct.unpack_from('>I',data,offset+8+size)[0]
        offset+=size+12
        if tag==b'IEND':ended=True;break
    assert ended and offset==len(data)
for row in manifest['card_types']['types']:assert (mod/row['icon']).is_file()
for row in manifest['cards']:
    description=row.get('description','');assert '\ufffd' not in description
    assert description.count('{')==description.count('}')
info=(mod/'assets/coin-normal/coin.txt').read_text();frames=int(re.search(r'frames=(\d+)',info)[1]);tails=int(re.search(r'tails=(\d+)',info)[1]);assert 0<tails<frames<=64
coins=[mod/'assets/coin-normal'/f'{n:02d}.png' for n in range(frames)];assert all(p.is_file() for p in coins)
fixture=r'''
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "png.h"
int main(int argc,char**argv){
 for(int i=1;i<argc;i++){
  FILE*f=fopen(argv[i],"rb");assert(f);fseek(f,0,SEEK_END);long n=ftell(f);rewind(f);
  unsigned char*data=malloc(n);assert(data&&fread(data,1,n,f)==(size_t)n);fclose(f);
  PngImage image={0};assert(Png_Decode(data,n,&image));assert(image.width>0&&image.height>0&&image.pixels);
  free(image.pixels);free(data);
 }printf("PASS production PNG decoder: %d coin frames, ASan/UBSan\n",argc-1);
}
'''
with tempfile.TemporaryDirectory() as temp:
    temp=Path(temp);(temp/'test.c').write_text(fixture);exe=temp/'test'
    subprocess.run(['gcc','-std=c11','-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-I'+str(mod),temp/'test.c',mod/'png.c','-o',exe],check=True)
    subprocess.run([exe,*coins],check=True,env={**os.environ,'ASAN_OPTIONS':'detect_leaks=0'})
print(f'PASS assets: {len(pngs)} PNGs, {frames} runtime coin frames, six custom icons, {len(manifest["cards"])} card entries')
