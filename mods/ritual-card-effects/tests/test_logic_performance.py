#!/usr/bin/env python3
"""Differential tests/CPU microbenchmarks of the actual changed C functions.

Engine callbacks are deterministic doubles; this is not an FPS benchmark or
an in-game Windows compatibility test. Baseline input must remain untouched.
"""
import argparse
import os
from pathlib import Path
import re
import subprocess
import tempfile


def balanced_block(text, opening):
    depth, state, i = 0, "code", opening
    while i < len(text):
        c, following = text[i], text[i:i + 2]
        if state == "line":
            if c == "\n":
                state = "code"
        elif state == "comment":
            if following == "*/":
                state, i = "code", i + 1
        elif state in ('"', "'"):
            if c == "\\":
                i += 1
            elif c == state:
                state = "code"
        elif following == "//":
            state, i = "line", i + 1
        elif following == "/*":
            state, i = "comment", i + 1
        elif c in ('"', "'"):
            state = c
        elif c == "{":
            depth += 1
        elif c == "}":
            depth -= 1
            if depth == 0:
                return text[opening:i + 1]
        i += 1
    raise ValueError("Unterminated C block")


def function(text, name):
    match = re.search(r"^static[^;\n]*\b" + re.escape(name) +
                      r"\([^;]*?\)\s*\{", text, re.M)
    if not match:
        raise ValueError(f"Function {name} not found")
    opening = text.index("{", match.start())
    return text[match.start():opening] + balanced_block(text, opening)


def declaration(text, name):
    for match in re.finditer(r"typedef\s+struct\s*\{", text):
        opening = text.index("{", match.start())
        block = balanced_block(text, opening)
        tail = text[opening + len(block):]
        suffix = re.match(r"\s*" + re.escape(name) + r"\s*;", tail)
        if suffix:
            return text[match.start():opening] + block + suffix.group()
    raise ValueError(f"Type {name} not found")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    here = Path(__file__).resolve().parent.parent
    parser.add_argument("--baseline", type=Path,
                        default=here.parents[1] / "baseline" / here.name)
    parser.add_argument("--optimized", type=Path, default=here)
    parser.add_argument("--sdk", type=Path,
                        default=Path("/workspace/Yu-Gi-Oh-Forbidden-Memories-Recompiled"))
    parser.add_argument("--cc", default=os.environ.get("CC", "gcc"))
    parser.add_argument("--sanitize", action="store_true")
    args = parser.parse_args()
    old = (args.baseline / "mod.c").read_text()
    new = (args.optimized / "mod.c").read_text()
    card_header = (args.sdk / "src/game/duel_card.h").read_text()
    types = ['#include "types.h"', '#include "game/card_constants.h"',
             '#include "game/duel_card_layout.h"',
             declaration(card_header, "DuelCardRecord")]
    for name in ("SengenjinMark", "SengenjinNegativeMark", "BlsMark",
                 "DokuroriderMark", "UltimateMark", "SeaRitualMark",
                 "RitualActivationPos", "RitualStatVisual"):
        types.append(declaration(old, name))
    types.append(declaration(new, "SengenjinStatSnapshot"))
    constants = set(("DUEL_SIDE_COUNT", "DUEL_FIELD_ROW_SIZE",
                     "DUEL_FIELD_SIDE_GRID_SLOT_COUNT", "DUEL_SCENE_PHASE_MASK"))
    for name in constants:
        found = None
        for header in ("game/duel_grid.h", "game/duel_scene_state.h"):
            source = (args.sdk / "src" / header).read_text()
            match = re.search(r"^#define\s+" + name + r"\b[^\n]*", source, re.M)
            if match:
                found = match.group()
                break
        if not found:
            raise ValueError(f"SDK constant {name} not found")
        types.append(found)
    # Preserve the uploaded mod's actual IDs, masks, queue limits and values.
    types += re.findall(r"^#define\s+[^\n\\]+$", old, re.M)
    types.append("enum { BEACON_JAVELIN, BEACON_BLS, BEACON_DOKU, BEACON_CRAB };")
    shared = ("monster_slot", "record_index", "side_for_record", "side_monster_mask",
              "take_mask_bit", "beacon_is_other_record", "card_type_id", "bls_identity",
              "bls_live", "dokurorider_identity", "dokurorider_live",
              "sengenjin_identity", "ultimate_identity", "ritual_effectively_present",
              "ultimate_effect_active", "beacon_source_count", "sengenjin_native_negative",
              "visual_queue_activation", "visual_queue_stat_kind", "visual_queue_stat")
    source_functions = [function(old, name) for name in shared]
    modified = ("sengenjin_filtered_battle_stat", "sengenjin_filtered_battle_stat_with_full",
                "bls_reconcile_aura_visuals", "dokurorider_reconcile_aura_visuals")
    for prefix, source in (("old", old), ("new", new)):
        for name in modified:
            if name.endswith("_with_full") and prefix == "old":
                continue
            extracted = function(source, name)
            for renamed in modified:
                extracted = re.sub(r"\b" + renamed + r"\b", prefix + "_" + renamed, extracted)
            source_functions.append(extracted)
        condition = source.index("    if(!sengenjin_preview_filter_guard && sengenjin_live_hint &&")
        opening = source.index("{", condition)
        preview = source[condition:opening] + balanced_block(source, opening)
        for renamed in modified:
            preview = re.sub(r"\b" + renamed + r"\b", prefix + "_" + renamed, preview)
        source_functions.append(f"static s32 {prefix}_preview(DuelCardRecord *card){{\n"
                                "    uint32_t packed=model_full_stats(card);\n"
                                "    int i=record_index(card);\n" + preview +
                                "\n    return (s32)packed;\n}")
    with tempfile.TemporaryDirectory(prefix="ritual-logic-") as temporary:
        root = Path(temporary)
        (root / "generated_logic_types.h").write_text("\n".join(types) + "\n")
        (root / "generated_logic_functions.h").write_text("\n\n".join(source_functions) + "\n")
        command = [args.cc, "-std=c11", "-O2", "-Wall", "-Wextra", "-Werror",
                   "-Wno-unused-function", "-DMEMORIES_PC", "-I", str(root),
                   "-I", str(args.sdk / "src"),
                   str(Path(__file__).with_suffix(".c")), "-o", str(root / "logic_test")]
        if args.sanitize:
            command[2:3] = ["-O1", "-g", "-fsanitize=address,undefined", "-fno-omit-frame-pointer"]
        subprocess.run(command, check=True)
        subprocess.run([str(root / "logic_test")], check=True)


if __name__ == "__main__":
    main()
