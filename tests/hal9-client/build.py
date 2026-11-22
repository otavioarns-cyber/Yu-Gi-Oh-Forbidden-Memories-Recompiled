"""Build own cooperative TW client in a private copy of the PR33 adapter; keep core pinned.
No private source/binary is copied into the repository. Not a full core rebuild.
"""
from pathlib import Path
import argparse, hashlib, importlib.util, json, shutil, subprocess
from elftools.elf.elffile import ELFFile

p = argparse.ArgumentParser()
p.add_argument('--reference-build', type=Path, required=True)
p.add_argument('--sdk', type=Path, required=True)
p.add_argument('--output', type=Path, required=True)
a = p.parse_args()
root = Path(__file__).resolve().parents[2]
sha = lambda p: hashlib.sha256(p.read_bytes()).hexdigest()
ref = a.reference_build
assert sha(ref/'package/time-wizard.o') == '46f63050a5761b6f7ca3ec44c7a267ee4a7f79ec1572fb3d41ecd1e6313d9093'
assert '#define MEMORIES_MOD_API 24' in (a.sdk/'include/pc/mods/mod_types.h').read_text().splitlines()
a.output.mkdir(parents=True, exist_ok=False)
dst = a.output/'package'
shutil.copytree(ref/'package', dst)
patch = root/'experiments/hal9-client/adapter.patch'
q = subprocess.run(['git','apply','--check',str(patch)], cwd=dst, capture_output=True, text=True)
assert q.returncode == 0, q.stderr
subprocess.run(['git','apply',str(patch)], cwd=dst, check=True)
for src in (root/'experiments/hal9-client/tw_client.inc',root/'experiments/hal9-handoff/selection_finalization_v1.h',root/'mods/ritual-duelists/mandatory_hand_selection_v1.h'):
    shutil.copy2(src,dst/'source'/src.name)
spec = importlib.util.spec_from_file_location('build_mod', a.sdk/'tools/build_mod.py')
b = importlib.util.module_from_spec(spec)
spec.loader.exec_module(b)
cc, flags, ld = b.compiler('i386')
flags = b.target_flags('i386',flags)+['-ffile-prefix-map='+str(dst.resolve())+'=/hal9/package','-ffile-prefix-map='+str(root.resolve())+'=/hal9/repo','-fdebug-compilation-dir=/hal9/build']
obj = a.output/'time_wizard_dual_play_compat.o'
steps = []
def run(cmd, name):
    q = subprocess.run(list(map(str,cmd)), text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, timeout=60)
    (a.output/(name+'.log')).write_text(q.stdout)
    steps.append(dict(name=name,returncode=q.returncode,command=list(map(str,cmd))))
    assert q.returncode == 0, q.stdout
run(cc+flags+['-DMemoriesModInit=TimeWizardV023MemoriesModInit','-c',dst/'source/time_wizard_dual_play_compat.c','-o',obj], 'adapter')
names = ['renamed-core.o','time_wizard_description_fit.o','time_wizard_evolution.o','time_wizard_encore.o','contracts_entry.o','ai_probe_i386.o']
objects = [Path('/workspace/work/hal20-timewizard-contracts/local/candidate-final')/n for n in names]
run(ld+['-o',a.output/'reference-relinked.o',objects[0],ref/'time_wizard_dual_play_compat.o',*objects[1:]], 'reference-relink')
assert sha(a.output/'reference-relinked.o') == sha(ref/'package/time-wizard.o'), 'Reference link inputs changed'
run(ld+['-o',dst/'time-wizard.o',objects[0],obj,*objects[1:]], 'link')
undefined = lambda p: sorted(x.split()[-1] for x in subprocess.check_output(['nm','-u',str(p)],text=True).splitlines())
old_imports=set(undefined(ref/'package/time-wizard.o'))
new_imports=set(undefined(dst/'time-wizard.o'))
assert not new_imports-old_imports and not old_imports-new_imports
assert 'D_8009B20C' in (a.sdk/'exports.i386.txt').read_text().split()
exports = lambda p: sorted(x.split()[-1] for x in subprocess.check_output(['nm','-g','--defined-only',str(p)],text=True).splitlines())
assert exports(ref/'package/time-wizard.o') == exports(dst/'time-wizard.o')
with (dst/'time-wizard.o').open('rb') as f:
    e = ELFFile(f)
    assert e.elfclass == 32 and e['e_machine'] == 'EM_386'
    # The legacy core contains an earlier same-named static function. The
    # adapter's registered callback is the last link-unit symbol, as traced.
    draw = e.get_section_by_name('.symtab').get_symbol_by_name('compat_draw_phase')[-1]['st_size']
changed = [str(f.relative_to(dst)) for f in dst.rglob('*') if f.is_file() and (not (ref/'package'/f.relative_to(dst)).is_file() or sha(f)!=sha(ref/'package'/f.relative_to(dst)))]
assert sorted(changed)==sorted(['source/time_wizard_dual_play_compat.c','time-wizard.o','source/tw_client.inc','source/selection_finalization_v1.h','source/mandatory_hand_selection_v1.h']),changed
receipt = dict(scope=__doc__,reference_sha256=sha(ref/'package/time-wizard.o'),candidate_sha256=sha(dst/'time-wizard.o'),draw_bytes=draw,patch_sha256=sha(patch),client_source_sha256=sha(root/'experiments/hal9-client/tw_client.inc'),unchanged_link_objects={n:sha(p) for n,p in zip(names,objects)},changed_package_files=changed,added_official_imports=sorted(new_imports-old_imports),all_imports=sorted(new_imports),steps=steps)
(a.output/'BUILD.json').write_text(json.dumps(receipt,indent=2)+'\n')
print(receipt['candidate_sha256'])
