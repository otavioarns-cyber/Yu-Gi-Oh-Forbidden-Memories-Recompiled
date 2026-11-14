"""Integrity and bounds, not a rendering or artistic-equivalence test."""
from pathlib import Path
import re, struct, zlib
from PIL import Image
root=Path(__file__).resolve().parents[1]
images=list((root/'assets').glob('*.png'))
assert len(images)==38
for path in images:
    data=path.read_bytes(); assert data[:8]==b'\x89PNG\r\n\x1a\n'
    pos=8
    while pos<len(data):
        size=struct.unpack_from('>I',data,pos)[0]
        chunk=data[pos+4:pos+8+size]
        assert zlib.crc32(chunk)&0xffffffff==struct.unpack_from('>I',data,pos+8+size)[0],path
        pos+=size+12
    assert pos==len(data)
    with Image.open(path) as im:
        im.load(); assert im.mode=='RGBA'
        i=int(path.name.split('-')[2].split('.')[0])
        expected=(1024,1024) if i in (16,17) else (512,512) if i==18 else (512,1024)
        if '-2x' in path.name: expected=tuple(v//2 for v in expected)
        assert im.size==expected,(path,im.size)
bins=list((root/'assets').glob('*.bin'));assert len(bins)==19
for i in range(19):
    w,h=(80,128) if i<16 else (160,240) if i<18 else (160,200)
    assert (root/f'assets/hd-layer-{i:02}.bin').stat().st_size==w*h*2
native=(root/'native_assets.h').read_text()
for name,count in [('native_background',320*240),('native_letters',128*96),('native_frame',30*21)]:
    content=re.search(name+r'[^=]*=\{(.*?)\};',native,re.S).group(1)
    values=list(map(int,re.findall(r'\d+',content)))
    assert len(values)==count and all(0<=v<=65535 for v in values),name
box=(root/'box_art.h').read_text();layers=re.findall(r'static const BoxSpan box_\d+\[\]=\{(.*?)\};',box,re.S)
assert len(layers)==16
count=0
for layer in layers:
    for x,y,n,a,rgb in re.findall(r'\{(\d+),(\d+),(\d+),(\d+),(0x[0-9a-f]+)\}',layer):
        x,y,n,a=map(int,(x,y,n,a)); assert 0<=x<x+n<=160 and 0<=y<300 and 0<=a<=255 and int(rgb,16)<=0xffffff
        count+=1
print(f'PASS: 38 PNG CRC/decode/dimensions, 19 binary buffers, three native arrays, 16 box layers / {count} bounded spans')
