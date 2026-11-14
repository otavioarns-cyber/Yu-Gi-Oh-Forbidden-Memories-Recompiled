"""Audit the one-header production delta against the frozen PR26 delivery."""
from pathlib import Path
import argparse,hashlib,json,subprocess
r=Path(__file__).resolve().parents[2];p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,required=True);a=p.parse_args();n=r/'notes/hal-25-boundary';sha=lambda f:hashlib.sha256(f.read_bytes()).hexdigest();base=json.loads((n/'PR26_PACKAGE_TREE.json').read_text());rows=[]
for name,files in base['packages'].items():
 d=r/'mods'/name;symbol='R' if name=='ritual-card-effects' else 'RD';changed=[]
 for path,digest in files.items():
  assert (d/path).is_file(),path
  if sha(d/path)!=digest:changed.append(path)
 expected={name+'.o','SDK-PROVENANCE.json','README-PREVIEW2.md'}
 if symbol=='RD':expected.add('mandatory_hand_consumer.h')
 assert set(changed)==expected,(name,changed)
 added={str(f.relative_to(d)) for f in d.rglob('*') if f.is_file() and '__pycache__' not in f.parts and str(f.relative_to(d)) not in files};assert not added,added
 prov=json.loads((d/'SDK-PROVENANCE.json').read_text());assert all(sha(d/f)==digest for f,digest in prov['production_inputs_sha256'].items());assert sha(d/(name+'.o'))==prov['artifact']['sha256']
 assert prov['production_inputs_sha256']==json.loads((n/'BUILD.json').read_text())['packages'][name]['production_inputs_sha256']
 abi=json.loads((n/('ABI_'+symbol+'.json')).read_text())['candidates'][0];assert abi['checks']['public_exports_unchanged'] and abi['checks']['imports_in_SDK'];assert not abi['removed_imports'];assert abi['added_imports']==([] if symbol=='R' else ['D_800E9F10'])
 rows.append(dict(package=name,status='PASS',changed=sorted(changed),unchanged=len(files)-len(changed),manifest_settings_assets_tests='byte-identical to PR26',public_exports=len(prov['artifact']['exports']),imports=len(prov['artifact']['imports'])))
for name in ('mandatory_hand_selection_v1.h','hand_selection_snapshot.h','mandatory_hand_lifecycle_v1.h'):
 assert sha(r/'mods/ritual-card-effects'/name)==sha(r/'mods/ritual-duelists'/name)
 for d in ('ritual-card-effects','ritual-duelists'):assert sha(r/'mods'/d/name)==base['packages'][d][name]
for path in ('src','tools','config','mods/ai-hard-mode','mods/drop-missing-cards'):
 assert not subprocess.run(['git','diff','--quiet','d649647c159c91d615c07d6960928a37e270b461','--',path],cwd=r).returncode,path
assert not subprocess.run(['git','diff','--quiet',base['commit'],'--','mods/ritual-duelists/mod.c','mods/ritual-card-effects/mod.c'],cwd=r).returncode
rows.append(dict(check='Frozen ABI1 and companion, original strategy/core/hooks and other official mods unchanged',status='PASS'))
a.output.write_text(json.dumps(rows,indent=2)+'\n');print('PASS',len(rows),'scope groups')
