"""Direct PE32/PE64 startup controls with writable cache, isolated prefix/display."""
from pathlib import Path
import argparse,subprocess,os,json
p=argparse.ArgumentParser();p.add_argument('--work',type=Path,required=True);p.add_argument('--wine',type=Path,required=True);p.add_argument('--wineserver',type=Path);p.add_argument('--xvfb',type=Path,required=True);a=p.parse_args();out=a.work.resolve();cache=out/'cache';cache.mkdir(exist_ok=True);env=dict(os.environ,DISPLAY=':93',WINEPREFIX=str(out/'prefix'),WINEDEBUG='-all',WINEDLLOVERRIDES='mscoree,mshtml=',XDG_CACHE_HOME=str(cache),MEMORIES_HEADLESS='1');rows=[]
with (out/'direct-xvfb.log').open('w') as f:
 x=subprocess.Popen([str(a.xvfb),':93','-screen','0','1280x800x24','-nolisten','tcp','-ac'],stdout=f,stderr=subprocess.STDOUT)
 try:
  for name in ('control64.exe','control32.exe','memories-pc.exe'):
   try:q=subprocess.run([str(a.wine),name],cwd=out,env=env,text=True,errors='backslashreplace',stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=25);code=q.returncode;log=q.stdout
   except subprocess.TimeoutExpired as e:code=124;log=e.stdout or '';log=log.decode(errors='backslashreplace') if isinstance(log,bytes) else log
   (out/(name+'-direct.log')).write_text(log);rows.append(dict(binary=name,returncode=code,status='PASS' if name=='control64.exe' and code==0 else 'AUTOMATION_UNAVAILABLE' if code else 'NOT_REACHED'));print(rows[-1],flush=True)
 finally:
  subprocess.run([str(a.wineserver or a.wine.parent/'wineserver'),'-k'],env=env,timeout=15,stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL);x.terminate();x.wait(timeout=5)
(out/'DIRECT.json').write_text(json.dumps(rows,indent=2)+'\n')
