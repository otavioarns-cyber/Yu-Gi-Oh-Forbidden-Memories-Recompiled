"""HAL-20 experimental integration, exact PR artifacts and local-only Dual.
Requires MEMORIES_AI_DIR, MEMORIES_DUAL_DIR, optional MEMORIES_I386_CC and
MEMORIES_I386_RUNNER. No dependency is downloaded or rebuilt automatically.
Exit nonzero on diagnostic failures; known F1 is not hidden as an xfail.
"""
from pathlib import Path
import argparse,collections,csv,hashlib,itertools,json,os,re,shlex,subprocess,tempfile,time
repo=Path(__file__).resolve().parents[2]
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--output',type=Path,required=True)
parser.add_argument('--stage',type=int,choices=[1,2,3,4])
parser.add_argument('--hm',type=int,choices=[0,1])
parser.add_argument('--dual',type=int,choices=[0,1])
parser.add_argument('--order',type=int,choices=[0,1,2])
parser.add_argument('--scenario',choices=['all','F1','rd-materialize','planner','transition','ritual'],default='all')
args=parser.parse_args();out=args.output.resolve();out.mkdir(parents=True,exist_ok=True)
cc=shlex.split(os.environ.get('MEMORIES_I386_CC','gcc'));prefix=shlex.split(os.environ.get('MEMORIES_I386_RUNNER',''))
ai=Path(os.environ.get('MEMORIES_AI_DIR',repo/'mods/ai-hard-mode')).resolve()
dual=Path(os.environ.get('MEMORIES_DUAL_DIR','/nonexistent/local-dual-play')).resolve()
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
metadata=json.loads((repo/'notes/hal-20/integracao-inicial/INPUTS.json').read_text())
preflight=[]
for component in metadata['components']:
    directory=repo/component['directory'];bad=[]
    actual={str(p.relative_to(directory)) for p in directory.rglob('*') if p.is_file() and '__pycache__' not in p.parts}
    expected={f['path'] for f in component['files']}
    for f in component['files']:
        p=directory/f['path']
        if not p.exists() or sha(p)!=f['sha256']:bad.append(f['path'])
    bad.extend(sorted(actual-expected))
    preflight.append(dict(name=component['id'],status='FAIL' if bad else 'PASS',mismatches=bad,files=len(expected)))
for name,path in [('ai-hard-mode',ai/'ai-hard-mode.o'),('dual-play',dual/'dual-play.o')]:
    expected=metadata['local_dependencies'][name]['sha256']
    status='PASS' if path.is_file() and sha(path)==expected else 'BLOCKED'
    preflight.append(dict(name=name,status=status,path=str(path),expected_sha256=expected,actual_sha256=sha(path) if path.is_file() else None))
results=dict(preflight=preflight,builds=[],profiles=[],limitations=[
 dict(name='Time Wizard',status='BLOCKED',reason='Core source/provenance unresolved; deliberately not loaded.'),
 dict(name='AI effect-aware adapter 0.1.1',status='BLOCKED',reason='Not supplied; official base 0.1.0 used.'),
 dict(name='Droplist 1.4.0',status='BLOCKED',reason='External package absent; internal viewer remains removed.'),
 dict(name='Full integrated drop balance',status='BLOCKED',reason='Drop Missing Cards enabled in actual manager profile; full disc catalog/table evaluation not implemented in synthetic engine.'),
 dict(name='Gameplay Windows/render/audio/controllers',status='SKIP',reason='No game image or Windows gameplay runtime.'),
 dict(name='Complete bonus-action award/turn loop',status='SKIP',reason='Quota gates tested; actual input/controller/award loop not simulated end to end.'),
 dict(name='Historical savestates',status='SKIP',reason='No old saves supplied; signatures/old state compatibility not bypassed.'),
 dict(name='All effects and full ecosystem',status='SKIP',reason='Selected integrated instances/transformations tested; not every effect/texture/strategy/game path.')])
