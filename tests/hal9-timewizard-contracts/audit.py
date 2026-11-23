"""Scope/ABI/provenance audit; no private bytes, source or artifacts emitted."""
from pathlib import Path
import argparse,subprocess,json,hashlib,sys
p=argparse.ArgumentParser();p.add_argument('--original',type=Path,required=True);p.add_argument('--candidate',type=Path,required=True);p.add_argument('--build',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();root=Path(__file__).resolve().parents[2];ref='eca0453cee9f4ac91082b73de756cd6afb7609b3';old=lambda f:subprocess.check_output(['git','show',ref+':'+f],cwd=root);sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest();rows=[]
mods=subprocess.check_output(['git','ls-tree','-r','--name-only',ref,'mods'],cwd=root,text=True).splitlines()
assert all((root/f).read_bytes()==old(f) for f in mods)
rows.append(dict(check='canonical_collection',status='PASS',files=len(mods),details='All inherited MOD files identical to PR30; no canonical .o replaced.'))
for f in ('src/pc/mods/manager.c','src/pc/mods/state_policy.h','src/pc/mods/hooks.c','src/pc/mods/modapi.h','src/pc/mods/hal20_qualified.h','src/pc/guest/state.c','src/pc/guest/state_subsystems.c','src/pc/platform/mods_window.c','src/game/main_run_duel.c'):
 assert (root/f).read_bytes()==old(f),f
rows.append(dict(check='policy_B_and_hook_semantics',status='PASS',details='PR30 B, public host ABI, controller, qualified guard, relink/order and UI unchanged.'))
s=old('src/pc/mods/mods.c').decode();s=s.replace('static void *resolve(const char *name, void *context)','\n#include "hal9_runtime.h"\nstatic void *resolve(const char *name, void *context)',1).replace('\n\n#include "hal9_runtime.h"','\n#include "hal9_runtime.h"',1).replace('    (void)context;\n    return Mods_Lookup(name);','    if(!strncmp(name,"Hal9_Core",9))return hal9_resolve(name,(Mod*)context);\n    return Mods_Lookup(name);',1)
s=s.replace('    if (ObjectLoader_Load(data, (size_t)size, resolve, mod, &mod->object, error, sizeof(error))) {','    Sha256(data,(size_t)size,mod->file_sha256); /* attest private imports before relocation */\n    if (ObjectLoader_Load(data, (size_t)size, resolve, mod, &mod->object, error, sizeof(error))) {',1).replace('    mod->code_hash = mod->object.hash;\n    Sha256(data,(size_t)size,mod->file_sha256);','    mod->code_hash = mod->object.hash;',1)
assert (root/'src/pc/mods/mods.c').read_text()==s
rows.append(dict(check='core_delta',status='PASS',details='Only private import resolver include/branch and same-buffer SHA attestation moved before relocation. Three read-only APIs opt-in, API24/i386 and exact caller digest.'))
changed=[];same=[]
for f in a.original.rglob('*'):
 if not f.is_file() or '__pycache__' in f.parts:continue
 rel=f.relative_to(a.original);g=a.candidate/rel
 (same if sha(f)==sha(g) else changed).append(rel.as_posix())
assert sorted(changed)==['source/time_wizard_dual_play_compat.c','time-wizard.o'],changed
rows.append(dict(check='private_package',status='PASS',unchanged_files=same,changed=changed,details='Manifest/settings/cards/sounds/Encore/AI C/evolution/description and original core evidence unchanged.'))
def names(p):
 return set(x.split()[-1] for x in subprocess.check_output(['nm','-g','--defined-only',p],text=True).splitlines()),set(x.split()[-1] for x in subprocess.check_output(['nm','-u',p],text=True).splitlines())
x,xi=names(a.original/'time-wizard.o');y,yi=names(a.candidate/'time-wizard.o');extra={'Hal9_CoreHardStateV1','Hal9_CoreDualDrawV1','Hal9_CoreOwnersReadyV1'};assert yi-xi==extra and xi-yi=={'__x86_indirect_thunk_ecx','__x86_indirect_thunk_edx'};assert not x-y
rows.append(dict(check='private_ABI_extension',status='PASS',imports_added=sorted(yi-xi),imports_removed=sorted(xi-yi),exports_added=sorted(y-x),exports_removed=sorted(x-y),details='Needs experimental core; NOT loadable by stock SDK24. All old public symbols preserved; new own probes/entry are additional.'))
receipt=json.loads((a.build/'BUILD.json').read_text());assert sha(a.candidate/'time-wizard.o')==receipt['candidate_sha256'];assert sha(a.original/'source/time-wizard-base-v0.23.1.o')==receipt['core_sha256']
sys.path.insert(0,str(root/'tests/hal20-field-state'));from elf_inventory import elf_info
receipt['function_fingerprints']=elf_info(a.candidate/'time-wizard.o');receipt['scope_audit']=rows
receipt['private_input_redistribution']='NOT_AUTHORIZED_OR_VERIFIED for bundled legacy third-party core/assets; own builder/patch sources and metadata only published. No private Dual files published.'
a.output.write_text(json.dumps(receipt,indent=2)+'\n');print('PASS',len(rows),'scope/ABI groups,',len(mods),'canonical files')
