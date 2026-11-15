"""Inspect/relink only in a private copy; never publish or claim full source rebuild."""
from pathlib import Path
import argparse,hashlib,json,subprocess,os,shlex,shutil,sys,ast,re
p=argparse.ArgumentParser();p.add_argument('--package',type=Path,required=True);p.add_argument('--sdk',type=Path,required=True);p.add_argument('--work',type=Path,required=True);a=p.parse_args();a.work.mkdir(parents=True,exist_ok=False);root=Path(__file__).resolve().parents[2];sys.path.insert(0,str(root/'tests/hal20-field-state'));from elf_inventory import elf_info
sha=lambda f:hashlib.sha256(f.read_bytes()).hexdigest();inventory=[dict(path=str(f.relative_to(a.package)),bytes=f.stat().st_size,sha256=sha(f)) for f in sorted(a.package.rglob('*')) if f.is_file() and '__pycache__' not in f.parts];private=a.work/'package';shutil.copytree(a.package,private);rows=[]
def execute(name,cmd):
 q=subprocess.run(list(map(str,cmd)),cwd=private,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=60);(a.work/(name+'.log')).write_text(q.stdout);rows.append(dict(test=name,command=list(map(str,cmd)),returncode=q.returncode,status='PASS' if q.returncode==0 else 'FAIL',log=name+'.log'));return q
received=elf_info(private/'time-wizard.o');core=elf_info(private/'source/time-wizard-base-v0.23.1.o');assert sha(private/'source/time-wizard-base-v0.23.1.o')=='5b4252dafbc07532f859cda92e49e75d15e531a880f90cc7924371b7e37f9b4a'
# Package's actual tests, run as-is, with host execution routed to QEMU only.
wrapper="import sys,subprocess,runpy,pathlib,shlex,os; old=subprocess.run; runner=shlex.split(os.environ.get('MEMORIES_I386_RUNNER','')); subprocess.run=lambda args,*a,**kw:old((runner+list(map(str,args))) if pathlib.Path(str(args[0])).name=='test' and pathlib.Path(str(args[0])).is_absolute() else args,*a,**kw); sys.argv=['source/tests/run_native_tests.py',sys.argv[1]]; runpy.run_path('source/tests/run_native_tests.py',run_name='__main__')"
execute('received-native-controller',[sys.executable,'-c',wrapper,a.sdk.resolve()])
execute('partial-relink-api24',[sys.executable,'source/build_v024.py',a.sdk.resolve()])
if rows[-1]['returncode']==0:execute('relinked-native-controller',[sys.executable,'-c',wrapper,a.sdk.resolve()])
cc=shlex.split(os.environ.get('MEMORIES_I386_CC','gcc'));runner=shlex.split(os.environ.get('MEMORIES_I386_RUNNER',''))
for name in ('test_dual_play_decisions','test_dual_play_hand_selection','test_evolutions'):
 flags=['-m32','-std=gnu11','-O1','-DMEMORIES_PC','-I'+str(a.sdk.resolve()/'include')]
 q=execute(name+'-build',cc+flags+[private/'source/tests'/(name+'.c'),'-o',a.work/name])
 if q.returncode==0:execute(name,runner+[a.work/name])
code="import sys,importlib.util,pathlib,subprocess; sdk=pathlib.Path(sys.argv[1]);sys.path.insert(0,str(sdk/'tools'));spec=importlib.util.spec_from_file_location('b',sdk/'tools/build_mod.py');b=importlib.util.module_from_spec(spec);spec.loader.exec_module(b);cc,flags,ld=b.compiler('i386');flags=b.target_flags('i386',flags);subprocess.run(cc+flags+['-c',sys.argv[2],'-o',sys.argv[3]],check=True)"
execute('available-AI-source-build-NOT-linked',[sys.executable,'-c',code,a.sdk.resolve(),private/'source/time_wizard_ai.c',a.work/'time-wizard-ai-only.o'])
relinked=elf_info(private/'time-wizard.o');exports=set((a.sdk/'exports.i386.txt').read_text().split());undef=[x.split()[-1] for x in subprocess.check_output(['nm','-u',str(private/'time-wizard.o')],text=True).splitlines()]
# Add-symbols are bounds-checked against the pinned core; bounds do not certify semantic ABI.
from elftools.elf.elffile import ELFFile
with (private/'source/time-wizard-base-v0.23.1.o').open('rb') as f:
 e=ELFFile(f);bss=e.get_section_by_name('.bss')['sh_size'];text=e.get_section_by_name('.text')['sh_size']
values={}
for node in ast.walk(ast.parse((private/'source/build_v024.py').read_text())):
 if isinstance(node,ast.Assign) and any(isinstance(t,ast.Name) and t.id=='aliases' for t in node.targets):values=ast.literal_eval(node.value)
assert all(v+4<=bss for v in values.values());assert values['tw_ext_plan']+200<=bss
report=dict(scope='Partial adapters + fixed binary core only; tests model wheel/engine boundaries. Not full rebuild, no Windows gameplay, no collection admission.',inventory=inventory,received=received,core=core,relinked=relinked,imports_missing_API24=sorted(set(undef)-exports),core_aliases=values,core_bss_bytes=bss,core_text_bytes=text,test_entry_offsets=dict(native_open=0x9210,native_card_use=0x7330),steps=rows)
(a.work/'RESULTADOS.json').write_text(json.dumps(report,indent=2)+'\n');assert not report['imports_missing_API24'];print([(x['test'],x['status']) for x in rows]);assert all(x['returncode']==0 for x in rows)
# Input immutability is mandatory even when tests or the relinker fail.
assert inventory==[dict(path=str(f.relative_to(a.package)),bytes=f.stat().st_size,sha256=sha(f)) for f in sorted(a.package.rglob('*')) if f.is_file() and '__pycache__' not in f.parts]
