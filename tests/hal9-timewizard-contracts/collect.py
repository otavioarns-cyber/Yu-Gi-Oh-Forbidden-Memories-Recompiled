"""Publish only own logs/metadata; never copy private .o, sources or assets."""
from pathlib import Path
import argparse,shutil,json,hashlib,collections,subprocess
p=argparse.ArgumentParser();p.add_argument('--work',type=Path,required=True);a=p.parse_args();root=Path(__file__).resolve().parents[2];out=root/'notes/hal-9';out.mkdir(exist_ok=True);logs=a.work/'logs';sha=lambda f:hashlib.sha256(f.read_bytes()).hexdigest()
raw_logs={}
groups={'runtime':'delivery','original':'original-runtime','collection':'collection-final','probe-no-fxsr':'probe-final-no-fxsr','native':'native-final','policy-B':'policy-B','prewrite-B':'prewrite-B','official':'official-final'};sums={}
for name,src in groups.items():
 target=out/name;target.mkdir(exist_ok=True)
 for f in (logs/src).iterdir():
  if f.suffix in ('.json','.csv','.log'):
   if f.suffix!='.log':shutil.copy2(f,target/f.name)
   else:
    text=f.read_text();counts=collections.Counter();removed=collections.Counter();kept=[]
    for line in text.splitlines():
     if line.startswith(('TRACE ','CALLBACK_EXIT ','CALLBACK_SELF ','CALLBACK_EDGE ','FIELD_WRITES ')):
      counts[line]+=1
      if counts[line]>3:removed[line]+=1;continue
     kept.append(line)
    raw_logs[name+'/'+f.name]=dict(sha256=sha(f),bytes=f.stat().st_size,lines=len(text.splitlines()),repeated_lines_omitted=sum(removed.values()))
    header='HAL9_LOG raw_sha256='+sha(f)+'; only exact duplicate trace lines after their third occurrence omitted; all results and distinct state lines preserved.\n'
    footer='\nHAL9_REPETITIONS '+json.dumps(dict(removed),ensure_ascii=False)+'\n' if removed else ''
    (target/f.name).write_text(header+'\n'.join(kept)+'\n'+footer)
 if (logs/src/'RESULTADOS.json').exists():
  j=json.loads((logs/src/'RESULTADOS.json').read_text());sums[name]=j.get('summary',dict(steps=[dict(name=x.get('name',str(i)),returncode=x['returncode']) for i,x in enumerate(j.get('steps',[]))]))
for src,dst in [('AUDIT-final.json','AUDIT.json'),('SOURCE_RECOVERY.json','SOURCE_RECOVERY.json'),('core-boundaries-final.json','CORE_BOUNDARIES.json'),('build-windows-final.log','BUILD_WINDOWS.log'),('official-final-driver.log','OFFICIAL.log')]:shutil.copy2(logs/src,out/dst)
shutil.copy2(a.work/'local/refs-before.tsv',out/'REFS_BEFORE.tsv');build=json.loads((a.work/'local/candidate-final/BUILD.json').read_text());repro=a.work/'local/repro-final/package/time-wizard.o';assert sha(repro)==build['candidate_sha256'];build['reproducible_second_output_sha256']=sha(repro);build['path_maps']='Two distinct output directories produce byte-identical file; fixed debug/file prefix maps.'
(out/'BUILD_CANDIDATE.json').write_text(json.dumps(build,indent=2)+'\n')
exe=root/'tmp/pc/win32/memories-pc.exe';(out/'WINDOWS.json').write_text(json.dumps(dict(build=dict(status='PASS',target='Windows PE32 i386',command='python tools/pc/build_game32.py --target windows --release',bytes=exe.stat().st_size,sha256=sha(exe)),runtime=dict(status='AUTOMATION_UNAVAILABLE',gameplay='NOT_REACHED',evidence='PR30 authenticated GitHub notice: Actions suspended for scale of usage. Current Actions API still lists no new run, latest 2026-10-06. Managed environment is Linux desktop, no native Windows runner/VM or configured remote identity, no legitimate disc image present.',alternatives='Existing isolated PE32 workflow can run on a legitimate authorized Windows runner after Actions restriction is resolved. No repeat of unchanged Wine/QEMU PE failures, no VM/OS license invented.',private_artifacts_published=False)),indent=2)+'\n')
sums['aggregate_candidate_and_collection']={'processes':sums['runtime']['processes']+sums['collection']['processes']+sums['probe-no-fxsr']['processes'],'checks_PASS':sums['runtime']['checks'].get('PASS',0)+sums['collection']['checks'].get('PASS',0)+sums['probe-no-fxsr']['checks'].get('PASS',0),'warning':'Contract/fixture/controller qualification, not full gameplay Windows. Historical FAIL controls below remain failures; NOT_REACHED does not become compatibility.'}
(out/'RAW_LOGS.json').write_text(json.dumps(raw_logs,indent=2)+'\n')
(out/'RESULTADOS.json').write_text(json.dumps(sums,indent=2)+'\n')
# No screenshots, image/sound data, compiled programs, core/private sources.
assert not [f for f in out.rglob('*') if f.is_file() and f.suffix in ('.o','.exe','.dll','.png','.wav','.zip','.bin','.cue')]
print(json.dumps(sums['aggregate_candidate_and_collection']))
