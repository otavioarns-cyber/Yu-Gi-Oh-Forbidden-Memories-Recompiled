"""Audit the consolidated scope, complete owned trees and unchanged strategy/ABI."""
from pathlib import Path
import argparse,hashlib,json,re,subprocess
root=Path(__file__).resolve().parents[2];p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,required=True);a=p.parse_args();sha=lambda f:hashlib.sha256(f.read_bytes()).hexdigest();trees=json.loads((root/'notes/hal-20-consolidacao/OWNED_TREES.json').read_text());rows=[]
for name,item in trees.items():
 d=root/'mods'/name
 actual={str(f.relative_to(d)) for f in d.rglob('*') if f.is_file() and '__pycache__' not in f.parts};assert actual==item['files'].keys(),name
 for f,digest in item['files'].items():assert sha(d/f)==digest,(name,f)
 rows.append(dict(package=name,status='PASS',files=len(actual),reference_commit=item['reference_commit'],scope='Every source, object, manifest, setting, recipe, test and asset identical to authorized reference.'))
base='d649647c159c91d615c07d6960928a37e270b461'
def old(path):return subprocess.check_output(['git','show',base+':'+path],cwd=root,text=True)
allowed={'AGENTS.md','src/game/main_run_duel.c','src/pc/guest/state.c','src/pc/mods/hooks.c','src/pc/mods/hooks.h','src/pc/mods/manager.c','src/pc/mods/mods.c','src/pc/mods/mods.h','src/pc/mods/hal20_qualified.h','src/pc/platform/mods_window.c','tools/pc/preview_mods.sh','tools/pc/preview_panels.sh','tools/pc/test_mods_lifecycle.py'}
changed=subprocess.check_output(['git','diff',base,'--name-only'],cwd=root,text=True).splitlines()
assert all(f in allowed or f.startswith(('mods/melhoria-dos-rituais/','mods/fusion-auxiliary-balance/','mods/ritual-card-effects/','mods/ritual-duelists/','notes/','tests/hal20-consolidacao/')) for f in changed)
for path in ('mods/ai-hard-mode','mods/drop-missing-cards','src/pc/mods/modapi.h'):
 assert not subprocess.run(['git','diff','--quiet',base,'--',path],cwd=root).returncode,path
h=(root/'src/pc/mods/hooks.c').read_text();assert h[:h.index('\n/* Experimental qualification reads')]==old('src/pc/mods/hooks.c')
def function(source,name):
 m=re.search(r'(?m)^(?:static )?(?:int|void) '+name+r'\([^;{}]*\)\s*\{',source);assert m,name
 start=m.start();at=m.end();n=1
 while n:
  c=source[at];n+=int(c=='{')-int(c=='}');at+=1
 return source[start:at]
for name in ('Mods_Order','order_ranked'):
 assert function((root/'src/pc/mods/manager.c').read_text(),name)==function(old('src/pc/mods/manager.c'),name)
for name in ('mandatory_hand_selection_v1.h','mandatory_hand_lifecycle_v1.h','hand_selection_snapshot.h'):
 assert sha(root/'mods/ritual-card-effects'/name)==sha(root/'mods/ritual-duelists'/name)
s=(root/'src/pc/guest/state.c').read_text();start=s.index('static int load(const char *path)');block=s[start:];assert block.index('Mods_Hal20StateLoadAllowed')<block.index('fseek(')<block.index('pending_image')
rows.append(dict(status='PASS',scope='AI/DMC and public host ABI unchanged; original hook management file retained verbatim plus read-only query; Mods_Order/order_ranked unchanged; common load preflight precedes parsing/remapping.'))
a.output.write_text(json.dumps(rows,indent=2)+'\n');print('PASS',len(rows),'scope groups')
