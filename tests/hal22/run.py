"""HAL-22 I2 diagnosis. Reuses exact existing AI/Dual and native engine objects.
No dependencies downloaded, no MOD rebuilt. Exit 1 retains diagnostic FAILs;
exit 2 means incomplete/blocked execution, never a production verdict.
"""
from pathlib import Path
import argparse, collections, csv, hashlib, json, os, re, shlex, subprocess, tempfile, time
repo = Path(__file__).resolve().parents[2]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--output', type=Path, required=True)
args = parser.parse_args()
out = args.output.resolve()
out.mkdir(parents=True, exist_ok=True)
metadata = json.loads((repo/'notes/hal-22/INPUTS.json').read_text())
cc = shlex.split(os.environ.get('MEMORIES_I386_CC', 'gcc'))
runner = shlex.split(os.environ.get('MEMORIES_I386_RUNNER', ''))
ai = Path(os.environ.get('MEMORIES_AI_DIR', '/nonexistent/ai-hard-mode')).resolve()
dual = Path(os.environ.get('MEMORIES_DUAL_DIR', '/nonexistent/private-dual')).resolve()
native = Path(os.environ.get('MEMORIES_ENGINE_OBJ_DIR', '/nonexistent/game32/obj')).resolve()
sha = lambda p: hashlib.sha256(p.read_bytes()).hexdigest()
results = {'scope':'HAL-22 I2 only; no Windows gameplay', 'preflight':[], 'builds':[], 'profiles':[], 'cross_profile':[], 'limitations':[
    {'name':'Gameplay Windows / complete duel / real opponent decks', 'status':'BLOCKED', 'reason':'No game image/catalog/decks/runtime; synthetic catalog and constructed board cannot prove live-game reachability.'},
    {'name':'Full effect/placement/field resolution', 'status':'MODEL', 'reason':'Actual hand controller consumes cards; simple burn/placement-to-field and UI completion are modeled between actions.'},
    {'name':'Native stock AI interpreter when AI OFF / no-plan fallback', 'status':'SKIP', 'reason':'AiScript_Init is actual code; AiScript_Run base fallback returns zero without executing disc bytecode. AI OFF test covers materializer chain only.'},
    {'name':'Full Mods_Apply/live UI lifecycle', 'status':'STATIC+MODEL', 'reason':'Actual manager ordering and hooks executed; late first load emulates activation sequence confirmed by mods.c, not full UI.'},
    {'name':'Global automatic order-only fix', 'status':'UNPROVEN', 'reason':'Safe startup H<D tested; late first-load can register H after already-loaded D despite configured H<D.'},
    {'name':'Production fix', 'status':'NOT_IMPLEMENTED', 'reason':'Diagnostic task only; no source, manifest, SDK or MOD objects changed.'},
]}
for item in metadata['local_dependencies']:
    path = (ai if item['id']=='ai-hard-mode' else dual)/item['path']
    results['preflight'].append({'name':item['id']+'/'+item['path'], 'status':'PASS' if path.is_file() and sha(path)==item['sha256'] else 'BLOCKED', 'expected_sha256':item['sha256'], 'actual_sha256':sha(path) if path.is_file() else None})
for group, base in [('native_engine_objects',native), ('public_source_files',repo)]:
    for item in metadata[group]:
        path = base/item['path']
        results['preflight'].append({'name':item['path'], 'status':'PASS' if path.is_file() and sha(path)==item['sha256'] else 'BLOCKED', 'expected_sha256':item['sha256'], 'actual_sha256':sha(path) if path.is_file() else None})

