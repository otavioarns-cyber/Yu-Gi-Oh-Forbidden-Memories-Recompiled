from pathlib import Path
import subprocess,tempfile,os,shlex,json
root=Path(__file__).resolve().parents[2];out=Path(os.environ['HAL20_OUTPUT']);out.mkdir(parents=True,exist_ok=True);cc=shlex.split(os.environ['MEMORIES_I386_CC']);runner=shlex.split(os.environ.get('MEMORIES_I386_RUNNER',''))
with tempfile.TemporaryDirectory() as tmp:
 t=Path(tmp)
 for name in ('planner.c','planner.h'):(t/name).write_bytes(subprocess.check_output(['git','show','d649647c159c91d615c07d6960928a37e270b461:mods/ai-hard-mode/'+name],cwd=root))
 flags=['-m32','-std=gnu11','-O1','-DMEMORIES_PC','-Isrc','-ffunction-sections','-fdata-sections'];renames=['Hm_PlanHand','Hm_PlanField','Hm_KeepValue','Hm_ChooseStar','Hm_StarBonus','Hm_StarMatchup'];commands=[cc+flags+['-D'+n+'=Old_'+n for n in renames]+['-c',str(t/'planner.c'),'-o',str(t/'old.o')],cc+flags+['tests/hal20-field-state/ai_field_parity.c','mods/ai-hard-mode/planner.c',str(t/'old.o'),'-Wl,--gc-sections','-o',str(t/'parity')],runner+[str(t/'parity')]]
 for i,cmd in enumerate(commands):
  q=subprocess.run(cmd,cwd=root,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=90);(out/('step'+str(i)+'.log')).write_text(q.stdout);assert not q.returncode,q.stdout;print('PASS',i,q.stdout[-500:])
