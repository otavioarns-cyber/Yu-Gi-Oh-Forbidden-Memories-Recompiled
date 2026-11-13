"""HAL-22 opt-in order-guard POC: experimental production diff, never a release.
Dual is a private local dependency. Only restart=true is added to temporary AI
manifest copies. A fresh OS process and the same persisted settings implement
restarts; the platform window restart callback is an observed test boundary.
Exit 1 keeps reproduced I2 invariants visible, 2 means incomplete/blocked run.
"""
from pathlib import Path
import argparse,collections,csv,hashlib,json,os,re,shlex,shutil,struct,subprocess,tempfile,time
repo=Path(__file__).resolve().parents[2]
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--output',type=Path,required=True)
parser.add_argument('--cohort',choices=['all','on','off'],default='all')
args=parser.parse_args();out=args.output.resolve();out.mkdir(parents=True,exist_ok=True)
meta=json.loads((repo/'notes/hal-22-24/PAIR_INPUTS.json').read_text())
ai=Path(os.environ.get('MEMORIES_AI_DIR','/nonexistent/ai')).resolve();dual=Path(os.environ.get('MEMORIES_DUAL_DIR','/nonexistent/dual')).resolve()
engine=Path(os.environ.get('MEMORIES_ENGINE_OBJ_DIR','/nonexistent/obj')).resolve()
sdk=Path(os.environ.get('MEMORIES_SDK_EXPORTS','/nonexistent/exports.i386.txt')).resolve()
cc=shlex.split(os.environ.get('MEMORIES_I386_CC','gcc'));runner=shlex.split(os.environ.get('MEMORIES_I386_RUNNER',''))
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
results={'scope':'HAL-22 experimental opt-in guard; real manager/window routines; partial game engine; no Windows gameplay','preflight':[],'builds':[],'profiles':[],'cross_profile':[], 'limitations':[
 {'name':'Full Windows gameplay and real opponent deck/catalog reachability','status':'BLOCKED','reason':'No game image or Windows duel runtime; partial engine/controller catalog retained from PR #15.'},
 {'name':'Platform restart/graphical window process handoff','status':'MODEL_BOUNDARY','reason':'Original staging/confirmation/Apply executed; Platform_RestartGame is observed success/failure stub, external runner launches fresh process. No desktop/mouse or platform exec claim.'},
 {'name':'Native stock AI when AI OFF','status':'SKIP','reason':'Materializer path tested; stock AiScript_Run VM fallback stays zero, without disc bytecode.'},
 {'name':'Other MODs and historical saves','status':'NOT_RUN','reason':'Real official/custom manifests tested for ordering; own delegating observers exercised. Other production MOD objects/gameplay and historical saves not integrated.'},
 {'name':'Definitive adoption','status':'NOT_APPROVED','reason':'Experimental source diff only on POC branch; candidate restart manifest generated in temporary copies.'}]}
checks=[]
for f in meta['local_dependencies']:checks.append((f, (ai if f['id']=='ai-hard-mode' else dual)/f['path']))
for f in meta['native_engine_objects']:checks.append((f,engine/f['path']))
for f in meta['public_source_files']:checks.append((f,repo/f['path']))
checks.append((meta['sdk_exports'],sdk))
own=Path(os.environ.get('MEMORIES_OWN_MODS_DIR','/nonexistent/own-mods')).resolve()
for item in meta['own_manifests']:checks.append((item,own/item['path']))
for f,p in checks:results['preflight'].append({'name':f.get('id','')+'/'+f['path'],'status':'PASS' if p.is_file() and sha(p)==f['sha256'] else 'BLOCKED','expected_sha256':f['sha256'],'actual_sha256':sha(p) if p.is_file() else None})

def finish():
 counts=collections.Counter(c['status'] for p in results['profiles'] for c in p['checks'])
 results['summary']={'cohorts':{str(g):dict(collections.Counter(c['status'] for p in results['profiles'] if p.get('guard')==g for c in p['checks'])) for g in [False,True]},'processes':len(results['profiles']),'checks':dict(counts),'cross_profile':dict(collections.Counter(c['status'] for c in results['cross_profile']))}
 (out/'RESULTADOS.json').write_text(json.dumps(results,ensure_ascii=False,indent=2)+'\n')
 with (out/'MATRIZ.csv').open('w',newline='') as file:
  w=csv.writer(file,lineterminator='\n');w.writerow(['profile','phase','candidate_restart','scenario','check','status','details','log'])
  for p in results['profiles']:
   for c in p['checks']:w.writerow([p['case'],p['phase'],int(p['restart']),c['scenario'],c['name'],c['status'],c['details'],p['execution']['log']])
 print('SUMMARY',results['summary'],flush=True)
if any(x['status']!='PASS' for x in results['preflight']):finish();raise SystemExit(2)

