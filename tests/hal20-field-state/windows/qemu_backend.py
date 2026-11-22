"""Bounded Wine32/QEMU routing probe. Copies only a disposable local runtime bin.
No game image, personal prefix, system install, or modification to the source runtime.
"""
from pathlib import Path
import argparse,os,shutil,subprocess,json,hashlib
p=argparse.ArgumentParser();p.add_argument('--runtime',type=Path,required=True);p.add_argument('--qemu',type=Path,required=True);p.add_argument('--sysroot',type=Path,required=True);p.add_argument('--control',type=Path,required=True);p.add_argument('--work',type=Path,required=True);a=p.parse_args();out=a.work.resolve();out.mkdir(parents=True,exist_ok=False);runtime=out/'runtime';(runtime/'bin').mkdir(parents=True)
for name in ('lib','share'):
 if (a.runtime/name).exists():(runtime/name).symlink_to((a.runtime/name).resolve(),target_is_directory=True)
for name in ('wine','wineserver','wine-preloader'):
 src=a.runtime/'bin'/name
 if not src.exists():continue
 dst=runtime/'bin'/(name+'.real');shutil.copy2(src,dst)
 # Paths passed as arguments, shell-quoted; never use user text as code.
 import shlex
 wrapper=runtime/'bin'/name;wrapper.write_text('#!/bin/sh\nexec '+shlex.join([str(a.qemu.resolve()),'-L',str(a.sysroot.resolve()),str(dst)])+' "$@"\n');wrapper.chmod(0o755)
env=dict(os.environ,WINEPREFIX=str(out/'prefix'),WINEARCH='win32',WINELOADER=str(runtime/'bin/wine'),WINELOADERNOEXEC='1',WINEDEBUG='-all',WINEDLLOVERRIDES='mscoree,mshtml=');rows=[]
for cpu in (None,'max'):
 e=dict(env)
 if cpu:e['QEMU_CPU']=cpu
 try:
  q=subprocess.run([str(runtime/'bin/wine'),str(a.control.resolve())],cwd=out,env=e,text=True,errors='backslashreplace',stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=35);code=q.returncode;log=q.stdout
 except subprocess.TimeoutExpired as error:code=124;log=error.stdout or '';log=log.decode(errors='backslashreplace') if isinstance(log,bytes) else log
 name='control-'+(cpu or 'default');(out/(name+'.log')).write_text(log);rows.append(dict(case=name,returncode=code,entry_marker='WINDOWS_CONTROL' in log,status='PASS' if code==0 and 'WINDOWS_CONTROL' in log else 'AUTOMATION_UNAVAILABLE'))
 subprocess.run([str(runtime/'bin/wineserver'),'-k'],env=e,stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL,timeout=10)
(out/'RESULTADOS.json').write_text(json.dumps(dict(cases=rows,qemu_sha256=hashlib.sha256(a.qemu.read_bytes()).hexdigest(),control_sha256=hashlib.sha256(a.control.read_bytes()).hexdigest(),scope='Backend control only; full Windows gameplay remains NOT_REACHED if control cannot run.'),indent=2)+'\n')
