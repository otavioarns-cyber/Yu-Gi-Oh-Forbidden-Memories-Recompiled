"""Run unchanged official Linux lifecycle/overlay/hooks/state tests.
Transport uses MEMORIES_I386_RUNNER. On the Debian11 sysroot pthread is a
separate link library, unlike glibc>=2.34; add it without changing tests.
"""
from pathlib import Path
import runpy,sys,subprocess,shlex,os,json,argparse
p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=False);root=Path(__file__).resolve().parents[2];os.chdir(root);sys.path.insert(0,str(root/'tools/pc'))
original=subprocess.run;runner=shlex.split(os.environ.get('MEMORIES_I386_RUNNER',''));rows=[]
def execute(args,*arg,**kw):
 args=list(map(str,args));cmd=args[:]
 if Path(cmd[0]).name=='gcc' and '-m32' in cmd and '-c' not in cmd:cmd+=['-lpthread']
 if Path(cmd[0]).parent.name=='mods-lifecycle' and Path(cmd[0]).name in ('lifecycle','overlay','hooks','lifecycle-state'):cmd=runner+cmd
 q=original(cmd,*arg,**kw);rows.append(dict(original=args,executed=cmd,returncode=q.returncode));return q
subprocess.run=execute;sys.argv=['tools/pc/test_mods_lifecycle.py','--target','linux']
try:runpy.run_path(sys.argv[0],run_name='__main__')
finally:(a.output/'RESULTADOS.json').write_text(json.dumps(dict(scope=__doc__,steps=rows),indent=2)+'\n')
