"""Collect own text evidence, preserving controls and excluding private artifacts."""
from pathlib import Path
import argparse,collections,hashlib,json,re,shutil,subprocess,sys,difflib
p=argparse.ArgumentParser();p.add_argument('--work',type=Path,required=True);a=p.parse_args();r=Path(__file__).resolve().parents[2];out=r/'notes/hal-9-recovery';out.mkdir(exist_ok=True);sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest();groups={};raw={};execution=collections.Counter();checks=collections.Counter()
folders={'lifecycle':'final-lifecycle','end-boundary':'final-end-boundary','projections':'final-projections','off-projection':'final-off-projection','primary':'final-primary','boundaries':'final-boundaries','regressions':'final-regressions','absence':'final-absence'}
def text_log(src,dst):
 counts=collections.Counter();omit=collections.Counter();lines=[]
 for line in src.read_text().splitlines():
  counts[line]+=1
  if counts[line]>3:omit[line]+=1
  else:lines.append(line)
 dst.write_text('\n'.join(lines)+'\n');return dict(sha256=sha(src),bytes=src.stat().st_size,identical_lines_omitted=dict(omit))
for name,folder in folders.items():
 src=a.work/'logs'/folder;dst=out/name;dst.mkdir(exist_ok=True);j=json.loads((src/'RESULTADOS.json').read_text());assert j['profiles'] and not j['summary']['incomplete'];assert all(x['returncode']==0 for x in j['builds']);groups[name]=j['summary'];execution.update(j['summary']['execution']);checks.update(j['summary']['checks'])
 for f in src.iterdir():
  if not f.is_file() or f.suffix not in ('.json','.csv','.log'):continue
  if f.suffix=='.log':raw[name+'/'+f.name]=text_log(f,dst/f.name)
  elif f.name=='RESULTADOS.json':
   published=json.loads(f.read_text())
   for q in published['profiles']:
    counts=collections.Counter();keep=[]
    for line in q.get('observations',[]):
     counts[line]+=1
     if counts[line]<=3:keep.append(line)
    q['observations']=keep;q['identical_observations_omitted']={k:v-3 for k,v in counts.items() if v>3}
   published['raw_sha256']=sha(f);(dst/f.name).write_text(json.dumps(published,indent=2)+'\n')
  else:shutil.copy2(f,dst/f.name)
summary=dict(processes=sum(j['processes'] for j in groups.values()),execution=dict(execution),checks=dict(checks),groups=groups,scope='Actual pinned objects/callbacks, native Main_RunDuel and available controller; InitScene IO/renderer body and new boards modeled. Not Windows gameplay. Historical reset34 FAIL separate.')
(out/'RESULTADOS.json').write_text(json.dumps(summary,indent=2)+'\n');(out/'RAW_LOGS.json').write_text(json.dumps(raw,indent=2)+'\n')
for source,target in ((a.work/'local/delivery/BUILD.json','BUILD_CANDIDATE.json'),(a.work/'local/repeat/BUILD.json','BUILD_REPEAT.json'),(a.work/'local/ABI.json','ABI.json'),(a.work/'logs/INVARIANTS.json','INVARIANTS.json'),(a.work/'REFS_BEFORE.tsv','REFS_BEFORE.tsv')):shutil.copy2(source,out/target)
controls=[];control_dir=out/'controls';control_dir.mkdir(exist_ok=True)
for src in (a.work/'logs').iterdir():
 if not src.is_dir() or src.name in folders.values():continue
 row=dict(name=src.name,logs={f.name:dict(sha256=sha(f),bytes=f.stat().st_size) for f in src.glob('*.log')})
 if (src/'RESULTADOS.json').exists():
  j=json.loads((src/'RESULTADOS.json').read_text());row.update(summary=j['summary'],builds=j['builds'],candidate=j.get('private_candidate_receipt'),failures=[dict(case=q['name'],classification=q['classification'],checks=[x for x in q['checks'] if x['status']=='FAIL']) for q in j['profiles'] if q['classification'] in ('FAIL','INCOMPLETE')])
 if (src/'STOPPED.json').exists():row['stopped']=json.loads((src/'STOPPED.json').read_text())
 if src.name=='candidate-v1':row['interpretation']='Eight FAIL of universal-recovery hypothesis after RD HANDOFF. Local reset works, RD monitor refuses changed world. Final oracle explicitly requires owner quarantine/REFUSED, never gameplay success.'
 if src.name=='lifecycle-v2':row['interpretation']='Harness errors: attempted access to inlined private function and replay through a one-shot fault proxy. Corrected to real public operations; incomplete results not delivery.'
 if src.name=='delivery-lifecycle':row['interpretation']='Caller ticket nonce was deliberately poisoned. Monotonicity must compare fresh server nonce to authentic offered nonce, not forged caller value; full old token still refused.'
 if src.name=='projection-label-control':row['interpretation']='Broad SAFE_REFUSAL label corrected: only latched adapter conflicts are REFUSED; projection-only probes without execution are NOT_REACHED. Checks unchanged.'
 controls.append(row)
 for f in src.glob('*.log'):
  if f.name in ('tw-object-h0-d0-before-m200.log','tw-object-h0-d0-before-m202.log','tw-object-h0-d1-before-m222.log','tw-object-h0-d1-before-m223.log','tw-object-h0-d0-before-m230.log','tw-object-h0-d0-before-m201.log'):
   text_log(f,control_dir/(src.name+'-'+f.name))
