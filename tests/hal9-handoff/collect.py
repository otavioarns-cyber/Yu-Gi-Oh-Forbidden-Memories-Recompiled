"""Own logs, contracts and fingerprints only. Never publish private objects/assets."""
from pathlib import Path
import argparse,collections,hashlib,json,re,shutil,subprocess,sys,difflib
p=argparse.ArgumentParser();p.add_argument('--work',type=Path,required=True);a=p.parse_args();r=Path(__file__).resolve().parents[2];out=r/'notes/hal-9-handoff';out.mkdir(exist_ok=True);sha=lambda f:hashlib.sha256(f.read_bytes()).hexdigest();summaries={};raw={}
groups={'primary':'delivery-primary','boundaries':'delivery-boundaries','extra':'delivery-extra','regressions':'delivery-regressions','absence':'final-absence','pre-hardening-control':'mutable-arguments-control'}
for name,folder in groups.items():
 src=a.work/'logs'/folder;dst=out/name;dst.mkdir(exist_ok=True);j=json.loads((src/'RESULTADOS.json').read_text());assert j['profiles'] and not j['summary']['incomplete'] and all(x['returncode']==0 for x in j['builds']),folder;summaries[name]=j['summary']
 for f in src.iterdir():
  if f.suffix not in ('.json','.csv','.log'):continue
  if f.suffix!='.log':
   if f.name=='RESULTADOS.json':
    published=json.loads(f.read_text());omitted=0
    for profile in published.get('profiles',[]):
     counts=collections.Counter();keep=[]
     for line in profile.get('observations',[]):
      counts[line]+=1
      if counts[line]<=3:keep.append(line)
      else:omitted+=1
     profile['observations']=keep
     profile['observation_repetitions_omitted']={k:v-3 for k,v in counts.items() if v>3}
    published['raw_json_sha256']=sha(f);published['exact_repeated_observations_omitted']=omitted
    assert published['summary']==j['summary'] and len(published['profiles'])==len(j['profiles'])
    (dst/f.name).write_text(json.dumps(published,indent=2)+'\n')
   else:shutil.copy2(f,dst/f.name)
   continue
  counts=collections.Counter();removed=collections.Counter();lines=[]
  for line in f.read_text().splitlines():
   if line.startswith(('TRACE ','CALLBACK_EXIT ','CALLBACK_SELF ','CALLBACK_EDGE ','FIELD_WRITES ','SUPPORT_PLANNER ','SUPPORT_SELECTION ','SUPPORT_HAND_LEDGER ')):
    counts[line]+=1
    if counts[line]>3:removed[line]+=1;continue
   lines.append(line.rstrip())
  raw[name+'/'+f.name]=dict(sha256=sha(f),bytes=f.stat().st_size,omitted_exact_repetitions=sum(removed.values()))
  (dst/f.name).write_text('RAW_SHA256 '+sha(f)+'; exact trace repeats after third occurrence omitted; trailing spaces normalized.\n'+'\n'.join(lines)+'\nREPETITIONS '+json.dumps(removed)+'\n')
