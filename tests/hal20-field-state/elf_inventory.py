"""Static ELF32 function fingerprints; never emit binary bytes/disassembly.
Artifact SHA and function bytes/relocations supplement runtime pointer binding.
Names/IDs are labels, not the qualification criterion.
"""
from pathlib import Path
import struct,hashlib

def elf_info(path):
 path=Path(path);d=path.read_bytes();assert d[:6]==b'\x7fELF\x01\x01';h=struct.unpack_from('<16sHHIIIIIHHHHHH',d);assert h[2]==3
 secs=[struct.unpack_from('<IIIIIIIIII',d,h[6]+i*h[11]) for i in range(h[12])];symbols={};functions=[]
 for si,s in enumerate(secs):
  if s[1]!=2:continue
  st=secs[s[6]];names=d[st[4]:st[4]+st[5]];lst=[]
  for at in range(s[4],s[4]+s[5],s[9]):
   n,value,size,info,other,section=struct.unpack_from('<IIIBBH',d,at);name=names[n:].split(b'\0')[0].decode(errors='replace');lst.append(name)
   if info&15==2 and section and section<len(secs) and size:
    raw=d[secs[section][4]+value:secs[section][4]+value+size]
    functions.append(dict(name=name,section=section,offset=value,bytes=size,body_sha256=hashlib.sha256(raw).hexdigest(),relocations=[]))
  symbols[si]=lst
 for rel in secs:
  if rel[1] not in (9,4):continue
  for at in range(rel[4],rel[4]+rel[5],rel[9]):
   offset,info=struct.unpack_from('<II',d,at);sym=info>>8;kind=info&255;names=symbols.get(rel[6],[])
   for f in functions:
    if f['section']==rel[7] and f['offset']<=offset<f['offset']+f['bytes']:f['relocations'].append([offset-f['offset'],kind,names[sym] if sym<len(names) else str(sym)])
 for f in functions:
  import json
  f['relocations_sha256']=hashlib.sha256(json.dumps(f['relocations'],separators=(',',':')).encode()).hexdigest()
 return dict(sha256=hashlib.sha256(d).hexdigest(),bytes=len(d),functions=functions)
