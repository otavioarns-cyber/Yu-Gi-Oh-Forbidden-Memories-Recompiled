"""Audit unchanged canonical packages and exact experimental source deltas.
This replaces no original assertion: PR13's immutable-source check must reject
changed candidates. This separate audit checks the authorized experimental diff.
"""
from pathlib import Path
import hashlib,json,tempfile,shutil,subprocess,argparse
repo=Path(__file__).resolve().parents[2];p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,required=True);a=p.parse_args()
sha=lambda f:hashlib.sha256(f.read_bytes()).hexdigest()
meta=json.loads((repo/'notes/hal-25/INPUTS.json').read_text());rows=[]
for c in meta['components']:
 for entry in c['files']:
  f=repo/c['directory']/entry['path'];assert sha(f)==entry['sha256'],f
 rows.append(dict(check='canonical-'+c['symbol'],status='PASS',files=len(c['files'])))
with tempfile.TemporaryDirectory(prefix='hal25-provenance-') as temp:
 for v in ('a','b'):
  original=repo/'mods/ritual-duelists';dst=Path(temp)/v;shutil.copytree(original,dst)
  subprocess.run(['patch','-p1','--batch','-i',str(repo/'experiments/hal25'/('alternative-'+v+'.patch'))],cwd=dst,check=True,stdout=subprocess.DEVNULL)
  if v=='b':shutil.copy2(repo/'experiments/hal25/selection-rules-v1.h',dst/'selection-rules-v1.h')
  changes=[str(f.relative_to(dst)) for f in sorted(dst.rglob('*')) if f.is_file() and (not (original/f.relative_to(dst)).exists() or sha(f)!=sha(original/f.relative_to(dst)))];assert changes==(['mod.c'] if v=='a' else ['mod.c','selection-rules-v1.h']),changes
  # No Queen IDs or private producer symbols may enter RD production patches.
  additions='\n'.join(x[1:] for x in (repo/'experiments/hal25'/('alternative-'+v+'.patch')).read_text().splitlines() if x.startswith('+') and not x.startswith('+++'))
  assert not any(x in additions for x in ('708','395','cosmo_','ObjectLoader_'))
  inputs={str(f.relative_to(dst)):sha(f) for f in sorted(dst.glob('*')) if f.is_file() and f.suffix in ('.c','.h')};assert len(list(dst.glob('*.c')))==16
  rows.append(dict(check='candidate-'+v,status='PASS',source_changes=changes,source_inputs_sha256=inputs,manifest_sha256=sha(dst/'mod.json')))
abi=json.loads((repo/'notes/hal-25/ABI.json').read_text())
for item in abi['candidates']:
 assert all(item['checks'].values());assert sha(repo/item['candidate']['path'])==item['candidate']['sha256']
assert not subprocess.run(['git','diff','--quiet',meta['base_commit'],'--','src','mods/ai-hard-mode','mods/drop-missing-cards'],cwd=repo).returncode
rows.append(dict(check='core-AI-Drop-unchanged',status='PASS'))
a.output.write_text(json.dumps(rows,indent=2)+'\n');print('PASS',len(rows),'provenance groups; exact source deltas and object ABI preserved')
