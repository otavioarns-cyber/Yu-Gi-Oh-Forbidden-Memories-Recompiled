"""HAL20 consolidated candidates with opt-in exact-artifact guard. No private dependencies published.
Engine/controller boundaries are modeled; not gameplay Windows certification.
"""
from pathlib import Path
from elf_inventory import elf_info
import argparse,collections,csv,hashlib,itertools,json,os,re,shlex,shutil,struct,subprocess,tempfile,time
repo=Path(__file__).resolve().parents[2];p=argparse.ArgumentParser(description=__doc__);p.add_argument('--with-field',action='store_true');p.add_argument('--test-terrain',type=int,default=0);p.add_argument('--original-field',action='store_true');p.add_argument('--spoof-field',action='store_true');p.add_argument('--field-bonus',action='store_true');p.add_argument('--state-preview',action='store_true');p.add_argument('--terrain',type=int,action='append');p.add_argument('--original-fusion',action='store_true');p.add_argument('--field',action='store_true');p.add_argument('--fence',action='store_true');p.add_argument('--manager',action='store_true');p.add_argument('--spoof',action='store_true');p.add_argument('--consolidation',action='store_true');p.add_argument('--subsets',action='store_true');p.add_argument('--providers',action='store_true');p.add_argument('--guard',choices=('0','1','both'),default='0');p.add_argument('--output',type=Path,required=True);p.add_argument('--smoke',action='store_true');p.add_argument('--hardening',action='store_true');p.add_argument('--pr25',action='store_true');p.add_argument('--composition',action='store_true');p.add_argument('--reference26',action='store_true');p.add_argument('--boundary',action='store_true');p.add_argument('--bcase',type=int,action='append');p.add_argument('--mixed',choices=('old-r','old-rd'));p.add_argument('--trace-returns',action='store_true');p.add_argument('--hcase',action='append',type=int);p.add_argument('--quick',action='store_true');p.add_argument('--recovery-only',action='store_true');p.add_argument('--reset-recovery-only',action='store_true');p.add_argument('--restrictions-only',action='store_true',help='Only additional real public position-rules probes; preserve prior Queen matrix');a=p.parse_args();a.spoof=a.spoof or a.spoof_field;out=a.output.resolve();out.mkdir(parents=True,exist_ok=True)
meta=json.loads((repo/'notes/hal-20-field-state'/('PR25_INPUTS.json' if a.pr25 else 'INPUTS.json')).read_text());sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest();cc=shlex.split(os.environ['MEMORIES_I386_CC']);runner=shlex.split(os.environ.get('MEMORIES_I386_RUNNER',''));engine=Path(os.environ['MEMORIES_ENGINE_OBJ_DIR']);sdk=Path(os.environ['MEMORIES_SDK_EXPORTS']);ai=Path(os.environ['MEMORIES_AI_DIR']);dual=Path(os.environ['MEMORIES_DUAL_DIR'])
dirs={'E':repo/'mods/field-effects-secondary-types','H':ai,'D':dual,'M':Path(os.environ['MEMORIES_RITUALS_DIR']),**{x['symbol']:repo/x['directory'] for x in meta['components']}};
if a.reference26:
 old=json.loads((repo/'notes/hal-20-field-state/PR26_INPUTS.json').read_text());meta['components']=old['components']
 dirs.update({c['symbol']:Path(os.environ['MEMORIES_PR26_DIR'])/c['directory'] for c in old['components']})
if a.pr25:
 dirs.update({k:Path(os.environ['MEMORIES_PR25_DIR'])/meta_c['directory'] for k in ('R','RD') for meta_c in meta['components'] if meta_c['symbol']==k})
if a.composition or a.boundary or a.subsets or a.providers or a.consolidation or a.manager or a.spoof or a.field or a.field_bonus:dirs['F']=Path(os.environ['MEMORIES_FUSION_DIR'])
if a.mixed:
 old=json.loads((repo/'notes/hal-20-field-state/PR25_INPUTS.json').read_text());symbol='R' if a.mixed=='old-r' else 'RD'
 old_c=next(c for c in old['components'] if c['symbol']==symbol)
 dirs[symbol]=Path(os.environ['MEMORIES_PR25_DIR'])/old_c['directory']
 meta['components']=[old_c if c['symbol']==symbol else c for c in meta['components']]
