"""Publish own text evidence only; keep private inputs/objects outside Git."""
from pathlib import Path
import argparse,collections,hashlib,json,re,shutil,subprocess,sys
p=argparse.ArgumentParser();p.add_argument('--work',type=Path,required=True);a=p.parse_args();r=Path(__file__).resolve().parents[2];out=r/'notes/hal-9-client';out.mkdir(exist_ok=True);sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest();totals=collections.Counter();checks=collections.Counter();raw={};groups={}
for name in ('primary','boundaries','absence','regressions'):
 src=a.work/'logs'/('delivery-'+name);dst=out/name;dst.mkdir(exist_ok=True);j=json.loads((src/'RESULTADOS.json').read_text());assert j['profiles'] and not j['summary']['incomplete'];assert all(x['returncode']==0 for x in j['builds']);groups[name]=j['summary'];totals.update(j['summary']['execution']);checks.update(j['summary']['checks'])
 for f in src.iterdir():
  if not f.is_file() or f.suffix not in ('.json','.csv','.log'):continue
  if f.suffix=='.log':
   counts=collections.Counter();omitted=collections.Counter();lines=[]
   for line in f.read_text().splitlines():
    counts[line]+=1
    if counts[line]>3:omitted[line]+=1
    else:lines.append(line)
   (dst/f.name).write_text('\n'.join(lines)+'\n')
   raw[name+'/'+f.name]=dict(sha256=sha(f),bytes=f.stat().st_size,identical_lines_omitted=sum(omitted.values()),omitted=dict(omitted))
  elif f.name=='RESULTADOS.json':
   for q in j['profiles']:
    counts=collections.Counter();keep=[]
    for line in q.get('observations',[]):
     counts[line]+=1
     if counts[line]<=3:keep.append(line)
    q['observations']=keep;q['identical_observations_omitted']=dict((k,v-3) for k,v in counts.items() if v>3)
   j['raw_sha256']=sha(f);(dst/f.name).write_text(json.dumps(j,indent=2)+'\n')
  else:shutil.copy2(f,dst/f.name)
summary=dict(processes=sum(x['processes'] for x in groups.values()),execution=dict(totals),checks=dict(checks),groups=groups,scope='Real relocated objects, actual available controllers and private Dual, partial engine fixture. Not Windows gameplay. Eight historical reset FAIL retained.')
(out/'RESULTADOS.json').write_text(json.dumps(summary,indent=2)+'\n');(out/'RAW_LOGS.json').write_text(json.dumps(raw,indent=2)+'\n')
for local,name in ((a.work/'local/delivery2/BUILD.json','BUILD_CANDIDATE.json'),(a.work/'local/repeat2/BUILD.json','BUILD_REPEAT.json'),(a.work/'local/ABI.json','ABI.json'),(a.work/'logs/CLIENT_INVARIANTS.json','CLIENT_INVARIANTS.json'),(a.work/'REFS_BEFORE.tsv','REFS_BEFORE.tsv')):shutil.copy2(local,out/name)
# Record controls separately; never aggregate unfinished jobs as passed tests.
controls=[]
for src in (a.work/'logs').iterdir():
 if not src.is_dir() or src.name in ('delivery-primary','delivery-boundaries','delivery-absence','delivery-regressions'):continue
 row=dict(name=src.name,logs={f.name:dict(sha256=sha(f),bytes=f.stat().st_size) for f in src.glob('*.log')})
 if (src/'RESULTADOS.json').exists():
  j=json.loads((src/'RESULTADOS.json').read_text());row['summary']=j['summary'];row['builds']=j['builds'];row['private_candidate_receipt']=j.get('private_candidate_receipt');row['failures']=[dict(case=q['name'],checks=[c for c in q['checks'] if c['status']=='FAIL']) for q in j['profiles'] if any(c['status']=='FAIL' for c in q['checks'])]
 if (src/'STOPPED.json').exists():row['stopped']=json.loads((src/'STOPPED.json').read_text())
 if src.name=='delivery-regressions-queen-control':
  row['functional_reclassification']=dict(count=8,case=26,legacy_classification='NOT_REACHED',correct_classification='FAIL functional regression',reason='Own provisional TW fence blocked the real outer mandatory Queen correction with RD OFF. New invariant explicitly requires Female action; corrected by delegating non-TW pre-gate action only without cooperative confirmation.')
 controls.append(row)
control_dir=out/'controls';control_dir.mkdir(exist_ok=True)
for folder,name in (('client-v2','tw-object-h0-d0-before-m102.log'),('client-faults-v3','tw-object-h0-d0-before-m112.log'),('delivery-boundaries-harness-control','build-harness.log'),('delivery-primary-harness-control','tw-object-h0-d0-before-m120.log'),('delivery-regressions-queen-control','tw-object-h0-d0-before-m26.log')):
 f=a.work/'logs'/folder/name
 if f.exists():
  counts=collections.Counter();lines=[]
  for line in f.read_text().splitlines():
   counts[line]+=1
   if counts[line]<=3:lines.append(line)
  (control_dir/(folder+'-'+name)).write_text('\n'.join(lines)+'\n')
