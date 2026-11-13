"""HAL24 diagnostic only. No pending/plan/quota writes in the main fixture.
Reuses exact API24 objects and native units. Builds own harness + observer only.
Exit1 preserves diagnostic FAIL; exit2 denotes blocked/incomplete execution.
"""
from pathlib import Path
import argparse,collections,csv,hashlib,itertools,json,os,re,shlex,shutil,struct,subprocess,tempfile,time
repo=Path(__file__).resolve().parents[2]
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,required=True)
out=p.parse_args().output.resolve();out.mkdir(parents=True,exist_ok=True)
meta=json.loads((repo/'notes/hal-22-24/INPUTS.json').read_text())
dirs={m:Path(os.environ.get(env,'/nonexistent/'+m)).resolve() for m,env in [('ai-hard-mode','MEMORIES_AI_DIR'),('dual-play','MEMORIES_DUAL_DIR'),('ritual-duelists','MEMORIES_RD_DIR'),('melhoria-dos-rituais','MEMORIES_RITUALS_DIR')]}
engine=Path(os.environ.get('MEMORIES_ENGINE_OBJ_DIR','/nonexistent/obj')).resolve();sdk=Path(os.environ.get('MEMORIES_SDK_EXPORTS','/nonexistent/sdk')).resolve()
cc=shlex.split(os.environ.get('MEMORIES_I386_CC','gcc'));runner=shlex.split(os.environ.get('MEMORIES_I386_RUNNER',''))
sha=lambda f:hashlib.sha256(f.read_bytes()).hexdigest()
results={'manifest_copy_checks':[],'scope':'HAL22+24 opt-in guard; real controller/planners; partial engine, no Windows gameplay','preflight':[],'builds':[],'profiles':[],'postflight':[]}
checks=[(f,dirs[f['id']]/f['path']) for f in meta['local_dependencies']]+[(f,engine/f['path']) for f in meta['native_engine_objects']]+[(f,repo/f['path']) for f in meta['public_source_files']]+[(meta['sdk_exports'],sdk)]
for f,path in checks:results['preflight'].append({'path':f.get('id','')+'/'+f['path'],'expected_sha256':f['sha256'],'actual_sha256':sha(path) if path.is_file() else None,'status':'PASS' if path.is_file() and sha(path)==f['sha256'] else 'BLOCKED'})
def finish():
 results['summary']={'processes':len(results['profiles']),'checks':dict(collections.Counter(c['status'] for q in results['profiles'] for c in q['checks'])),'blocked':sum(q['execution']['returncode'] not in (0,1) or not q['complete'] for q in results['profiles'])}
 (out/'RESULTADOS.json').write_text(json.dumps(results,ensure_ascii=False,indent=2)+'\n')
 with (out/'MATRIZ.csv').open('w',newline='') as file:
  w=csv.writer(file,lineterminator='\n');w.writerow(['case','order','AI_requested','Dual_requested','restart','variant','lifecycle','rules','phase','check','status','details','log'])
  for q in results['profiles']:
   for c in q['checks']:w.writerow([q['case'],*[q[k] for k in ('order','ai','dual','restart','variant','lifecycle','rules','phase')],c['name'],c['status'],c['details'],q['execution']['log']])
 results['summary']['cohorts']={str(g):dict(collections.Counter(c['status'] for q in results['profiles'] if q['guard']==g and not q['lifecycle'].startswith('arm-') for c in q['checks'])) for g in (0,1)}
 (out/'RESULTADOS.json').write_text(json.dumps(results,ensure_ascii=False,indent=2)+'\n')
 print('SUMMARY',results['summary'],flush=True)
if any(q['status']!='PASS' for q in results['preflight']):finish();raise SystemExit(2)
def execute(name,command,env=None):
 start=time.monotonic();r=subprocess.run(list(map(str,command)),cwd=repo,env=env,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=60)
 (out/(name+'.log')).write_text(r.stdout)
 return {'returncode':r.returncode,'seconds':round(time.monotonic()-start,4),'log':name+'.log'},r.stdout

