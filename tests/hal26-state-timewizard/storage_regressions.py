"""Official save-slot/menu tests, real temporary file I/O, no personal save/game."""
from pathlib import Path
import argparse,subprocess,json,os
p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=False);root=Path(__file__).resolve().parents[2];rows=[];env=dict(os.environ,TMPDIR=str(a.output))
for n in ('save_slots','save_menu'):
 sources=['tests/pc/'+n+'_test.c','src/pc/saves/save_slots.c','src/pc/platform/paths.c']+(['src/pc/saves/save_menu.c'] if n=='save_menu' else [])
 # Match CMake's default unoptimized strict unit build; retain Werror.
 for tag,cmd in [('build',['cc','-std=gnu11','-Wall','-Wextra','-Wpedantic','-Werror','-Isrc','-Itests/pc',*sources,'-o',str(a.output/n)]),('run',[str(a.output/n)])]:
  q=subprocess.run(cmd,cwd=root,env=env,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=60);(a.output/(n+'-'+tag+'.log')).write_text(q.stdout);rows.append(dict(test=n+'-'+tag,command=cmd,returncode=q.returncode));assert not q.returncode,q.stdout
# Reproduce the earlier -O1/Werror compile problem in unchanged baseline source.
src='src/pc/saves/save_slots.c';base=a.output/'save_slots-baseline.c';base.write_bytes(subprocess.check_output(['git','show','3284ec493d179f4b23cc7230a2979a1c2b122873:'+src],cwd=root));assert base.read_bytes()==(root/src).read_bytes()
for name,path in [('baseline',str(base)),('candidate',src)]:
 cmd=['cc','-std=gnu11','-O1','-Wall','-Wextra','-Wpedantic','-Werror','-Isrc','-Isrc/pc/saves','-c',path,'-o',str(a.output/(name+'.o'))];q=subprocess.run(cmd,cwd=root,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=60);(a.output/(name+'-O1-Werror.log')).write_text(q.stdout);rows.append(dict(test=name+'-O1-Werror',returncode=q.returncode,status='PREEXISTING_BUILD_FAIL' if q.returncode else 'PASS',command=cmd));assert q.returncode!=0 and 'format-truncation' in q.stdout
(a.output/'RESULTADOS.json').write_text(json.dumps(dict(scope='Official storage unit file/menu state machine tests; not gameplay/UI Windows. Default strict build PASS; unchanged baseline and candidate both fail extra optimized GCC Werror compile.',steps=rows),indent=2)+'\n');print('PASS official save slots/menu; preserved preexisting -O1/Werror failure separately')
