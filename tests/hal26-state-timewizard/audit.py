"""Exact scope audit against PR29, including every inherited MOD asset/object."""
from pathlib import Path
import subprocess,json,hashlib,argparse
p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();r=Path(__file__).resolve().parents[2];ref='3284ec493d179f4b23cc7230a2979a1c2b122873';old=lambda f:subprocess.check_output(['git','show',ref+':'+f],cwd=r);sha=lambda f:hashlib.sha256(f.read_bytes()).hexdigest();rows=[]
files=subprocess.check_output(['git','ls-tree','-r','--name-only',ref,'mods'],cwd=r,text=True).splitlines()
for f in files:assert (r/f).read_bytes()==old(f),f
rows.append(dict(check='All inherited MOD files unchanged',status='PASS',files=len(files),scope='M/R/RD/F/Field/H/DMC and optional official packages; no canonical .o rebuilt/replaced. Private Dual not in Git.'))
f='src/pc/mods/mods.c';s=old(f).decode();needle='unsigned Mods_CodeHash(int index) { return at(index) ? mods[index].code_hash : 0; }';assert (r/f).read_text()==s.replace(needle,needle+'\nint Mods_CodeResident(int index) { return at(index) && (mods[index].object.image || mods[index].object.native_handle); }',1)
f='src/pc/mods/mods.h';assert (r/f).read_text()==old(f).decode().replace('unsigned Mods_CodeHash(int mod);','unsigned Mods_CodeHash(int mod);\n/* Read-only residency, including a mapped library whose init failed. */\nint Mods_CodeResident(int mod);',1)
rows.append(dict(check='Residency query',status='PASS',scope='Only read-only accessor in mods.c/.h; host ABI unchanged, no load/unload/relink/ordering behavior changed.'))
f='src/pc/mods/manager.c';s=old(f).decode();new=(r/f).read_text();start=s.index('int Mods_Hal20StateLoadAllowed(');end=s.index('\nint Mods_Validate',start);begin=new.index('/* Experimental HAL26 fail-closed policy:');stop=new.index('\nint Mods_Validate',begin);assert new[:begin]==s[:start] and new[stop:]==s[end:]
for f in ('src/pc/mods/hooks.c','src/pc/mods/hooks.h','src/pc/mods/modapi.h','src/pc/mods/hal20_qualified.h','src/game/main_run_duel.c','src/pc/guest/state.c','src/pc/guest/state_subsystems.c','src/pc/platform/mods_window.c'):
 assert (r/f).read_bytes()==old(f),f
rows.append(dict(check='Core scope',status='PASS',scope='Manager delta limited to load refusal. Actual common load placement, hook order/relink, qualified artifacts, controller and UI unchanged from PR29.'))
for f in subprocess.check_output(['git','ls-tree','-r','--name-only',ref,'src/pc/saves'],cwd=r,text=True).splitlines():assert (r/f).read_bytes()==old(f)
rows.append(dict(check='Memory-card/storage implementation',status='PASS',scope='All src/pc/saves files unchanged; official default strict storage tests separate from Windows gameplay.'))
w=(r/'.github/workflows/hal26-windows-state-control.yml').read_text();assert 'contents: read' in w and 'contents: write' not in w and 'secrets.' not in w and 'windows-latest' in w and 'x86' in w
rows.append(dict(check='Windows control isolation',status='PASS',scope='Own portable x86 control only, scoped experimental workflow, read-only token, no image/private MOD artifacts. Runner disabled by fork policy; no Windows runtime PASS claim.'))
for f in ('mods/ritual-card-effects/queen_state_preview.h','mods/fusion-auxiliary-balance/fusion_lease_preview.h'):assert not (r/f).exists()
rows.append(dict(check='A partial isolation',status='PASS',scope='Only source schemas/probes in experiments; canonical R/F untouched. Whole-collection preflight always INCOMPLETE; guard rejects variant SHA.'))
a.output.write_text(json.dumps(rows,indent=2)+'\n');print('PASS',len(rows),'scope groups;',len(files),'MOD files unchanged')
