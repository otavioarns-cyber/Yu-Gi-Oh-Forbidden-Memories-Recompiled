"""Build isolated R Queen + F lease variants; canonical collection untouched."""
from pathlib import Path
import argparse,subprocess,sys,shutil,json,hashlib,difflib
p=argparse.ArgumentParser();p.add_argument('--sdk',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();root=Path(__file__).resolve().parents[2];a.output.mkdir(parents=True,exist_ok=False)
q=subprocess.run([sys.executable,str(root/'tests/hal20-field-state/build_state_preview.py'),'--sdk',str(a.sdk),'--output',str(a.output/'R')],cwd=root,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=60);(a.output/'R-build.log').write_text(q.stdout);assert q.returncode==0,q.stdout
src=root/'mods/fusion-auxiliary-balance';dst=a.output/'fusion-auxiliary-balance';shutil.copytree(src,dst);f=dst/'mod.c';before=f.read_text();needle='int MemoriesModInit(';assert before.count(needle)==1;s=before.replace(needle,'#include "fusion_lease_preview.h"\n'+needle,1);needle='    mod->reset=aux_reset;mod->applied=ea_apply;';assert needle in s;s=s.replace(needle,needle+'\n    if(!fl_init(mod))return 0;',1);f.write_text(s)
for name in ['fusion_lease_preview.h','fusion_lease_schema_v1.h']:shutil.copy2(root/'experiments/hal26'/name,dst/name)
for name in ['mandatory_hand_lifecycle_v1.h','mandatory_hand_selection_v1.h']:shutil.copy2(root/'mods/ritual-duelists'/name,dst/name)
(a.output/'fusion.patch').write_text(''.join(difflib.unified_diff(before.splitlines(True),s.splitlines(True),fromfile='a/mod.c',tofile='b/mod.c')))
q=subprocess.run([sys.executable,str(a.sdk/'tools/build_mod.py'),str(dst),'--target','i386'],cwd=root,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=60);(a.output/'F-build.log').write_text(q.stdout);assert q.returncode==0,q.stdout
(a.output/'BUILD.json').write_text(json.dumps(dict(scope='Partial Queen+Fusion lease schemas only, no complete owner coverage and no collection admission.',artifacts=[dict(path=str(p),bytes=p.stat().st_size,sha256=hashlib.sha256(p.read_bytes()).hexdigest()) for p in [a.output/'R/ritual-card-effects/ritual-card-effects.o',dst/'fusion-auxiliary-balance.o']]),indent=2)+'\n')
print('PASS isolated pair SDK24/i386; no canonical object replaced')
