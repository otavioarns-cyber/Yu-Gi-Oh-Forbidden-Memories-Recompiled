"""Run isolated regression fixtures; needs GCC and an API-24 source checkout.

MEMORIES_I386_CC selects a multilib/sysroot compiler (default gcc).
MEMORIES_I386_RUNNER optionally prefixes ELF32 execution, e.g. QEMU -L sysroot.
This does not run the game or the Dual Play MOD.
"""
import os
from pathlib import Path
import shlex
import subprocess
import sys
import tempfile

mod = Path(__file__).resolve().parents[1]
root = mod.parents[1]
assert (root / 'src/pc/mods/mod_types.h').is_file(), 'Run from a source checkout'
os.environ.setdefault('MEMORIES_SDK', str(root / 'tmp/pc/win32/sdk'))
def run(args):
    subprocess.run(list(map(str,args)),cwd=root,check=True)
for name in ['test_rules_provider.py','test_surrender_logic.py','test_deck_visual_origins.py']:
    run([sys.executable, mod/'tests'/name])
with tempfile.TemporaryDirectory() as temp:
    temp=Path(temp)
    run(['gcc','-std=c11','-O1','-fsanitize=undefined','-I'+str(mod),mod/'tests/adapter_test.c','-o',temp/'adapter'])
    run([temp/'adapter'])
    run([*shlex.split(os.environ.get('MEMORIES_I386_CC','gcc')),'-m32','-w','-std=gnu11','-O1',
         '-ffunction-sections','-fdata-sections','-DMEMORIES_PC','-I'+str(root/'src'),
         mod/'tests/preview2_instances.c','-Wl,--gc-sections','-o',temp/'instances'])
    run([*shlex.split(os.environ.get('MEMORIES_I386_RUNNER','')),temp/'instances'])
    run([*shlex.split(os.environ.get('MEMORIES_I386_CC','gcc')),'-m32','-std=gnu11','-O1','-I'+str(root/'src'),
         mod/'tests/preview2_loader.c',root/'src/pc/mods/object_loader.c',root/'src/pc/mods/mod_libc.c',
         root/'src/pc/guest/branch_thunks.c',root/'src/pc/compat/fs.c','-lm','-o',temp/'loader'])
    run([*shlex.split(os.environ.get('MEMORIES_I386_RUNNER','')),temp/'loader',mod/'rituals.o'])
print('PASS isolated preview2 suite; gameplay and actual Dual Play integration remain untested')
