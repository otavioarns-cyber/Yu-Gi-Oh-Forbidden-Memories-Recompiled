"""Compare immutable core boundaries without claiming a full source rebuild/ABI."""
from pathlib import Path
import argparse,hashlib,json,collections
from elftools.elf.elffile import ELFFile
p=argparse.ArgumentParser();p.add_argument('--core',type=Path,required=True);p.add_argument('--relinked',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args()
def read(path):
 with path.open('rb') as f:
  e=ELFFile(f);secs=list(e.iter_sections());sym=e.get_section_by_name('.symtab');rel=collections.defaultdict(list)
  for s in secs:
   if s['sh_type'] in ('SHT_REL','SHT_RELA'):
    for r in s.iter_relocations():rel[s['sh_info']].append((r['r_offset'],r['r_info_type']))
  out=[]
  for s in sym.iter_symbols():
   i=s['st_shndx']
   if s['st_info']['type']!='STT_FUNC' or not isinstance(i,int) or not s['st_size']:continue
   start=s['st_value'];end=start+s['st_size'];raw=secs[i].data()[start:end];masked=bytearray(raw)
   for at,kind in rel[i]:
    if start<=at<end and kind in (1,2):masked[at-start:at-start+4]=b'\0'*4
   out.append(dict(name=s.name,section=secs[i].name,offset=start,bytes=len(raw),raw_sha256=hashlib.sha256(raw).hexdigest(),relocation_masked_sha256=hashlib.sha256(masked).hexdigest()))
  return out

old=read(a.core);new=read(a.relinked);rows=[]
for f in old:
 candidates=[x for x in new if x['section']==f['section'] and x['offset']==f['offset'] and x['bytes']==f['bytes']];same=[x for x in candidates if x['relocation_masked_sha256']==f['relocation_masked_sha256']];rows.append(dict(core=f,matched_names=[x['name'] for x in same],status='PASS' if same else 'DIFFERENT'))
a.output.write_text(json.dumps(dict(scope='Original core functions remain in the relinked prefix. Relocation operands are masked for comparison; bindings/runtime/whole source/ABI equivalence are NOT established by this check.',core_sha256=hashlib.sha256(a.core.read_bytes()).hexdigest(),relinked_sha256=hashlib.sha256(a.relinked.read_bytes()).hexdigest(),rows=rows),indent=2)+'\n');print('Core prefix masked comparison',collections.Counter(x['status'] for x in rows))