def execute(name,command,cwd=repo):
    command=list(map(str,command));started=time.monotonic()
    p=subprocess.run(command,cwd=cwd,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
    (out/(name+'.log')).write_text(p.stdout)
    return dict(command=command,cwd=str(cwd),returncode=p.returncode,seconds=round(time.monotonic()-started,4),log=name+'.log'),p.stdout
def finish():
    results['summary']=dict(profiles=len(results['profiles']),checks=dict(collections.Counter(r['status'] for p in results['profiles'] for r in p['cases'])),global_limits=dict(collections.Counter(r['status'] for r in results['limitations'])))
    (out/'RESULTADOS.json').write_text(json.dumps(results,ensure_ascii=False,indent=2)+'\n')
    with (out/'MATRIZ.csv').open('w',newline='') as f:
        w=csv.writer(f,lineterminator="\n");w.writerow(['profile','stage','AI_HARD_MODE','Dual_Play','order','scenario','status','evidence'])
        for p in results['profiles']:
            for r in p['cases']:w.writerow([p['name'],p['stage'],p['hm'],p['dual'],p['order'],r['name'],r['status'],r.get('log',p['execution']['log'])])
        for r in results['limitations']:w.writerow(['global','','','','',r['name'],r['status'],'RESULTADOS.json'])
    counts=collections.Counter(r['status'] for p in results['profiles'] for r in p['cases'])
    print('SUMMARY',dict(counts),'profiles',len(results['profiles']),flush=True)
if any(p['status']!='PASS' for p in preflight):
    finish();raise SystemExit('BLOCKED: exact local dependencies/immutable PR artifacts required; see RESULTADOS.json')
with tempfile.TemporaryDirectory(prefix='hal20-integrated-') as temp:
    temp=Path(temp);harness=temp/'harness';order=temp/'order'
    for name,command in [
        ('build-harness',[*cc,'-m32','-std=gnu11','-O1','-DMEMORIES_PC','-Isrc','tests/hal20-integration/harness.c','src/pc/mods/object_loader.c','src/pc/mods/mod_libc.c','src/pc/guest/branch_thunks.c','src/pc/compat/fs.c','src/pc/mods/hooks.c','src/pc/mods/events.c','src/pc/mods/json.c','-lm','-o',harness]),
        ('build-order',[*cc,'-m32','-std=gnu11','-O1','-DMEMORIES_PC','-ffunction-sections','-fdata-sections','-Wl,--gc-sections','-Isrc','tests/hal20-integration/order.c','src/pc/mods/manager.c','src/pc/mods/json.c','-o',order])]:
        result,output=execute(name,command);results['builds'].append(result)
        if result['returncode']:finish();raise SystemExit('Build failed: '+name)
    profiles=[(stage,hm,dual_value,0) for stage in range(1,5) for hm,dual_value in itertools.product([0,1],repeat=2)]
    profiles += [(4,hm,dual_value,variant) for variant in [1,2] for hm,dual_value in itertools.product([0,1],repeat=2)]
    # A minimal counterexample, excluding Ritual/Fusion/RD, isolates the AI/Dual boundary.
    profiles.append((1,1,1,2))
    filters=[args.stage,args.hm,args.dual,args.order]
    for stage,hm,dual_value,variant in profiles:
        values=(stage,hm,dual_value,variant)
        if any(f is not None and f!=v for f,v in zip(filters,values)):continue
        name=f's{stage}-h{hm}-d{dual_value}-o{variant}'
        ordering,order_log=execute(name+'-order',[*prefix,order,repo,dual,stage,hm,dual_value,variant],temp)
        match=re.search(r'^EFFECTIVE_ORDER (.+)$',order_log,re.M)
        execution,output=execute(name,[*prefix,harness,repo,ai,dual,stage,hm,dual_value,variant,args.scenario],temp)
        cases=[dict(name=n,status=s,details=d) for n,s,d in re.findall(r'^RESULT (\S+) (PASS|FAIL) (.*)$',output,re.M)]
        if execution['returncode'] not in [0,2] or 'SUMMARY ' not in output:cases.append(dict(name='fixture-execution',status='ERROR',details='Incomplete fixture; do not classify as production failure.'))
        if ordering['returncode']:cases.append(dict(name='manager-order',status='ERROR',details='Ordering fixture failed.'))
        else:cases.append(dict(name='manager-order',status='PASS',details='Official Mods_Order; DMC ON; manifests unchanged.'))
        actual=re.findall(r'^LOADED owner=(\S+) .* enabled=1$',output,re.M)
        expected=[x for x in (match.group(1).split() if match else []) if x!='drop-missing-cards']
        cases.append(dict(name='manager-order-vs-initialized-hooks',status='PASS' if actual==expected else 'FAIL',details=f'actual={actual}; expected={expected}'))
        profiles_record=dict(name=name,stage=stage,hm=hm,dual=dual_value,order=variant,effective_order=match.group(1).split() if match else [],ordering=ordering,execution=execution,cases=cases,
                             observations=re.findall(r'^OBSERVED (.+)$',output,re.M),chains=re.findall(r'^CHAIN (.+)$',output,re.M))
        results['profiles'].append(profiles_record)
        print(name,dict(collections.Counter(r['status'] for r in cases)),flush=True)
finish()
raise SystemExit(any(r['status'] in ['FAIL','ERROR'] for p in results['profiles'] for r in p['cases']))
