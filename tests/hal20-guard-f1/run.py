"""HAL20 immutable packages + exact PR20 guard. No private dependencies published.
Engine/controller boundaries are modeled; not gameplay Windows certification.
"""
from pathlib import Path
import argparse,collections,csv,hashlib,itertools,json,os,re,shlex,shutil,struct,subprocess,tempfile,time
repo=Path(__file__).resolve().parents[2];p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,required=True);p.add_argument('--smoke',action='store_true');p.add_argument('--queen',action='store_true');a=p.parse_args();out=a.output.resolve();out.mkdir(parents=True,exist_ok=True)
meta=json.loads((repo/'notes/hal-20/guard-f1/INPUTS.json').read_text());sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest();cc=shlex.split(os.environ['MEMORIES_I386_CC']);runner=shlex.split(os.environ.get('MEMORIES_I386_RUNNER',''));engine=Path(os.environ['MEMORIES_ENGINE_OBJ_DIR']);sdk=Path(os.environ['MEMORIES_SDK_EXPORTS']);ai=Path(os.environ['MEMORIES_AI_DIR']);dual=Path(os.environ['MEMORIES_DUAL_DIR'])
dirs={'H':ai,'D':dual,**{x['symbol']:repo/x['directory'] for x in meta['components']}};ids={k:json.loads((v/'mod.json').read_text())['id'] for k,v in dirs.items()};objects={k:json.loads((v/'mod.json').read_text())['library']+'.o' for k,v in dirs.items()};byid={ids[k]:v for k,v in dirs.items()}
checks=[(x,engine/x['path']) for x in meta['native_engine_objects']]+[(x,repo/x['path']) for x in meta['public_source_files']]+[(x,repo/x['path']) for x in meta['guard_files']]+[(meta['sdk_exports'],sdk)]
checks += [(x,byid[x['id']]/x['path']) for x in meta['local_dependencies']]
checks += [(x,repo/c['directory']/x['path']) for c in meta['components'] for x in c['files']]
results={'scope':__doc__,'preflight':[],'builds':[],'profiles':[],'postflight':[]}
for x,f in checks:results['preflight'].append(dict(path=str(f),status='PASS' if f.is_file() and sha(f)==x['sha256'] else 'BLOCKED'))
def finish():
 results['summary']={'processes':len(results['profiles']),'checks':dict(collections.Counter(c['status'] for q in results['profiles'] for c in q['checks'])),'execution':dict(collections.Counter(q['classification'] for q in results['profiles'])),'incomplete':sum(not q['complete'] for q in results['profiles'])}
 (out/'RESULTADOS.json').write_text(json.dumps(results,ensure_ascii=False,indent=2)+'\n')
 with (out/'MATRIZ.csv').open('w') as f:
  w=csv.writer(f,lineterminator='\n');w.writerow(['case','composition','guard','scenario','classification','check','status','details','log'])
  for q in results['profiles']:
   for c in q['checks'] or [dict(name='execution',status=q['classification'],details='See raw log')]:w.writerow([q['name'],q['composition'],q['guard'],q['scenario'],q['classification'],c['name'],c['status'],c['details'],q['log']])
 print('SUMMARY',results['summary'],flush=True)
if any(x['status']!='PASS' for x in results['preflight']):finish();raise SystemExit(2)
def execute(name,cmd,env=None):
 st=time.monotonic();p=subprocess.run(list(map(str,cmd)),cwd=repo,env=env,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=60);(out/(name+'.log')).write_text(p.stdout);return dict(returncode=p.returncode,seconds=round(time.monotonic()-st,3),log=name+'.log'),p.stdout