def execute(name, command):
    command = list(map(str, command));start = time.monotonic()
    p = subprocess.run(command,cwd=repo,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
    (out/(name+'.log')).write_text(p.stdout)
    return {'command':command,'returncode':p.returncode,'seconds':round(time.monotonic()-start,4),'log':name+'.log'},p.stdout

def finish():
    counts = collections.Counter(x['status'] for p in results['profiles'] for x in p['cases'])
    results['summary'] = {'profiles':len(results['profiles']),'checks':dict(counts),'cross_profile':dict(collections.Counter(x['status'] for x in results['cross_profile']))}
    (out/'RESULTADOS.json').write_text(json.dumps(results,ensure_ascii=False,indent=2)+'\n')
    with (out/'MATRIZ.csv').open('w',newline='') as f:
        w = csv.writer(f,lineterminator='\n');w.writerow(['profile','scenario','check','status','details','evidence'])
        for p in results['profiles']:
            for c in p['cases']:w.writerow([p['name'],c['scenario'],c['name'],c['status'],c['details'],p['execution']['log']])
        for c in results['limitations']:w.writerow(['global','',c['name'],c['status'],c['reason'],'RESULTADOS.json'])
    print('SUMMARY',results['summary'],flush=True)
if any(p['status']!='PASS' for p in results['preflight']):
    finish();raise SystemExit(2)
with tempfile.TemporaryDirectory(prefix='hal22-') as folder:
    folder = Path(folder);harness=folder/'harness';order=folder/'order'
    build_h=[*cc,'-m32','-std=gnu11','-O1','-DMEMORIES_PC','-Isrc','tests/hal22/harness.c','src/pc/mods/object_loader.c','src/pc/mods/mod_libc.c','src/pc/guest/branch_thunks.c','src/pc/compat/fs.c','src/pc/mods/hooks.c','src/pc/mods/events.c','src/pc/mods/json.c','-lm','-o',harness]
    build_o=[*cc,'-m32','-std=gnu11','-O1','-DMEMORIES_PC','-ffunction-sections','-fdata-sections','-Wl,--gc-sections','-Isrc','tests/hal22/order.c','src/pc/mods/manager.c','src/pc/mods/json.c','-o',order]
    for name,cmd in [('build-harness',build_h),('build-order',build_o)]:
        build,log=execute(name,cmd);results['builds'].append(build)
        if build['returncode']:finish();raise SystemExit(2)
    for variant,mode,name in [(0,'all','O0'),(1,'all','O1'),(2,'all','O2'),(0,'late-ai-first-load','O0-late-AI')]:
        ordering,orderlog=execute(name+'-order',[*runner,order,ai,dual,variant])
        execution,log=execute(name,[*runner,harness,repo,ai,dual,native,variant,mode])
        cases=[];scenario='init'
        for line in log.splitlines():
            if line.startswith('CASE '):scenario=line[5:]
            match=re.match(r'RESULT (\S+) (PASS|FAIL) (.*)',line)
            if match:cases.append(dict(zip(['name','status','details'],match.groups()),scenario=scenario))
            if line.startswith('NOT_EXECUTED '):cases.append({'name':line.split()[1],'status':'BLOCKED','details':line,'scenario':scenario})
        if execution['returncode'] not in [0,2] or 'SUMMARY ' not in log:cases.append({'name':'fixture-completion','status':'ERROR','details':'Incomplete fixture, not a production failure.','scenario':'global'})
        cases.append({'name':'official-manager-order','status':'PASS' if ordering['returncode']==0 else 'ERROR','details':orderlog.strip(),'scenario':'global'})
        profile={'name':name,'requested_order':variant,'mode':mode,'ordering':ordering,'execution':execution,'cases':cases,'observations':re.findall(r'^(?:OBSERVED|BONUS_SNAPSHOT|LIFECYCLE) (.*)$',log,re.M)}
        results['profiles'].append(profile)
        print(name,dict(collections.Counter(x['status'] for x in cases)),flush=True)
    results['postflight'] = []
    for group, base in [('native_engine_objects',native), ('public_source_files',repo)]:
        for item in metadata[group]:
            p = base/item['path']
            results['postflight'].append({'name':item['path'],'status':'PASS' if p.is_file() and sha(p)==item['sha256'] else 'FAIL'})
    for item in metadata['local_dependencies']:
        p = (ai if item['id']=='ai-hard-mode' else dual)/item['path']
        results['postflight'].append({'name':item['id']+'/'+item['path'],'status':'PASS' if p.is_file() and sha(p)==item['sha256'] else 'FAIL'})
    # Compare initial plans, before any wrong ledger can affect bonus filtering.
    plans=[re.findall(r'first-id=(\d+) category=(\d+)', '\n'.join(p['observations'])) for p in results['profiles']]
    results['cross_profile'].append({'name':'same-unforced-initial-choices','status':'PASS' if plans and len(plans[0])==2 and all(x==plans[0] for x in plans) else 'FAIL','plans':plans})
finish()
if any(x['status']=='ERROR' for p in results['profiles'] for x in p['cases']):raise SystemExit(2)
raise SystemExit(int(any(x['status']=='FAIL' for p in results['profiles'] for x in p['cases']) or any(x['status']=='FAIL' for x in results['cross_profile']) or any(x['status']=='FAIL' for x in results['postflight'])))