def execute(name,command,env=None):
 start=time.monotonic();command=list(map(str,command));p=subprocess.run(command,cwd=repo,env=env,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=60)
 (out/(name+'.log')).write_text(p.stdout)
 return {'command':command,'returncode':p.returncode,'seconds':round(time.monotonic()-start,4),'log':name+'.log'},p.stdout

def export_names(files):
 names=set();allowed=set(sdk.read_text().splitlines())
 for path in files:
  data=path.read_bytes();assert data[:6]==b'\x7fELF\x01\x01'
  # Include runtime symbol() requests only if the SDK exports that identifier.
  names.update(x[:-1].decode() for x in re.findall(rb'[A-Za-z_][A-Za-z_0-9]*\x00',data) if x[:-1].decode() in allowed)
  h=struct.unpack_from('<16sHHIIIIIHHHHHH',data);off,size,count=h[6],h[11],h[12]
  sections=[struct.unpack_from('<IIIIIIIIII',data,off+i*size) for i in range(count)]
  for sh in sections:
   if sh[1]!=2:continue
   st=sections[sh[6]];strings=data[st[4]:st[4]+st[5]]
   for at in range(sh[4],sh[4]+sh[5],sh[9]):
    sym=struct.unpack_from('<IIIBBH',data,at)
    if sym[0] and sym[-1]==0:names.add(strings[sym[0]:].split(b'\0')[0].decode())
 return sorted(names)