def exports(files):
 names={'HAL2224_AfterPlanner','Duel_DrawFieldCards'};allow=set(sdk.read_text().split())
 for f in files:
  d=f.read_bytes();names.update(x[:-1].decode() for x in re.findall(rb'[A-Za-z_][A-Za-z_0-9]*\x00',d) if x[:-1].decode() in allow);h=struct.unpack_from('<16sHHIIIIIHHHHHH',d);secs=[struct.unpack_from('<IIIIIIIIII',d,h[6]+i*h[11]) for i in range(h[12])]
  for s in secs:
   if s[1]!=2:continue
   st=secs[s[6]];strings=d[st[4]:st[4]+st[5]]
   for at in range(s[4],s[4]+s[5],s[9]):
    n,v,z,b,o,i=struct.unpack_from('<IIIBBH',d,at)
    if n and not i:names.add(strings[n:].split(b'\0')[0].decode())
 return sorted(names)
# Singletons, all pairs/trios, and complete powerset; every profile gets a new process.
cases=[]
def add(active,guard,scenario='controller',order='safe',variant=5,where='before'):
 active=tuple(k for k in ('M','R','F','H','RD','D') if k in active);name='-'.join(active)+f'-g{guard}-{scenario}-{order}-v{variant}-{where}';q=dict(name=name,active=active,composition='+'.join(active),guard=guard,scenario=scenario,order=order,variant=variant,where=where)
 if not any(x['name']==name for x in cases):cases.append(q)
keys=('M','R','F','H','RD','D')
for size in range(1,7):
 for active in itertools.combinations(keys,size):
  for guard in (0,1):
   add(active,guard)
   if 'F' in active:add(active,guard,'providers')
   if 'M' in active:add(active,guard,'ritual')
for active in [('M',),('R',),('F',),('RD',),('M','R','F'),('H','RD','D'),keys]:
 for g in (0,1):add(active,g,'state')
for o,g in itertools.product(('O0','O2'),(0,1)):
 add(('H','RD','D'),g,order=o);add(keys,g,order=o)
for g in (0,1):add(keys,g,where='after');add(('H','RD','D'),g,variant=0)
if a.queen:
 cases=[]
 for active,g,where in itertools.product((keys,('R','H','D'),('R','RD','D'),('R','H'),('R','RD')),(0,1),('before','after')):add(active,g,where=where)
if a.smoke:
 wanted=[('H+RD+D',1,'controller'),('M+R+F+H+RD+D',1,'controller'),('M+R+F+H+RD+D',0,'controller'),('M+R+F',1,'providers'),('M+R+F',1,'ritual'),('M+R+F',1,'state')]
 cases=[q for q in cases if q['order']=='safe' and q['where']=='before' and q['variant']==5 and (q['composition'],q['guard'],q['scenario']) in wanted]
