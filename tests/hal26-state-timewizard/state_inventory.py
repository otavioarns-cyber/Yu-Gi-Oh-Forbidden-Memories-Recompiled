"""Conservative metadata inventory of every writable ELF object symbol.
No object bytes/private source/BSS snapshot emitted. Pointer-bearing types are
not serialization permission; absence of DWARF or pointers is not coverage.
"""
from pathlib import Path
import argparse,json,csv,hashlib,collections
from elftools.elf.elffile import ELFFile
p=argparse.ArgumentParser();p.add_argument('--dual',type=Path,required=True);p.add_argument('--timewizard',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True);root=Path(__file__).resolve().parents[2]
mods={k:root/'mods'/v for k,v in [('M','melhoria-dos-rituais'),('R','ritual-card-effects'),('RD','ritual-duelists'),('F','fusion-auxiliary-balance'),('E','field-effects-secondary-types'),('H','ai-hard-mode')]};mods.update(D=a.dual,TW=a.timewizard);rows=[];artifacts=[]
def name(d):
 x=d.attributes.get('DW_AT_name');return x.value.decode(errors='replace') if x and isinstance(x.value,bytes) else ''
def pointer(d,seen=None):
 if d is None:return None
 seen=set() if seen is None else seen
 if d.offset in seen:return False
 seen.add(d.offset)
 if d.tag in ('DW_TAG_pointer_type','DW_TAG_reference_type','DW_TAG_rvalue_reference_type'):return True
 if 'DW_AT_type' in d.attributes:return pointer(d.get_DIE_from_attribute('DW_AT_type'),seen)
 if d.tag in ('DW_TAG_structure_type','DW_TAG_union_type','DW_TAG_class_type'):
  children=[pointer(x.get_DIE_from_attribute('DW_AT_type'),seen.copy()) for x in d.iter_children() if 'DW_AT_type' in x.attributes];return True if True in children else None if None in children else False
 return False
for role,folder in mods.items():
 manifest=json.loads((folder/'mod.json').read_text());f=folder/(manifest['library']+'.o');digest=hashlib.sha256(f.read_bytes()).hexdigest();types={}
 with f.open('rb') as h:
  e=ELFFile(h)
  if e.has_dwarf_info():
   for cu in e.get_dwarf_info().iter_CUs():
    for d in cu.iter_DIEs():
     if d.tag=='DW_TAG_variable' and name(d) and 'DW_AT_type' in d.attributes:
      t=d.get_DIE_from_attribute('DW_AT_type');types[name(d)]=dict(type=name(t) or t.tag,pointer_bearing=pointer(t))
  symbols=e.get_section_by_name('.symtab');seen=set()
  for s in symbols.iter_symbols():
   index=s['st_shndx'];n=s.name
   if s['st_info']['type']!='STT_OBJECT' or not isinstance(index,int) or not n or not s['st_size']:continue
   sec=e.get_section(index)
   if not sec['sh_flags']&1:continue
   key=(n,index,s['st_value'],s['st_size'])
   if key in seen:continue
   seen.add(key);t=types.get(n,{})
   rows.append(dict(owner=role,mod_id=manifest['id'],artifact_sha256=digest,name=n,section=sec.name,offset=s['st_value'],bytes=s['st_size'],**t,coverage='REGISTERED_PARTIAL' if role=='RD' and n=='save_state' or role=='D' and n=='g_state' else 'NO_COMPLETE_OWNER_SCHEMA',warning='Metadata only; audit reset/heap/native references before any serialization.'))
 artifacts.append(dict(owner=role,id=manifest['id'],artifact_sha256=digest,bytes=f.stat().st_size,has_dwarf=bool(types),writable_objects=len(seen)))
summary=dict(scope=__doc__,artifacts=artifacts,by_owner=dict(collections.Counter(x['owner'] for x in rows)),pointer_bearing_objects=sum(x.get('pointer_bearing') is True for x in rows),unknown_types=sum('pointer_bearing' not in x or x.get('pointer_bearing') is None for x in rows),rows=rows)
(a.output/'STATE_INVENTORY.json').write_text(json.dumps(summary,indent=2)+'\n')
with (a.output/'STATE_INVENTORY.csv').open('w') as f:
 keys=['owner','mod_id','artifact_sha256','name','section','offset','bytes','type','pointer_bearing','coverage','warning'];w=csv.DictWriter(f,fieldnames=keys,lineterminator='\n');w.writeheader();w.writerows(rows)
print('PASS writable-symbol inventory',summary['by_owner'],'pointer-bearing',summary['pointer_bearing_objects'],'unknown',summary['unknown_types'])
