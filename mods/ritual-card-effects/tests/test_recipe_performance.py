#!/usr/bin/env python3
"""Compare actual recipe headers with the unchanged v0.78.39 baseline.

Local CPU benchmark, not Windows gameplay or FPS measurement. Requires a
native C compiler and the target game's source headers. No game image needed.
"""
import argparse
import os
from pathlib import Path
import re
import statistics
import subprocess
import tempfile


def main():
    here = Path(__file__).resolve().parent.parent
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--baseline', type=Path,
                        default=here.parents[1] / 'baseline' / here.name)
    parser.add_argument('--optimized', type=Path, default=here)
    parser.add_argument('--sdk', type=Path,
                        default=Path('/workspace/Yu-Gi-Oh-Forbidden-Memories-Recompiled'))
    parser.add_argument('--cc', default=os.environ.get('CC', 'gcc'))
    parser.add_argument('--sanitize', action='store_true')
    args = parser.parse_args()
    fixture = (args.baseline / 'tests/test_provider_generated.c').read_text()
    fixture = fixture[:fixture.index('static void stats(')]
    body = Path(__file__).with_suffix('.c').read_text()
    results, timings, executables = {}, {}, {}
    with tempfile.TemporaryDirectory(prefix='ritual-recipes-') as directory:
        temporary = Path(directory)
        for label, folder in [('baseline', args.baseline), ('optimized', args.optimized)]:
            # Replace the fixture matcher with the verbatim production function.
            source = (folder / 'ritual_recipe_compat.h').read_text()
            pattern = r'static int compat_requirement_matches\([^)]*\)\s*\{.*?\n\}'
            matcher = re.search(pattern, source, re.S)
            if matcher is None:
                raise ValueError('Production matcher not found')
            production = re.sub(pattern,
                                lambda _: matcher.group(), fixture, count=1, flags=re.S)
            path = temporary / (label + '.c')
            path.write_text(production + '\n' + body)
            executable = temporary / label
            flags = ['-O2']
            if args.sanitize:
                flags = ['-O1', '-g', '-fsanitize=address,undefined', '-fno-omit-frame-pointer']
            subprocess.run([args.cc, '-std=c11', *flags, '-Wall', '-Wextra',
                            '-Werror', '-Wno-unused-function', '-DMEMORIES_PC',
                            '-I', str(folder), '-I', str(args.sdk / 'src'),
                            str(path), '-o', str(executable)], check=True)
            executables[label] = executable
        samples = {label: [] for label in executables}
        for repeat in range(1 if args.sanitize else 7):
            order = list(executables)
            if repeat % 2:
                order.reverse()
            for label in order:
                executable = executables[label]
                command = [str(executable)] + ([] if args.sanitize else ['--bench'])
                output = subprocess.check_output(command, text=True)
                result = output.splitlines()[0]
                if label in results and results[label] != result:
                    raise AssertionError('Non-deterministic result')
                results[label] = result
                if not args.sanitize:
                    samples[label].append(float(re.search(r'ns/query=([\d.]+)', output).group(1)))
        for label, values in samples.items():
            if values:
                timings[label] = statistics.median(values)
        if results['baseline'] != results['optimized']:
            raise AssertionError(f'Recipe behavior differs: {results}')
        print('PASS recipe differential:', results['optimized'])
        if timings:
            print('CPU median ns/query:', timings,
                  'reduction=%.1f%%' % (100 * (1 - timings['optimized']/timings['baseline'])))


if __name__ == '__main__':
    main()
