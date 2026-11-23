"""Publish only own sources and textual evidence; private objects remain local."""
from pathlib import Path
import argparse,collections,hashlib,json,re,shutil,subprocess,sys
p=argparse.ArgumentParser();p.add_argument('--work',type=Path,required=True);a=p.parse_args();r=Path(__file__).resolve().parents[2];out=r/'notes/hal-9-rd-lifecycle';out.mkdir(exist_ok=True);sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest();groups={};raw={};execution=collections.Counter();checks=collections.Counter()
def compress(src,dst):
 counts=collections.Counter();lines=[]
 for line in src.read_text().splitlines():
  counts[line]+=1
  if counts[line]<=3:lines.append(line)
 dst.write_text('\n'.join(lines)+'\n');return dict(sha256=sha(src),bytes=src.stat().st_size,identical_lines_omitted={k:v-3 for k,v in counts.items() if v>3})
for group in ('owner','extra','bonus','blocked','regressions','absence'):
 src=a.work/'logs'/('final-'+group);dst=out/group;dst.mkdir(exist_ok=True);j=json.loads((src/'RESULTADOS.json').read_text());assert not j['summary']['incomplete']
 if group=='owner':j['profiles']=[q for q in j['profiles'] if q['mode'] not in (314,319)]
 ex=collections.Counter(q['classification'] for q in j['profiles']);ch=collections.Counter(c['status'] for q in j['profiles'] for c in q['checks']);summary=dict(processes=len(j['profiles']),execution=dict(ex),checks=dict(ch),incomplete=0);groups[group]=summary;execution.update(ex);checks.update(ch)
 j['raw_sha256']=sha(src/'RESULTADOS.json');j['summary']=summary
 for q in j['profiles']:
  counts=collections.Counter();keep=[]
  for line in q.get('observations',[]):
   counts[line]+=1
   if counts[line]<=3:keep.append(line)
  q['observations']=keep;q['identical_observations_omitted']={k:v-3 for k,v in counts.items() if v>3}
  raw[group+'/'+q['log']]=compress(src/q['log'],dst/q['log'])
 (dst/'RESULTADOS.json').write_text(json.dumps(j,indent=2)+'\n')
 import csv
 with (src/'MATRIZ.csv').open() as f:
  rd=csv.DictReader(f);rows=[q for q in rd if not(group=='owner' and q['case'].endswith(('-m314','-m319')))];fields=rd.fieldnames
 assert {x['case'] for x in rows}=={q['name'] for q in j['profiles']},group
 with (dst/'MATRIZ.csv').open('w') as f:
  wr=csv.DictWriter(f,fieldnames=fields,lineterminator="\n");wr.writeheader();wr.writerows(rows)
 for f in src.glob('build*.log'):raw[group+'/'+f.name]=compress(f,dst/f.name)
summary=dict(processes=sum(g['processes'] for g in groups.values()),execution=dict(execution),checks=dict(checks),groups=groups,scope='Actual loaded owner callbacks and native Main_RunDuel; InitScene IO/frontend/renderer, phase-entry dispatch and new boards modeled. No Windows gameplay certification.')
assert summary['processes']==json.loads((a.work/'logs/INVARIANTS.json').read_text())['profiles']
(out/'RESULTADOS.json').write_text(json.dumps(summary,indent=2)+'\n');(out/'RAW_LOGS.json').write_text(json.dumps(raw,indent=2)+'\n')
for src,dst in ((a.work/'local/candidate-v2/BUILD.json','BUILD_CANDIDATE.json'),(a.work/'local/repeat-v2/BUILD.json','BUILD_REPEAT.json'),(a.work/'local/ABI.json','ABI.json'),(a.work/'local/ABI_LAYOUT.json','ABI_LAYOUT.json'),(a.work/'local/REFERENCE_SOURCE_AUDIT.json','REFERENCE_SOURCE_AUDIT.json'),(a.work/'logs/INVARIANTS.json','INVARIANTS.json'),(a.work/'REFS_BEFORE.tsv','REFS_BEFORE.tsv')):shutil.copy2(src,out/dst)
shutil.copy2(a.work/'local/candidate-v2/owner.patch',r/'experiments/hal9-rd-lifecycle/owner.patch')
controls=[];cd=out/'controls';cd.mkdir(exist_ok=True)
for src in sorted((a.work/'logs').iterdir()):
 if not src.is_dir() or src.name.startswith('final-'):continue
 f=src/'RESULTADOS.json'
 if not f.exists():continue
 j=json.loads(f.read_text());controls.append(dict(name=src.name,summary=j['summary'],source_sha256=j.get('test_sources_sha256'),owner_source_sha256=j.get('rd_owner_sources_sha256'),raw_sha256=sha(f),failures=[dict(case=q['name'],classification=q['classification'],checks=[c for c in q['checks'] if c['status']=='FAIL']) for q in j['profiles'] if q['classification'] in ('FAIL','INCOMPLETE')]))
 for f in src.glob('tw-object-h0-d0-before-m*.log'):
  if f.name.endswith(('-m300.log','-m301.log','-m309.log','-m312.log','-m313.log')):compress(f,cd/(src.name+'-'+f.name))
