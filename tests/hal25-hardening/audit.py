"""Audit exact PR25 deltas, provenance, frozen ABI, settings, assets and scope.
No object recompilation or private-package publication.
"""
from pathlib import Path
import argparse,hashlib,json,subprocess
root=Path(__file__).resolve().parents[2];p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,required=True);a=p.parse_args();sha=lambda f:hashlib.sha256(f.read_bytes()).hexdigest();n=root/'notes/hal-25-hardening';old=json.loads((n/'PR25_PACKAGE_TREE.json').read_text());rows=[]
for name,files in old['packages'].items():
 d=root/'mods'/name;symbol='R' if name=='ritual-card-effects' else 'RD';changed=[]
 for path,digest in files.items():
  f=d/path;assert f.is_file(),path
  if sha(f)!=digest:changed.append(path)
 expected={'mod.c','mandatory_hand_provider.h' if symbol=='R' else 'mandatory_hand_consumer.h',name+'.o','SDK-PROVENANCE.json','README-PREVIEW2.md'}
 assert set(changed)==expected,(name,changed)
 added={str(f.relative_to(d)) for f in d.rglob('*') if f.is_file() and '__pycache__' not in f.parts and str(f.relative_to(d)) not in files}
 assert added=={'mandatory_hand_lifecycle_v1.h'},added
 prov=json.loads((d/'SDK-PROVENANCE.json').read_text());assert all(sha(d/path)==digest for path,digest in prov['production_inputs_sha256'].items())
 assert prov['production_inputs_sha256']==json.loads((n/'BUILD.json').read_text())['packages'][name]['production_inputs_sha256']
 assert sha(d/(name+'.o'))==prov['artifact']['sha256']
 abi=json.loads((n/('ABI_'+symbol+'.json')).read_text());assert all(abi['candidates'][0]['checks'].values())
 rows.append(dict(package=name,status='PASS',changed=sorted(changed),added=sorted(added),unchanged=len(files)-len(changed),manifest_settings_assets_tests='byte-identical to PR25',native_exports=len(prov['artifact']['exports']),imports=len(prov['artifact']['imports'])))
for name in ('mandatory_hand_selection_v1.h','hand_selection_snapshot.h','mandatory_hand_lifecycle_v1.h'):assert sha(root/'mods/ritual-card-effects'/name)==sha(root/'mods/ritual-duelists'/name)
for path in ('src','tools','config','mods/ai-hard-mode','mods/drop-missing-cards'):
 assert subprocess.run(['git','diff','--quiet','d649647c159c91d615c07d6960928a37e270b461','--',path],cwd=root).returncode==0,path
# Verify strategic planner body unchanged, independently of the surrounding wrapper.
path='mods/ritual-duelists/mod.c';previous=subprocess.check_output(['git','show',old['commit']+':'+path],cwd=root,text=True);current=(root/path).read_text()
def planner(s):return s[s.index('static s32 rd_ai_plan(void)'):s.index('static s32 rd_ai(void)')]
assert planner(previous)==planner(current)
for f in (root/'tests/hal25-hardening').glob('*'):
 if f.suffix in ('.c','.inc'):assert not any(x in f.read_text() for x in ('local(RE,"cosmo_force','local(RE,"cosmo_constrain','install_test_bridge','policy_bridge'))
rows.append(dict(check='frozen-selection-ABI-snapshot-identical-new-companion-identical-core-and-strategy-unchanged-no-private-Queen-bridge',status='PASS'))
a.output.write_text(json.dumps(rows,indent=2)+'\n');print('PASS',len(rows),'audit groups')
