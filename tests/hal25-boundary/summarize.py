"""Aggregate exact final receipts; preserve diagnostic and development failures separately."""
from pathlib import Path
import argparse,collections,csv,hashlib,json,tarfile
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--logs',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True);(a.output/'logs').mkdir(exist_ok=True)
final=['final-regression-174','final-recovery-64','final-reset-16','final-restrictions-2','final-hardening-72','final-composition-72','final-boundary-128','final-derived-quota-16'];extra=['PR26-derived-native-negative','final-guard-reference','final-canonical-R-RD'];dev=['boundary-initial','pr26-derived-selection-negative','native-Both-diagnostic']
profiles=[];suites=[]
for name in final+extra+dev:
 d=a.logs/name;f=d/'RESULTADOS.json';j=json.loads(f.read_text()) if f.exists() else json.loads((d/'SUMMARY.json').read_text());category='FINAL' if name in final else 'DIAGNOSTIC_FUNCTIONAL' if name=='PR26-derived-native-negative' else 'DEVELOPMENT_ONLY' if name in dev else 'REFERENCE_OR_HISTORICAL_AUDIT';summary=j.get('summary',j) if isinstance(j,dict) else j;suites.append(dict(suite=name,category=category,summary=summary))
 for row in j.get('profiles',[]) if isinstance(j,dict) else []:
  log=(d/row['log']).read_text();profiles.append(dict(suite=name,category=category,case=row['name'],composition=row['composition'],classification=row['classification'],mode=row['mode'],checks=row['checks'],log=name+'/'+row['log'],observations=[x for x in log.splitlines() if x.startswith(('RECOVERY ','COMBINED ','CONSUME_','MSTATE ','POISON ','MODEL_ANIMATION ','INSTRUMENTED ','ACTION first ','ACTION bonus ','NOT_REACHED ','REFUSED '))]))
 files=[f for f in sorted(d.iterdir()) if f.is_file() and f.suffix in ('.json','.csv','.log')];idx=[dict(path=f.name,bytes=f.stat().st_size,sha256=hashlib.sha256(f.read_bytes()).hexdigest()) for f in files]
 with tarfile.open(a.output/'logs'/(name+'.tar.gz'),'w:gz',compresslevel=9) as tar:
  for f in files:tar.add(f,arcname=name+'/'+f.name)
 (a.output/'logs'/(name+'-SHA256.json')).write_text(json.dumps(idx,indent=2)+'\n')
rows=[x for x in profiles if x['category']=='FINAL'];summary=dict(processes=len(rows),checks=dict(collections.Counter(c['status'] for x in rows for c in x['checks'])),classifications=dict(collections.Counter(x['classification'] for x in rows)))
report=dict(scope='Partial i386 host. Safe refusals/nonreachability are not successful gameplay; Windows automation and canonical audit remain separate.',summary=summary,suites=suites,profiles=profiles)
(a.output/'RESULTADOS.json').write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n')
with (a.output/'MATRIZ.csv').open('w') as f:
 w=csv.writer(f,lineterminator='\n');w.writerow(['category','suite','case','composition','classification','check','status','details','log'])
 for row in profiles:
  for c in row['checks']:w.writerow([row['category'],row['suite'],row['case'],row['composition'],row['classification'],c['name'],c['status'],c['details'],row['log']])
with (a.output/'FRONTEIRAS.csv').open('w') as f:
 w=csv.writer(f,lineterminator='\n');w.writerow(['category','suite','case','classification','observation','log'])
 for row in profiles:
  for line in row['observations']:w.writerow([row['category'],row['suite'],row['case'],row['classification'],line,row['log']])
print(summary)
