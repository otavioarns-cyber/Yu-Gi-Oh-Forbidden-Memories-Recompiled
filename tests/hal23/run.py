"""HAL-23 diagnostic. Reuses hashed API24 objects; builds only own fixture.
No game image, Dual redistribution, pending injection, or production patch.
Exit 1 preserves diagnostic invariant failures; 2 denotes blocked execution.
"""
from pathlib import Path
import argparse, collections, csv, hashlib, itertools, json, os, re, shlex, shutil, struct, subprocess, tempfile, time
repo=Path(__file__).resolve().parents[2]
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,required=True)
out=p.parse_args().output.resolve();out.mkdir(parents=True,exist_ok=True)
meta=json.loads((repo/'notes/hal-23/INPUTS.json').read_text())
dirs={name:Path(os.environ.get(env,'/nonexistent/'+name)).resolve() for name,env in [('ai-hard-mode','MEMORIES_AI_DIR'),('dual-play','MEMORIES_DUAL_DIR'),('ritual-duelists','MEMORIES_RD_DIR')]}
engine=Path(os.environ.get('MEMORIES_ENGINE_OBJ_DIR','/nonexistent/obj')).resolve()
sdk=Path(os.environ.get('MEMORIES_SDK_EXPORTS','/nonexistent/exports.i386.txt')).resolve()
cc=shlex.split(os.environ.get('MEMORIES_I386_CC','gcc'));runner=shlex.split(os.environ.get('MEMORIES_I386_RUNNER',''))
sha=lambda f:hashlib.sha256(f.read_bytes()).hexdigest()
results={'scope':'HAL-23 diagnostic; actual manager, RD planner and selected native controller units; partially modeled motor, no Windows gameplay','preflight':[], 'builds':[], 'profiles':[], 'postflight':[]}
checks=[(f,dirs[f['id']]/f['path']) for f in meta['local_dependencies']]+[(f,engine/f['path']) for f in meta['native_engine_objects']]+[(f,repo/f['path']) for f in meta['public_source_files']]+[(meta['sdk_exports'],sdk)]
for f,path in checks:results['preflight'].append({'path':f.get('id','')+'/'+f['path'],'expected_sha256':f['sha256'],'actual_sha256':sha(path) if path.is_file() else None,'status':'PASS' if path.is_file() and sha(path)==f['sha256'] else 'BLOCKED'})

def finish():
 results['summary']={'processes':len(results['profiles']),'checks':dict(collections.Counter(c['status'] for q in results['profiles'] for c in q['checks'])),'blocked':sum(q['execution']['returncode'] not in (0,1) for q in results['profiles'])}
 (out/'RESULTADOS.json').write_text(json.dumps(results,ensure_ascii=False,indent=2)+'\n')
 with (out/'MATRIZ.csv').open('w',newline='') as file:
  w=csv.writer(file,lineterminator='\n');w.writerow(['case','order','dual','restart','method','boundary','direction','preloaded','aftermath','phase','check','status','details','log'])
  for q in results['profiles']:
   for c in q['checks']:w.writerow([q['case'],*[q[k] for k in ('order','dual','restart','method','boundary','direction','preloaded','aftermath','phase')],c['name'],c['status'],c['details'],q['execution']['log']])
 print('SUMMARY',results['summary'],flush=True)
if any(q['status']!='PASS' for q in results['preflight']):finish();raise SystemExit(2)

def execute(name,command,env=None):
 start=time.monotonic();command=list(map(str,command));r=subprocess.run(command,cwd=repo,env=env,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=60)
 (out/(name+'.log')).write_text(r.stdout)
 return {'returncode':r.returncode,'seconds':round(time.monotonic()-start,4),'log':name+'.log'},r.stdout

def exports(files):
 names={'HAL23_AfterPlanner'};allowed=set(sdk.read_text().splitlines())
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
def case(order,dual,restart,method,boundary,direction='off-on',preloaded=0,aftermath='none',reboot=False):
 name='-'.join(map(str,(order,'d'+str(dual),'r'+str(restart),method,boundary,direction,'p'+str(preloaded),aftermath)))
 assert name not in [q['case'] for q in cases]
 cases.append(dict(case=name,order=order,dual=dual,restart=restart,method=method,boundary=boundary,direction=direction,preloaded=preloaded,aftermath=aftermath,reboot=reboot))
# All manager paths, hook orders and actual first-load vs already-loaded cases.
for order,d,r,method,transition in itertools.product(('O0','O1','O2'),(0,1),(0,1),('apply','direct','reload'),('first','preloaded','on-off')):
 case(order,d,r,method,'gap','on-off' if transition=='on-off' else 'off-on',int(transition=='preloaded'))
# UI/frame scheduling controls: no test callback between planner and swap.
for order,d,r,boundary,transition in itertools.product(('O0','O1','O2'),(0,1),(0,1),('before','after'),('first','on-off')):
 case(order,d,r,'apply',boundary,'on-off' if transition=='on-off' else 'off-on')
# Actual window confirmation/Apply with restart success observed, cancellation/failure.
for d,r,method,boundary in itertools.product((0,1),(0,1),('window','window-cancel','window-fail'),('gap','after')):
 case('O2',d,r,method,boundary,reboot=(method=='window'))
# Conditional consequence and cleanup controls. No writes to private pending.
for d,r,aftermath in itertools.product((0,1),(0,1),('next','reset','cancel','deal','deal-unregistered','save')):
 case('O2',d,r,'apply','gap',aftermath=aftermath,reboot=(aftermath=='save'))
