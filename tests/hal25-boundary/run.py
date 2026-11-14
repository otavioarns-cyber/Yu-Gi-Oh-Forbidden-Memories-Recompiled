"""HAL25 hardening of real R/RD contract; no production guard. No private dependencies published.
Engine/controller boundaries are modeled; not gameplay Windows certification.
"""
from pathlib import Path
from elf_inventory import elf_info
import argparse,collections,csv,hashlib,itertools,json,os,re,shlex,shutil,struct,subprocess,tempfile,time
repo=Path(__file__).resolve().parents[2];p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,required=True);p.add_argument('--smoke',action='store_true');p.add_argument('--hardening',action='store_true');p.add_argument('--pr25',action='store_true');p.add_argument('--composition',action='store_true');p.add_argument('--reference26',action='store_true');p.add_argument('--boundary',action='store_true');p.add_argument('--bcase',type=int,action='append');p.add_argument('--mixed',choices=('old-r','old-rd'));p.add_argument('--trace-returns',action='store_true');p.add_argument('--hcase',action='append',type=int);p.add_argument('--quick',action='store_true');p.add_argument('--recovery-only',action='store_true');p.add_argument('--reset-recovery-only',action='store_true');p.add_argument('--restrictions-only',action='store_true',help='Only additional real public position-rules probes; preserve prior Queen matrix');a=p.parse_args();out=a.output.resolve();out.mkdir(parents=True,exist_ok=True)
meta=json.loads((repo/'notes/hal-25-boundary'/('PR25_INPUTS.json' if a.pr25 else 'INPUTS.json')).read_text());sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest();cc=shlex.split(os.environ['MEMORIES_I386_CC']);runner=shlex.split(os.environ.get('MEMORIES_I386_RUNNER',''));engine=Path(os.environ['MEMORIES_ENGINE_OBJ_DIR']);sdk=Path(os.environ['MEMORIES_SDK_EXPORTS']);ai=Path(os.environ['MEMORIES_AI_DIR']);dual=Path(os.environ['MEMORIES_DUAL_DIR'])
dirs={'H':ai,'D':dual,'M':Path(os.environ['MEMORIES_RITUALS_DIR']),**{x['symbol']:repo/x['directory'] for x in meta['components']}};
if a.reference26:
 old=json.loads((repo/'notes/hal-25-boundary/PR26_INPUTS.json').read_text());meta['components']=old['components']
 dirs.update({c['symbol']:Path(os.environ['MEMORIES_PR26_DIR'])/c['directory'] for c in old['components']})
if a.pr25:
 dirs.update({k:Path(os.environ['MEMORIES_PR25_DIR'])/meta_c['directory'] for k in ('R','RD') for meta_c in meta['components'] if meta_c['symbol']==k})
if a.composition or a.boundary:dirs['F']=Path(os.environ['MEMORIES_FUSION_DIR'])
if a.mixed:
 old=json.loads((repo/'notes/hal-25-boundary/PR25_INPUTS.json').read_text());symbol='R' if a.mixed=='old-r' else 'RD'
 old_c=next(c for c in old['components'] if c['symbol']==symbol)
 dirs[symbol]=Path(os.environ['MEMORIES_PR25_DIR'])/old_c['directory']
 meta['components']=[old_c if c['symbol']==symbol else c for c in meta['components']]
ids={k:json.loads((v/'mod.json').read_text())['id'] for k,v in dirs.items()};objects={k:json.loads((v/'mod.json').read_text())['library']+'.o' for k,v in dirs.items()};byid={ids[k]:v for k,v in dirs.items()}
checks=[(x,engine/x['path']) for x in meta['native_engine_objects']]+[(x,repo/x['path']) for x in meta['public_source_files']]+[(meta['sdk_exports'],sdk)]
checks += [(x,byid[x['id']]/x['path']) for x in meta['local_dependencies']]
checks += [(x,dirs[c['symbol']]/x['path']) for c in meta['components'] for x in c['files']]
if a.composition or a.boundary:checks += [(x,dirs['F']/x['path']) for x in meta['fusion_reference']['files']]
results={'test_sources_sha256':{str(f.relative_to(repo)):sha(f) for f in (repo/'tests/hal25-boundary').glob('*') if f.is_file()},'scope':__doc__,'preflight':[],'builds':[],'profiles':[],'postflight':[]}
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
 st=time.monotonic();p=subprocess.run(list(map(str,cmd)),cwd=repo,env=env,text=True,errors='backslashreplace',stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=60);(out/(name+'.log')).write_text(p.stdout);return dict(returncode=p.returncode,seconds=round(time.monotonic()-st,3),log=name+'.log'),p.stdout