(out/'CONTROLS.json').write_text(json.dumps(controls,indent=2)+'\n')
(out/'CANDIDATES.json').write_text(json.dumps({f.parent.name:json.loads(f.read_text()) for f in (a.work/'local').glob('*/BUILD.json')},indent=2)+'\n')
# Exact own sources for earlier test/runtime and introduced projection control.
version=a.work/'local/runtime-before-end.inc';shutil.copy2(version,control_dir/'runtime-before-end.inc')
shutil.copy2(a.work/'local/candidate-v1/package/source/tw_client.inc',control_dir/'tw_client-v1.inc')
old=(r/'experiments/hal9-client/tw_client.inc').read_text();new=(r/'experiments/hal9-recovery/tw_client.inc').read_text();(out/'TW_CLIENT_DELTA.patch').write_text(''.join(difflib.unified_diff(old.splitlines(True),new.splitlines(True),fromfile='a/PR35/tw_client.inc',tofile='b/candidate/tw_client.inc')))
ref='e5c97876009e0e57127166039821426a617729db';files=subprocess.check_output(['git','ls-tree','-r','--name-only',ref,'mods'],cwd=r,text=True).splitlines();assert len(files)==507
for f in files:assert (r/f).read_bytes()==subprocess.check_output(['git','show',ref+':'+f],cwd=r)
assert not subprocess.check_output(['git','diff',ref,'--','src','mods','config','.github'],cwd=r)
b=json.loads((a.work/'local/delivery/BUILD.json').read_text());b2=json.loads((a.work/'local/repeat/BUILD.json').read_text());assert b['candidate_sha256']==b2['candidate_sha256'];assert b['client_source_sha256']==sha(r/'experiments/hal9-recovery/tw_client.inc')
(out/'AUDIT.json').write_text(json.dumps(dict(reference=ref,canonical_mod_files_identical=len(files),src_mods_config_workflows='UNCHANGED',TW_reference_sha256=b['reference_sha256'],TW_private_candidate_sha256=b['candidate_sha256'],repeat_byte_identical=True,unchanged_link_objects=b['unchanged_link_objects'],unchanged_imports_exports=True,Dual_sha256=sha(Path('/workspace/work/hal20-dual/package/dual-play.o')),RD_sha256=sha(Path('/workspace/work/hal9-selection-handoff/local/candidate-rd/ritual-duelists/ritual-duelists.o')),manifest_settings_assets='UNCHANGED',frozen_ABI='UNCHANGED',policy='OBSERVE default; KEEP_RD/ALLOW_TW explicit tests only',guard='UNCHANGED; TW not admitted',savestate='B preserved; A disabled',private_binaries_assets_published=False),indent=2)+'\n')
sys.path.insert(0,str(r/'tests/hal20-field-state'));from elf_inventory import elf_info
paths={}
for folder in [r/'mods'/d for d in ('melhoria-dos-rituais','ritual-card-effects','fusion-auxiliary-balance','field-effects-secondary-types','ai-hard-mode')]+[Path('/workspace/work/hal9-selection-handoff/local/candidate-rd/ritual-duelists'),Path('/workspace/work/hal20-dual/package'),a.work/'local/delivery/package']:
 m=json.loads((folder/'mod.json').read_text());paths[m['id']]=folder/(m['library']+'.o')
info={k:elf_info(v) for k,v in paths.items()};wrappers={}
for group,mode in (('final-lifecycle',200),('final-lifecycle',202),('final-lifecycle',203),('final-lifecycle',211),('final-end-boundary',214),('final-projections',221),('final-projections',222),('final-off-projection',231),('final-primary',120)):
 f=a.work/'logs'/group/f'tw-object-h1-d1-before-m{mode}.log';s=f.read_text();nodes=[]
 for owner,code,target,name,size,node in re.findall(r'^CALLBACK_NODE owner=(\S+) code=(\w+) target=(\S+) function=(\S+) bytes=(\d+) node=(-?\d+)$',s,re.M):
  if target not in ('Duel_InitScene','AiScript_Run','func_8001BAF0','func_80028220','DuelScene_UpdateHandActions','DuelScene_UpdateCardPlacement'):continue
  matches=[{k:v[k] for k in ('section','offset','bytes','body_sha256','relocations_sha256')} for v in info.get(owner,{}).get('functions',[]) if v['name']==name and v['bytes']==int(size)]
  nodes.append(dict(node=int(node),owner=owner,target=target,function=name,bytes=int(size),artifact_sha256=info.get(owner,{}).get('sha256'),code_hash=code,symbol_matches=matches))
 j=json.loads((a.work/'logs'/group/'RESULTADOS.json').read_text())
 wrappers[str(mode)]=dict(log=f.name,harness_sha256=j['harness_sha256'],test_callback_source_sha256=j['recovery_sources_sha256'],nodes=nodes,edges=sorted(set(re.findall(r'^CALLBACK_EDGE parent=(-?\d+) child=(-?\d+)$',s,re.M))),test_interposition='Projection probes replace only own predecessor slots. Their C callbacks are part of the full hashed harness, not independently registered MODs. Registry edges alone do not describe this inserted test shim.',admission='None; names never qualify a callback')
(out/'WRAPPERS.json').write_text(json.dumps(wrappers,indent=2)+'\n')
assert not [f for f in out.rglob('*') if f.is_file() and f.suffix.lower() in ('.o','.exe','.dll','.bin','.iso','.cue','.zip','.png','.wav','.mrg','.pyc')]
print(json.dumps(summary))
