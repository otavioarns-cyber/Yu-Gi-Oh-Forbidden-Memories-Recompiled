"""Regression for six user-custom HD attribute icons inside Fusion Effect descriptions."""
from pathlib import Path
import json, re
from PIL import Image
root=Path(__file__).resolve().parents[1]
m=json.loads((root/'mod.json').read_text(encoding='utf-8'))
assert m['min_api']>=15
names=('Light','Dark','Earth','Water','Fire','Wind')
entries=m['card_types']['types']
assert len(entries)==6
bytype={e['id']:e for e in entries}
assert set(bytype)==set(range(249,255))
for i,name in enumerate(names):
    e=bytype[249+i]
    assert e['name']=='FXAttribute'+name
    path=root/e['icon']
    assert path.is_file() and path.suffix=='.png'
    with Image.open(path) as im:
        assert im.size==(256,256) and im.mode=='RGBA'
        a=im.getchannel('A')
        assert all(im.getpixel(p)[3]==0 for p in ((0,0),(0,255),(255,0),(255,255)))
        assert a.getbbox() is not None
byid={c.get('replace'):c for c in m['cards']}
for cid,attribute in ((531,'Earth'),(473,'Fire'),(389,'Dark')):
    text=byid[cid]['description']
    assert '{f8 0B %02X}'%(0xF0+names.index(attribute)) in text
    assert attribute.upper() not in text
    for line in text.splitlines()[1:]:
        visible=re.sub(r'\{f8 0A [0-9A-Fa-f]{2}\}', '', line)
        visible=re.sub(r'\{f8 0B [0-9A-Fa-f]{2}\}', '@@', visible)
        assert len(visible)<=21,(cid,visible,len(visible))
src=(root/'mod.c').read_text()
assert 'AUX_HOOK(DuelEffect_AppendEntry,aux_attribute_icon_text_entry,original_append_text_entry)' in src
assert 'TypeIcons_MarkIcon(249 + (int)(value-0xF0))' in src
assert 'if(id==272)r3_queue_rebirth_fire(i,id);' in (root/'fusion_round3_runtime.h').read_text()
assert 'DuelEffect_AllocateRequest(0xB)' in src
print('PASS: six HD custom attribute assets, F8 logical 16x16 layout, description width, renderer hook, Mavelus fire')