def exports(files):
 names={'HAL2224_AfterPlanner','Duel_DrawFieldCards','HAL_HARDENING_INTERPOSE'};allow=set(sdk.read_text().split())
 for f in files:
  d=f.read_bytes();names.update(x[:-1].decode() for x in re.findall(rb'[A-Za-z_][A-Za-z_0-9]*\x00',d) if x[:-1].decode() in allow);h=struct.unpack_from('<16sHHIIIIIHHHHHH',d);secs=[struct.unpack_from('<IIIIIIIIII',d,h[6]+i*h[11]) for i in range(h[12])]
  for s in secs:
   if s[1]!=2:continue
   st=secs[s[6]];strings=d[st[4]:st[4]+st[5]]
   for at in range(s[4],s[4]+s[5],s[9]):
    n,v,z,b,o,i=struct.unpack_from('<IIIBBH',d,at)
    if n and not i:names.add(strings[n:].split(b'\0')[0].decode())
 return sorted(names)
# Main cases have real pending/plans; only named cancel probes write a tuple.
cases=[];keys=('M','R','H','RD','D')
for h,d,order,effects in itertools.product((0,1),(0,1),('before','after'),('on','off','absent')):
 for condition in (range(7) if effects=='on' else (0,)):
  cases.append(dict(variant='real',bridge=0,ai=h,dual=d,rd=1,where=order,effects=effects,condition=condition,scenario='controller',mode=0,cancel=0))
for fault,h,d,order in itertools.product(range(1,10),(0,1),(0,1),('before','after')):
 cases.append(dict(variant='real',bridge=0,ai=h,dual=d,rd=1,where=order,effects='late-on' if fault==7 else 'on',condition=4 if fault==4 else 0,scenario='controller',mode=0,cancel=fault))
for h,d,order in itertools.product((0,1),(0,1),('before','after')):
 cases.append(dict(variant='real',bridge=0,ai=h,dual=d,rd=0,where=order,effects='on',condition=0,scenario='controller',mode=0,cancel=0))
for order in ('before','after'):
 cases.append(dict(variant='real',bridge=0,ai=0,dual=0,rd=0,where=order,effects='on',condition=0,scenario='controller',mode=0,cancel=9))
for order in ('before','after'):cases.append(dict(variant='real',bridge=0,ai=0,dual=0,rd=1,where=order,effects='on',condition=5,scenario='ritual',mode=0,cancel=0))
for order in ('before','after'):
 cases.append(dict(variant='real',bridge=0,ai=0,dual=0,rd=1,where=order,effects='on',condition=0,scenario='contract',mode=0,cancel=0))
for mode,dual in itertools.product(range(1,9),(0,1)):
 cases.append(dict(variant='real',bridge=0,ai=0,dual=dual,rd=1,where='after',effects='absent',condition=0,scenario='fault-provider',mode=mode,cancel=0))
if a.restrictions_only:cases=[dict(variant='real',bridge=0,ai=1,dual=1,rd=1,where=o,effects='on',condition=5,scenario='other-restrictions',mode=0,cancel=0) for o in ('before','after')]
if a.hardening:
 cases=[]
 for hc,h,d,o in itertools.product(((1,2,3,4,5,6,7,8,9,10,14,15,22) if a.pr25 else (1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,20,21,22)),(0,1),(0,1),('before','after')):
  if hc not in (2,3,4,5,22) and (h or d):continue
  cases.append(dict(variant='hardening',bridge=0,ai=h,dual=d,rd=1,where=o,effects='cold-off' if hc==20 else 'on' if hc in (2,3,4,5,11,13,21,22) else 'absent',condition=4 if hc in (11,22) else 0,scenario='hardening',mode=hc,cancel=0))
if a.hcase:cases=[q for q in cases if q['mode'] in a.hcase]
if a.composition:
 keys=('M','F','R','H','RD','D')
 cases=[dict(variant='collection',bridge=0,ai=h,dual=d,rd=1,where=o,effects='on',condition=c,scenario='controller',mode=fo,cancel=0,arank=ar) for h,d,o,fo,c,ar in itertools.product((0,1),(0,1),('before','after'),(-30,30),(0,4,5),(0,50)) if h or ar==0]

if a.boundary:
 keys=('M','F','R','H','RD','D')
 cases=[dict(variant='boundary',bridge=0,ai=h,dual=d,rd=1,where=o,effects='on' if b in (6,20,21,22,23) else 'absent',condition=5,scenario='boundary-ritual' if b>=20 else 'boundary-combined' if b in (10,11,12,13) else 'boundary-life',mode=b,cancel=0,arank=0) for b,h,d,o in itertools.product((1,2,3,4,5,6,7,8,10,11,12,13,20,21,22,23),(0,1),(0,1),('before','after'))]
 if a.bcase:cases=[q for q in cases if q['mode'] in a.bcase]
