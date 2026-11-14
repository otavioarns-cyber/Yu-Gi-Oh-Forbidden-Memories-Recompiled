"""Bind observed hook pointers to exact ELF artifacts and function fingerprints.
Never export object bytes. Labels alone never qualify a callback.
"""
from pathlib import Path
from elf_inventory import elf_info
import argparse,collections,csv,json,os,re
root=Path(__file__).resolve().parents[2]
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--suite',type=Path,required=True);p.add_argument('--output',type=Path,required=True)
a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True)
meta=json.loads((root/'notes/hal-25-hardening/INPUTS.json').read_text())
dirs=[root/c['directory'] for c in meta['components']]
dirs += [Path(os.environ[x]) for x in ('MEMORIES_AI_DIR','MEMORIES_DUAL_DIR','MEMORIES_RITUALS_DIR','MEMORIES_FUSION_DIR')]
artifacts={}
for d in dirs:
 m=json.loads((d/'mod.json').read_text());f=d/(m['library']+'.o');artifacts[m['id']]=elf_info(f)
engine=Path(os.environ['MEMORIES_ENGINE_OBJ_DIR'])
native={x['path'][9:-4]:elf_info(engine/x['path']) for x in meta['native_engine_objects']}
result=json.loads((a.suite/'RESULTADOS.json').read_text())
for name,artifact in result.get('test_objects',{}).items():artifacts['hal2224-'+('wrapper' if name=='interposer' else name)]=artifact
nodes={};profiles=[];unresolved=[]
for profile in result['profiles']:
 log=(a.suite/profile['log']).read_text();bound={};edge_rows=[]
 for line in log.splitlines():
  if not line.startswith(('CALLBACK_NODE ','NATIVE_NODE ')):continue
  fields=dict(x.split('=',1) for x in line.split()[1:]);index=int(fields['node'])
  artifact=native.get(fields.get('unit')) if index<0 else artifacts.get(fields.get('owner'))
  candidates=[] if not artifact else [f for f in artifact['functions'] if f['name']==fields['function'] and f['bytes']==int(fields['bytes'])]
  if len(candidates)!=1:
   unresolved.append(dict(profile=profile['name'],fields=fields,matches=len(candidates)));continue
  f=candidates[0];key=':'.join((artifact['sha256'],str(f['section']),str(f['offset']),fields['target']))
  bound[index]=key
  if key not in nodes:
   nodes[key]=dict(artifact_sha256=artifact['sha256'],loader_code=fields['code'],owner=fields.get('owner','native'),target=fields['target'],function=f['name'],bytes=f['bytes'],section=f['section'],offset=f['offset'],body_sha256=f['body_sha256'],relocations_sha256=f['relocations_sha256'],registration_profiles=0,trace_calls=0,critical_returns=0,children={},inclusive={},self={},qualification='EXACT_ARTIFACT_AND_RUNTIME_POINTER')
  nodes[key]['registration_profiles']+=1
 for line in log.splitlines():
  if line.startswith('TRACE '):
   f=dict(x.split('=',1) for x in line.split()[1:]);matches=[k for k in bound.values() if nodes[k]['owner']==f.get('owner') and nodes[k]['target']==f.get('name')]
   if len(set(matches))==1:nodes[matches[0]]['trace_calls']+=1
  elif line.startswith('CALLBACK_EDGE '):
   f=dict(x.split('=',1) for x in line.split()[1:]);parent=int(f['parent']);child=int(f['child'])
   pk=bound.get(parent,'MODEL_OR_UNBOUND:'+str(parent));ck=bound.get(child,'MODEL_OR_UNBOUND:'+str(child));edge_rows.append([pk,ck])
   if parent in bound:
    row=nodes[pk]['children'];row[ck]=row.get(ck,0)+1
  elif line.startswith(('CALLBACK_SELF ','CALLBACK_EXIT ')):
   f=dict(x.split('=',1) for x in line.split()[1:]);index=int(f.pop('node'));f.pop('target',None)
   if index not in bound:continue
   n=nodes[bound[index]];kind='self' if line.startswith('CALLBACK_SELF') else 'inclusive'
   if kind=='inclusive':n['critical_returns']+=1
   for name,value in f.items():
    bucket=n[kind].setdefault(name,{});bucket[value]=bucket.get(value,0)+1
 status='REFUSED' if 'SAFE_REFUSAL ' in log else profile['classification']
 profiles.append(dict(case=profile['name'],classification=status,ranks=dict(M=-50,F=profile['mode'],R=-20 if profile['where']=='before' else 40,RD=10,H=profile.get('arank',0),D=20),node_bindings={str(k):v for k,v in bound.items()},edges_in_execution_order=edge_rows,notes='Repeated edges retained; MODEL_OR_UNBOUND engine VM/ritual body is not a qualified native callback.'))
report=dict(scope='Actual runtime pointer binding + artifact/body/relocation hashes. Return/self metrics only for four known cdecl critical targets; self is exclusive relative to traced critical children, not every C call.',unresolved=unresolved,nodes=nodes,profiles=profiles)
(a.output/'WRAPPERS.json').write_text(json.dumps(report,indent=2)+'\n')
with (a.output/'WRAPPERS.csv').open('w') as out:
 w=csv.writer(out,lineterminator='\n');w.writerow(['artifact_sha256','loader_code','target','function','body_sha256','relocations_sha256','registered_profiles','trace_calls','critical_returns','observed_children','self_metrics','scope'])
 for n in nodes.values():w.writerow([n['artifact_sha256'],n['loader_code'],n['target'],n['function'],n['body_sha256'],n['relocations_sha256'],n['registration_profiles'],n['trace_calls'],n['critical_returns'],json.dumps(n['children'],sort_keys=True),json.dumps(n['self'],sort_keys=True),'OBSERVED_BRANCH_ONLY' if n['critical_returns'] else 'REGISTERED_NOT_RETURN_QUALIFIED'])
print('nodes',len(nodes),'unresolved',len(unresolved),'profiles',len(profiles),'classification',dict(collections.Counter(x['classification'] for x in profiles)))
raise SystemExit(bool(unresolved))