# Case: name, restart candidate, boot H/D, ranks H/D, folder scan inversion, actions across fresh processes.
cases=[
 ('safe-original',False,(1,1),(0,1),False,['none']),
 ('safe-restart',True,(1,1),(0,1),False,['none']),
 ('late-ai-apply-original',False,(0,1),(0,1),False,['late-apply','none']),
 ('late-ai-direct-original',False,(0,1),(0,1),False,['late-direct','none']),
 ('late-ai-reload-original',False,(0,1),(0,1),False,['late-reload','none']),
 ('late-ai-apply-restart',True,(0,1),(0,1),False,['late-apply','none']),
 ('late-ai-direct-restart',True,(0,1),(0,1),False,['late-direct','none']),
 ('late-ai-reload-restart',True,(0,1),(0,1),False,['late-reload','none']),
 ('late-dual-original',False,(1,0),(0,1),False,['late-dual','none']),
 ('late-dual-restart',True,(1,0),(0,1),False,['late-dual','none']),
 ('off-on-original',False,(1,1),(0,1),False,['off-on','none']),
 ('off-on-restart',True,(1,1),(0,1),False,['off-on','none']),
 ('persist-off-restart',True,(1,1),(0,1),False,['persist-off','late-apply','none']),
 ('window-first-ai-original',False,(0,1),(0,1),False,['window-ai','none']),
 ('window-first-ai-restart',True,(0,1),(0,1),False,['window-ai','none']),
 ('window-cancel-restart',True,(0,1),(0,1),False,['window-cancel','none']),
 ('rank-invert-original',False,(1,1),(0,1),False,['rank-invert','none']),
 ('rank-invert-restart',True,(1,1),(0,1),False,['rank-invert','none']),
 ('rank-repair-restart',True,(1,1),(1,0),False,['rank-repair','none']),
 ('restart-fails-repair',True,(1,1),(1,0),False,['restart-fails','none']),
 ('equal-rank-ai-first',True,(1,1),(0,0),False,['none']),
 ('equal-rank-dual-first',True,(1,1),(0,0),True,['none']),
 ('only-ai-safe',True,(1,0),(1,0),False,['none']),
 ('only-dual-safe',True,(0,1),(1,0),False,['none']),
 ('observers-between',True,(1,1),(0,2),False,['off-on']),
 ('observers-around',True,(1,1),(1,3),False,['none']),
 ('arm-unsafe',True,(1,1),(1,0),False,['arm-unsafe']),
 ('arm-repair-fails',True,(1,1),(1,0),False,['arm-repair-fails','none']),
 ('arm-repair-cancel',True,(1,1),(1,0),False,['arm-repair-cancel','none']),
 ('arm-pending-off',True,(1,1),(1,0),False,['arm-pending-off']),
]
with tempfile.TemporaryDirectory(prefix='hal22-order-guard-') as temp:
 temp=Path(temp);header=temp/'export_names.h';names=export_names([ai/'ai-hard-mode.o',dual/'dual-play.o']+[engine/f['path'] for f in meta['native_engine_objects']])
 names=sorted(set(names)|{'Duel_DrawFieldCards'})
 header.write_text('#define TEST_EXPORT_COUNT '+str(len(names))+'\nstatic const char*test_export_names[]={'+','.join(json.dumps(n) for n in names)+'};\n')
 results['export_table']={'entries':len(names),'generation':'Undefined ELF32 imports plus exact identifiers in binaries matching official SDK exports; test-owned pointers populated before Mods_Load.'}
 guard_main=temp/'guard_main.o'
 build,log=execute('build-duel-entry',[*cc,'-D_LANGUAGE_C','-DLANGUAGE_C','-m32','-std=gnu11','-O1','-DMEMORIES_PC','-Isrc','-fno-pic','-fno-pie','-fno-stack-protector','-fno-asynchronous-unwind-tables','-fno-unwind-tables','-c','src/game/main_run_duel.c','-o',guard_main]);results['builds'].append(build)
 if build['returncode']:finish();raise SystemExit(2)
 binary=temp/'harness'
 sources=['tests/hal2224-pair/harness.c','tests/hal2224-pair/window_bridge.c','src/pc/compat/fs.c','src/pc/mods/mods.c','src/pc/mods/events.c','src/pc/mods/hooks.c','src/pc/mods/manager.c','src/pc/mods/overlap.c','src/pc/mods/mod_libc.c','src/pc/mods/object_loader.c','src/pc/guest/branch_thunks.c','src/pc/mod_packets.c','src/pc/mods/json.c','src/pc/platform/settings.c','src/pc/platform/paths.c']
 build,log=execute('build-harness',[*cc,'-m32','-std=gnu11','-O1','-D_FILE_OFFSET_BITS=64','-DMEMORIES_PC','-ffunction-sections','-fdata-sections','-I'+str(temp),'-Isrc',*sources,'-Wl,--gc-sections','-Wl,--wrap=Hooks_Add','-Wl,--wrap=Mods_RegisterState','-Wl,--wrap=ObjectLoader_Load','-lpng','-lm','-lpthread','-o',binary]);results['builds'].append(build)
 if build['returncode']:finish();raise SystemExit(2)
 observer=temp/'observer.o'
 build,log=execute('build-observer',[*cc,'-m32','-std=gnu11','-O1','-DMEMORIES_PC','-Isrc','-fno-pic','-fno-pie','-fno-stack-protector','-fno-asynchronous-unwind-tables','-c','tests/hal2224-pair/observer.c','-o',observer]);results['builds'].append(build)
 if build['returncode']:finish();raise SystemExit(2)
 order=temp/'order'
 build,log=execute('build-order',[*cc,'-m32','-std=gnu11','-O1','-DMEMORIES_PC','-ffunction-sections','-fdata-sections','-Wl,--gc-sections','-Isrc','tests/hal2224-pair/order.c','src/pc/mods/manager.c','src/pc/mods/json.c','-o',order]);results['builds'].append(build)
 if build['returncode']:finish();raise SystemExit(2)
 own=Path(os.environ.get('MEMORIES_OWN_MODS_DIR','/nonexistent/own-mods')).resolve()
 for item in meta['own_manifests']:
  if not (own/item['path']).is_file() or sha(own/item['path'])!=item['sha256']:raise SystemExit('BLOCKED: exact prior own manifests required')
 regression,log=execute('order-regression', [*runner,order,repo/'mods',dual,own])
 results['cross_profile'].append({'name':'official-and-own-order-regression','status':'PASS' if regression['returncode']==0 else 'FAIL','execution':regression})
 hooks=temp/'hooks'
 build,log=execute('build-official-hooks',[*cc,'-m32','-std=gnu11','-O0','-DMEMORIES_PC','-Isrc','tests/pc/hooks_test.c','src/pc/mods/hooks.c','src/pc/compat/fs.c','-o',hooks]);results['builds'].append(build)
 if build['returncode']:finish();raise SystemExit(2)
 regression,log=execute('official-hooks-regression', [*runner,hooks]);results['cross_profile'].append({'name':'official-hooks-regression','status':'PASS' if regression['returncode']==0 else 'FAIL','execution':regression})
 for guard in [False,True]:
  if args.cohort!='all' and guard!=(args.cohort=='on'):continue
  for name,restart,boot,ranks,invert,actions in cases:
   if name.startswith('arm-') and not guard:continue
   case_name=('guard-on-' if guard else 'guard-off-')+name;root=temp/case_name;root.mkdir();manifest_hashes={}
   for mod,src in [('ai-hard-mode',ai),('dual-play',dual)]:
    folder=root/'mods'/('00-dual' if invert and mod=='dual-play' else 'zz-ai' if invert else mod);folder.mkdir(parents=True)
    shutil.copy2(src/(mod+'.o'),folder/(mod+'.o'))
    manifest=json.loads((src/'mod.json').read_text())
    if mod=='ai-hard-mode' and restart:
     manifest['restart']=True;(folder/'mod.json').write_text(json.dumps(manifest,ensure_ascii=False,indent=2)+'\n')
    else:shutil.copy2(src/'mod.json',folder/'mod.json')
    actual=json.loads((folder/'mod.json').read_text());expected=json.loads((src/'mod.json').read_text())
    if mod=='ai-hard-mode' and restart:expected['restart']=True
    assert actual==expected;assert sha(src/(mod+'.o'))==sha(folder/(mod+'.o'))
    manifest_hashes[mod]=sha(folder/'mod.json')
   if name.startswith('observers-'):
    for i,label in enumerate(['a','b']):
     f=root/'mods'/('hal22-observer-'+label);f.mkdir();shutil.copy2(observer,f/'observer.o');(f/'mod.json').write_text(json.dumps({'id':'hal22-observer-'+label,'library':'observer','enabled':True}))
   settings=root/'settings.txt';settings.write_text(f'experimental.hal2224_order_guard={int(guard and not name.startswith("arm-"))}\nmod.ai-hard-mode={boot[0]}\nmod.dual-play={boot[1]}\nmod.ai-hard-mode.order={ranks[0]}\nmod.dual-play.order={ranks[1]}\n')
   if name=='observers-between':
    with settings.open('a') as f:f.write('mod.hal22-observer-a.order=1\nmod.hal22-observer-b.order=3\n')
   if name=='observers-around':
    with settings.open('a') as f:f.write('mod.hal22-observer-a.order=0\nmod.hal22-observer-b.order=2\n')
   env={k:v for k,v in os.environ.items() if not k.startswith('MEMORIES_MOD')}
   env.update(MEMORIES_MODS_DIR=str(root/'mods'),MEMORIES_USER_DIR=str(root/'user'),MEMORIES_SETTINGS=str(settings))
   previous_pid=None
   for phase,action in enumerate(actions):
    tag=f'{case_name}-p{phase}';execution,log=execute(tag,[*runner,binary,repo,engine,action,phase,guard_main],env)
    rows=[];scenario='init'
    for line in log.splitlines():
     if line.startswith('CASE '):scenario=line[5:]
     match=re.match(r'RESULT (\S+) (PASS|FAIL) (.*)',line)
     if match:rows.append(dict(zip(['name','status','details'],match.groups()),scenario=scenario))
     if line.startswith('NOT_EXECUTED '):rows.append({'scenario':scenario,'name':line.split()[1],'status':'BLOCKED','details':line})
     if line.startswith('NOT_RUN '):rows.append({'scenario':'global','name':'controller-suite','status':'SKIP','details':line})
    if execution['returncode'] not in [0,1] or 'SUMMARY ' not in log:rows.append({'scenario':'global','name':'complete-execution','status':'ERROR','details':'Fixture incomplete; do not classify as production failure.'})
    pid_match=re.search(r'^PROCESS phase=\d+ pid=(\d+)$',log,re.M);pid=int(pid_match.group(1)) if pid_match else None
    if phase:rows.append({'scenario':'restart','name':'fresh-process','status':'PASS' if pid and pid!=previous_pid else 'ERROR','details':f'Previous pid={previous_pid}; new pid={pid}; same settings file reloaded by Settings_Load/Mods_Load.'})
    profile={'case':case_name,'guard':guard,'phase':phase,'action':action,'restart':restart,'initial_ranks':ranks,'folder_inverted':invert,'manifest_sha256':manifest_hashes,'execution':execution,'pid':pid,'settings_after':settings.read_text(),'checks':rows,'observations':re.findall(r'^(?:LIFECYCLE|PROBE|WINDOW|DEFAULTS|OBSERVED|BONUS_SNAPSHOT|ADMISSION|VISIBLE_ERROR|TRANSACTION|ARMED_GUARD) (.*)$',log,re.M)}
    results['profiles'].append(profile);previous_pid=pid
    print(tag,dict(collections.Counter(x['status'] for x in rows)),flush=True)
 # Compare unforced FIRST planner choices, unaffected by wrong bonus accounting.
 plans=[re.findall(r'first-id=(\d+) category=(\d+)','\n'.join(p['observations'])) for p in results['profiles']]
 executed=[p for p in plans if p]
 results['cross_profile'].append({'name':'same-unforced-first-choices','status':'PASS' if executed and all(p==[('344','2'),('35','1')] for p in executed) else 'FAIL','executed_suites':len(executed)})
results['postflight']=[{'name':f.get('id','')+'/'+f['path'],'status':'PASS' if p.is_file() and sha(p)==f['sha256'] else 'FAIL'} for f,p in checks]
finish()
if any(c['status']=='ERROR' for p in results['profiles'] for c in p['checks']):raise SystemExit(2)
raise SystemExit(int(any(c['status']=='FAIL' for p in results['profiles'] for c in p['checks']) or any(c['status']=='FAIL' for c in results['cross_profile']+results['postflight'])))