if a.original_fusion:
 old=json.loads((repo/'notes/hal-20-consolidacao/INPUTS.json').read_text());meta['components']=[next(x for x in old['components'] if x['symbol']=='F') if c['symbol']=='F' else c for c in meta['components']];meta['fusion_reference']=old['fusion_reference']
if a.original_field:
 dirs['E']=Path(os.environ['MEMORIES_FIELD_ORIGINAL_DIR']);component=next(c for c in meta['components'] if c['symbol']=='E');component['files']=[dict(path=f.name,bytes=f.stat().st_size,sha256=sha(f)) for f in dirs['E'].iterdir() if f.is_file()]
if a.state_preview:
 dirs['R']=Path(os.environ['MEMORIES_STATE_PREVIEW_DIR']);component=next(c for c in meta['components'] if c['symbol']=='R');component['files']=[dict(path=str(f.relative_to(dirs['R'])),bytes=f.stat().st_size,sha256=sha(f)) for f in dirs['R'].rglob('*') if f.is_file() and '__pycache__' not in f.parts]
ids={k:json.loads((v/'mod.json').read_text())['id'] for k,v in dirs.items()};objects={k:json.loads((v/'mod.json').read_text())['library']+'.o' for k,v in dirs.items()};byid={ids[k]:v for k,v in dirs.items()}
checks=[(x,engine/x['path']) for x in meta['native_engine_objects']]+[(x,repo/x['path']) for x in meta['public_source_files']]+[(meta['sdk_exports'],sdk)]
checks += [(x,byid[x['id']]/x['path']) for x in meta['local_dependencies']]
checks += [(x,dirs[c['symbol']]/x['path']) for c in meta['components'] for x in c['files']]
if a.composition or a.boundary or a.subsets or a.providers or a.consolidation or a.manager or a.spoof or a.field or a.field_bonus:checks += [(x,dirs['F']/x['path']) for x in meta['fusion_reference']['files']]
results={'test_sources_sha256':{str(f.relative_to(repo)):sha(f) for f in (repo/'tests/hal20-field-state').glob('*') if f.is_file()},'scope':__doc__,'preflight':[],'builds':[],'profiles':[],'postflight':[]}
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
 names={'Duel_SetupCardRecord','DuelEffect_ApplyTerrain','func_80037DA4','func_80035E20','HAL2224_AfterPlanner','Duel_DrawFieldCards','HAL_HARDENING_INTERPOSE'};allow=set(sdk.read_text().split())
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
cases=[];keys=('M','F','R','H','RD','D')
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
if a.subsets or a.providers:
 keys=('M','F','R','H','RD','D')
 cases=[]
 for mask in range(1,64):
  inc=tuple(k for i,k in enumerate(keys) if mask&(1<<i))
  cases.append(dict(variant='subsets',bridge=0,ai=int('H' in inc),dual=int('D' in inc),rd=int('RD' in inc),where='before',effects='on' if 'R' in inc else 'absent',condition=0,scenario='providers' if a.providers else 'controller',mode=-30,cancel=0,arank=0,include=inc))
if a.field or a.field_bonus:
 keys=('M','F','R','H','RD','D','E');cases=[dict(variant='field-bonus' if a.field_bonus else 'field',bridge=0,ai=h,dual=d,rd=1,where=o,effects='on',condition=0,scenario='field-controller' if a.field_bonus else 'field',mode=0,cancel=0,arank=0,field=f,eo=e) for f,e,h,d,o in itertools.product(range(7),(0,1),(0,1),(0,1),('before','after'))]
if a.consolidation or a.manager or a.spoof:
 keys=('M','F','R','H','RD','D','E') if a.spoof_field else ('M','F','R','H','RD','D');cases=[dict(variant='consolidation',bridge=0,ai=1,dual=d,rd=1,where=o,effects='on',condition=0,scenario='consolidation',mode=m,cancel=0,arank=0,include=tuple(k for k in keys if k!='D' or d)) for m,d,o in itertools.product(range(6,12) if a.manager else (12,) if a.spoof else range(1,6),(0,1),('before','after'))]
 if a.manager:
  for q in cases:
   if q['mode']==10:q['ai']=0;q['include']=tuple(k for k in q['include'] if k!='H')
