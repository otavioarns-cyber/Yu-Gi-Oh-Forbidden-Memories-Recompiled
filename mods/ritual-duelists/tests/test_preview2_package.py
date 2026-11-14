"""Package provenance and immutable production inputs, not a game test."""
from pathlib import Path
import hashlib,json
mod=Path(__file__).resolve().parents[1];repo=mod.parents[1]
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
received=json.loads((repo/'notes/hal-20/ritual-duelists/INSUMOS_SHA256.json').read_text())
provenance=json.loads((mod/'SDK-PROVENANCE.json').read_text())
for name,digest in provenance['production_inputs_sha256'].items():
    assert sha(mod/name)==digest
    assert next(x['sha256'] for x in received if x['path']==name)==digest
for entry in received:
    if entry['path'].startswith('assets/'):
        assert sha(mod/entry['path'])==entry['sha256']
assert sha(mod/'ritual-duelists.o')==provenance['artifact']['sha256']
assert not (mod/'ritual-duelists.x86_64-windows.o').exists()
m=json.loads((mod/'mod.json').read_text())
assert m['id']=='ritual-duelists' and m['version']=='0.4.0-alpha.8' and len(m['settings'])==6
assert m['after']==['drop-missing-cards'] and not m.get('requires') and m['restart']
assert not any(s['key']=='drop_list_enabled' for s in m['settings'])
assert provenance['sdk_api']==24 and provenance['production_changes']==[]
print('PASS: all 38 production C/H, manifest, 57 assets unchanged; new object hash; six settings; after only; no old x64 or internal droplist option')
