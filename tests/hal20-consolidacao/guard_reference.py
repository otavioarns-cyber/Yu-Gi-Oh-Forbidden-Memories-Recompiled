"""Exercise the PR20 version gate without incorporating it in production/core.
Hook traversal is deliberately unreachable with unqualified new RD objects.
"""
from pathlib import Path
import os,argparse,subprocess,shlex,json,hashlib,tempfile
root=Path(__file__).resolve().parents[2];p=argparse.ArgumentParser(description=__doc__);p.add_argument('--reference',type=Path,required=True);p.add_argument('--rd-code',required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True);src=a.reference.resolve()/'src';cc=shlex.split(os.environ['MEMORIES_I386_CC']);runner=shlex.split(os.environ.get('MEMORIES_I386_RUNNER',''));rows=[]
with tempfile.TemporaryDirectory(prefix='hal25-guard-reference-') as tmp:
 tmp=Path(tmp);commands=[cc+['-m32','-O1','-ffunction-sections','-fdata-sections','-I'+str(src),'-c',str(src/'pc/mods/manager.c'),'-o',str(tmp/'reference.o')],cc+['-m32','-O1','-I'+str(src),str(root/'tests/hal20-consolidacao/guard_probe.c'),str(tmp/'reference.o'),'-Wl,--gc-sections','-o',str(tmp/'probe')]]
 for i,cmd in enumerate(commands):
  q=subprocess.run(cmd,cwd=root,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT);(a.output/('build-'+str(i)+'.log')).write_text(q.stdout)
  if q.returncode:raise SystemExit(q.returncode)
 for ai in (0,1):
  for dual in (0,1):
   q=subprocess.run(runner+[str(tmp/'probe'),a.rd_code,str(ai),str(dual)],text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT);rows.append(dict(ai=ai,dual=dual,status='FAIL' if q.returncode else 'REFUSED' if dual else 'NOT_APPLICABLE',output=q.stdout));print(rows[-1],flush=True)
(a.output/'RESULTADOS.json').write_text(json.dumps(dict(reference_commit=subprocess.check_output(['git','rev-parse','HEAD'],cwd=a.reference,text=True).strip(),manager_sha256=hashlib.sha256((src/'pc/mods/manager.c').read_bytes()).hexdigest(),scope='Unchanged PR20 version predicate, no duel/callback traversal; unavailable chain stubs must not run',cases=rows),indent=2)+'\n');raise SystemExit(any(r['status']=='FAIL' for r in rows))