if a.state_preview:
 keys=('M','F','R','H','RD','D','E');cases=[dict(variant='state-preview',bridge=0,ai=h,dual=d,rd=1,where=o,effects='on',condition=0,scenario='state-preview',mode=m,cancel=0,arank=0,include=('M','F','R','RD','E')+('H',)*h+('D',)*d) for m,h,d,o in itertools.product(range(1,9),(0,1),(0,1),('before','after'))]
if a.with_field:
 if 'E' not in keys:keys=tuple(keys)+('E',)
 for q in cases:q['eo']=1;q['field']=a.test_terrain
if a.terrain:cases=[q for q in cases if q.get('field',-1) in a.terrain]
cases=[dict(q,guard=g) for q in cases for g in ((0,1) if a.guard=='both' else (int(a.guard),))]
for q in cases:
 q['active']=tuple(k for k in keys if k=='RD' and q['rd'] or k=='M' and ((a.composition or a.boundary or a.field or a.field_bonus) or q['scenario']=='ritual') or k=='F' and (a.composition or a.boundary or a.field or a.field_bonus) or k=='R' and q['effects'] not in ('absent','cold-off') or k=='H' and q['ai'] or k=='D' and q['dual']);q['active']=q.get('include',q['active']);
 if q.get('eo',0):q['active']+=('E',)
 q['composition']='+'.join(q['active']);q['name']=f"{q['variant']}-g{q['guard']}-bridge{q['bridge']}-h{q['ai']}-d{q['dual']}-rd{q['rd']}-{q['where']}-{q['effects']}-c{q['condition']}-{q['scenario']}-m{q['mode']}-x{q['cancel']}"+(f"-a{q['arank']}" if 'arank' in q else '')
for q in cases:
 if a.field or a.field_bonus:q['name']+=f"-field{q['field']}-E{q['eo']}"
 if a.subsets or a.providers:q['name']+='-'+q['composition'].replace('+','-')
