"""HAL21 isolated policies. Exact PR12 sources + API24 SDK required locally.
Only patches/candidates and own tests are published; no consumer MOD modified.
Baseline/hide FAIL remain in results; exit 0 requires the packaged candidate PASS.
Exit 1 is candidate failure; 2 denotes incomplete. No baseline failure is hidden.
"""
from pathlib import Path
import argparse,collections,difflib,hashlib,json,os,re,shlex,shutil,struct,subprocess,tempfile,time
repo=Path(__file__).resolve().parents[2];parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--output',type=Path,required=True);parser.add_argument('--candidate-source',type=Path,required=True);parser.add_argument('--artifact-dir',type=Path,default=repo/'artifacts/hal21');args=parser.parse_args();out=args.output.resolve();artifact_dir=args.artifact_dir.resolve();candidate=args.candidate_source.resolve();out.mkdir(parents=True,exist_ok=True)
source=Path(os.environ['MEMORIES_FUSION_SOURCE']).resolve();old=Path(os.environ['MEMORIES_FUSION_API18']).resolve();ritual=Path(os.environ['MEMORIES_RITUAL_SOURCE']).resolve();tw=Path(os.environ['MEMORIES_TIME_WIZARD_SOURCE']).resolve();sdk=Path(os.environ['MEMORIES_SDK_DIR']).resolve();modcc=os.environ['MEMORIES_MOD_CC'];cc=shlex.split(os.environ['MEMORIES_I386_CC']);runner=shlex.split(os.environ.get('MEMORIES_I386_RUNNER',''))
meta=json.loads((repo/'notes/hal-21/fix/INPUTS.json').read_text());sha=lambda f:hashlib.sha256(f.read_bytes()).hexdigest()
results={'scope':'HAL21 F1 policies; real objects/registry/manager, synthetic engine; no gameplay Windows','preflight':[],'builds':[],'originals':[],'candidate_tests':[],'manager_profiles':[],'postflight':[],'artifacts':{}}
checkfiles=[]
for f in meta['fusion_source']:checkfiles.append((f,source/f['path']))
for f in meta.get('consumer_sources',[]):checkfiles.append((f,(tw if f['id']=='time-wizard' else ritual/f['path'])))
for f,p in checkfiles:results['preflight'].append(dict(path=str(p),status='PASS' if p.is_file() and sha(p)==f['sha256'] else 'BLOCKED'))
assert sha(old)==meta['objects'][0]['sha256'];assert sha(source/'fusion-auxiliary-balance.o')==meta['objects'][1]['sha256'];assert sha(ritual/'ritual-card-effects.o')==meta['objects'][2]['sha256']
def execute(name,cmd,env=None,cwd=repo):
 start=time.monotonic();p=subprocess.run(list(map(str,cmd)),cwd=cwd,env=env,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=180);(out/(name+'.log')).write_text(p.stdout);return {'log':name+'.log','returncode':p.returncode,'seconds':round(time.monotonic()-start,3)},p.stdout

def finish():
 results['summary']={'original_F1_returncodes':[x['execution']['returncode'] for x in results['originals']],'candidate_loader':{x['name']:x['execution']['returncode'] for x in results['candidate_tests']},'manager':{policy:dict(collections.Counter(c['status'] for p in results['manager_profiles'] if p['policy']==policy for c in p['checks'])) for policy in ('baseline','hide','revoke','candidate')}}
 (out/'RESULTADOS.json').write_text(json.dumps(results,ensure_ascii=False,indent=2)+'\n');print('SUMMARY',results['summary'],flush=True)
if any(x['status']!='PASS' for x in results['preflight']):finish();raise SystemExit(2)
def build(name,command):
 execution,log=execute(name,command);results['builds'].append(execution)
 if execution['returncode']:finish();raise SystemExit(2)
 return log

def function(src,name):
 at=src.index(name+'(');start=src.rfind('\nstatic ',0,at)+1;brace=src.index('{',at);depth=1;end=brace+1
 while depth:depth+=(src[end]=='{')-(src[end]=='}');end+=1
 return src[start:end]+'\n'

