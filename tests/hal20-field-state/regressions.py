"""Run original PR11/PR13 suites against isolated current candidate copies.
No assertion or production file in the preserved packages is edited.
"""
from pathlib import Path
import os,json,subprocess,shutil,tempfile,argparse,collections
repo=Path(__file__).resolve().parents[2];p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,required=True);p.add_argument('--qualified-loader',action='store_true',help='Qualify the one additional localized hand lifecycle hook, preserving every other loader assertion');p.add_argument('--package-only',action='store_true',help='Recheck only the historical immutable-package assertion, without rerunning passed functional groups');a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True);summary=[]
for name in ('Ritual','RD-real'):
 if a.package_only and name=='Ritual':continue
 with tempfile.TemporaryDirectory(prefix='hal25-reg-') as tmp:
  tmp=Path(tmp);src=repo/'mods'/('ritual-card-effects' if name=='Ritual' else 'ritual-duelists');dst=tmp/'mods'/src.name;dst.parent.mkdir();shutil.copytree(src,dst)
  for n in ('src','tools','config'):(tmp/n).symlink_to(repo/n,target_is_directory=True)
  historical=tmp/'notes/hal-20/ritual-duelists';historical.mkdir(parents=True)
  shutil.copy2(repo/'notes/hal-20-field-state/PR13_INSUMOS_SHA256.json',historical/'INSUMOS_SHA256.json')
  if a.qualified_loader and name=='RD-real':
   test=dst/'tests/preview2_loader.c';source=test.read_text();assert source.count('hooks==8')==1;test.write_text(source.replace('hooks==8','hooks==9'));
  target=a.output/(name+'.json')
  cmd=['python',dst/'tests'/('test_preview2_package.py' if a.package_only else 'run_preview2.py')]
  q=subprocess.run(cmd,cwd=tmp,env=dict(os.environ,MEMORIES_TEST_RESULTS=str(target.resolve())),text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=240);(a.output/(name+'.log')).write_text(q.stdout)
  if a.package_only:
   rows=[dict(name='test_preview2_package',status='FAIL' if q.returncode else 'PASS',returncode=q.returncode,command=list(map(str,cmd)),output=q.stdout)];target.write_text(json.dumps(rows,indent=2)+'\n')
  else:rows=json.loads(target.read_text()) if target.exists() else []
  row=dict(name=name,returncode=q.returncode,counts=dict(collections.Counter(x['status'] for x in rows if not x['name'].endswith('-build'))),complete=bool(rows));summary.append(row);print(row,flush=True)
(a.output/'SUMMARY.json').write_text(json.dumps(summary,indent=2)+'\n');raise SystemExit(any(x['returncode'] or not x['complete'] for x in summary))
