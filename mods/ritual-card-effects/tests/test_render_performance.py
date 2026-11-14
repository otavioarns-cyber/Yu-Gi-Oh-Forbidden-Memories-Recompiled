#!/usr/bin/env python3
"""Compare actual render callbacks against a preserved mod source directory.

No SDK, game image, external packages or window system is needed. The C fixture
captures native sprite arguments and host overlay commands; it does not emulate
the game's GPU or measure Windows frame rate. Optional CPU timings use mocked
render submission and the local C library, without call-count instrumentation.
"""
import argparse
import os
from pathlib import Path
import re
import shlex
import subprocess
import tempfile


def function(source: str, name: str) -> str:
    pattern = rf"^static[^\n]*\b{re.escape(name)}\([^;]*?\)\s*\{{"
    match = re.search(pattern, source, re.MULTILINE)
    if not match:
        raise ValueError(f"Function definition not found: {name}")
    depth = 0
    tokens = re.compile(r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|/\*.*?\*/|//[^\n]*|[{}]', re.DOTALL)
    for token in tokens.finditer(source, match.end() - 1):
        if token.group() == "{":
            depth += 1
        elif token.group() == "}":
            depth -= 1
            if depth == 0:
                return source[match.start():token.end()]
    raise ValueError(f"Unclosed function: {name}")


def rename(source: str, names: dict[str, str]) -> str:
    return re.sub(r"\b(?:" + "|".join(map(re.escape, names)) + r")\b",
                  lambda match: names[match.group()], source)


def generated_sources(baseline: Path, current: Path, destination: Path) -> None:
    old = (baseline / "mod.c").read_text()
    new = (current / "mod.c").read_text()
    callbacks = ("chakra_native_popup", "visual_card_frame_draw", "overlay", "overlay_signature")
    variants = []
    for prefix, source in (("baseline", old), ("optimized", new)):
        names = {name: f"{prefix}_{name}" for name in callbacks}
        variants.extend(rename(function(source, name), names) for name in callbacks)
    (destination / "render_callbacks.inc").write_text("\n\n".join(variants))
    helpers = []
    for name in ("monster_slot", "ritual_counter_selected_record", "overlay_hash", "javelin_live", "hungry_live"):
        # Shared helpers must not silently drift away from the preserved source.
        if function(old, name) != function(new, name):
            raise ValueError(f"Shared test helper changed; update fixture deliberately: {name}")
        target = {"ritual_counter_selected_record": "actual_selected_record",
                  "overlay_hash": "actual_overlay_hash"}.get(name, name)
        helpers.append(rename(function(old, name), {name: target}))
    for filename, name in (("garma_visual.h", "garma_live"), ("tri_horned.h", "tri_identity"), ("tri_horned.h", "tri_live")):
        old_helper = (baseline / filename).read_text()
        new_helper = (current / filename).read_text()
        if function(old_helper, name) != function(new_helper, name):
            raise ValueError(f"Shared test helper changed: {name}")
        helpers.append(function(old_helper, name))
    (destination / "render_helpers.inc").write_text("\n\n".join(helpers))
    # Copy actual constants, never duplicate approved texture positions/art sizes.
    constants = []
    for filename in ("mod.c", "garma_visual.h", "native_vfx_textures.h", "chakra_green_atlas.h"):
        content = (current / filename).read_text()
        constants.extend(line for line in content.splitlines()
                         if re.match(r"#define (?:STAT_VISUAL_\w+|STAT_POPUP_FRAMES|HUNGRY_POPUP_FRAMES|CHAKRA_POPUP_FRAMES|GARMA_SWORD_ANIMATION_FRAMES|GARMA_SWORD_ID|TRI_HORNED_ID|JAVELIN_BEETLE_ID|HUNGRY_BURGER_ID|CHAKRA_ID|RITUAL_(?:HORN|JAVELIN|HUNGRY|GARMA)_\w+|CHAKRA_(?:PLUS|ATK|ATLAS|GLYPH)_\w+) ", line))
    (destination / "render_constants.inc").write_text("\n".join(constants))


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    current = Path(__file__).resolve().parent.parent
    parser.add_argument("--baseline", type=Path,
                        default=current.parent.parent / "baseline" / current.name,
                        help="Preserved original mod directory containing mod.c")
    parser.add_argument("--compiler", default=os.environ.get("CC", "gcc"))
    parser.add_argument("--benchmark", action="store_true")
    parser.add_argument("--sanitize", action="store_true", help="Also run the differential fixture under ASan/UBSan")
    args = parser.parse_args()
    with tempfile.TemporaryDirectory(prefix="ritual-render-tests-") as directory:
        temporary = Path(directory)
        generated_sources(args.baseline.resolve(), current, temporary)
        fixture = current / "tests" / "test_render_performance.c"
        common = shlex.split(args.compiler) + ["-std=c99", "-O2", "-Wall", "-Wextra",
                  "-Wno-unused-but-set-variable", "-Wno-unused-function", "-Wno-misleading-indentation",
                  "-I", str(temporary), str(fixture)]
        executable = temporary / "render-tests"
        subprocess.run(common + ["-o", str(executable)], check=True)
        subprocess.run([str(executable)], check=True)
        if args.sanitize:
            executable = temporary / "render-sanitized"
            subprocess.run(common + ["-fsanitize=address,undefined", "-fno-omit-frame-pointer",
                                     "-o", str(executable)], check=True)
            subprocess.run([str(executable)], check=True)
        if args.benchmark:
            executable = temporary / "render-benchmark"
            subprocess.run(common + ["-DRENDER_BENCHMARK", "-o", str(executable)], check=True)
            subprocess.run([str(executable)], check=True)


if __name__ == "__main__":
    main()
