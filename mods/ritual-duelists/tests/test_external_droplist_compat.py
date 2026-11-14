"""Static regression for Ritual Duelists leaving Free Duel drop visualization to Droplist.

Usage: python tests/test_external_droplist_compat.py [path-to-droplist-1.4.0.zip]
Checks only file boundaries and imports; does not simulate in-game input or live mods.
"""
import json, pathlib, subprocess, sys, zipfile, re
root=pathlib.Path(__file__).resolve().parents[1]
manifest=json.loads((root/'mod.json').read_text(encoding='utf-8'))
source=(root/'mod.c').read_text(encoding='utf-8')
assert manifest['id']=='ritual-duelists'
assert manifest['library']=='ritual-duelists'
assert manifest['after']==['drop-missing-cards']
assert manifest['min_api']==12
assert not any(setting['key']=='drop_list_enabled' for setting in manifest['settings'])
assert 'mod->frame=0;' in source
assert 'mod->overlay=rd_overlay;' in source
assert 'mod->overlay_signature=rd_overlay_signature;' in source
assert 'Rd_DropRewardsInit' in source
assert 'Rd_StarterUIInit' in source
assert 'Rd_DropList' not in source
assert not (root/'rd_droplist.c').exists()
assert not (root/'rd_droplist.h').exists()
assert not (root/'rd_dl11_sprites.h').exists()
assert not (root/'assets/ui').exists()
# Only the newly rebuilt target is installable in this delivery.
for target in ('ritual-duelists.o',):
    out=subprocess.check_output(['nm','-g',str(root/target)],text=True)
    assert 'Rd_DropList' not in out, target
    assert 'MemoriesModInit' in out, target
if len(sys.argv)>1:
    with zipfile.ZipFile(sys.argv[1]) as z:
        name='droplist/mod.json'
        friend=json.loads(z.read(name))
        assert friend['id']=='droplist' and friend['version']=='1.4.0'
        assert friend['enabled'] and friend['library']=='droplist'
        for obj in ('droplist.o','droplist.x86_64-windows.o'):
            p=root/('tmp_'+obj)
            data=z.read('droplist/'+obj)
            # The external viewer needs a runtime accessor of active drop tables;
            # detect it in ELF's dynamic/static string table without disassembly.
            assert b'Tables_PoolFor\x00' in data, obj
    print('PASS: supplied external Droplist 1.4.0 imports runtime drop tables')
else:
    print('SKIP: external Droplist 1.4.0 package not supplied; no live viewer/input validation')
print('PASS: Ritual Duelists i386 has no internal viewer; gameplay callbacks retained')
