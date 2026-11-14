"""Local experimental archive proof. No GitHub release, ROM, saves or Dual files.
Copies immutable qualified MODs, not the builder's newly debug-rebuilt copies.
"""
from pathlib import Path
import argparse,hashlib,json,os,shutil,struct,zipfile
repo=Path(__file__).resolve().parents[2];p=argparse.ArgumentParser(description=__doc__);p.add_argument('--build',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--report',type=Path,required=True);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=True);root=a.output/'HAL20-EXPERIMENTAL';root.mkdir();sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest();meta=json.loads((repo/'notes/hal-20/guard-f1/INPUTS.json').read_text());exe=a.build/'memories-pc.exe';data=exe.read_bytes();at=struct.unpack_from('<I',data,0x3c)[0];assert data[at:at+4]==b'PE\0\0' and struct.unpack_from('<H',data,at+4)[0]==0x14c and struct.unpack_from('<H',data,at+24)[0]==0x10b;assert b'experimental.hal2224_order_guard' in data
for n in ('memories-pc.exe','SDL3.dll','memories-pc.pdb','buildid'):shutil.copy2(a.build/n,root/n)
shutil.copy2(repo/'LICENSE',root/'LICENSE');(root/'symbols').mkdir();bid=(a.build/'buildid').read_text().strip()
for n in (bid+'.txt',bid+'.vars.txt'):shutil.copy2(a.build/'symbols'/n,root/'symbols'/n)
for c in meta['components']:
 src=repo/c['directory'];dst=root/c['directory'];dst.mkdir(parents=True)
 for f in c['files']:
  source=src/f['path'];assert sha(source)==f['sha256'];target=dst/f['path'];target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(source,target)
ai=Path(os.environ['MEMORIES_AI_DIR']);dst=root/'mods/ai-hard-mode';dst.mkdir()
for n in ('mod.json','ai-hard-mode.o'):shutil.copy2(ai/n,dst/n)
for f in meta['local_dependencies']:
 if f['id']=='ai-hard-mode':assert sha(dst/f['path'])==f['sha256']
shutil.copytree(repo/'mods/drop-missing-cards',root/'mods/drop-missing-cards')
shutil.copy2(repo/'tests/hal20-guard-f1/WINDOWS.md',root/'README-WINDOWS.md');(root/'tools').mkdir();shutil.copy2(repo/'tests/hal20-guard-f1/windows/Run-Profile.ps1',root/'tools/Run-Profile.ps1')
profiles=[];ids={'M':'melhoria-dos-rituais','R':'ritual-card-effects-test','F':'fusion-auxiliary-balance','H':'ai-hard-mode','RD':'ritual-duelists','D':'dual-play'}
for name,active in [('melhoria',['M']),('ritual',['R']),('fusion',['F']),('ritual-duelists',['RD']),('own-mrf',['M','R','F']),('h-dual',['H','D']),('rd-dual',['RD','D']),('minimal-h-rd-dual',['H','RD','D']),('full-refused',list(ids))]:
 expectation='Recusa esperada antes do duelo; nao liberar o guard.' if name=='full-refused' else 'Perfil de teste; gameplay Windows ainda nao validado.'
 profiles.append(dict(name=name,active=active,requires_dual='D' in active,expectation=expectation));dst=root/'profiles'/f'{name}.txt';dst.parent.mkdir(exist_ok=True);ranks={'M':-30,'R':-20,'F':-10,'H':0,'RD':10,'D':20};dst.write_text('experimental.hal2224_order_guard=1\nmod.drop-missing-cards=1\nmod.drop-missing-cards.order=-50\n'+''.join(f'mod.{id}={int(k in active)}\nmod.{id}.order={ranks[k]}\n' for k,id in ids.items()))
files=[dict(path=str(f.relative_to(root)),sha256=sha(f),bytes=f.stat().st_size) for f in sorted(root.rglob('*')) if f.is_file()];assert not any('/dual-play/' in x['path'] for x in files);assert len([x for x in files if x['path'].endswith('/fusion-auxiliary-balance.o')])==1
private=[dict(path='mods/dual-play/'+f['path'],sha256=f['sha256']) for f in meta['local_dependencies'] if f['id']=='dual-play']
manifest=dict(experimental_only=True,gameplay_Windows_validated=False,profiles=profiles,files=files,private_dual_required_locally=private,guard_commit=meta['guard_commit']);(root/'MANIFEST.json').write_text(json.dumps(manifest,ensure_ascii=False,indent=2)+'\n')
archive=a.output/'HAL20-EXPERIMENTAL-WINDOWS-i386.zip'
with zipfile.ZipFile(archive,'w',zipfile.ZIP_DEFLATED) as z:
 for f in sorted(root.rglob('*')):
  if f.is_file():z.write(f,str(f.relative_to(a.output)))
with zipfile.ZipFile(archive) as z:
 assert z.testzip() is None
 for f in files:assert hashlib.sha256(z.read('HAL20-EXPERIMENTAL/'+f['path'])).hexdigest()==f['sha256']
report=dict(archive=archive.name,sha256=sha(archive),bytes=archive.stat().st_size,archive_valid=True,local_only=True,private_Dual_included=False,ROM_or_personal_saves_included=False,gameplay_Windows_validated=False,PE32_i386=True,guard_optin_string_in_core=True,guard_source_files=meta['guard_files'],link_report=json.loads((a.build/'link-report.json').read_text()),files=files,profiles=profiles)
a.report.write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n');print('PASS local package',archive.name,archive.stat().st_size,'bytes',len(files),'files; no Dual/ROM/save; no release')
