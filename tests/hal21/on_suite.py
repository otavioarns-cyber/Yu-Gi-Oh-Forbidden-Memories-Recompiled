"""Run unchanged PR12 ON regression fixtures against each reconstructed policy.
The standalone fusion runtime mock lacks an owner lifecycle; always_on.h models
ON only. OFF correctness is tested on real ELF objects/manager in run.py.
"""
from pathlib import Path
import os,subprocess,tempfile,json,shutil,argparse,collections
repo=Path(__file__).resolve().parents[2];p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,required=True);out=p.parse_args().output.resolve();out.mkdir(parents=True,exist_ok=True);src=Path(os.environ['MEMORIES_FUSION_SOURCE']).resolve();candidate=repo/'mods/fusion-auxiliary-balance';results=[]
env=dict(os.environ,ASAN_OPTIONS='detect_leaks=0')
def execute(policy,name,cmd):
 q=subprocess.run(list(map(str,cmd)),cwd=repo,env=env,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=120);log=policy+'-'+name+'.log';(out/log).write_text(q.stdout);results.append(dict(policy=policy,name=name,status='PASS' if q.returncode==0 else 'FAIL',returncode=q.returncode,log=log));return q.returncode==0

def function(s,name):
 at=s.index(name+'(');start=s.rfind('\nstatic ',0,at)+1;br=s.index('{',at);end=br+1;depth=1
 while depth:depth+=(s[end]=='{')-(s[end]=='}');end+=1
 return s[start:end]+'\n'
with tempfile.TemporaryDirectory(prefix='hal21-on-') as tmp:
 tmp=Path(tmp);stub=tmp/'always_on.h';stub.write_text('static int aux_owner_applied(void){return 1;}\n')
 for policy in ('baseline','hide','revoke','candidate'):
  mod=tmp/policy;shutil.copytree(candidate if policy=='candidate' else src,mod)
  if policy in ('hide','revoke'):
   q=subprocess.run(['patch','-p1','--batch','-i',str(repo/'tests/hal21/reference'/(policy+'.patch'))],cwd=mod,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True);assert q.returncode==0,q.stdout
  for f in sorted((mod/'tests').glob('test_*.py')):execute(policy,f.stem,['python',f])
  s=(mod/'mod.c').read_text();(tmp/'finalize_under_test.h').write_text(function(s,'aux_fusion_special')+function(s,'aux_card_finalize'))
  for f in sorted((mod/'tests').glob('test_*.c')):
   exe=tmp/(policy+'-'+f.stem);cmd=['gcc','-std=gnu11','-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-DMEMORIES_PC','-I'+str(repo/'src'),'-I'+str(tmp),*(['-include',stub] if policy!='candidate' else []),f,'-o',exe]
   if execute(policy,f.stem+'-build',cmd):execute(policy,f.stem,[exe])
(out/'RESULTADOS.json').write_text(json.dumps({'scope':'Unchanged ON logic/assertions; standalone owner always-ON mock; real OFF tested separately','results':results,'summary':{policy:dict(collections.Counter(r['status'] for r in results if r['policy']==policy and not r['name'].endswith('-build'))) for policy in ('baseline','hide','revoke','candidate')}},indent=2)+'\n');print('SUMMARY',json.loads((out/'RESULTADOS.json').read_text())['summary']);raise SystemExit(any(r['status']=='FAIL' for r in results))