def exports(files):
 names={'HAL2224_AfterPlanner','Duel_DrawFieldCards'};allowed=set(sdk.read_text().splitlines())
 for f in files:
  data=f.read_bytes();assert data[:6]==b'\x7fELF\x01\x01'
  names.update(x[:-1].decode() for x in re.findall(rb'[A-Za-z_][A-Za-z_0-9]*\x00',data) if x[:-1].decode() in allowed)
  h=struct.unpack_from('<16sHHIIIIIHHHHHH',data);off,size,count=h[6],h[11],h[12];sections=[struct.unpack_from('<IIIIIIIIII',data,off+i*size) for i in range(count)]
  for sh in sections:
   if sh[1]!=2:continue
   st=sections[sh[6]];strings=data[st[4]:st[4]+st[5]]
   for at in range(sh[4],sh[4]+sh[5],sh[9]):
    s=struct.unpack_from('<IIIBBH',data,at)
    if s[0] and s[-1]==0:names.add(strings[s[0]:].split(b'\0')[0].decode())
 return sorted(names)
cases=[]
def case(o,h,d,v,method='none',restart=1,rules='absent',rd=1,guard=1,tie=0,wrapper='none',reverse=0,wrank=50):
 name=f'g{guard}-{o}-h{h}-r{rd}-d{d}-v{v}-{method}-restart{restart}-{rules}-t{tie}-w{wrapper}-x{reverse}-p{wrank}'
 assert not any(x['case']==name for x in cases)
 cases.append(dict(case=name,order=o,ai=h,dual=d,variant=v,lifecycle=method,restart=restart,rules=rules,rd=rd,guard=guard,tie=tie,wrapper=wrapper,reverse=reverse,wrank=wrank))
# PR19 matrix retained for the protected cohort, with controls exposing I1/I2.
for o,h,d,v in itertools.product(('O0','O1','O2'),(0,1),(0,1),range(6)):case(o,h,d,v)
for o,h,v in itertools.product(('O0','O1','O2'),(0,1),(6,7,8)):case(o,h,1,v,rules={6:'field',7:'hand',8:'both'}[v])
for o,method,r in itertools.product(('O0','O1','O2'),('late-ai','late-dual','late-rd','cycle-ai','cycle-dual','cycle-rd','reload'),(0,1)):case(o,1,1,5,method,r)
# Every proper subset, including individual mods and no-Dual composition.
for o,h,r,d in itertools.product(('O0','O1','O2'),(0,1),(0,1),(0,1)):
 if r==0:case(o,h,d,5,rd=r)
for o,reverse in itertools.product(('O1','O2'),(0,1)):case(o,1,1,5,tie=1,reverse=reverse)
case('O1',1,1,5,tie=1,reverse=2)
case('O1',1,1,5,wrapper='late-swallow')
case('O1',1,1,5,wrapper='late-transparent')
for wrank,wrapper in itertools.product((1,3),('transparent','swallow','controller')):case('O1',1,1,5,wrapper=wrapper,wrank=wrank)
for o in ('O0','O1','O2'):
 for wrapper in ('transparent','swallow','unrelated'):case(o,1,1,5,wrapper=wrapper)
for method in ('window-invert','window-ai','window-ai-cancel','window-ai-fail'):
 case('O1',1,1,5,method)
for method in ('window-repair','window-repair-cancel','window-repair-fail'):
 case('O0',1,1,5,method)
for method in ('arm-none','arm-window-repair','arm-window-repair-cancel','arm-window-repair-fail'):
 case('O0',1,1,5,method,guard=0)
