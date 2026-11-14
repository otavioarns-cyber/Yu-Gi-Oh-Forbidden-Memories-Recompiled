"""Check original packages, exact experimental deltas, manifests/assets and ABI.
No recompilation or dependency download. Original reference hashes accompany PR24.
"""
from pathlib import Path
import argparse,hashlib,json,subprocess
root=Path(__file__).resolve().parents[2];p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,required=True);a=p.parse_args();sha=lambda f:hashlib.sha256(f.read_bytes()).hexdigest();rows=[]
original=json.loads((root/'notes/hal-25-contract/ORIGINAL_INPUTS_PR24.json').read_text())
for c in original['components']:
 d=root/c['directory'];changes=[]
 for x in c['files']:
  f=d/x['path'];assert f.exists(),f
  if sha(f)!=x['sha256']:changes.append(x['path'])
 assert set(changes)=={'mod.c',d.name+'.o','SDK-PROVENANCE.json','README-PREVIEW2.md'}
 expected={'mandatory_hand_selection_v1.h','hand_selection_snapshot.h','mandatory_hand_provider.h' if c['symbol']=='R' else 'mandatory_hand_consumer.h'}
 old={x['path'] for x in c['files']};added={str(f.relative_to(d)) for f in d.rglob('*') if f.is_file() and '__pycache__' not in f.parts and str(f.relative_to(d)) not in old};assert added==expected
 prov=json.loads((d/'SDK-PROVENANCE.json').read_text())
 assert all(sha(d/name)==digest for name,digest in prov['production_inputs_sha256'].items())
 assert sha(d/(d.name+'.o'))==prov['artifact']['sha256']
 rows.append(dict(component=c['symbol'],status='PASS',changed=changes,added=sorted(added),other_original_files_unchanged=len(c['files'])-len(changes)))
for name in ('mandatory_hand_selection_v1.h','hand_selection_snapshot.h'):assert sha(root/'mods/ritual-card-effects'/name)==sha(root/'mods/ritual-duelists'/name)
for label in ('R','RD'):
 b=json.loads((root/'notes/hal-25-contract'/('ABI_'+label+'.json')).read_text())
 assert b['candidates'][0]['checks']['public_exports_unchanged']
 assert b['candidates'][0]['checks']['imports_in_SDK']
 assert not b['candidates'][0]['removed_imports']
assert not subprocess.run(['git','diff','--quiet',original['base_commit'],'--','src','mods/ai-hard-mode','mods/drop-missing-cards'],cwd=root).returncode
assert not any(x in ''.join(f.read_text() for f in (root/'tests/hal25-contract').glob('*') if f.suffix in ('.c','.inc')) for x in ('local(RE,"cosmo_force','local(RE,"cosmo_constrain','policy_bridge','install_test_bridge'))
rows.append(dict(check='core-other-official-mods-unchanged-shared-headers-and-no-private-bridge',status='PASS'))
a.output.write_text(json.dumps(rows,indent=2)+'\n');print('PASS',len(rows),'source/provenance groups')
