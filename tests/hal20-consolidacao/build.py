"""Full-source rebuild of the two real experimental packages in fresh copies.
Does not replace the published objects or use received objects as build inputs.
"""
from pathlib import Path
import argparse,shutil,subprocess,hashlib,json
root=Path(__file__).resolve().parents[2];p=argparse.ArgumentParser(description=__doc__);p.add_argument('--sdk',type=Path,required=True);p.add_argument('--work',type=Path,required=True);a=p.parse_args();a.work.mkdir(parents=True,exist_ok=False);rows=[]
for name in ('ritual-card-effects','ritual-duelists'):
 dst=a.work.resolve()/name;shutil.copytree(root/'mods'/name,dst);obj=dst/(name+'.o');obj.unlink()
 cmd=['python',str(a.sdk.resolve()/'tools/build_mod.py'),str(dst),'--target','i386'];q=subprocess.run(cmd,cwd=root,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT);(a.work/(name+'.log')).write_text(q.stdout)
 if q.returncode:raise SystemExit(q.returncode)
 rows.append(dict(package=name,command=cmd,bytes=obj.stat().st_size,sha256=hashlib.sha256(obj.read_bytes()).hexdigest()))
(a.work/'BUILD.json').write_text(json.dumps(rows,indent=2)+'\n')
