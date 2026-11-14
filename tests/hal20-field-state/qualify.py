"""Generate the candidate receipt/header from exact reviewed objects and real registrations.
Do not use this to approve an untested replacement; provenance/behavior review is separate.
"""
from pathlib import Path
import argparse,json,re,os,hashlib
from elf_inventory import elf_info
root=Path(__file__).resolve().parents[2];p=argparse.ArgumentParser();p.add_argument('--log',type=Path,required=True);a=p.parse_args();log=a.log.read_text();targets=['func_8001BAF0','DuelScene_UpdateHandActions','AiScript_Run','Duel_SetupCardRecord','DuelEffect_ApplyTerrain','func_80037DA4','func_80035E20'];dirs=[Path(os.environ['MEMORIES_AI_DIR']),root/'mods/ritual-duelists',Path(os.environ['MEMORIES_DUAL_DIR']),root/'mods/ritual-card-effects',root/'mods/melhoria-dos-rituais',root/'mods/fusion-auxiliary-balance',root/'mods/field-effects-secondary-types'];roles=[];callbacks=[]
for role,d in enumerate(dirs):
 m=json.loads((d/'mod.json').read_text());obj=d/(m['library']+'.o');artifact=elf_info(obj);match=re.search(r'^LOADED '+re.escape(m['id'])+r' active=1 code=([0-9a-f]{8}) ',log,re.M);assert match,m['id'];code=match[1];roles.append(dict(id=m['id'],sha256=artifact['sha256'],loader_code=code,object_bytes=artifact['bytes'],binary_in_this_branch=role!=2))
 for line in log.splitlines():
  if not line.startswith('CALLBACK_NODE owner='+m['id']+' '):continue
  fields=dict(x.split('=',1) for x in line.split()[1:])
  if fields['target'] not in targets:continue
  found=[f for f in artifact['functions'] if f['name']==fields['function'] and f['bytes']==int(fields['bytes'])];assert len(found)==1,(m['id'],fields)
  f=found[0];callbacks.append(dict(role=role,target=fields['target'],function=f['name'],bytes=f['bytes'],body_sha256=f['body_sha256'],relocations_sha256=f['relocations_sha256']))
r=elf_info(root/'mods/ritual-card-effects/ritual-card-effects.o');f=next(x for x in r['functions'] if x['name']=='selection_evaluate');producer={k:f[k] for k in ('name','bytes','body_sha256','relocations_sha256')}
meta=dict(roles=roles,targets=targets,callbacks=callbacks,producer=producer,scope='Reviewed exact artifacts; IDs locate roles only. Private Dual metadata only, no source/manifest/object distributed.');(root/'notes/hal-20-field-state/QUALIFIED_ARTIFACTS.json').write_text(json.dumps(meta,indent=2)+'\n')
s='/* Experimental exact-artifact qualification; see notes/hal-20-field-state. */\n';s+='enum{HAL20_ROLE_COUNT=7,HAL20_TARGET_COUNT=7};\nstatic const char*const hal20_targets[]={'+','.join(json.dumps(x) for x in targets)+'};\n';s+='typedef struct{const char*id;unsigned code;const char*sha;} Hal20Role;\nstatic const Hal20Role hal20_roles[]={\n'+''.join('{"%s",0x%su,"%s"},\n'%(r['id'],r['loader_code'],r['sha256']) for r in roles)+'};\n';s+='typedef struct{int role;const char*target,*function;size_t bytes;} Hal20Callback;\nstatic const Hal20Callback hal20_callbacks[]={\n'+''.join('{%d,"%s","%s",%d},\n'%(r['role'],r['target'],r['function'],r['bytes']) for r in callbacks)+'};\n';s+='static const size_t hal20_producer_bytes=%d;\n'%producer['bytes'];(root/'src/pc/mods/hal20_qualified.h').write_text(s);print('Qualified metadata:',len(roles),'artifacts,',len(callbacks),'callbacks,',len(targets),'targets')
