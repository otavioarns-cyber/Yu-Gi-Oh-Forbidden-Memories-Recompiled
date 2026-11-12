"""Isolated API24 suite; no game/disc or other MOD loaded.

Requires Python, Pillow, GCC and i386 compilation support. MEMORIES_I386_CC
selects an optional sysroot wrapper; MEMORIES_I386_RUNNER may prefix QEMU.
MEMORIES_TEST_RESULTS records exact commands/output as JSON. OFF regression
is deliberately retained as FAIL (exit 2): fixing production needs approval.
"""
from pathlib import Path
import json,os,shlex,subprocess,sys,tempfile
mod=Path(__file__).resolve().parents[1];repo=mod.parents[1];results=[]
env={**os.environ,'ASAN_OPTIONS':'detect_leaks=0'}
def run(name,args):
    args=list(map(str,args));p=subprocess.run(args,cwd=repo,env=env,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
    results.append({'name':name,'status':'PASS' if not p.returncode else 'FAIL','returncode':p.returncode,'command':args,'output':p.stdout})
    print(name,results[-1]['status'],flush=True);print(p.stdout,end='',flush=True)
    return not p.returncode
for p in sorted((mod/'tests').glob('test_*.py')):run(p.stem,[sys.executable,p])
run('run_fusionist',[sys.executable,mod/'tests/run_fusionist.py'])
with tempfile.TemporaryDirectory() as temp:
    temp=Path(temp)
    for p in sorted((mod/'tests').glob('test_*.c')):
        if p.stem=='test_fusionist_runtime':continue
        exe=temp/p.stem
        if run(p.stem+'-build',['gcc','-std=gnu11','-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-DMEMORIES_PC','-I'+str(repo/'src'),p,'-o',exe]):run(p.stem,[exe])
    exe=temp/'loader'
    if run('preview2_loader-build', [*shlex.split(os.environ.get('MEMORIES_I386_CC','gcc')),'-m32','-std=gnu11','-O1','-DMEMORIES_PC','-I'+str(repo/'src'),mod/'tests/preview2_loader.c',repo/'src/pc/mods/object_loader.c',repo/'src/pc/mods/mod_libc.c',repo/'src/pc/guest/branch_thunks.c',repo/'src/pc/compat/fs.c','-lm','-o',exe]):
        command=[*shlex.split(os.environ.get('MEMORIES_I386_RUNNER','')),exe,mod/'fusion-auxiliary-balance.o',mod]
        run('preview2_loader',command)
        run('preview2_contract_off',command+['--strict-off'])
target=os.environ.get('MEMORIES_TEST_RESULTS')
if target:Path(target).write_text(json.dumps(results,ensure_ascii=False,indent=2)+'\n')
print('SUMMARY test groups (build steps separate):',{s:sum(r['status']==s and not r['name'].endswith('-build') for r in results) for s in ['PASS','FAIL']})
sys.exit(any(r['status']=='FAIL' for r in results))