def symbols(files):
 names=set();allowed=set((sdk/'exports.i386.txt').read_text().split())
 for f in files:
  d=f.read_bytes();assert d[:6]==b'\x7fELF\x01\x01';names.update(s[:-1].decode() for s in re.findall(rb'[A-Za-z_][A-Za-z_0-9]*\x00',d) if s[:-1].decode() in allowed)
  h=struct.unpack_from('<16sHHIIIIIHHHHHH',d);secs=[struct.unpack_from('<IIIIIIIIII',d,h[6]+i*h[11]) for i in range(h[12])]
  for sh in secs:
   if sh[1]!=2:continue
   st=secs[sh[6]];strings=d[st[4]:st[4]+st[5]]
   for at in range(sh[4],sh[4]+sh[5],sh[9]):
    v=struct.unpack_from('<IIIBBH',d,at)
    if v[0] and v[-1]==0:names.add(strings[v[0]:].split(b'\0')[0].decode())
 return sorted(names)
with tempfile.TemporaryDirectory(prefix='hal21-') as tmp:
 tmp=Path(tmp);variants={'baseline':source}
 # Header-only dependencies of the unchanged legacy loader are provided via -I.
 legacy=(source/'tests/preview2_loader.c').read_text().replace('"../model.h"','"model.h"').replace('"../origin_effects_v1.h"','"origin_effects_v1.h"').replace('"../ritual_effect_intelligence_v1.h"','"ritual_effect_intelligence_v1.h"');(tmp/'legacy.c').write_text(legacy)
 build('build-original-loader',cc+['-m32','-std=gnu11','-O1','-DMEMORIES_PC','-Isrc','-iquote'+str(source),tmp/'legacy.c','src/pc/mods/object_loader.c','src/pc/mods/mod_libc.c','src/pc/guest/branch_thunks.c','src/pc/compat/fs.c','-lm','-o',tmp/'legacy'])
 for name,obj in [('api18',old),('api24',source/'fusion-auxiliary-balance.o')]:
  ex,log=execute('original-'+name+'-F1',runner+[tmp/'legacy',obj,source,'--strict-off']);results['originals'].append({'object':name,'execution':ex,'observed':re.findall(r'^(?:object_hash|OFF_CONTRACT|PASS).*$',log,re.M)})
  if ex['returncode']!=2:finish();raise SystemExit(2)
 ex,log=execute('baseline-ON-characterization',runner+[tmp/'legacy',source/'fusion-auxiliary-balance.o',source]);results['candidate_tests'].append({'name':'baseline-ON','execution':ex,'observed':re.findall(r'^(?:object_hash|PASS).*$',log,re.M)})
 for policy in ('hide','revoke'):
  dest=tmp/policy;shutil.copytree(source,dest);variants[policy]=dest
  ex,log=execute('apply-patch-'+policy,['patch','-p1','--batch','-i',repo/'tests/hal21/reference'/(policy+'.patch')],cwd=dest);results['builds'].append(ex)
  if ex['returncode']:finish();raise SystemExit(2)
  ex,log=execute('sdk-build-'+policy,['python',sdk/'tools/build_mod.py',dest,'--target','i386'],env=dict(os.environ,MEMORIES_MOD_CC=modcc));results['builds'].append(ex)
  if ex['returncode']:finish();raise SystemExit(2)
  artifact=artifact_dir/policy/'fusion-auxiliary-balance.o';artifact.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(dest/'fusion-auxiliary-balance.o',artifact)
  results['artifacts'][policy]={'sha256':sha(artifact),'bytes':artifact.stat().st_size,'path':str(artifact.relative_to(repo)) if artifact.is_relative_to(repo) else str(artifact),'source_diff':[f['path'] for f in meta['fusion_source'] if f['path'].endswith(('.c','.h','mod.json')) and sha(dest/f['path'])!=f['sha256']]}
  for strict in (False,True):
   ex,log=execute(policy+('-F1' if strict else '-ON-characterization'),runner+[tmp/'legacy',artifact,dest]+(['--strict-off'] if strict else []));results['candidate_tests'].append({'name':policy+('-F1' if strict else '-ON'),'execution':ex,'observed':re.findall(r'^(?:object_hash|OFF_CONTRACT|PASS).*$',log,re.M)})
 # Test the object actually published in mods/, never a copied reference object.
 variants['candidate']=candidate
 results['artifacts']['candidate']={'path':str(candidate/'fusion-auxiliary-balance.o'),'sha256':sha(candidate/'fusion-auxiliary-balance.o'),'bytes':(candidate/'fusion-auxiliary-balance.o').stat().st_size}
 for strict in (False,True):
  ex,log=execute('candidate'+('-F1' if strict else '-ON-characterization'),runner+[tmp/'legacy',candidate/'fusion-auxiliary-balance.o',candidate]+(['--strict-off'] if strict else []));results['candidate_tests'].append({'name':'candidate'+('-F1' if strict else '-ON'),'execution':ex,'observed':re.findall(r'^(?:object_hash|OFF_CONTRACT|PASS).*$',log,re.M)})
  if ex['returncode']:finish();raise SystemExit(2)
 # Actual supplied source functions, not rewritten consumers.
 (tmp/'consumer_slices.h').write_text(function((ritual/'performance_chaos.h').read_text(),'ritual_encore_eligible')+function((ritual/'mod.c').read_text(),'borrowed_lease')+function(tw.read_text(),'musician_present'))
 shutil.copy2(tw,tmp/'time_wizard_under_test.c')
 names=symbols([source/'fusion-auxiliary-balance.o',ritual/'ritual-card-effects.o']+[variants[p]/'fusion-auxiliary-balance.o' for p in ('hide','revoke','candidate')]);(tmp/'export_names.h').write_text('#define TEST_EXPORT_COUNT '+str(len(names))+'\nstatic const char*test_export_names[]={'+','.join(json.dumps(n) for n in names)+'};\n')
 sources=['tests/hal21/manager_fixture.c','tests/hal21/manager_bridge.c','tests/hal21/tw_adapter.c','src/pc/compat/fs.c','src/pc/mods/events.c','src/pc/mods/hooks.c','src/pc/mods/manager.c','src/pc/mods/overlap.c','src/pc/mods/mod_libc.c','src/pc/mods/object_loader.c','src/pc/guest/branch_thunks.c','src/pc/mod_packets.c','src/pc/mods/json.c','src/pc/platform/settings.c','src/pc/platform/paths.c']
 build('build-real-manager',cc+['-m32','-std=gnu11','-O1','-D_FILE_OFFSET_BITS=64','-DMEMORIES_PC','-ffunction-sections','-fdata-sections','-I'+str(tmp),'-iquote'+str(source),'-Isrc',*sources,'-Wl,--gc-sections','-Wl,--wrap=Hooks_Add','-Wl,--wrap=Mods_RegisterState','-Wl,--wrap=ObjectLoader_Load','-lpng','-lm','-lpthread','-o',tmp/'manager'])
 for policy in ('baseline','hide','revoke','candidate'):
  for order in ('FR','RF','F'):
   case=tmp/(policy+'-'+order);mods=case/'mods';mods.mkdir(parents=True)
   for id,src in [('fusion-auxiliary-balance',variants[policy])]+([('ritual-card-effects-test',ritual)] if 'R' in order else []):
    dst=mods/id;dst.mkdir();shutil.copy2(src/'mod.json',dst/'mod.json');obj='ritual-card-effects.o' if id.startswith('ritual') else 'fusion-auxiliary-balance.o';shutil.copy2(src/obj,dst/obj)
    if (src/'assets').is_dir():shutil.copytree(src/'assets',dst/'assets')
   settings=case/'settings.txt';settings.write_text('mod.fusion-auxiliary-balance=1\nmod.ritual-card-effects-test=1\nmod.fusion-auxiliary-balance.order='+str(order.index('F'))+'\nmod.ritual-card-effects-test.order='+str(order.index('R') if 'R' in order else 0)+'\n')
   env=dict(os.environ,MEMORIES_MODS_DIR=str(mods),MEMORIES_SETTINGS=str(settings),MEMORIES_USER_DIR=str(case/'user'),HAL21_CANDIDATE_CHECKS='1' if policy=='candidate' else '0');ex,log=execute('manager-'+policy+'-'+order,runner+[tmp/'manager',repo,'revoke' if policy=='candidate' else policy],env)
   results['manager_profiles'].append({'policy':policy,'order':order,'execution':ex,'checks':[dict(name=n,status=s,details=d) for n,s,d in re.findall(r'^RESULT (\S+) (PASS|FAIL) (.*)$',log,re.M)],'observed':re.findall(r'^(?:CONTRACT|IDENTITY|TIME_WIZARD|FORCED_APPLIED|LOADED|SUMMARY).*$',log,re.M),'complete':'SUMMARY ' in log})
   if ex['returncode'] not in (0,1) or 'SUMMARY ' not in log:finish();raise SystemExit(2)
 # Independent process, actual registry with Fusion absent (not a null-returning mock).
 case=tmp/'absent';mods=case/'mods';dst=mods/'ritual-card-effects-test';dst.mkdir(parents=True)
 for name in ('mod.json','ritual-card-effects.o'):shutil.copy2(ritual/name,dst/name)
 if (ritual/'assets').is_dir():shutil.copytree(ritual/'assets',dst/'assets')
 settings=case/'settings.txt';settings.write_text('mod.ritual-card-effects-test=1\n')
 ex,log=execute('manager-absent-R',runner+[tmp/'manager',repo,'baseline'],dict(os.environ,MEMORIES_MODS_DIR=str(mods),MEMORIES_SETTINGS=str(settings),MEMORIES_USER_DIR=str(case/'user')))
 results['manager_profiles'].append({'policy':'absent','order':'R','execution':ex,'checks':[dict(name=n,status=s,details=d) for n,s,d in re.findall(r'^RESULT (\S+) (PASS|FAIL) (.*)$',log,re.M)],'complete':'SUMMARY ' in log})
 if ex['returncode'] or 'SUMMARY ' not in log:finish();raise SystemExit(2)
 # Next startup with installed Fusion preference OFF; no callback/state from prior process carried over.
 dst=mods/'fusion-auxiliary-balance';dst.mkdir()
 for name in ('mod.json','fusion-auxiliary-balance.o'):shutil.copy2(candidate/name,dst/name)
 settings.write_text('mod.ritual-card-effects-test=1\nmod.fusion-auxiliary-balance=0\n')
 ex,log=execute('manager-restart-startup-OFF',runner+[tmp/'manager',repo,'baseline'],dict(os.environ,MEMORIES_MODS_DIR=str(mods),MEMORIES_SETTINGS=str(settings),MEMORIES_USER_DIR=str(case/'user')))
 results['manager_profiles'].append({'policy':'startup-off','order':'FR','execution':ex,'checks':[dict(name=n,status=s,details=d) for n,s,d in re.findall(r'^RESULT (\S+) (PASS|FAIL) (.*)$',log,re.M)],'complete':'SUMMARY ' in log})
 if ex['returncode'] or 'SUMMARY ' not in log:finish();raise SystemExit(2)
 for f,p in checkfiles:results['postflight'].append(dict(path=str(p),status='PASS' if sha(p)==f['sha256'] else 'FAIL'))
finish();raise SystemExit(1 if len([p for p in results['manager_profiles'] if p['policy']=='candidate'])!=3 or any(p['execution']['returncode'] or any(c['status']!='PASS' for c in p['checks']) for p in results['manager_profiles'] if p['policy']=='candidate') else 0)
