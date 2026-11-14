"""Stage one pinned collection privately; never fetch Dual or publish its files."""
from pathlib import Path
import argparse,hashlib,json,shutil
root=Path(__file__).resolve().parents[2];p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,required=True);p.add_argument('--profile',choices=('collection','owned-only'),default='collection');p.add_argument('--ai',type=Path);p.add_argument('--dual',type=Path);a=p.parse_args();out=a.output.resolve();assert not out.is_relative_to(root),'Private staging must be outside the Git working tree';assert not out.exists(),'Output already exists; will not overwrite it';assert a.profile=='owned-only' or (a.ai and a.dual),'Full collection needs exact official AI and locally authorized Dual directories'
sha=lambda f:hashlib.sha256(f.read_bytes()).hexdigest();trees=json.loads((root/'notes/hal-20-consolidacao/OWNED_TREES.json').read_text());qualified=json.loads((root/'notes/hal-20-consolidacao/QUALIFIED_ARTIFACTS.json').read_text());pins={x['id']:x for x in qualified['roles']};sources=[]
for name,entry in trees.items():
 src=root/'mods'/name
 for f,digest in entry['files'].items():assert sha(src/f)==digest,(name,f,'wrong bytes')
 sources.append((src,list(entry['files']),False))
if a.profile=='collection':
 for src,mod in ((a.ai,'ai-hard-mode'),(a.dual,'dual-play')):
  src=src.resolve();m=json.loads((src/'mod.json').read_text());assert m['id']==mod;obj=m['library']+'.o';assert sha(src/obj)==pins[mod]['sha256'],(mod,'unqualified object')
  if mod=='dual-play':assert sha(src/'mod.json')=='c3244ab680764a0d930c12929c5adf8ae3128e4d6b981f5e06ff3eb624839563','Unexpected private manifest'
  else:assert sha(src/'mod.json')=='e1fb7bc7ddcc4d7cd57b56818cc3cc4a71dc03b07c13d5abfefeb6a08c79e90f','Unexpected official manifest'
  sources.append((src,['mod.json',obj],mod=='ai-hard-mode'))
sources.append((root/'mods/drop-missing-cards',['mod.json'],False));out.mkdir(parents=True);mods=out/'mods';mods.mkdir();receipt=[]
for src,files,restart in sources:
 m=json.loads((src/'mod.json').read_text());dst=mods/m['id'];dst.mkdir();assert sum(d.name==m['id'] for d in mods.iterdir())==1
 for f in files:(dst/f).parent.mkdir(parents=True,exist_ok=True);shutil.copy2(src/f,dst/f)
 if restart:m['restart']=True;(dst/'mod.json').write_text(json.dumps(m,ensure_ascii=False,indent=2)+'\n')
 receipt.append(dict(id=m['id'],source_manifest_sha256=sha(src/'mod.json'),staged_manifest_sha256=sha(dst/'mod.json'),experimental_restart_override=restart,files={f:sha(dst/f) for f in files}))
ranks={'drop-missing-cards':-100,'melhoria-dos-rituais':-50,'fusion-auxiliary-balance':-30,'ritual-card-effects-test':-20,'ai-hard-mode':0,'ritual-duelists':10,'dual-play':20}
(out/'settings.txt').write_text('experimental.hal2224_order_guard=1\n'+''.join(f'mod.{m["id"]}=1\nmod.{m["id"]}.order={ranks[m["id"]]}\n' for m in receipt))
(out/'RECEIPT.json').write_text(json.dumps(dict(profile=a.profile,warning='Experimental collection; save-state loads deliberately refused; no Windows gameplay certification; do not redistribute private Dual.',mods=receipt),indent=2)+'\n');print('PASS staged',len(receipt),'distinct IDs; all original strategic settings retained; output stays private:',out)
