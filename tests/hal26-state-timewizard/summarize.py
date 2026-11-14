"""Final explicit suites only; consumer qualification FAIL separate from collection checks."""
from pathlib import Path
import argparse,json,csv,tarfile,gzip,io,hashlib,collections,re,shutil
p=argparse.ArgumentParser();p.add_argument('--logs',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True)
root=Path(__file__).resolve().parents[2];known=json.loads((root/'notes/hal-20-field-state/wrappers/WRAPPERS.json').read_text())['nodes'].values()
groups={'canonical_collection':['state-B-hd-final','consolidation-guarded','manager','tw-residency-final'],'partial_A_NOT_ADMITTED':['pair-issuer-final','pair-generated-gap','pair-guard-refused']};summary={'scope':'Targeted continuation of PR29. Collection checks and Time Wizard source-consumer qualification are different assessments. No full native save/load or Windows gameplay certification.','groups':{}};audits=[]
with (a.output/'MATRIZ.csv').open('w') as f:
 w=csv.writer(f,lineterminator='\n');w.writerow(['group','suite','case','composition','guard','scenario','classification','check','status','details','log'])
 for group,names in groups.items():
  rows=[];processes=0;classes=collections.Counter();checks=collections.Counter()
  for name in names:
   d=a.logs/name;j=json.loads((d/'RESULTADOS.json').read_text());s=j['summary'];assert s['processes'] and not s['incomplete'] and not s['checks'].get('FAIL'),(name,s);assert all(x['status']=='PASS' for x in j['preflight']+j['postflight']);rows.append(dict(suite=name,**s));processes+=s['processes'];classes.update(s['execution']);checks.update(s['checks'])
   with (d/'MATRIZ.csv').open() as q:
    reader=csv.reader(q);next(reader)
    for line in reader:w.writerow([group,name,*line])
   if name=='tw-residency-final':
    for q in j['profiles']:
     raw=(d/q['log']).read_text();observations=[]
     nodes=[dict(owner=o,code=c,target=t,function=n,bytes=int(b),node=int(i)) for o,c,t,n,b,i in re.findall(r'^CALLBACK_NODE owner=(\S+) code=(\S+) target=(\S+) function=(\S+) bytes=(\d+) node=(-?\d+)$',raw,re.M)]
     for k,status,rest in re.findall(r'^AUDIT (TW_\w+) (PASS|FAIL|REFUSED) (.*)$',raw,re.M):
      owner=re.search(r'predecessor_owner=(\S+)',rest).group(1);target='AiScript_Run' if k=='TW_AI_DETECTOR' else 'DuelScene_UpdateDrawPhase';matched=[dict(v) for v in nodes if v['owner']==owner and v['target']==target]
      for v in matched:
       match=next((z for z in known if z['owner']==v['owner'] and z['target']==v['target'] and z['function']==v['function'] and z['loader_code']==v['code'] and z['bytes']==v['bytes']),None)
       if match:v.update({f:match[f] for f in ('artifact_sha256','body_sha256','relocations_sha256')})
      observations.append(dict(audit=k,status=status,details=rest,callback_nodes=matched,known_hash_binding=bool(matched) and all('artifact_sha256' in v for v in matched)))
     status='FAIL' if any(v['status']=='FAIL' for v in observations) else 'REFUSED' if any(v['status']=='REFUSED' for v in observations) else 'PASS'
     audits.append(dict(case=q['name'],guard=q['guard'],status=status,scope='Supplied source predicates replayed on actual relocated callbacks; guard0 observer is instrumentation. Not complete Time Wizard gameplay.',observations=observations))
   target=a.output/(name+'.logs.tar.gz')
   with target.open('wb') as raw:
    with gzip.GzipFile(fileobj=raw,mode='wb',mtime=0) as z:
     with tarfile.open(fileobj=z,mode='w') as t:
      for source in sorted(d.iterdir()):
       if not source.is_file() or source.suffix not in ('.json','.csv','.log'):continue
       data=source.read_bytes();assert b'\0' not in data;v=tarfile.TarInfo(name+'/'+source.name);v.size=len(data);v.mtime=0;t.addfile(v,io.BytesIO(data))
  summary['groups'][group]=dict(suites=rows,processes=processes,execution=dict(classes),checks=dict(checks))
summary['timewizard_source_qualification']=dict(processes=len(audits),execution=dict(collections.Counter(x['status'] for x in audits)),guard_on_execution=dict(collections.Counter(x['status'] for x in audits if x['guard']==1)),warning='FAIL is source-contract mismatch, not demonstrated full-game illegal action. REFUSED is direct-signature mismatch. These failures are not suppressed by passing diagnostic checks.')
(a.output/'TIMEWIZARD_CONSUMER_AUDIT.json').write_text(json.dumps(audits,indent=2)+'\n');(a.output/'RESULTADOS.json').write_text(json.dumps(summary,indent=2)+'\n')
with (a.output/'TIMEWIZARD_QUALIFICATION.csv').open('w') as f:
 w=csv.writer(f,lineterminator='\n');w.writerow(['case','guard','classification','audit','status','details'])
 for row in audits:
  for v in row['observations']:w.writerow([row['case'],row['guard'],row['status'],v['audit'],v['status'],v['details']])
for g,v in summary['groups'].items():print(g,{k:x for k,x in v.items() if k!='suites'})
print('Time Wizard qualification',summary['timewizard_source_qualification'])
