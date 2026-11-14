"""Rebuild mutually exclusive HAL25 experiments in separate output directories.
The canonical packages remain PR11/PR13. SDK root and compiler are external.
"""
from pathlib import Path
import argparse,os,shutil,subprocess,json,hashlib
repo=Path(__file__).resolve().parents[2]
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--sdk',type=Path,required=True)
p.add_argument('--work',type=Path,required=True,help='New directory, outside mods/; preserved for provenance')
a=p.parse_args();work=a.work.resolve();work.mkdir(parents=True,exist_ok=False)
sha=lambda f:hashlib.sha256(f.read_bytes()).hexdigest()
rows=[]
for variant in ('a','b'):
 dst=work/('alternative-'+variant);shutil.copytree(repo/'mods/ritual-duelists',dst)
 subprocess.run(['patch','-p1','--batch','-i',str(repo/'experiments/hal25'/('alternative-'+variant+'.patch'))],cwd=dst,check=True)
 if variant=='b':shutil.copy2(repo/'experiments/hal25/selection-rules-v1.h',dst/'selection-rules-v1.h')
 # Remove PR13's object before invoking the official full-source builder.
 (dst/'ritual-duelists.o').unlink()
 cmd=['python',str(a.sdk.resolve()/'tools/build_mod.py'),str(dst),'--target','i386']
 q=subprocess.run(cmd,cwd=repo,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
 (work/('build-'+variant+'.log')).write_text(q.stdout)
 if q.returncode:raise SystemExit(q.returncode)
 obj=dst/'ritual-duelists.o';rows.append(dict(variant=variant,command=cmd,compiler=os.environ.get('MEMORIES_MOD_CC'),sha256=sha(obj),bytes=obj.stat().st_size))
(work/'BUILD.json').write_text(json.dumps(rows,indent=2)+'\n')
