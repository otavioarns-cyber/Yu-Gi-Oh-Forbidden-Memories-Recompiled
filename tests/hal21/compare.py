"""Check the candidate against PR12 + approved patch and PR21 ELF references."""
from pathlib import Path
import os,hashlib,json,struct,tempfile,subprocess,shutil,argparse
repo=Path(__file__).resolve().parents[2];p=argparse.ArgumentParser(description=__doc__);p.add_argument('--source',type=Path,required=True);p.add_argument('--references',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();candidate=repo/'mods/fusion-auxiliary-balance';sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest();checks={}
with tempfile.TemporaryDirectory(prefix='hal21-compare-') as tmp:
 tmp=Path(tmp);shutil.copytree(a.source,tmp/'expected');subprocess.run(['patch','-p1','--batch','-i',str(repo/'tests/hal21/reference/revoke.patch')],cwd=tmp/'expected',check=True,stdout=subprocess.DEVNULL)
 inputs=[x for x in candidate.iterdir() if x.is_file() and x.suffix in ('.c','.h')];differences=[x.name for x in inputs if sha(x)!=sha(a.source/x.name)];checks['only_three_production_sources_changed']=sorted(differences)==['fusionist_runtime.h','mod.c','owner_f.h'];checks['exact_approved_patch']=all(sha(x)==sha(tmp/'expected'/x.name) for x in inputs)
 assets=[x for x in (candidate/'assets').rglob('*') if x.is_file()];checks['asset_inventory_identical']={str(x.relative_to(candidate/'assets')) for x in assets}=={str(x.relative_to(a.source/'assets')) for x in (a.source/'assets').rglob('*') if x.is_file()};checks['all_assets_identical']=all(sha(x)==sha(a.source/x.relative_to(candidate)) for x in assets);checks['manifest_identical']=sha(candidate/'mod.json')==sha(a.source/'mod.json')
 # No hidden production C/H subdirectories or library additions are accepted.
 checks['source_inventory_identical']={str(x.relative_to(candidate)) for x in candidate.rglob('*') if x.is_file() and x.suffix in ('.c','.h') and 'tests' not in x.relative_to(candidate).parts}=={str(x.relative_to(a.source)) for x in a.source.rglob('*') if x.is_file() and x.suffix in ('.c','.h') and 'tests' not in x.relative_to(a.source).parts}
 def elf(f):
  d=f.read_bytes();h=struct.unpack_from('<16sHHIIIIIHHHHHH',d);assert d[:6]==b'\x7fELF\x01\x01' and h[1:3]==(1,3);secs=[struct.unpack_from('<IIIIIIIIII',d,h[6]+i*h[11]) for i in range(h[12])];strings=secs[h[13]];strings=d[strings[4]:strings[4]+strings[5]];parts={}
  for s in secs[1:]:
   name=strings[s[0]:].split(b'\0')[0].decode();data=bytes(s[5]) if s[1]==8 else d[s[4]:s[4]+s[5]];parts[name]={'type':s[1],'flags':s[2],'bytes':s[5],'alignment':s[8],'sha256':hashlib.sha256(data).hexdigest()}
  return {'sha256':sha(f),'bytes':len(d),'sections':parts}
 objects={'candidate':elf(candidate/'fusion-auxiliary-balance.o'),'PR12-api24':elf(a.source/'fusion-auxiliary-balance.o'),'PR21-revoke':elf(a.references/'revoke/fusion-auxiliary-balance.o'),'PR21-hide':elf(a.references/'hide/fusion-auxiliary-balance.o')}
 def loaded(v):return {k:s for k,s in v['sections'].items() if s['flags']&2}
 checks['allocated_sections_identical_to_approved_revoke']=loaded(objects['candidate'])==loaded(objects['PR21-revoke'])
 nonalloc_differences=[k for k,v in objects['candidate']['sections'].items() if v!=objects['PR21-revoke']['sections'].get(k) and not v['flags']&2]
 checks['only_debug_sections_differ_from_approved_revoke']=all(k.startswith('.debug') for k in nonalloc_differences)
 result={'checks':checks,'changed_production_files':sorted(differences),'production_files':{x.name:sha(x) for x in sorted(inputs)},'manifest_sha256':sha(candidate/'mod.json'),'assets':[{'path':str(x.relative_to(candidate)),'sha256':sha(x)} for x in sorted(assets)],'objects':objects,'nonallocated_differences_vs_PR21_revoke':nonalloc_differences,'scope':'Unchanged manifest/assets/public headers. Approved patch byte-exact. Allocated-section identity excludes debug; ABI/relocations/loader hash checked separately.'};a.output.write_text(json.dumps(result,ensure_ascii=False,indent=2)+'\n');print(checks);raise SystemExit(not all(checks.values()))