protocol=[summaries[x] for x in ('primary','boundaries','extra','absence')];final=protocol+[summaries['regressions']]
def aggregate(items):return dict(processes=sum(x['processes'] for x in items),checks=dict(sum((collections.Counter(x['checks']) for x in items),collections.Counter())),execution=dict(sum((collections.Counter(x['execution']) for x in items),collections.Counter())),incomplete=sum(x['incomplete'] for x in items))
summaries['protocol_total']=aggregate(protocol);summaries['delivery_total']=aggregate(final);summaries['interpretation']='Controlled real planners/objects plus lab client/faults, NOT gameplay Windows. REFUSED is safe denial, not a completed duel; eight regression FAIL are inherited reset terminal hypothesis. Pre-hardening control is separate, not added to candidate coverage.'
(out/'RESULTADOS.json').write_text(json.dumps(summaries,indent=2)+'\n');(out/'RAW_LOGS.json').write_text(json.dumps(raw,indent=2)+'\n')
for src,dst in [('local/candidate-rd/BUILD.json','BUILD_CANDIDATE.json'),('logs/ABI.json','ABI.json'),('logs/TRACE_INVARIANTS.json','TRACE_INVARIANTS.json'),('local/refs-before.tsv','REFS_BEFORE.tsv')]:shutil.copy2(a.work/src,out/dst)
# Canonical source/ABI/manifest/settings/assets and all legacy artifacts immutable.
ref='c82e1efbfd0a49e37652364398adebdb85ecd043';files=subprocess.check_output(['git','ls-tree','-r','--name-only',ref,'mods'],cwd=r,text=True).splitlines();assert all((r/f).read_bytes()==subprocess.check_output(['git','show',ref+':'+f],cwd=r) for f in files)
assert not subprocess.check_output(['git','diff',ref,'--name-only','--','src','mods','config','.github'],cwd=r,text=True).strip()
build=json.loads((out/'BUILD_CANDIDATE.json').read_text());lab=a.work/'local/candidate-rd/ritual-duelists';assert sha(lab/'ritual-duelists.o')==build['candidate_sha256'];changed=[str(f.relative_to(lab)) for f in lab.rglob('*') if f.is_file() and (r/'mods/ritual-duelists'/f.relative_to(lab)).is_file() and sha(f)!=sha(r/'mods/ritual-duelists'/f.relative_to(lab))];assert sorted(changed)==['mod.c','ritual-duelists.o']
(out/'RD_LAB.patch').write_text(''.join(difflib.unified_diff((r/'mods/ritual-duelists/mod.c').read_text().splitlines(True),(lab/'mod.c').read_text().splitlines(True),fromfile='a/mods/ritual-duelists/mod.c',tofile='b/LAB-ONLY/ritual-duelists/mod.c')))
T=Path('/workspace/work/hal9-tw-rd-bonus/local/final/package/time-wizard.o');D=Path('/workspace/work/hal20-dual/package/dual-play.o');assert sha(T)=='46f63050a5761b6f7ca3ec44c7a267ee4a7f79ec1572fb3d41ecd1e6313d9093';assert sha(D)=='77cd32ff89de6ca37084113fcafb933a98a1cfeea52a78a936b24640b11a2378'
audit=dict(reference=ref,canonical_mod_files_identical=len(files),production_src_mods_config_workflows='UNCHANGED',frozen_ABI_files_sha256={str(f.relative_to(r)):sha(f) for d in ('ritual-duelists','ritual-card-effects') for f in (r/'mods'/d).glob('mandatory_hand*') if f.is_file()},TW_sha256=sha(T),Dual_sha256=sha(D),RD_original_sha256=build['reference_sha256'],RD_laboratory_sha256=build['candidate_sha256'],private_lab_changed_existing_files=changed,lab_added_own_files=['selection_finalization_v1.h','rd_companion.inc'],guard='UNCHANGED; no admission',policy_B='UNCHANGED; A disabled',policy='Explicit lab parameters only; no canonical default',legacy_core='UNCHANGED',new_private_objects_assets_published=False)
(out/'AUDIT.json').write_text(json.dumps(audit,indent=2)+'\n')
# Selected registered callbacks: artifact + body + relocation hashes and actual edges.
sys.path.insert(0,str(r/'tests/hal20-field-state'));from elf_inventory import elf_info
paths={}
for directory in [r/'mods'/d for d in ('melhoria-dos-rituais','ritual-card-effects','fusion-auxiliary-balance','field-effects-secondary-types','ai-hard-mode')]+[lab,D.parent,T.parent]:
 m=json.loads((directory/'mod.json').read_text());paths[m['id']]=directory/(m['library']+'.o')
info={k:elf_info(v) for k,v in paths.items()};wrappers={}
for case,folder in ((51,'delivery-primary'),(52,'delivery-primary'),(53,'delivery-primary'),(76,'delivery-boundaries'),(84,'delivery-extra')):
 f=a.work/'logs'/folder/f'tw-object-h1-d1-before-m{case}.log';s=f.read_text();nodes=[]
 for owner,code,target,name,size,node in re.findall(r'^CALLBACK_NODE owner=(\S+) code=(\w+) target=(\S+) function=(\S+) bytes=(\d+) node=(-?\d+)$',s,re.M):
  if target not in ('AiScript_Run','func_8001BAF0','func_8001B938','func_80028220','DuelScene_UpdateHandActions','DuelScene_UpdateCardPlacement'):continue
  matches=[{k:v[k] for k in ('section','offset','bytes','body_sha256','relocations_sha256')} for v in info.get(owner,{}).get('functions',[]) if v['name']==name and v['bytes']==int(size)]
  nodes.append(dict(node=int(node),owner=owner,artifact_sha256=info.get(owner,{}).get('sha256'),code_hash=code,target=target,function=name,bytes=int(size),symbol_matches=matches))
 wrappers[str(case)]=dict(log=f.name,nodes=nodes,delegation_edges=sorted(set(re.findall(r'^CALLBACK_EDGE parent=(-?\d+) child=(-?\d+)$',s,re.M))),scope='Names are labels, not admission. Inspect actual SELF/physical/ledger changes. No guard expansion.')
(out/'WRAPPERS.json').write_text(json.dumps(wrappers,indent=2)+'\n')
(out/'COMPANION_FUNCTIONS.json').write_text(json.dumps(dict(artifact_sha256=info['ritual-duelists']['sha256'],functions=[{k:v[k] for k in ('name','section','offset','bytes','body_sha256','relocations_sha256')} for v in info['ritual-duelists']['functions'] if v['name'].startswith('hf_')]),indent=2)+'\n')
assert not [f for f in out.rglob('*') if f.is_file() and f.suffix in ('.o','.exe','.dll','.bin','.png','.wav','.cue','.iso','.zip')]
print(summaries['delivery_total'])