# Safe preloaded chain and atomic-boundary controls for follow-up consumption.
for order,d in itertools.product(('O0','O1','O2'),(0,1)):
 case(order,d,0,'apply','after',aftermath='next')
 case(order,d,0,'apply','gap',preloaded=1,aftermath='next')

with tempfile.TemporaryDirectory(prefix='hal23-') as tmp:
 tmp=Path(tmp);header=tmp/'export_names.h';names=exports([dirs[m]/(m+'.o') for m in dirs]+[engine/f['path'] for f in meta['native_engine_objects']])
 header.write_text('#define TEST_EXPORT_COUNT '+str(len(names))+'\nstatic const char*test_export_names[]={'+','.join(json.dumps(n) for n in names)+'};\n')
 sources=['tests/hal23/harness.c','tests/hal23/window_bridge.c','src/pc/compat/fs.c','src/pc/mods/mods.c','src/pc/mods/events.c','src/pc/mods/hooks.c','src/pc/mods/manager.c','src/pc/mods/overlap.c','src/pc/mods/mod_libc.c','src/pc/mods/object_loader.c','src/pc/guest/branch_thunks.c','src/pc/mod_packets.c','src/pc/mods/json.c','src/pc/platform/settings.c','src/pc/platform/paths.c']
 builds=[('build-harness',cc+['-m32','-std=gnu11','-O1','-D_FILE_OFFSET_BITS=64','-DMEMORIES_PC','-ffunction-sections','-fdata-sections','-I'+str(tmp),'-Isrc',*sources,'-Wl,--gc-sections','-Wl,--wrap=Hooks_Add','-Wl,--wrap=Mods_RegisterState','-Wl,--wrap=ObjectLoader_Load','-lpng','-lm','-lpthread','-o',str(tmp/'harness')]),('build-observer',cc+['-m32','-std=gnu11','-O1','-DMEMORIES_PC','-Isrc','-fno-pic','-fno-pie','-fno-stack-protector','-fno-asynchronous-unwind-tables','-c','tests/hal23/observer.c','-o',str(tmp/'observer.o')])]
 for name,cmd in builds:
  execution,log=execute(name,cmd);results['builds'].append(execution)
  if execution['returncode']:finish();raise SystemExit(2)
 results['test_object_sha256']=sha(tmp/'observer.o')
 for i,q in enumerate(cases):
  folder=tmp/q['case'];folder.mkdir();mods=folder/'mods';mods.mkdir()
  for name,src in {**dirs,'drop-missing-cards':repo/'mods/drop-missing-cards'}.items():
   dest=mods/name;dest.mkdir();manifest=json.loads((src/'mod.json').read_text())
   if name=='ai-hard-mode' and q['restart']:manifest['restart']=True
   (dest/'mod.json').write_text(json.dumps(manifest,ensure_ascii=False))
   if name in dirs:shutil.copy2(src/(name+'.o'),dest/(name+'.o'))
  observer=mods/'hal23-observer';observer.mkdir();shutil.copy2(tmp/'observer.o',observer/'observer.o');(observer/'mod.json').write_text(json.dumps({'id':'hal23-observer','library':'observer','enabled':True}))
  orders={'O0':('ai-hard-mode','dual-play','ritual-duelists'),'O1':('ai-hard-mode','ritual-duelists','dual-play'),'O2':('dual-play','ritual-duelists','ai-hard-mode')}[q['order']]
  boot_h=int(q['direction']=='on-off' or q['preloaded'])
  settings=folder/'settings.txt';settings.write_text(f'mod.ai-hard-mode={boot_h}\nmod.dual-play={q["dual"]}\nmod.ritual-duelists=1\nmod.drop-missing-cards=1\nmod.drop-missing-cards.order=-10\nmod.hal23-observer.order=100\n'+''.join(f'mod.{name}.order={rank}\n' for rank,name in enumerate(orders)))
  env=dict(os.environ,MEMORIES_MODS_DIR=str(mods),MEMORIES_SETTINGS=str(settings),MEMORIES_USER_DIR=str(folder/'user'))
  phases=(0,1) if q['reboot'] else (0,)
  for phase in phases:
   logname=q['case']+('-reboot' if phase else '')
   execution,log=execute(logname,runner+[tmp/'harness',repo,engine,q['method'],q['boundary'],q['direction'],str(q['preloaded']),q['aftermath'],str(phase)],env)
   item={k:v for k,v in q.items() if k!='reboot'};item.update(phase=phase,execution=execution,checks=[{'name':n,'status':s,'details':details} for n,s,details in re.findall(r'^RESULT (\S+) (PASS|FAIL) (.*)$',log,re.M)])
   item['observed']=re.findall(r'^(?:OBSERVED|FOLLOWUP|BOOT|CHANGE|PROFILE_SIGNATURE|WINDOW.*|SAVE_CHUNK|SIMULATED_LOAD|NOT_REACHED).*$',log,re.M)
   if execution['returncode'] not in (0,1) or 'SUMMARY ' not in log:item['checks'].append({'name':'complete-execution','status':'BLOCKED','details':'See unfiltered log; no success inferred.'})
   results['profiles'].append(item)
  if i%30==0:print('Completed',i+1,'/',len(cases),flush=True)
for f,path in checks:results['postflight'].append({'path':f.get('id','')+'/'+f['path'],'status':'PASS' if sha(path)==f['sha256'] else 'FAIL'})
finish()
if results['summary']['blocked'] or any(q['status']!='PASS' for q in results['postflight']):raise SystemExit(2)
raise SystemExit(1 if results['summary']['checks'].get('FAIL') else 0)
