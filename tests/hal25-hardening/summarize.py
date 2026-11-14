"""Publish evidence without turning safe refusals or audit failures into gameplay PASS.
No test execution, assertion changes or private object publication.
"""
from pathlib import Path
import argparse,collections,csv,hashlib,json,tarfile
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--logs',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True)
suites=['regression-174-final','recovery-64-final','reset-16-final','restrictions-2-final','hardening-72-final','manager-initialized-final','abi-size-final','composition-final','abi-old-r','abi-old-rd','baseline-qualified-oracle','pr25-late-identity','guard-final','original-regressions']
profiles=[];summaries=[]
for name in suites:
 d=a.logs/name;f=d/'RESULTADOS.json'
 j=json.loads(f.read_text()) if f.exists() else json.loads((d/'SUMMARY.json').read_text())
 summaries.append(dict(suite=name,summary=j.get('summary',j) if isinstance(j,dict) else j))
 for row in j.get('profiles',[]) if isinstance(j,dict) else []:
  # Replace only affected logging/size probes with their explicit later executions.
  if name=='hardening-72-final' and row['mode'] in (12,13):continue
  log=(d/row['log']).read_text();status='REFUSED' if 'SAFE_REFUSAL ' in log else row['classification']
  profiles.append(dict(suite=name,case=row['name'],scenario=row['scenario'],mode=row['mode'],composition=row['composition'],classification=status,checks=row['checks'],log=name+'/'+row['log'],observations=[line for line in log.splitlines() if line.startswith(('RECOVERY ','HARDENING ','MANAGER ','NOT_REACHED ','INSTRUMENTED ','ACTION first ','ACTION bonus ','TRANSACTION ','TEST_ONLY '))]))
 final=[x for x in profiles if x['suite'] not in ('baseline-qualified-oracle','pr25-late-identity')]
 indexes=[]
 for path in sorted(d.iterdir()):
  if path.is_file() and path.suffix in ('.json','.csv','.log'):
   # Replaced uninitialized diagnostic buffer logs are intentionally excluded;
   # original suite summary retains the historical execution, later probes are canonical.
   if name=='hardening-72-final' and ('-m12-' in path.name or '-m13-' in path.name):continue
   indexes.append(dict(path=path.name,bytes=path.stat().st_size,sha256=hashlib.sha256(path.read_bytes()).hexdigest()))
 archive=a.output/'logs'/(name+'.tar.gz');archive.parent.mkdir(exist_ok=True)
 with tarfile.open(archive,'w:gz',compresslevel=9) as tar:
  for item in indexes:tar.add(d/item['path'],arcname=name+'/'+item['path'])
 (a.output/'logs'/(name+'-SHA256.json')).write_text(json.dumps(indexes,indent=2)+'\n')
counts=dict(collections.Counter(x['classification'] for x in final));checks=dict(collections.Counter(c['status'] for x in final for c in x['checks']))
report=dict(scope='Partial i386 host. Process PASS means assertions met, not gameplay certification. Historical audit and guard results are separate.',suites=summaries,final_processes=len(final),final_classification=counts,final_checks=checks,profiles=profiles)
(a.output/'RESULTADOS.json').write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n')
with (a.output/'MATRIZ.csv').open('w') as f:
 w=csv.writer(f,lineterminator='\n');w.writerow(['suite','case','composition','classification','check','status','details','log'])
 for row in profiles:
  for c in row['checks']:w.writerow([row['suite'],row['case'],row['composition'],row['classification'],c['name'],c['status'],c['details'],row['log']])
with (a.output/'CANCELAMENTOS.csv').open('w') as f:
 w=csv.writer(f,lineterminator='\n');w.writerow(['suite','case','classification','observation','log'])
 for row in final:
  if row['scenario']=='hardening' or row['suite'] in ('recovery-64-final','reset-16-final'):
   for observation in row['observations']:w.writerow([row['suite'],row['case'],row['classification'],observation,row['log']])
print('FINAL',len(final),counts,checks)
