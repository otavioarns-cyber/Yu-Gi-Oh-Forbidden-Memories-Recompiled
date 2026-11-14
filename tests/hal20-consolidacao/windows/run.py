"""Isolated WinAPI control + original Windows PE32 startup probes under Wine.
Wine is a compatibility layer, not a native Windows OS. No game data synthesized.
"""
from pathlib import Path
import argparse,hashlib,json,os,subprocess,time,shutil
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--wine',type=Path,required=True);p.add_argument('--xvfb',type=Path,required=True);p.add_argument('--cc-root',type=Path,required=True);p.add_argument('--game-dir',type=Path,required=True);p.add_argument('--work',type=Path,required=True);a=p.parse_args();a.work.mkdir(parents=True,exist_ok=False);out=a.work.resolve();src=Path(__file__).resolve().parent;sha=lambda f:hashlib.sha256(f.read_bytes()).hexdigest();build=[]
for tag,binary,cfile in [('i686','control32.exe','hello.c'),('x86_64','control64.exe','hello.c'),('x86_64','driver64.exe','driver.c')]:
 cmd=[a.cc_root/(tag+'-w64-mingw32-clang'),src/cfile,'-O1','-o',out/binary];q=subprocess.run(list(map(str,cmd)),text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT);(out/(binary+'-build.log')).write_text(q.stdout);assert not q.returncode;build.append(dict(binary=binary,source_sha256=sha(src/cfile),artifact_sha256=sha(out/binary),command=list(map(str,cmd))))
for name in ('memories-pc.exe','SDL3.dll'):shutil.copy2(a.game_dir/name,out/name)
env=dict(os.environ,DISPLAY=':92',WINEPREFIX=str(out/'prefix'),WINEARCH='win64',WINEDEBUG='-all',WINEDLLOVERRIDES='mscoree,mshtml=',MEMORIES_USER_DIR='Z:'+str(out/'game-user').replace('/','\\'))
xf=(out/'xvfb.log').open('w');xp=subprocess.Popen([str(a.xvfb),':92','-screen','0','1280x800x24','-nolisten','tcp','-ac'],stdout=xf,stderr=subprocess.STDOUT);rows=[]
try:
 time.sleep(1)
 for name,args,extra in [('prefix-init',['wineboot','--init'],{}),('control64',['driver64.exe','control64.exe'],{}),('control32',['driver64.exe','control32.exe'],{}),('game-headless',['driver64.exe','memories-pc.exe'],{'MEMORIES_HEADLESS':'1'}),('game-visible',['driver64.exe','memories-pc.exe'],{})]:
  start=time.monotonic()
  try:
   q=subprocess.run([str(a.wine),*args],cwd=out,env=dict(env,**extra),text=True,errors='backslashreplace',stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=40);rc=q.returncode;log=q.stdout
  except subprocess.TimeoutExpired as e:rc=124;log=(e.stdout or b'').decode(errors='backslashreplace') if isinstance(e.stdout,bytes) else e.stdout or ''
  (out/(name+'.log')).write_text(log);rows.append(dict(case=name,returncode=rc,seconds=round(time.monotonic()-start,3),status='PASS' if name in ('prefix-init','control64') and rc==0 else 'AUTOMATION_UNAVAILABLE' if rc else 'NOT_REACHED',log=name+'.log'));print(rows[-1],flush=True)
finally:
 subprocess.run([str(a.wine.parent/'wineserver'),'-k'],env=env,timeout=15,stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL);xp.terminate();xp.wait(timeout=5)
report=dict(scope=__doc__,wine_version=subprocess.check_output([str(a.wine),'--version'],text=True).strip(),game_sha256=sha(out/'memories-pc.exe'),SDL3_sha256=sha(out/'SDL3.dll'),builds=build,cases=rows,gameplay='NOT_REACHED: PE32 backend must pass the control before game checks. No legal game disc found in supplied workspace; no image downloaded or invented.')
(out/'RESULTADOS.json').write_text(json.dumps(report,indent=2)+'\n')