artifacts={f.parent.name:json.loads(f.read_text()) for f in (a.work/'local').glob('*/BUILD.json')}
(out/'CANDIDATES.json').write_text(json.dumps(artifacts,indent=2)+'\n')
(out/'CONTROLS.json').write_text(json.dumps(controls,indent=2)+'\n')
ref='7855862525222ae6dbecfa02ae607352a41cc675';files=subprocess.check_output(['git','ls-tree','-r','--name-only',ref,'mods'],text=True,cwd=r).splitlines()
assert len(files)==507
for f in files:assert (r/f).read_bytes()==subprocess.check_output(['git','show',ref+':'+f],cwd=r)
assert not subprocess.check_output(['git','diff',ref,'--','src','mods','config','.github'],cwd=r)
b=json.loads((a.work/'local/delivery2/BUILD.json').read_text());b2=json.loads((a.work/'local/repeat2/BUILD.json').read_text());assert b['candidate_sha256']==b2['candidate_sha256']
(out/'AUDIT.json').write_text(json.dumps(dict(reference=ref,canonical_mod_files_identical=len(files),src_mods_config_workflows='UNCHANGED',TW_reference_sha256=b['reference_sha256'],TW_private_candidate_sha256=b['candidate_sha256'],repeat_byte_identical=True,Dual_sha256=sha(Path('/workspace/work/hal20-dual/package/dual-play.o')),RD_sha256=sha(Path('/workspace/work/hal9-selection-handoff/local/candidate-rd/ritual-duelists/ritual-duelists.o')),legacy_link_objects=b['unchanged_link_objects'],manifest_settings_assets='UNCHANGED',frozen_ABI='UNCHANGED',policy='OBSERVE default; KEEP_RD/ALLOW_TW private tests only',guard='UNCHANGED / TW NOT admitted',savestate='B preserved; A disabled',private_binary_assets_published=False),indent=2)+'\n')
sys.path.insert(0,str(r/'tests/hal20-field-state'));from elf_inventory import elf_info
paths={}
for folder in [r/'mods'/d for d in ('melhoria-dos-rituais','ritual-card-effects','fusion-auxiliary-balance','field-effects-secondary-types','ai-hard-mode')]+[Path('/workspace/work/hal9-selection-handoff/local/candidate-rd/ritual-duelists'),Path('/workspace/work/hal20-dual/package'),a.work/'local/delivery2/package']:
 m=json.loads((folder/'mod.json').read_text());paths[m['id']]=folder/(m['library']+'.o')
info={k:elf_info(v) for k,v in paths.items()};wrappers={}
for group,mode in (('primary',102),('primary',103),('primary',113),('primary',120),('boundaries',121),('boundaries',123)):
 f=a.work/'logs'/('delivery-'+group)/f'tw-object-h1-d1-before-m{mode}.log';s=f.read_text();nodes=[]
 for owner,code,target,name,size,node in re.findall(r'^CALLBACK_NODE owner=(\S+) code=(\w+) target=(\S+) function=(\S+) bytes=(\d+) node=(-?\d+)$',s,re.M):
  if target not in ('AiScript_Run','func_8001BAF0','func_8001B938','func_80028220','DuelScene_UpdateHandActions','DuelScene_UpdateCardPlacement'):continue
  matches=[{k:v[k] for k in ('section','offset','bytes','body_sha256','relocations_sha256')} for v in info.get(owner,{}).get('functions',[]) if v['name']==name and v['bytes']==int(size)]
  nodes.append(dict(node=int(node),owner=owner,artifact_sha256=info.get(owner,{}).get('sha256'),code_hash=code,target=target,function=name,bytes=int(size),symbol_matches=matches))
 wrappers[str(mode)]=dict(log=str(f.relative_to(a.work/'logs')),nodes=nodes,edges=sorted(set(re.findall(r'^CALLBACK_EDGE parent=(-?\d+) child=(-?\d+)$',s,re.M))),scope='Names are labels only. Hash, actual edge, SELF state/identity/consumption and effective callbacks qualify this fixture, never production admission.')
(out/'WRAPPERS.json').write_text(json.dumps(wrappers,indent=2)+'\n')
assert not [f for f in out.rglob('*') if f.is_file() and f.suffix.lower() in ('.o','.exe','.dll','.bin','.png','.wav','.cue','.iso','.zip','.mrg')]
print(json.dumps(summary))
