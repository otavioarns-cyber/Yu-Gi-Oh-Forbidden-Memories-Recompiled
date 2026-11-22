import json
from pathlib import Path
p=Path(__file__).resolve().parent
base=json.loads((p/'base31_manifest.json').read_text())
new=json.loads((p.parent/'mod.json').read_text())
ids={613,707,370,69,385,427,467,442,509,471,564,223,465,531,572,565}
oldcards={c['replace']:c for c in base['cards']};cards={c['replace']:c for c in new['cards']}
round3={521,618,437,473,482,389,639,15,545,617,230,86,294,133,215,594,533,43,272,30,487,458}
assert set(cards)-set(oldcards)==ids|round3
assert all(cards[x]['description'].startswith('{f8 0A 07}<Fusion Effect>') for x in round3)
for id,c in oldcards.items():
 if id==358:
  assert {k:v for k,v in cards[id].items() if k!='description'}=={k:v for k,v in c.items() if k!='description'}
  assert '{f8 0B 00}' in cards[id]['description']
 else:assert cards[id]==c,id
assert 594 in cards and 487 in cards and 458 in cards
assert new['settings'][:len(base['settings'])]==base['settings']
assert len(new['settings'])-len(base['settings'])==40
assert [x['key'] for x in new['settings'][-2:]]==['effect_ai','effect_ai_trace']
assert new['equips']==base['equips']
assert new['fusions'][:len(base['fusions'])]==base['fusions']
assert not any('remove' in r for r in new['fusions'][len(base['fusions']):])
pairs={tuple(sorted(r['with'])):r['result'] for r in new['fusions'] if 'with' in r}
assert len(pairs)==sum('with' in r for r in new['fusions'])
for row in json.loads((p/'recipe_audit.json').read_text()):
 pair=tuple(sorted(row['materials']))
 assert pairs.get(pair,row['beta31'])==row['result'],row
 if row['beta31'] and row['beta31']!=row['result']:assert pair==(36,97)
assert (74,433) not in pairs
for id in ids:
 c=cards[id]
 assert c['description'].startswith('{f8 0A 07}<Fusion Effect>{f8 0A 00}\n')
 assert '<Effect>' not in c['description']
assert '1000' in cards[565]['description'] and '500' not in cards[565]['description']
print('PASS: 16 Round02 effects + 22 Round03 effects, older cards/settings/recipes intact')
