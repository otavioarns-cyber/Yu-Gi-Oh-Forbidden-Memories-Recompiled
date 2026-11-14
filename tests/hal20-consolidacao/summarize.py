"""Collect explicit final suites; expected historical failures remain separate."""
from pathlib import Path
import argparse,json,collections,csv,gzip,tarfile,io,hashlib,shutil
p=argparse.ArgumentParser();p.add_argument('--logs',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True)
names=['composition-guard-initial','providers','subsets-final','consolidation','manager','spoof','fence-final','reg-default-final','reg-hardening-final','reg-boundary','reg-recovery','reg-reset-recovery','reg-restrictions'];rows=[];counts=collections.Counter();checks=collections.Counter();processes=0;archives=[]
with (a.output/'MATRIZ.csv').open('w') as f:
 w=csv.writer(f,lineterminator='\n');w.writerow(['suite','case','composition','guard','scenario','classification','check','status','details','log'])
 for name in names:
  d=a.logs/name;j=json.loads((d/'RESULTADOS.json').read_text());s=j['summary'];assert s['processes']>0 and not s['incomplete'] and not s['checks'].get('FAIL'),(name,s)
  assert all(x['status']=='PASS' for x in j['preflight']+j['postflight']);rows.append(dict(suite=name,**s));counts.update(s['execution']);checks.update(s['checks']);processes+=s['processes']
  with (d/'MATRIZ.csv').open() as q:
   r=csv.reader(q);next(r)
   for line in r:w.writerow([name,*line])
  target=a.output/(name+'.logs.tar.gz')
  with target.open('wb') as raw:
   with gzip.GzipFile(fileobj=raw,mode='wb',mtime=0) as gz:
    with tarfile.open(fileobj=gz,mode='w') as t:
     for source in sorted(d.iterdir()):
      if not source.is_file() or source.suffix not in ('.json','.csv','.log'):continue
      b=source.read_bytes();info=tarfile.TarInfo(name+'/'+source.name);info.size=len(b);info.mtime=0;info.mode=0o644;t.addfile(info,io.BytesIO(b))
  archives.append(dict(file=target.name,bytes=target.stat().st_size,sha256=hashlib.sha256(target.read_bytes()).hexdigest()))
controls=json.loads((a.logs/'reference26/RESULTADOS.json').read_text());assert controls['summary']['execution']=={'FAIL':4} and controls['summary']['checks']['FAIL']==8
(a.output/'CONTROLES_PR26.json').write_text(json.dumps(controls,indent=2)+'\n')
with tarfile.open(a.output/'reference26.logs.tar.gz','w:gz') as t:
 for f in sorted((a.logs/'reference26').glob('*.log')):t.add(f,arcname='reference26/'+f.name)
summary=dict(scope='Partial host/controller, no gameplay Windows certification. PASS checks can establish an expected refusal or reproduce a known defect; see check details.',candidate_suites=rows,total=dict(processes=processes,execution=dict(counts),checks=dict(checks)),historical_controls=controls['summary'],canonical=json.loads((a.logs/'regressions/SUMMARY.json').read_text()),archive_hashes=archives)
(a.output/'RESULTADOS.json').write_text(json.dumps(summary,indent=2)+'\n');print(summary['total'])