if a.mixed:cases=[q for q in cases if q['rd'] and not q['ai'] and not q['dual'] and q['effects']=='on' and q['condition'] in (0,4,5) and q['scenario']=='controller' and not q['mode'] and not q['cancel']]
for q in cases:
 q['active']=tuple(k for k in keys if k=='RD' and q['rd'] or k=='M' and ((a.composition or a.boundary) or q['scenario']=='ritual') or k=='F' and (a.composition or a.boundary) or k=='R' and q['effects'] not in ('absent','cold-off') or k=='H' and q['ai'] or k=='D' and q['dual']);q['composition']='+'.join(q['active']);q['guard']=0;q['name']=f"{q['variant']}-bridge{q['bridge']}-h{q['ai']}-d{q['dual']}-rd{q['rd']}-{q['where']}-{q['effects']}-c{q['condition']}-{q['scenario']}-m{q['mode']}-x{q['cancel']}"+(f"-a{q['arank']}" if 'arank' in q else '')
if a.reset_recovery_only:cases=[q for q in cases if q['rd'] and q['scenario']=='controller' and q['cancel'] in (3,4)]
if a.recovery_only:cases=[q for q in cases if q['rd'] and q['scenario']=='controller' and 1<=q['cancel']<=8]
if a.quick:cases=[q for q in cases if q['ai']==q['dual']==0]
if a.smoke:cases=[q for q in cases if q['ai']==q['dual']==0 and q['effects']=='on' and q['condition']==q['cancel']==0 and (a.composition or q['mode']==0)]
if not cases:finish();raise SystemExit('No profiles selected: not a successful test run')
with tempfile.TemporaryDirectory(prefix='hal25-real-') as tmp:
 tmp=Path(tmp);names=exports([dirs[k]/objects[k] for k in keys]+[engine/x['path'] for x in meta['native_engine_objects']]);(tmp/'export_names.h').write_text('#define TEST_EXPORT_COUNT '+str(len(names))+'\nstatic const char*test_export_names[]={'+','.join(json.dumps(x) for x in names)+'};\n')
 sources=['tests/hal25-boundary/harness.c','tests/hal25-boundary/manager_bridge.c','src/pc/compat/fs.c','src/pc/mods/events.c','src/pc/mods/hooks.c','src/pc/mods/manager.c','src/pc/mods/overlap.c','src/pc/mods/mod_libc.c','src/pc/mods/object_loader.c','src/pc/guest/branch_thunks.c','src/pc/mod_packets.c','src/pc/mods/json.c','src/pc/platform/settings.c','src/pc/platform/paths.c']
 builds=[('build-harness',cc+['-m32','-std=gnu11','-O1','-D_FILE_OFFSET_BITS=64','-DMEMORIES_PC','-ffunction-sections','-fdata-sections','-I'+str(tmp),'-Isrc',*sources,'-Wl,--gc-sections','-Wl,--wrap=Hooks_Add','-Wl,--wrap=Mods_RegisterState','-Wl,--wrap=ObjectLoader_Load','-lpng','-lm','-lpthread','-o',tmp/'harness'])]
 builds.append(('build-observer',cc+['-D_LANGUAGE_C','-DLANGUAGE_C','-m32','-std=gnu11','-O1','-DMEMORIES_PC','-Isrc','-fno-pic','-fno-pie','-fno-stack-protector','-fno-asynchronous-unwind-tables','-c','tests/hal25-boundary/observer.c','-o',tmp/'observer.o']))
 builds.append(('build-interposer',cc+['-D_LANGUAGE_C','-DLANGUAGE_C','-m32','-std=gnu11','-O1','-DMEMORIES_PC','-Isrc','-fno-pic','-fno-pie','-fno-stack-protector','-fno-asynchronous-unwind-tables','-c','tests/hal25-boundary/interposer.c','-o',tmp/'interposer.o']))
 for name,cmd in builds:
  e,l=execute(name,cmd);results['builds'].append(e)
  if e['returncode']:finish();raise SystemExit(2)
 results['test_objects']={k:elf_info(tmp/(k+'.o')) for k in ('observer','interposer')}
 results['harness_sha256']=sha(tmp/'harness')
 for i,q in enumerate(cases):
  folder=tmp/q['name'];mods=folder/'mods';mods.mkdir(parents=True)
  for k,src in dirs.items():
   if k=='R' and q['effects']=='absent':continue
   dst=mods/ids[k];dst.mkdir();shutil.copy2(src/'mod.json',dst/'mod.json');shutil.copy2(src/objects[k],dst/objects[k])
   if (src/'assets').is_dir():shutil.copytree(src/'assets',dst/'assets')
  shutil.copytree(repo/'mods/drop-missing-cards',mods/'drop-missing-cards')
  dst=mods/'hal2224-observer';dst.mkdir();shutil.copy2(tmp/'observer.o',dst/'observer.o');(dst/'mod.json').write_text(json.dumps({'id':'hal2224-observer','library':'observer','enabled':True}))
  if a.hardening and q['mode'] in (2,3,4,5,22) or a.boundary and q['mode'] in (12,13):
   dst=mods/'hal2224-wrapper';dst.mkdir();shutil.copy2(tmp/'interposer.o',dst/'interposer.o');(dst/'mod.json').write_text(json.dumps({'id':'hal2224-wrapper','library':'interposer','enabled':True}))
  ranks={'M':-50,'H':q.get('arank',0),'RD':10,'D':20,'R':-20 if q['where']=='before' else 40,'F':q['mode'] if a.composition else -30}
  settings=folder/'settings.txt';settings.write_text(f"test.order-probe={int(a.composition and q.get('arank',0)==50)}\ntest.return-trace={int(a.trace_returns)}\ntest.bcase={q['mode'] if a.boundary else 0}\ntest.hcase={7 if a.boundary and q['mode'] in (3,4) else q['mode'] if a.hardening else 0}\nmod.hal2224-wrapper.order={100 if a.boundary or q['mode'] in (4,5,22) else 5}\nmod.hal2224-wrapper.mode={(7 if q['mode']==12 else 6) if a.boundary else 5 if q['mode']==22 else q['mode']-1 if q['mode'] in (2,3,4,5) else 0}\nmod.hal2224-wrapper.limit=1\ntest.condition={q['condition']}\ntest.effects-off={int(q['effects'] in ('off','late-on'))}\ntest.bridge={q['bridge']}\ntest.badmode={0 if a.hardening or a.composition or a.boundary else q['mode']}\ntest.fault={q['cancel']}\ntest.recovery={int(a.recovery_only or a.reset_recovery_only)}\ntest.fresh-after-quarantine={int(a.reset_recovery_only)}\ntest.full={int(q['condition']==3)}\ntest.copies={int(q['condition']==4)}\nmod.drop-missing-cards=1\nmod.drop-missing-cards.order=-100\nmod.hal2224-observer.order=100\n"+''.join(f"mod.{ids[k]}={int(k in q['active'])}\nmod.{ids[k]}.order={ranks[k]}\n" for k in keys))
  env=dict(os.environ,MEMORIES_MODS_DIR=str(mods),MEMORIES_SETTINGS=str(settings),MEMORIES_USER_DIR=str(folder/'user'))
  e,l=execute(q['name'],runner+[tmp/'harness',repo,engine,str(q['condition']),q['scenario'],'main',tmp],env);complete='SUMMARY ' in l and e['returncode'] in (0,1);refused=False;checks2=[dict(name=n,status=s,details=d) for n,s,d in re.findall(r'^RESULT (\S+) (PASS|FAIL) (.*)$',l,re.M)];classification='INCOMPLETE' if not complete else 'REFUSED' if refused else 'FAIL' if any(x['status']=='FAIL' for x in checks2) else 'NOT_REACHED' if ('NOT_REACHED first-action' in l or 'NOT_REACHED controller wrapper' in l or 'NOT_REACHED VM reports' in l or 'NOT_REACHED boundary' in l) else 'REFUSED' if 'REFUSED boundary' in l or 'SAFE_REFUSAL ' in l else 'REFUSED' if a.boundary and q['mode'] in (2,6,7,12,13) else 'REFUSED' if q['scenario']=='hardening' and q['mode'] in (1,9,10,16,17,21) else 'PASS'
  results['profiles'].append({**q,**e,'classification':classification,'complete':complete,'checks':checks2,'observations':re.findall(r'^(?:ADMISSION|FIRST_UNQUALIFIED|ACTION |BONUS |F1 |RITUAL |LIFECYCLE|HAL23 |NOT_RUN|NOT_REACHED|CHAIN |QUEEN |GATE_INPUT |BRIDGE |TEST_ONLY |STATE |BOARD ).*$',l,re.M)})
  if i%20==0:print('Completed',i+1,'/',len(cases),q['name'],classification,flush=True)
for x,f in checks:results['postflight'].append(dict(path=str(f),status='PASS' if sha(f)==x['sha256'] else 'FAIL'))
finish();raise SystemExit(2 if results['summary']['incomplete'] else 1 if results['summary']['checks'].get('FAIL') else 0)
