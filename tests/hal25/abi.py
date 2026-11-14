"""Compare i386 ELF public ABI/imports with PR13; private symbol additions allowed."""
from pathlib import Path
import argparse,json,struct,hashlib
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--original',type=Path,required=True);p.add_argument('--candidate',type=Path,action='append',required=True);p.add_argument('--sdk-exports',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args()
def info(f):
 d=f.read_bytes();assert d[:6]==b'\x7fELF\x01\x01';h=struct.unpack_from('<16sHHIIIIIHHHHHH',d);assert h[1:3]==(1,3) # REL, i386
 sec=[struct.unpack_from('<IIIIIIIIII',d,h[6]+i*h[11]) for i in range(h[12])];imp=[];pub=[];api='runtime init checked by run.py'
 for s in sec:
  if s[1]!=2:continue
  ss=sec[s[6]];st=d[ss[4]:ss[4]+ss[5]]
  for at in range(s[4],s[4]+s[5],s[9]):
   name,value,size,bind,other,index=struct.unpack_from('<IIIBBH',d,at);name=st[name:].split(b'\0')[0].decode()
   if not name:continue
   if not index and bind>>4:imp.append(name)
   elif bind>>4 in (1,2):pub.append(name)
   if name=='MemoriesModApiVersion':api=struct.unpack_from('<I',d,sec[index][4]+value)[0]
 return dict(path=str(f),sha256=hashlib.sha256(d).hexdigest(),bytes=len(d),format='ELF32 little-endian REL i386 (SDK Windows MOD format)',api=api,imports=sorted(imp),exports=sorted(pub))
original=info(a.original);allow=set(a.sdk_exports.read_text().split());rows=[]
for f in a.candidate:
 c=info(f);checks=dict(public_exports_unchanged=c['exports']==original['exports'],imports_unchanged=c['imports']==original['imports'],imports_in_SDK=not(set(c['imports'])-allow));rows.append(dict(candidate=c,checks=checks))
a.output.write_text(json.dumps(dict(original=original,candidates=rows),indent=2)+'\n');print([(r['candidate']['sha256'],r['checks']) for r in rows]);raise SystemExit(any(not all(r['checks'].values()) for r in rows))
