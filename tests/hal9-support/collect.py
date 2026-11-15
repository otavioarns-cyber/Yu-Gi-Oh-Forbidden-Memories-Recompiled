"""Own evidence only; no binaries, private code, images or saves."""
from pathlib import Path
import argparse,json,hashlib,shutil,collections,subprocess
p=argparse.ArgumentParser();p.add_argument('--work',type=Path,required=True);p.add_argument('--timewizard',type=Path,required=True);p.add_argument('--dual',type=Path,required=True);a=p.parse_args();r=Path(__file__).resolve().parents[2];out=r/'notes/hal-9-support';out.mkdir(exist_ok=True);sha=lambda f:hashlib.sha256(f.read_bytes()).hexdigest();summary={};raw={}
for name,folder in [('primary','final'),('bonus-reset-trace','boundary-trace'),('evolution-goal-failures','evolution'),('asset-boundary','asset-boundary'),('collection','collection-regression')]:
 src=a.work/'logs'/folder;dst=out/name;dst.mkdir(exist_ok=True);j=json.loads((src/'RESULTADOS.json').read_text());summary[name]=j['summary']
 for f in src.iterdir():
  if f.suffix not in ('.json','.csv','.log'):continue
  if f.suffix!='.log':shutil.copy2(f,dst/f.name);continue
  counts=collections.Counter();removed=collections.Counter();keep=[]
  for line in f.read_text().splitlines():
   if line.startswith(('TRACE ','CALLBACK_EXIT ','CALLBACK_SELF ','CALLBACK_EDGE ','FIELD_WRITES ','SUPPORT_PLANNER ')):
    counts[line]+=1
    if counts[line]>3:removed[line]+=1;continue
   keep.append(line.rstrip())
  raw[name+'/'+f.name]={'sha256':sha(f),'bytes':f.stat().st_size,'omitted_exact_repetitions':sum(removed.values())};(dst/f.name).write_text('RAW_SHA256 '+sha(f)+'; repeated trace lines after third occurrence omitted; trailing whitespace normalized.\n'+'\n'.join(keep)+'\nREPETITIONS '+json.dumps(removed)+'\n')
(out/'RESULTADOS.json').write_text(json.dumps(summary,indent=2)+'\n');(out/'RAW_LOGS.json').write_text(json.dumps(raw,indent=2)+'\n');shutil.copy2(a.work/'local/engine/BINDING.json',out/'NATIVE_BINDING.json');shutil.copy2(a.work/'local/refs-before.tsv',out/'REFS_BEFORE.tsv')
ref='377e8ca4e606da68b3c29f815eff4a646a5ba917';mods=subprocess.check_output(['git','ls-tree','-r','--name-only',ref,'mods'],cwd=r,text=True).splitlines()
assert all((r/f).read_bytes()==subprocess.check_output(['git','show',ref+':'+f],cwd=r) for f in mods)
assert not subprocess.check_output(['git','diff',ref,'--name-only','--','src','mods','config','.github'],cwd=r,text=True).strip()
T=a.timewizard/'time-wizard.o';D=a.dual/'dual-play.o';assert sha(T)=='6470024d45bd099b7800bd5f84ccdcef1a895a8a2447ecef058af11ada97d5d7';assert sha(D)=='77cd32ff89de6ca37084113fcafb933a98a1cfeea52a78a936b24640b11a2378'
(out/'AUDIT.json').write_text(json.dumps(dict(parent=ref,canonical_mod_files_identical=len(mods),production_src_mods_config_workflows='unchanged',TW_sha256=sha(T),Dual_sha256=sha(D),public_abi_and_guard='unchanged, TW not admitted',savestate_policy='B unchanged, A disabled',new_private_binaries_published=False,new_production_patch=False,fixture_changes=['enemy record geometry','display source identity field6B','native slot argument decoding','COMMON binding only','real native CardUse body','16.667ms simulated host clock','mock wheel transport delivers native chosen sector','input presses/read-only callback traces'],scope='No speculative core reconstruction or strategic change.'),indent=2)+'\n')
assert not [f for f in out.rglob('*') if f.is_file() and f.suffix in ('.o','.exe','.png','.wav','.bin','.zip','.cue')]
print(summary)