j=json.loads((a.work/'logs/final-owner/RESULTADOS.json').read_text());control=[q for q in j['profiles'] if q['mode']==314];controls.append(dict(name='persistent-no-ACK-fault-314',summary=dict(processes=len(control),checks=dict(collections.Counter(c['status'] for q in control for c in q['checks']))),interpretation='Fault proxy remained installed after reconnect, correctly refusing every new confirmation. Extra314 removes only the test fault after real disconnect ACK. Production candidate unchanged.',profiles=control))
labels=[q for q in j['profiles'] if q['mode']==319];controls.append(dict(name='bonus-label319',interpretation='Dual OFF has no bonus; original log label PASS concerned the completed new-duel action. Final focused bonus repeat explicitly emits NOT_REACHED for the absent bonus. Checks unchanged.',profiles=labels))
for q in labels:
 compress(a.work/'logs/final-owner'/q['log'],cd/('bonus-label-'+q['log']))
 old=out/'owner'/q['log']
 if old.exists():old.unlink()
compress(a.work/'logs/final-owner/tw-object-h0-d0-before-m314.log',cd/'persistent-no-ACK-314.log');shutil.copy2(a.work/'local/runtime-before-ack-fault.inc',cd/'runtime-before-ack-fault.inc')
for name in ('extra','bonus'):shutil.copy2(a.work/'local'/('runtime-'+name+'.inc'),cd/('runtime-'+name+'.inc'))
(out/'CONTROLS.json').write_text(json.dumps(controls,indent=2)+'\n')
ref='c8a5605852707589d3d166ad153a88fdd0017b30';files=subprocess.check_output(['git','ls-tree','-r','--name-only',ref,'mods'],cwd=r,text=True).splitlines();assert len(files)==507
for f in files:assert (r/f).read_bytes()==subprocess.check_output(['git','show',ref+':'+f],cwd=r)
assert not subprocess.check_output(['git','diff',ref,'--','src','mods','config','.github'],cwd=r)
b=json.loads((a.work/'local/candidate-v2/BUILD.json').read_text());b2=json.loads((a.work/'local/repeat-v2/BUILD.json').read_text());assert b['candidate_sha256']==b2['candidate_sha256']
(out/'AUDIT.json').write_text(json.dumps(dict(reference=ref,canonical_mod_files_identical=len(files),src_mods_config_workflows='UNCHANGED',RD_reference_sha256=b['reference_sha256'],RD_private_candidate_sha256=b['candidate_sha256'],repeat_byte_identical=True,TW_private_sha256=sha(Path('/workspace/work/hal9-tw-recovery-hardening/local/delivery/package/time-wizard.o')),Dual_sha256=sha(Path('/workspace/work/hal20-dual/package/dual-play.o')),manifest_settings_assets_ABI1_companion='UNCHANGED',guard='UNCHANGED; TW not admitted',savestate='B preserved; A disabled',default='OBSERVE; KEEP/ALLOW only explicit tests',private_binaries_published=False),indent=2)+'\n')
sys.path.insert(0,str(r/'tests/hal20-field-state'));from elf_inventory import elf_info
paths={}
for folder in [r/'mods'/d for d in ('melhoria-dos-rituais','ritual-card-effects','fusion-auxiliary-balance','field-effects-secondary-types','ai-hard-mode')]+[a.work/'local/candidate-v2/ritual-duelists',Path('/workspace/work/hal20-dual/package'),Path('/workspace/work/hal9-tw-recovery-hardening/local/delivery/package')]:
 m=json.loads((folder/'mod.json').read_text());paths[m['id']]=folder/(m['library']+'.o')
info={k:elf_info(v) for k,v in paths.items()};wrappers={}
for mode in (301,303,304,309,312,313,319):
 src=a.work/'logs/final-owner';f=src/f'tw-object-h1-d1-before-m{mode}.log';s=f.read_text();nodes=[]
 for owner,code,target,name,size,node in re.findall(r'^CALLBACK_NODE owner=(\S+) code=(\w+) target=(\S+) function=(\S+) bytes=(\d+) node=(-?\d+)$',s,re.M):
  if target not in ('Main_RunDuel','Duel_InitScene','Duel_PopulateCombinedDeckData','AiScript_Run','func_8001BAF0','func_80028220','DuelScene_UpdateHandActions','DuelScene_UpdateCardPlacement'):continue
  matches=[{k:v[k] for k in ('section','offset','bytes','body_sha256','relocations_sha256')} for v in info.get(owner,{}).get('functions',[]) if v['name']==name and v['bytes']==int(size)]
  nodes.append(dict(node=int(node),owner=owner,target=target,function=name,bytes=int(size),artifact_sha256=info.get(owner,{}).get('sha256'),code_hash=code,symbol_matches=matches))
 wrappers[str(mode)]=dict(log=f.name,nodes=nodes,edges=sorted(set(re.findall(r'^CALLBACK_EDGE parent=(-?\d+) child=(-?\d+)$',s,re.M))),admission='None; source labels do not qualify callbacks. Main/Init permit and exact pinned artifacts are experimental only.')
(out/'WRAPPERS.json').write_text(json.dumps(wrappers,indent=2)+'\n')
assert not [f for f in out.rglob('*') if f.is_file() and f.suffix.lower() in ('.o','.exe','.dll','.bin','.iso','.cue','.zip','.png','.wav','.mrg','.pyc')]
print(json.dumps(summary))
