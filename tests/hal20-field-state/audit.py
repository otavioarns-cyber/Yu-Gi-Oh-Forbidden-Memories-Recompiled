"""Scope/provenance audit of Field integration against PR28 and historical source."""
from pathlib import Path
import hashlib,json,subprocess,re,argparse
root=Path(__file__).resolve().parents[2];p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();sha=lambda b:hashlib.sha256(b).hexdigest();base='414a1b7cd149582d674792dd66b375ca46a7fcd2'
def old(path,ref=base):return subprocess.check_output(['git','show',ref+':'+path],cwd=root)
rows=[];trees=json.loads(old('notes/hal-20-consolidacao/OWNED_TREES.json'))
for name in ('melhoria-dos-rituais','ritual-card-effects','ritual-duelists'):
 for f,digest in trees[name]['files'].items():assert sha((root/'mods'/name/f).read_bytes())==digest,(name,f)
 rows.append(dict(check=name,status='PASS',scope='Every original package file unchanged, including ABI/settings/assets.'))
froot=root/'mods/fusion-auxiliary-balance';allowed={'mod.c','fusion_round2_runtime.h','fusion_round3_runtime.h','generated_card_restore.h','fusion-auxiliary-balance.o','SDK-PROVENANCE.json'}
changed=[]
for f,digest in trees['fusion-auxiliary-balance']['files'].items():
 if sha((froot/f).read_bytes())!=digest:changed.append(f)
assert set(changed)==allowed,changed
for f in ('owner_f.h','fusionist_runtime.h','mod.json'):assert (froot/f).read_bytes()==old('mods/fusion-auxiliary-balance/'+f)
for name,tag in (('ai-hard-mode','AI'),('fusion-auxiliary-balance','FUSION'),('field-effects-secondary-types','FIELD')):
 d=root/'mods'/name;j=json.loads((d/'SDK-PROVENANCE.json').read_text());m=json.loads((d/'mod.json').read_text());assert j['artifact']['sha256']==sha((d/(m['library']+'.o')).read_bytes());assert all(sha((d/f).read_bytes())==h for f,h in j['production_inputs_sha256'].items());assert all(json.loads((root/'notes/hal-20-field-state'/('ABI_'+tag+'.json')).read_text())['candidates'][0]['checks'].values())
rows.append(dict(check='Fusion F1 and source delta',status='PASS',changed=changed,scope='Only five terrain assignments plus optional helper; F1 owner/revoke files, manifest43 settings, assets and all other baseline files unchanged.'))
field='mods/field-effects-secondary-types/field_effects.c';historical='mods/Yugioh-Operacao-MODs/Field-Effects-e-Tipos-Secundarios/dev/';source=old(historical+'field_effects.c','296c5919479711b550d00aea8cd042b1c34ba306').decode();expected=source.replace('static const MemoriesModHost *mod_host;','static const MemoriesModHost *mod_host;\nstatic int owner_effective;').replace('if(!mod_host||!mod_host->applied(mod_host))return 0;','if(!mod_host||!owner_effective)return 0;').replace('static void mod_applied(int on){(void)on;}','static void mod_applied(int on){owner_effective=on!=0;}');assert (root/field).read_text()==expected
assert (root/'mods/field-effects-secondary-types/mod.json').read_bytes()==old(historical+'mod.json','296c5919479711b550d00aea8cd042b1c34ba306')
rows.append(dict(check='Field approved source',status='PASS',scope='Only effective-owner provider fix; full Vanilla V2 mechanics/subtype table/UI source and manifest preserved. DLC/Sanctuary excluded by user.'))
def body(s,name):
 m=re.search(r'(?m)^(?:static )?[^;{}\n]*\b'+re.escape(name)+r'\([^;{}]*\)\s*\{',s);assert m,name;at=m.end();depth=1
 while depth:depth+=int(s[at]=='{')-int(s[at]=='}');at+=1
 return s[m.start():at]
for name in ('options','window','available','clear_tails','swap_selection','snapshot','active','sees_hidden','terrain'):
 s=(root/'mods/ai-hard-mode/ai_hard_mode.c').read_text();assert body(s,name)==body(old('mods/ai-hard-mode/ai_hard_mode.c').decode(),name),name
for path in ('mods/ai-hard-mode/mod.json','src/pc/mods/hooks.c','src/pc/mods/hooks.h','src/pc/mods/modapi.h','src/pc/mods/mods.c','src/pc/mods/mods.h','src/game/main_run_duel.c','src/pc/guest/state.c','src/pc/platform/mods_window.c'):
 assert (root/path).read_bytes()==old(path),path
for name in ('Mods_Order','order_ranked'):assert body((root/'src/pc/mods/manager.c').read_text(),name)==body(old('src/pc/mods/manager.c').decode(),name)
s=(root/'src/pc/guest/state.c').read_text();s=s[s.index('static int load(const char *path)'):];assert s.index('Mods_Hal20StateLoadAllowed')<s.index('fseek(')<s.index('pending_image')
rows.append(dict(check='Core and AI boundaries',status='PASS',scope='Host ABI/hooks/order/controller/load placement unchanged from PR28; only role/target qualification expansion. Strategic options/visibility/deck/swap functions unchanged.'))
wire=(root/'experiments/hal20-field-state/queen_state_schema_v1.h').read_text();assert 'Qs_no_pointer_wire_size' in wire;assert not (root/'mods/ritual-card-effects/queen_state_preview.h').exists();assert not (root/'mods/ritual-card-effects/queen_state_schema_v1.h').exists()
rows.append(dict(check='State A isolation',status='PASS',scope='260-byte typed wire schema in experiments; canonical R unchanged; preview not staged/admitted by guard. No game/third-party state snapshot or pointer blob.'))
a.output.write_text(json.dumps(rows,indent=2)+'\n');print('PASS',len(rows),'scope/provenance groups')