if a.reset_recovery_only:cases=[q for q in cases if q['rd'] and q['scenario']=='controller' and q['cancel'] in (3,4)]
if a.recovery_only:cases=[q for q in cases if q['rd'] and q['scenario']=='controller' and 1<=q['cancel']<=8]
if a.quick:cases=[q for q in cases if q['ai']==q['dual']==0]
if a.smoke:cases=[q for q in cases if q['ai']==q['dual']==0 and q['effects']=='on' and q['condition']==q['cancel']==0 and (a.composition or a.providers or a.subsets or q['mode']==0)]
if not cases:finish();raise SystemExit('No profiles selected: not a successful test run')
with tempfile.TemporaryDirectory(prefix='hal25-real-') as tmp:
 tmp=Path(tmp);names=exports([dirs[k]/objects[k] for k in keys]+[engine/x['path'] for x in meta['native_engine_objects']]);(tmp/'export_names.h').write_text('#define TEST_EXPORT_COUNT '+str(len(names))+'\nstatic const char*test_export_names[]={'+','.join(json.dumps(x) for x in names)+'};\n')
 sources=['tests/hal20-field-state/harness.c','tests/hal20-field-state/manager_bridge.c','tests/hal20-field-state/window_bridge.c','src/pc/compat/fs.c','src/pc/mods/events.c','src/pc/mods/hooks.c','src/pc/mods/manager.c','src/pc/guest/sha256.c','src/pc/mods/overlap.c','src/pc/mods/mod_libc.c','src/pc/mods/object_loader.c','src/pc/guest/branch_thunks.c','src/pc/mod_packets.c','src/pc/mods/json.c','src/pc/platform/settings.c','src/pc/platform/paths.c']
 builds=[('build-harness',cc+['-m32','-std=gnu11','-O1','-D_FILE_OFFSET_BITS=64','-DMEMORIES_PC','-ffunction-sections','-fdata-sections','-I'+str(tmp),'-Isrc',*sources,'-Wl,--gc-sections','-Wl,--wrap=Hooks_Add','-Wl,--wrap=Hooks_Inspect','-Wl,--wrap=Mods_RegisterState','-Wl,--wrap=ObjectLoader_Load','-lpng','-lm','-lpthread','-o',tmp/'harness'])]
 if a.fence:builds.append(('build-duel-entry',cc+['-D_LANGUAGE_C','-DLANGUAGE_C','-m32','-std=gnu11','-O1','-DMEMORIES_PC','-Isrc','-fno-pic','-fno-pie','-fno-stack-protector','-fno-asynchronous-unwind-tables','-fno-unwind-tables','-c','src/game/main_run_duel.c','-o',tmp/'src_game_guard_main.c.o']))
 builds.append(('build-observer',cc+['-D_LANGUAGE_C','-DLANGUAGE_C','-m32','-std=gnu11','-O1','-DMEMORIES_PC','-Isrc','-fno-pic','-fno-pie','-fno-stack-protector','-fno-asynchronous-unwind-tables','-c','tests/hal20-field-state/observer.c','-o',tmp/'observer.o']))
 builds.append(('build-interposer',cc+['-D_LANGUAGE_C','-DLANGUAGE_C','-m32','-std=gnu11','-O1','-DMEMORIES_PC','-Isrc','-fno-pic','-fno-pie','-fno-stack-protector','-fno-asynchronous-unwind-tables','-c','tests/hal20-field-state/interposer.c','-o',tmp/'interposer.o']))
 for name,cmd in builds:
  e,l=execute(name,cmd);results['builds'].append(e)
  if e['returncode']:finish();raise SystemExit(2)
 results['native_test_objects']={'guard_main':elf_info(tmp/'src_game_guard_main.c.o')} if a.fence else {}
 results['test_objects']={k:elf_info(tmp/(k+'.o')) for k in ('observer','interposer')}
 results['harness_sha256']=sha(tmp/'harness')
 for i,q in enumerate(cases):
  folder=tmp/q['name'];mods=folder/'mods';mods.mkdir(parents=True)
  for k,src in dirs.items():
   if k=='R' and q['effects']=='absent':continue
   dst=mods/ids[k];dst.mkdir();shutil.copy2(src/'mod.json',dst/'mod.json');shutil.copy2(src/objects[k],dst/objects[k])
   if a.manager and k=='H':
    manifest=json.loads((dst/'mod.json').read_text());manifest['restart']=True;(dst/'mod.json').write_text(json.dumps(manifest))
   if a.spoof_field and k=='E':shutil.copy2(Path(os.environ['MEMORIES_FIELD_ORIGINAL_DIR'])/objects[k],dst/objects[k])
   if a.spoof and not a.spoof_field and k=='F':
    with (dst/objects[k]).open('ab') as changed:changed.write(b'HAL20-TEST-UNQUALIFIED-BYTES')
   if (src/'assets').is_dir():shutil.copytree(src/'assets',dst/'assets')
  shutil.copytree(repo/'mods/drop-missing-cards',mods/'drop-missing-cards')
  dst=mods/'hal2224-observer';dst.mkdir();shutil.copy2(tmp/'observer.o',dst/'observer.o');(dst/'mod.json').write_text(json.dumps({'id':'hal2224-observer','library':'observer','enabled':not q['guard']}))
  if a.hardening and q['mode'] in (2,3,4,5,22) or a.boundary and q['mode'] in (12,13):
   dst=mods/'hal2224-wrapper';dst.mkdir();shutil.copy2(tmp/'interposer.o',dst/'interposer.o');(dst/'mod.json').write_text(json.dumps({'id':'hal2224-wrapper','library':'interposer','enabled':True}))
  ranks={'M':-50,'H':q.get('arank',0),'RD':10,'D':20,'R':-20 if q['where']=='before' else 40,'E':-40,'F':q['mode'] if a.composition else -30}
  settings=folder/'settings.txt';settings.write_text(f"experimental.hal2224_order_guard={q['guard']}\ntest.with-field={int(a.with_field)}\ntest.state-preview={int(a.state_preview)}\ntest.state-mode={q['mode'] if a.state_preview else 0}\ntest.field={q.get('field',0)}\ntest.field-probe={int(a.field)}\nmod.field-effects-secondary-types={q.get('eo',0)}\nmod.field-effects-secondary-types.order=-40\ntest.fence={int(a.fence)}\ntest.consolidation={q['mode'] if a.consolidation or a.manager else 0}\ntest.expect-admission={int(not q['guard'] or not a.spoof and not a.state_preview and not(q['ai'] and q['rd'] and q.get('arank',0)==50))}\ntest.order-probe={int(a.composition and q.get('arank',0)==50)}\ntest.return-trace={int(a.trace_returns or q['guard'])}\ntest.bcase={q['mode'] if a.boundary else 0}\ntest.hcase={7 if a.boundary and q['mode'] in (3,4) else q['mode'] if a.hardening else 0}\nmod.hal2224-wrapper.order={100 if a.boundary or q['mode'] in (4,5,22) else 5}\nmod.hal2224-wrapper.mode={(7 if q['mode']==12 else 6) if a.boundary else 5 if q['mode']==22 else q['mode']-1 if q['mode'] in (2,3,4,5) else 0}\nmod.hal2224-wrapper.limit=1\ntest.condition={q['condition']}\ntest.effects-off={int(q['effects'] in ('off','late-on'))}\ntest.bridge={q['bridge']}\ntest.badmode={q['mode'] if q['scenario']=='fault-provider' else 0}\ntest.fault={q['cancel']}\ntest.recovery={int(a.recovery_only or a.reset_recovery_only)}\ntest.fresh-after-quarantine={int(a.reset_recovery_only)}\ntest.full={int(q['condition']==3)}\ntest.copies={int(q['condition']==4)}\nmod.drop-missing-cards=1\nmod.drop-missing-cards.order=-100\nmod.hal2224-observer.order=100\n"+''.join(f"mod.{ids[k]}={int(k in q['active'])}\nmod.{ids[k]}.order={ranks[k]}\n" for k in keys))
  env=dict(os.environ,MEMORIES_MODS_DIR=str(mods),MEMORIES_SETTINGS=str(settings),MEMORIES_USER_DIR=str(folder/'user'))
  e,l=execute(q['name'],runner+[tmp/'harness',repo,engine,str(q['condition']),('field-boundary-ritual' if a.with_field and q['scenario']=='boundary-ritual' else q['scenario']),'main',tmp],env);complete='SUMMARY ' in l and e['returncode'] in (0,1);refused=False;checks2=[dict(name=n,status=s,details=d) for n,s,d in re.findall(r'^RESULT (\S+) (PASS|FAIL) (.*)$',l,re.M)];classification='INCOMPLETE' if not complete else 'REFUSED' if refused else 'FAIL' if any(x['status']=='FAIL' for x in checks2) else 'NOT_REACHED' if ('NOT_REACHED first-action' in l or 'NOT_REACHED controller wrapper' in l or 'NOT_REACHED VM reports' in l or 'NOT_REACHED boundary' in l or 'NOT_REACHED stock retail' in l) else 'REFUSED' if 'ADMISSION REFUSED' in l or 'REFUSED boundary' in l or 'SAFE_REFUSAL ' in l else 'REFUSED' if a.boundary and q['mode'] in (2,6,7,12,13) else 'REFUSED' if q['scenario']=='hardening' and q['mode'] in (1,9,10,16,17,21) else 'PASS'
  results['profiles'].append({**q,**e,'classification':classification,'complete':complete,'checks':checks2,'observations':re.findall(r'^(?:ADMISSION|FIRST_UNQUALIFIED|ACTION |BONUS |F1 |RITUAL |LIFECYCLE|HAL23 |NOT_RUN|NOT_REACHED|CHAIN |QUEEN |GATE_INPUT |BRIDGE |TEST_ONLY |STATE |BOARD ).*$',l,re.M)})
  if i%20==0:print('Completed',i+1,'/',len(cases),q['name'],classification,flush=True)
for x,f in checks:results['postflight'].append(dict(path=str(f),status='PASS' if sha(f)==x['sha256'] else 'FAIL'))
finish();raise SystemExit(2 if results['summary']['incomplete'] else 1 if results['summary']['checks'].get('FAIL') else 0)
