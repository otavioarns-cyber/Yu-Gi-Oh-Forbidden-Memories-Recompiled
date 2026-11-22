"""Received native-controller regressions on the actual pinned candidate.
Wheel/presentation and three host queries are modeled here. Real manager/owner
queries and complete .o execute separately in object_cases.inc. No Windows claim.
"""
from pathlib import Path
import argparse,subprocess,sys,os,shlex,json,hashlib
p=argparse.ArgumentParser();p.add_argument('--package',type=Path,required=True);p.add_argument('--sdk',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=False)
rows=[]
def run(name,cmd):
 q=subprocess.run(list(map(str,cmd)),cwd=a.package,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=120);(a.output/(name+'.log')).write_text(q.stdout);rows.append(dict(name=name,returncode=q.returncode,command=list(map(str,cmd))));return q.returncode
# Hook only execution transport and explicit host query boundaries. Every other
# missing native-controller import stays the received zero-filled crash trap.
wrapper=r'''
import sys,subprocess,runpy,pathlib,shlex,os
old=subprocess.run;runner=shlex.split(os.environ.get('MEMORIES_I386_RUNNER',''))
def execute(args,*a,**kw):
 args=list(map(str,args))
 if args[0]=='as':
  f=pathlib.Path(args[2]);s=f.read_text()
  for name in ('Hal9_CoreHardStateV1','Hal9_CoreDualDrawV1','Hal9_CoreOwnersReadyV1'):
   s=s.replace('.comm '+name+',65536,16','\n.text\n.globl '+name+'\n'+name+': xor %eax,%eax; ret\n')
  f.write_text(s)
 if pathlib.Path(args[0]).name=='test' and pathlib.Path(args[0]).is_absolute():args=runner+args
 return old(args,*a,**kw)
subprocess.run=execute
sys.argv=['source/tests/run_native_tests.py',sys.argv[1]]
runpy.run_path('source/tests/run_native_tests.py',run_name='__main__')
'''
run('actual-candidate-native-controller',[sys.executable,'-c',wrapper,a.sdk.resolve()])
cc=shlex.split(os.environ.get('MEMORIES_I386_CC','gcc'));runner=shlex.split(os.environ.get('MEMORIES_I386_RUNNER',''))
for name in ('test_dual_play_decisions','test_dual_play_hand_selection','test_evolutions'):
 if run(name+'-build',cc+['-m32','-std=gnu11','-O1','-DMEMORIES_PC','-I'+str(a.sdk.resolve()/'include'),a.package.resolve()/'source/tests'/(name+'.c'),'-o',a.output.resolve()/name])==0:run(name,runner+[a.output.resolve()/name])
(a.output/'RESULTADOS.json').write_text(json.dumps(dict(scope=__doc__,candidate_sha256=hashlib.sha256((a.package/'time-wizard.o').read_bytes()).hexdigest(),steps=rows),indent=2)+'\n');print([(x['name'],x['returncode']) for x in rows]);raise SystemExit(any(x['returncode'] for x in rows))