with tempfile.TemporaryDirectory(prefix='hal20-guard-f1-') as tmp:
 tmp=Path(tmp);names=exports([dirs[k]/objects[k] for k in keys]+[engine/x['path'] for x in meta['native_engine_objects']]);(tmp/'export_names.h').write_text('#define TEST_EXPORT_COUNT '+str(len(names))+'\nstatic const char*test_export_names[]={'+','.join(json.dumps(x) for x in names)+'};\n')
 sources=['tests/hal20-guard-f1/harness.c','tests/hal20-guard-f1/manager_bridge.c','tests/hal20-guard-f1/window_bridge.c','src/pc/compat/fs.c','src/pc/mods/events.c','src/pc/mods/hooks.c','src/pc/mods/manager.c','src/pc/mods/overlap.c','src/pc/mods/mod_libc.c','src/pc/mods/object_loader.c','src/pc/guest/branch_thunks.c','src/pc/mod_packets.c','src/pc/mods/json.c','src/pc/platform/settings.c','src/pc/platform/paths.c']
 builds=[('build-harness',cc+['-m32','-std=gnu11','-O1','-D_FILE_OFFSET_BITS=64','-DMEMORIES_PC','-ffunction-sections','-fdata-sections','-I'+str(tmp),'-Isrc',*sources,'-Wl,--gc-sections','-Wl,--wrap=Hooks_Add','-Wl,--wrap=Mods_RegisterState','-Wl,--wrap=ObjectLoader_Load','-lpng','-lm','-lpthread','-o',tmp/'harness'])]
 for name,src,dst in [('duel-entry','src/game/main_run_duel.c','src_game_guard_main.c.o'),('observer','tests/hal20-guard-f1/observer.c','observer.o')]:builds.append(('build-'+name,cc+['-D_LANGUAGE_C','-DLANGUAGE_C','-m32','-std=gnu11','-O1','-DMEMORIES_PC','-Isrc','-fno-pic','-fno-pie','-fno-stack-protector','-fno-asynchronous-unwind-tables','-fno-unwind-tables','-c',src,'-o',tmp/dst]))
 for name,cmd in builds:
  e,l=execute(name,cmd);results['builds'].append(e)
  if e['returncode']:finish();raise SystemExit(2)
 for i,q in enumerate(cases):
  folder=tmp/q['name'];mods=folder/'mods';mods.mkdir(parents=True)
  for k,src in dirs.items():
   dst=mods/ids[k];dst.mkdir();shutil.copy2(src/'mod.json',dst/'mod.json');shutil.copy2(src/objects[k],dst/objects[k])
   if (src/'assets').is_dir():shutil.copytree(src/'assets',dst/'assets')
  shutil.copytree(repo/'mods/drop-missing-cards',mods/'drop-missing-cards')
  dst=mods/'hal2224-observer';dst.mkdir();shutil.copy2(tmp/'observer.o',dst/'observer.o');(dst/'mod.json').write_text(json.dumps({'id':'hal2224-observer','library':'observer','enabled':True}))
  order={'safe':['H','RD','D'],'O0':['H','D','RD'],'O2':['D','RD','H']}[q['order']];ranks={k:i*10 for i,k in enumerate(order)};ranks.update({k:(30+i*10 if q['where']=='after' else -30+i*10) for i,k in enumerate(('M','R','F'))})
  settings=folder/'settings.txt';settings.write_text(f'test.queen={int(a.queen)}\nexperimental.hal2224_order_guard={q["guard"]}\nmod.drop-missing-cards=1\nmod.drop-missing-cards.order=-50\nmod.hal2224-observer.order=100\n'+''.join(f'mod.{ids[k]}={int(k in q["active"])}\nmod.{ids[k]}.order={ranks[k]}\n' for k in keys))
  env=dict(os.environ,MEMORIES_MODS_DIR=str(mods),MEMORIES_SETTINGS=str(settings),MEMORIES_USER_DIR=str(folder/'user'))
  e,l=execute(q['name'],runner+[tmp/'harness',repo,engine,q['variant'],q['scenario'],'main',tmp],env);complete='SUMMARY ' in l and e['returncode'] in (0,1);refused=bool(re.search(r'^ADMISSION .*ready=0',l,re.M));checks2=[dict(name=n,status=s,details=d) for n,s,d in re.findall(r'^RESULT (\S+) (PASS|FAIL) (.*)$',l,re.M)];classification='INCOMPLETE' if not complete else 'REFUSED' if refused else 'FAIL' if any(x['status']=='FAIL' for x in checks2) else 'NOT_REACHED' if 'NOT_REACHED first-action' in l else 'PASS'
  results['profiles'].append({**q,**e,'classification':classification,'complete':complete,'checks':checks2,'observations':re.findall(r'^(?:ADMISSION|FIRST_UNQUALIFIED|ACTION |BONUS |F1 |RITUAL |LIFECYCLE|HAL23 |NOT_RUN|NOT_REACHED|CHAIN |QUEEN |GATE_INPUT ).*$',l,re.M)})
  if i%20==0:print('Completed',i+1,'/',len(cases),q['name'],classification,flush=True)
for x,f in checks:results['postflight'].append(dict(path=str(f),status='PASS' if sha(f)==x['sha256'] else 'FAIL'))
finish();raise SystemExit(2 if results['summary']['incomplete'] else 1 if results['summary']['checks'].get('FAIL') else 0)
