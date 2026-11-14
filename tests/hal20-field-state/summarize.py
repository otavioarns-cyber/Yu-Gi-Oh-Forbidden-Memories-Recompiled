"""Archive explicit final suites, separating canonical composition, A probe, historical controls."""
from pathlib import Path
import argparse,json,collections,csv,gzip,tarfile,io,hashlib
p=argparse.ArgumentParser();p.add_argument('--logs',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True)
groups={
 'collection':['field-unguarded','field-guarded-final','field-bonus','field-negatives','field-old-object-refused','field-return-trace','field-native-ritual','state-policy-B','reg-default','reg-hardening','reg-boundary','reg-recovery','reg-reset-recovery','reg-restrictions','composition','providers'],
 'A_NOT_ADMITTED':['state-preview-complete','state-preview-guard-refused'],
 'historical_expected_FAILURE':['fusion-original-control-final','field-original-control-final','reference26']}
summary={'scope':'Partial engine/controller and simulated state roundtrip, not Windows gameplay. A is a limited non-admitted probe, not complete persistence. Expected historical FAIL preserved. Counts are processes/checks, not unique gameplay scenarios.','groups':{},'archives':[]}
with (a.output/'MATRIZ.csv').open('w') as f:
 w=csv.writer(f,lineterminator='\n');w.writerow(['group','suite','case','composition','guard','scenario','classification','check','status','details','log'])
 for group,names in groups.items():
  rows=[];counts=collections.Counter();checks=collections.Counter();n=0
  for name in names:
   d=a.logs/name;j=json.loads((d/'RESULTADOS.json').read_text());s=j['summary'];assert s['processes'] and not s['incomplete'],(name,s)
   assert all(x['status']=='PASS' for x in j['preflight']+j['postflight']),name
   if group!='historical_expected_FAILURE':assert not s['checks'].get('FAIL'),(name,s)
   counts.update(s['execution']);checks.update(s['checks']);n+=s['processes'];rows.append(dict(suite=name,**s))
   with (d/'MATRIZ.csv').open() as q:
    r=csv.reader(q);next(r)
    for line in r:w.writerow([group,name,*line])
   target=a.output/(name+'.logs.tar.gz')
   with target.open('wb') as raw:
    with gzip.GzipFile(fileobj=raw,mode='wb',mtime=0) as gz:
     with tarfile.open(fileobj=gz,mode='w') as t:
      for source in sorted(d.iterdir()):
       if not source.is_file() or source.suffix not in ('.json','.csv','.log'):continue
       b=source.read_bytes();assert b'\x00' not in b,source;info=tarfile.TarInfo(name+'/'+source.name);info.size=len(b);info.mtime=0;info.mode=0o644;t.addfile(info,io.BytesIO(b))
   summary['archives'].append(dict(file=target.name,bytes=target.stat().st_size,sha256=hashlib.sha256(target.read_bytes()).hexdigest()))
  summary['groups'][group]=dict(suites=rows,processes=n,execution=dict(counts),checks=dict(checks))
summary['canonical_R_RD']=json.loads((a.logs/'R-RD-regression/SUMMARY.json').read_text())
summary['canonical_Fusion']=json.loads((a.logs/'Fusion-regression.json').read_text())
(a.output/'RESULTADOS.json').write_text(json.dumps(summary,indent=2)+'\n')
for g,s in summary['groups'].items():print(g,{k:v for k,v in s.items() if k!='suites'})
