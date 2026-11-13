import json
from pathlib import Path
root=Path(__file__).resolve().parent.parent
src=(root/"mod.c").read_text()
start=src.index("static void aux_viewer_draw(")
end=src.index("static int aux_authorized_fusion",start)
viewer=src[start:end]
assert "fmr_title_tint_first_line" not in viewer
assert "fmr_title_tint_effect" in viewer
m=json.loads((root/"mod.json").read_text())
desc=next(x["description"] for x in m["cards"] if x.get("replace")==370)
assert "its DEF >= this card's ATK" in desc
assert "lose up to\n{f8 0A 06}500{f8 0A 00} LP (0 = defeat)" in desc
assert len(desc.splitlines())==5
print("PASS: titles only; Labyrinth Tank description is five lines")

import re
assert all(len(re.sub(r"\{f8 [^}]+\}","",line))<29 for line in desc.splitlines()[1:])
print("PASS: Labyrinth Tank fits four short body lines")
