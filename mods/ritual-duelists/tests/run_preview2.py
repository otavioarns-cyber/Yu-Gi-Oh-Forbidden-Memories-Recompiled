"""API24 isolated suite: inherited test recipes + sanitizers + real ELF32 loader.
SDK headers from MEMORIES_SDK (SDK root). For emulation use MEMORIES_I386_CC
and MEMORIES_I386_RUNNER; exact commands/results via MEMORIES_TEST_RESULTS.
No external MOD is installed. Do not interpret these mocks as game validation.
"""
from pathlib import Path
import json,os,re,shlex,subprocess,sys,tempfile
mod=Path(__file__).resolve().parents[1];repo=mod.parents[1];results=[]
sdk=Path(os.environ.get('MEMORIES_SDK',repo/'tmp/pc/win32/sdk'))
env={**os.environ,'ASAN_OPTIONS':'detect_leaks=0'}
def run(name,args):
    args=list(map(str,args));p=subprocess.run(args,cwd=mod,env=env,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
    results.append(dict(name=name,status='PASS' if not p.returncode else 'FAIL',returncode=p.returncode,command=args,output=p.stdout))
    print(name,results[-1]['status'],flush=True)
    if p.returncode:print(p.stdout[-2500:],flush=True)
    return p.returncode==0
with tempfile.TemporaryDirectory() as temp:
    temp=Path(temp)
    recipes=[shlex.split(line)[1:] for line in (mod/'tests/run_all_beta.sh').read_text().splitlines() if line.startswith('compile ')]
    recipes += [['drop_rewards'],['starter_choice','starter_rules.c','profiles.c']]
    for name,*sources in recipes:
        exe=temp/name
        if run(name+'-build',['gcc','-DRD_TEST_PROVIDER','-O1','-g','-std=gnu11','-fsanitize=address,undefined','-fno-omit-frame-pointer','-Itests/mocks','-I.','-I'+str(sdk/'include'),f'tests/test_{name}.c',*sources,'-o',exe]):run(name,[exe])
    for p in sorted((mod/'tests').glob('*.py')):
        if p.name==Path(__file__).name:continue
        run(p.stem,[sys.executable,p])
    exe=temp/'loader'
    if run('preview2_loader-build', [*shlex.split(os.environ.get('MEMORIES_I386_CC','gcc')),'-m32','-std=gnu11','-O1','-DMEMORIES_PC','-I'+str(repo/'src'),mod/'tests/preview2_loader.c',repo/'src/pc/mods/object_loader.c',repo/'src/pc/mods/mod_libc.c',repo/'src/pc/guest/branch_thunks.c',repo/'src/pc/compat/fs.c','-lm','-o',exe]):
        run('preview2_loader',[*shlex.split(os.environ.get('MEMORIES_I386_RUNNER','')),exe,mod/'ritual-duelists.o',mod])
results.extend([
 dict(name='gameplay-Windows-save-load-render-audio-controller',status='SKIP',reason='No game image or Windows runtime; synthetic engine only.'),
 dict(name='Droplist-1.4.0-external-binary-runtime',status='SKIP',reason='External package not supplied; own boundary regression executed.'),
 dict(name='ecosystem-integration-old-savestates-other-targets',status='SKIP',reason='Isolated i386 scope; partners inspected statically, no historic savestate claim.')])
target=os.environ.get('MEMORIES_TEST_RESULTS')
if target:Path(target).write_text(json.dumps(results,ensure_ascii=False,indent=2)+'\n')
print('SUMMARY (build steps separate):',{s:sum(r['status']==s and not r['name'].endswith('-build') for r in results) for s in ['PASS','FAIL','SKIP']})
sys.exit(any(r['status']=='FAIL' for r in results))
