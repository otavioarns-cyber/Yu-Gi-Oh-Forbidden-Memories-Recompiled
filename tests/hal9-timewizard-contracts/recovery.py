"""Search available Git history for exact core definitions; emit metadata only.
A negative result covers only the supplied refs/history, not unavailable backups.
"""
from pathlib import Path
import argparse,subprocess,json,time,re,hashlib
p=argparse.ArgumentParser();p.add_argument('--refs',type=Path,required=True);p.add_argument('--package',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();root=Path(__file__).resolve().parents[2];heads=[x.split()[0] for x in a.refs.read_text().splitlines()];t=time.monotonic();q=subprocess.run(['git','log',*heads,'--format=%H %s','--name-only','--regexp-ignore-case','-G','TimeWizardOriginalMemoriesModInit|snapshot_targets','--','*.c','*.h'],cwd=root,capture_output=True,text=True,timeout=120)
assert q.returncode==0,q.stderr
sources=[dict(path=f.relative_to(a.package).as_posix(),sha256=hashlib.sha256(f.read_bytes()).hexdigest()) for f in sorted((a.package/'source').glob('*.c'))]
r=dict(scope=__doc__,heads=len(heads),refs_sha256=hashlib.sha256(a.refs.read_bytes()).hexdigest(),history_search=dict(returncode=q.returncode,matching_bytes=len(q.stdout),seconds=round(time.monotonic()-t,3),pattern='TimeWizardOriginalMemoriesModInit|snapshot_targets',include=['*.c','*.h']),history_hits=q.stdout.splitlines(),available_C=sources,full_core_recovered=False,debug_info='Only description-fit compilation-unit metadata in supplied core; primary gameplay/AI core source absent.',required_for_full_rebuild='Exact 0.23.1 primary gameplay+AI+roulette sources/headers and original toolchain/build/link recipe, matching pinned core; current AI C is an unlinked adapter.',remaining_option='Strictly pinned object/API24 compatibility repair, not reconstruction of missing effects.')
a.output.write_text(json.dumps(r,indent=2)+'\n');print('PASS recovery search',len(heads),'heads;',len(q.stdout),'matching bytes; no full core found')
