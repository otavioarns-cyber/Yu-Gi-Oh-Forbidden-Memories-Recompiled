"""Publish own evidence only: no object, private source, assets, disc or saves."""
from pathlib import Path
import argparse,collections,hashlib,json,re,shutil,sys
p=argparse.ArgumentParser();p.add_argument('--work',type=Path,required=True);a=p.parse_args();root=Path(__file__).resolve().parents[2];out=root/'notes/hal-9-compat';out.mkdir(exist_ok=True);sha=lambda f:hashlib.sha256(f.read_bytes()).hexdigest();raw={};summaries={}
groups={'final-regression':'final-regression','final-qualification':'final-qualification','reference32':'reference32','snapshot-only-control':'distinct-trace','native':'native-final','collection':'collection','policy-B':'policy-B','prewrite-B':'prewrite-B'}
for name,folder in groups.items():
 src=a.work/'logs'/folder;dst=out/name;dst.mkdir(exist_ok=True);j=json.loads((src/'RESULTADOS.json').read_text());summaries[name]=j.get('summary',dict(steps=[dict(name=x.get('name',x.get('test',str(i))),returncode=x['returncode']) for i,x in enumerate(j.get('steps',[]))]))
 assert not j.get('summary',{}).get('incomplete',0)
 for f in src.iterdir():
  if f.suffix not in ('.json','.csv','.log'):continue
  if f.suffix!='.log':shutil.copy2(f,dst/f.name);continue
  counts=collections.Counter();removed=collections.Counter();kept=[]
  for line in f.read_text().splitlines():
   if line.startswith(('TRACE ','CALLBACK_EXIT ','CALLBACK_SELF ','CALLBACK_EDGE ','FIELD_WRITES ','SUPPORT_PLANNER ','SUPPORT_SELECTION ','SUPPORT_HAND_LEDGER ')):
    counts[line]+=1
    if counts[line]>3:removed[line]+=1;continue
   kept.append(line.rstrip())
  raw[name+'/'+f.name]=dict(sha256=sha(f),bytes=f.stat().st_size,omitted_exact_repetitions=sum(removed.values()))
  (dst/f.name).write_text('RAW_SHA256 '+sha(f)+'; exact repeated trace lines after third occurrence omitted; trailing spaces normalized.\n'+'\n'.join(kept)+'\nREPETITIONS '+json.dumps(removed)+'\n')
final=[summaries[x] for x in ('final-regression','final-qualification')];summaries['final_combined']={'processes':sum(x['processes'] for x in final),'checks':dict(sum((collections.Counter(x['checks']) for x in final),collections.Counter())),'execution':dict(sum((collections.Counter(x['execution']) for x in final),collections.Counter())),'scope':'Distinct final cases, controlled fixtures/real callbacks; NOT Windows. Native, collection and historical controls below are separate and not added as independent gameplay.'}
(out/'RESULTADOS.json').write_text(json.dumps(summaries,indent=2)+'\n');(out/'RAW_LOGS.json').write_text(json.dumps(raw,indent=2)+'\n');shutil.copy2(a.work/'local/refs-before.tsv',out/'REFS_BEFORE.tsv');shutil.copy2(a.work/'logs/RESOURCES.json',out/'RESOURCES.json');build=json.loads((a.work/'local/final/BUILD.json').read_text());assert build['candidate_sha256']==sha(a.work/'local/candidate-roles/package/time-wizard.o');build['reproducible_second_output_sha256']=sha(a.work/'local/candidate-roles/package/time-wizard.o');(out/'BUILD_CANDIDATE.json').write_text(json.dumps(build,indent=2)+'\n')
# Runtime registered callback + full artifact SHA + body and relocation hashes.
sys.path.insert(0,str(root/'tests/hal20-field-state'));from elf_inventory import elf_info
paths={}
for directory in [root/'mods'/x for x in ('melhoria-dos-rituais','ritual-card-effects','ritual-duelists','fusion-auxiliary-balance','field-effects-secondary-types','ai-hard-mode')]+[Path('/workspace/work/hal20-dual/package'),a.work/'local/final/package']:
 m=json.loads((directory/'mod.json').read_text());paths[m['id']]=directory/(m['library']+'.o')
info={k:elf_info(f) for k,f in paths.items()};inventory={}
for mode in (44,45,46):
 log=a.work/'logs/final-qualification'/f'tw-object-h1-d1-before-m{mode}.log';s=log.read_text();nodes=[]
 for owner,code,target,name,size,node in re.findall(r'^CALLBACK_NODE owner=(\S+) code=(\w+) target=(\S+) function=(\S+) bytes=(\d+) node=(-?\d+)$',s,re.M):
  if target not in ('AiScript_Run','func_8001BAF0','func_8001B938','func_80028220','DuelScene_UpdateHandActions','DuelScene_UpdateCardPlacement','DuelScene_UpdateCardUse'):continue
  matches=[{k:f[k] for k in ('section','offset','bytes','body_sha256','relocations_sha256')} for f in info.get(owner,{}).get('functions',[]) if f['name']==name and f['bytes']==int(size)]
  nodes.append(dict(node=int(node),owner=owner,artifact_sha256=info.get(owner,{}).get('sha256'),code_hash=code,target=target,function=name,bytes=int(size),symbol_matches=matches))
 inventory[str(mode)]=dict(log=log.name,nodes=nodes,delegation_edges=sorted(set(re.findall(r'^CALLBACK_EDGE parent=(-?\d+) child=(-?\d+)$',s,re.M))),interpretation='Registration is not delegation. Examine CALLBACK_SELF and SUPPORT_SELECTION/HAND_LEDGER plus consume records. No admission by name.')
(out/'WRAPPERS.json').write_text(json.dumps(inventory,indent=2)+'\n')
assert not [f for f in out.rglob('*') if f.is_file() and f.suffix in ('.o','.exe','.dll','.bin','.png','.wav','.cue','.iso','.zip')]
print(summaries['final_combined'])