for o,h,v in itertools.product(('O0','O1','O2'),(0,1),(0,5)):case(o,h,1,v,guard=0,restart=0)
with tempfile.TemporaryDirectory(prefix='hal24-') as tmp:
 tmp=Path(tmp);objects={m:('rituals.o' if m=='melhoria-dos-rituais' else m+'.o') for m in dirs};names=exports([dirs[m]/objects[m] for m in dirs]+[engine/f['path'] for f in meta['native_engine_objects']]);(tmp/'export_names.h').write_text('#define TEST_EXPORT_COUNT '+str(len(names))+'\nstatic const char*test_export_names[]={'+','.join(json.dumps(n) for n in names)+'};\n')
 sources=['tests/hal2224/harness.c','tests/hal2224/window_bridge.c','src/pc/compat/fs.c','src/pc/mods/mods.c','src/pc/mods/events.c','src/pc/mods/hooks.c','src/pc/mods/manager.c','src/pc/mods/overlap.c','src/pc/mods/mod_libc.c','src/pc/mods/object_loader.c','src/pc/guest/branch_thunks.c','src/pc/mod_packets.c','src/pc/mods/json.c','src/pc/platform/settings.c','src/pc/platform/paths.c']
 guard_main=tmp/'src_game_guard_main.c.o'
 execution,log=execute('build-duel-entry',cc+['-D_LANGUAGE_C','-DLANGUAGE_C','-m32','-std=gnu11','-O1','-DMEMORIES_PC','-Isrc','-fno-pic','-fno-pie','-fno-stack-protector','-fno-asynchronous-unwind-tables','-fno-unwind-tables','-c','src/game/main_run_duel.c','-o',guard_main]);results['builds'].append(execution)
 if execution['returncode']:finish();raise SystemExit(2)
 for name,command in [('build-harness',cc+['-m32','-std=gnu11','-O1','-D_FILE_OFFSET_BITS=64','-DMEMORIES_PC','-ffunction-sections','-fdata-sections','-I'+str(tmp),'-Isrc',*sources,'-Wl,--gc-sections','-Wl,--wrap=Hooks_Add','-Wl,--wrap=Mods_RegisterState','-Wl,--wrap=ObjectLoader_Load','-lpng','-lm','-lpthread','-o',tmp/'harness']),('build-observer',cc+['-m32','-std=gnu11','-O1','-DMEMORIES_PC','-Isrc','-fno-pic','-fno-pie','-fno-stack-protector','-fno-asynchronous-unwind-tables','-c','tests/hal2224/observer.c','-o',tmp/'observer.o'])]:
  execution,log=execute(name,command);results['builds'].append(execution)
  if execution['returncode']:finish();raise SystemExit(2)
 execution,log=execute('build-wrapper',cc+['-m32','-std=gnu11','-O1','-DMEMORIES_PC','-Isrc','-fno-pic','-fno-pie','-fno-stack-protector','-fno-asynchronous-unwind-tables','-c','tests/hal2224/wrapper.c','-o',tmp/'wrapper.o']);results['builds'].append(execution)
 if execution['returncode']:finish();raise SystemExit(2)
 results['observer_object_sha256']=sha(tmp/'observer.o')
 for i,q in enumerate(cases):
  folder=tmp/q['case'];folder.mkdir();mods=folder/'mods';mods.mkdir()
  for m,src in {**dirs,'drop-missing-cards':repo/'mods/drop-missing-cards'}.items():
   if m=='melhoria-dos-rituais' and q['rules']=='absent':continue
   dest=mods/((('0' if m=='ai-hard-mode' else '2' if m=='dual-play' else '1') if q['reverse']==2 else ('0' if m=='dual-play' else '2' if m=='ai-hard-mode' else '1'))+'-'+m if q['reverse'] else m);dest.mkdir();manifest=json.loads((src/'mod.json').read_text())
   if m=='ai-hard-mode' and q['restart']:
    manifest['restart']=True;(dest/'mod.json').write_text(json.dumps(manifest,ensure_ascii=False))
   else:
    shutil.copy2(src/'mod.json',dest/'mod.json')
    assert sha(src/'mod.json')==sha(dest/'mod.json')
    results['manifest_copy_checks'].append({'case':q['case'],'id':m,'sha256':sha(dest/'mod.json'),'status':'PASS'})
   if m in dirs:shutil.copy2(src/objects[m],dest/objects[m])
  dest=mods/'hal2224-observer';dest.mkdir();shutil.copy2(tmp/'observer.o',dest/'observer.o');(dest/'mod.json').write_text(json.dumps({'id':'hal2224-observer','library':'observer','enabled':True}))
  if q['wrapper']!='none':
   w=mods/'hal2224-wrapper';w.mkdir();shutil.copy2(tmp/'wrapper.o',w/'wrapper.o');(w/'mod.json').write_text(json.dumps({'id':'hal2224-wrapper','library':'wrapper','enabled':True}))
  order={'O0':('ai-hard-mode','dual-play','ritual-duelists'),'O1':('ai-hard-mode','ritual-duelists','dual-play'),'O2':('dual-play','ritual-duelists','ai-hard-mode')}[q['order']]
  settings=folder/'settings.txt';h=0 if q['lifecycle'].startswith('window-ai') or q['lifecycle']=='late-ai' else q['ai'];d=0 if q['lifecycle']=='late-dual' else q['dual'];settings.write_text(f'experimental.hal2224_order_guard={q["guard"]}\nmod.hal2224-wrapper.order={q["wrank"]}\nmod.hal2224-wrapper={0 if q["wrapper"].startswith("late-") else 1}\ntest.late_wrapper={int(q["wrapper"].startswith("late-"))}\nmod.hal2224-wrapper.controller={int(q["wrapper"]=="controller")}\nmod.hal2224-wrapper.swallow={int(q["wrapper"] in ("swallow","late-swallow"))}\nmod.hal2224-wrapper.unrelated={int(q["wrapper"]=="unrelated")}\nmod.ai-hard-mode={h}\nmod.dual-play={d}\nmod.ritual-duelists={0 if q["lifecycle"]=="late-rd" else q["rd"]}\nmod.drop-missing-cards=1\nmod.drop-missing-cards.order=-10\nmod.melhoria-dos-rituais.order=-5\nmod.hal2224-observer.order=100\n'+''.join(f'mod.{m}.order={0 if q["tie"] else i*2 if q["wrapper"]!="none" else i}\n' for i,m in enumerate(order))+ (f'mod.melhoria-dos-rituais.min_field={dict(field=3,hand=0,both=1)[q["rules"]]}\n' if q['rules']!='absent' else ''))
  env=dict(os.environ,MEMORIES_MODS_DIR=str(mods),MEMORIES_SETTINGS=str(settings),MEMORIES_USER_DIR=str(folder/'user'))
  for phase in (('main','reboot') if q['lifecycle'] in ('late-ai','late-dual','late-rd') or 'repair' in q['lifecycle'] or q['lifecycle'].startswith('window-ai') else ('main',)):
   execution,log=execute(q['case']+('-reboot' if phase=='reboot' else ''),runner+[tmp/'harness',repo,engine,str(q['variant']),q['lifecycle'],phase,tmp],env)
   item={**q,'phase':phase,'execution':execution,'complete':'SUMMARY ' in log,'checks':[{'name':n,'status':s,'details':d} for n,s,d in re.findall(r'^RESULT (\S+) (PASS|FAIL) (.*)$',log,re.M)],'observations':re.findall(r'^(?:ADMISSION|VISIBLE_ERROR|WINDOW|ACTION|BONUS |PLAN |RULES |SNAPSHOT|LIFECYCLE|NOT_REACHED|NOT_RUN).*$',log,re.M),'vm_completion_calls':log.count('MODEL_VM_COMPLETION')}
   if execution['returncode'] not in (0,1) or not item['complete']:item['checks'].append({'name':'complete-execution','status':'BLOCKED','details':'Unfiltered log; no result inferred.'})
   results['profiles'].append(item)
  if i%24==0:print('Completed',i+1,'/',len(cases),flush=True)
for f,path in checks:results['postflight'].append({'path':f.get('id','')+'/'+f['path'],'status':'PASS' if sha(path)==f['sha256'] else 'FAIL'})
finish()
if results['summary']['blocked'] or any(q['status']!='PASS' for q in results['postflight']):raise SystemExit(2)
raise SystemExit(1 if results['summary']['checks'].get('FAIL') else 0)
