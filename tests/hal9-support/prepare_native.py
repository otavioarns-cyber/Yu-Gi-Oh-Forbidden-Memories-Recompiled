"""Loader-only fixture binding: the resident COMMON D_8009B150 belongs to
pinned guest memory. Change this single ELF symbol to an import, preserving all
section bytes/relocations. Never write the official input or production source.
"""
from pathlib import Path
from elftools.elf.elffile import ELFFile
import argparse,json,hashlib,struct
p=argparse.ArgumentParser();p.add_argument('--engine',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();a.output.mkdir(exist_ok=False);name='src_game_func_80019608.c.o';sha=lambda b:hashlib.sha256(b).hexdigest()
for f in a.engine.glob('*.o'):
 if f.name!=name:(a.output/f.name).symlink_to(f.resolve())
f=a.engine/name;data=bytearray(f.read_bytes());old=bytes(data)
with f.open('rb') as h:
 e=ELFFile(h);s=e.get_section_by_name('.symtab');changed=[]
 for i,sym in enumerate(s.iter_symbols()):
  if sym['st_shndx']=='SHN_COMMON':
   assert sym.name=='D_8009B150';off=s['sh_offset']+i*s['sh_entsize'];struct.pack_into('<II',data,off+4,0,0);struct.pack_into('<H',data,off+14,0);changed.append(sym.name)
 assert changed==['D_8009B150']
 for sec in e.iter_sections():
  if sec.name!='.symtab':assert old[sec['sh_offset']:sec['sh_offset']+sec['sh_size']]==data[sec['sh_offset']:sec['sh_offset']+sec['sh_size']]
(a.output/name).write_bytes(data);(a.output/'BINDING.json').write_text(json.dumps(dict(original_sha256=sha(old),fixture_sha256=sha(data),changed=changed,scope=__doc__),indent=2)+'\n');print('PASS native function bytes/relocations unchanged; COMMON binding only')
