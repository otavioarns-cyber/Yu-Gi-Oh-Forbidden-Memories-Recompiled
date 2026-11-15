"""Validate all packaged PNGs/WAVs/text references, without game/disc data.

Runs the production decoder on runtime coin PNGs with ASan/UBSan.
Pillow decodes all image references, including sources larger than the runtime decoder limit.
Does not render or play audio. Requires Pillow.
"""
from pathlib import Path
import json, os, re, struct, subprocess, tempfile, wave, zlib
from PIL import Image

mod = Path(__file__).resolve().parents[1]
manifest = json.loads((mod / 'mod.json').read_text())
pngs = sorted((mod / 'assets').rglob('*.png'))
for path in pngs:
    with Image.open(path) as image:
        image.load()
    data = path.read_bytes()
    assert data[:8] == b'\x89PNG\r\n\x1a\n', path
    offset = 8
    ended = False
    while offset < len(data):
        length = struct.unpack_from('>I', data, offset)[0]
        tag = data[offset + 4:offset + 8]
        chunk = data[offset + 8:offset + 8 + length]
        assert offset + 12 + length <= len(data), path
        expected = struct.unpack_from('>I', data, offset + 8 + length)[0]
        assert zlib.crc32(tag + chunk) & 0xffffffff == expected, path
        offset += 12 + length
        if tag == b'IEND':
            ended = True
            break
    assert ended and offset == len(data), path
for folder in ['coin-normal', 'coin-atem']:
    info = (mod / 'assets' / folder / 'coin.txt').read_text()
    frames = int(re.search(r'frames=(\d+)', info)[1])
    tails = int(re.search(r'tails=(\d+)', info)[1])
    assert 0 < tails < frames <= 64
    assert all((mod / 'assets' / folder / f'{n:02d}.png').is_file() for n in range(frames))
audio = []
for path in sorted((mod / 'assets').rglob('*.wav')):
    with wave.open(str(path)) as wav:
        frames = wav.readframes(wav.getnframes())
        assert len(frames) == wav.getnframes() * wav.getnchannels() * wav.getsampwidth()
        audio.append((path.name, wav.getnchannels(), wav.getsampwidth(), wav.getframerate(), wav.getnframes()))
for entry in manifest['text']:
    data = (mod / entry['file']).read_text(encoding='utf-8')
    assert data.startswith('@bank descriptions') and re.search(r'^\[[0-9A-Fa-f]+\]', data, re.M)
    assert '\ufffd' not in data
    assert data.count('{') == data.count('}')
for entry in manifest['audio']['sfx'].values():
    assert (mod / entry['file']).is_file()
fixture = r'''
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "png.h"
int main(int argc, char**argv) {
    for(int i=1;i<argc;i++) {
        FILE*f=fopen(argv[i],"rb");assert(f);fseek(f,0,SEEK_END);long n=ftell(f);rewind(f);
        unsigned char*data=malloc(n);assert(data&&fread(data,1,n,f)==(size_t)n);fclose(f);
        PngImage image={0};assert(Png_Decode(data,n,&image));
        assert(image.width>0&&image.height>0&&image.pixels);free(image.pixels);free(data);
    }
    printf("PASS production PNG decoder: %d images (ASan/UBSan, no rendering)\n",argc-1);
}
'''
with tempfile.TemporaryDirectory() as temp:
    temp = Path(temp)
    (temp / 'test.c').write_text(fixture)
    exe = temp / 'test'
    subprocess.run(['gcc', '-std=c11', '-O1', '-g', '-fsanitize=address,undefined',
                    '-fno-omit-frame-pointer', '-I' + str(mod), temp / 'test.c',
                    mod / 'png.c', '-o', exe], check=True)
    runtime_pngs = [p for p in pngs if p.parent.name in ("coin-normal", "coin-atem")]
    subprocess.run([exe, *runtime_pngs], check=True, env={**os.environ, 'ASAN_OPTIONS': 'detect_leaks=0'})
print(f'PASS asset integrity: {len(pngs)} PNGs, {len(audio)} WAVs, {len(manifest["text"])} texts, both coin sequences')
print('WAV (name, channels, bytes/sample, Hz, frames):', audio)
