"""Isolated API24 regression suite. No disc image, Windows or integrated MODs.

Needs GCC (including i386 support), Python and Pillow. MEMORIES_I386_CC may
select a sysroot wrapper. MEMORIES_I386_RUNNER may prefix QEMU and its sysroot.
MEMORIES_TEST_RESULTS optionally receives JSON with exact commands/results.
Historical differential benchmarks need MEMORIES_RITUAL_BASELINE; otherwise
they are explicitly skipped, never compared against the same source as itself.
"""
from pathlib import Path
import json, os, shlex, subprocess, sys, tempfile

mod = Path(__file__).resolve().parents[1]
repo = mod.parents[1]
assert (repo / 'src/pc/mods/object_loader.c').is_file(), 'Run inside the API24 source checkout'
env = {**os.environ, 'ASAN_OPTIONS': 'detect_leaks=0'}
results = []

def run(name, args):
    args = list(map(str, args))
    p = subprocess.run(args, cwd=repo, env=env, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    results.append({'name': name, 'status': 'PASS' if not p.returncode else 'FAIL',
                    'command': args, 'returncode': p.returncode, 'output': p.stdout})
    print(name, results[-1]['status'], flush=True)
    print(p.stdout, end='', flush=True)
    return not p.returncode

baseline = os.environ.get('MEMORIES_RITUAL_BASELINE')
for path in sorted((mod / 'tests').glob('test_*.py')):
    args = [sys.executable, path]
    if path.stem in ['test_logic_performance', 'test_recipe_performance', 'test_render_performance']:
        if not baseline:
            results.append({'name': path.stem, 'status': 'SKIP',
                            'reason': 'Historical v0.78.39 baseline absent from uploaded package; no performance claim'})
            print(path.stem, 'SKIP historical baseline absent', flush=True)
            continue
        assert Path(baseline).resolve() != mod.resolve(), 'A self-comparison is not a historical regression'
        args += ['--baseline', baseline, '--sanitize']
        if path.stem != 'test_render_performance':
            args += ['--sdk', repo]
    run(path.stem, args)
with tempfile.TemporaryDirectory() as temp:
    temp = Path(temp)
    for name in ['test_provider_generated', 'test_effect_intelligence', 'test_effect_title_instance',
                 'test_effect_title_visual', 'test_yamadron_ai_policy', 'mask_equip_staging_test']:
        exe = temp / name
        if run(name + '-build', ['gcc', '-std=gnu11', '-O1', '-g', '-fsanitize=address,undefined',
              '-fno-omit-frame-pointer', '-DMEMORIES_PC', '-I' + str(repo/'src'), '-I' + str(mod),
              mod / 'tests' / (name + '.c'), '-o', exe]):
            run(name, [exe])
    exe = temp / 'loader'
    if run('preview2_loader-build', [*shlex.split(os.environ.get('MEMORIES_I386_CC', 'gcc')),
           '-m32', '-std=gnu11', '-O1', '-DMEMORIES_PC', '-I' + str(repo/'src'),
           mod/'tests/preview2_loader.c', repo/'src/pc/mods/object_loader.c',
           repo/'src/pc/mods/mod_libc.c', repo/'src/pc/guest/branch_thunks.c',
           repo/'src/pc/compat/fs.c', '-lm', '-o', exe]):
        run('preview2_loader', [*shlex.split(os.environ.get('MEMORIES_I386_RUNNER', '')),
                               exe, mod/'ritual-card-effects.o', mod])
target = os.environ.get('MEMORIES_TEST_RESULTS')
if target:
    Path(target).write_text(json.dumps(results, ensure_ascii=False, indent=2) + '\n')
summary = {s: sum(r['status'] == s and not r['name'].endswith('-build') for r in results)
           for s in ['PASS', 'FAIL', 'SKIP']}
print('SUMMARY (test groups; build steps listed separately):', summary)
sys.exit(any(r['status'] == 'FAIL' for r in results))
