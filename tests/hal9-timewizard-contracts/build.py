"""Strictly pinned compatibility surgery; never claim recovered/full core source.
Only three audited Boolean loads change to state-preserving calls. Delegation,
all effects/RNG/animations and other core executable bytes stay unchanged.
"""
from pathlib import Path
import argparse,subprocess,sys,shutil,json,hashlib,struct,importlib.util,difflib
from elftools.elf.elffile import ELFFile
p=argparse.ArgumentParser();p.add_argument('--package',type=Path,required=True);p.add_argument('--sdk',type=Path,required=True);p.add_argument('--output',type=Path,required=True);a=p.parse_args();a.output.mkdir(parents=True,exist_ok=False);root=Path(__file__).resolve().parents[2];dst=a.output/'package';shutil.copytree(a.package,dst);src=dst/'source';sha=lambda f:hashlib.sha256(f.read_bytes()).hexdigest();core=src/'time-wizard-base-v0.23.1.o';pins=json.loads((root/'notes/hal-9/TIMEWIZARD_INPUTS.json').read_text())['files']
for item in pins:assert sha(a.package/item['path'])==item['sha256'],item['path']
assert sha(core)=='5b4252dafbc07532f859cda92e49e75d15e531a880f90cc7924371b7e37f9b4a'
assert '#define MEMORIES_MOD_API 24' in (a.sdk/'include/pc/mods/mod_types.h').read_text().splitlines(), 'SDK must be API24'
sys.path.insert(0,str(a.sdk/'tools'));spec=importlib.util.spec_from_file_location('build_mod',a.sdk/'tools/build_mod.py');b=importlib.util.module_from_spec(spec);spec.loader.exec_module(b)
rows=[]
def run(cmd,name):
 q=subprocess.run(list(map(str,cmd)),text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,timeout=60);(a.output/(name+'.log')).write_text(q.stdout);rows.append(dict(name=name,command=list(map(str,cmd)),returncode=q.returncode));assert q.returncode==0,q.stdout
# Add two undefined symbols using binutils, then edit only the three known
# instruction operands/opcodes and relocation records. No function moves.
aliased=a.output/'aliased-core.o';run(['objcopy','--add-symbol','Hal9_ReadHardEAX=.bss:0,global','--add-symbol','Hal9_ReadHardECX=.bss:0,global',core,aliased],'add-probe-symbols');data=bytearray(aliased.read_bytes());changes=[]
with aliased.open('rb') as f:
 e=ELFFile(f);symbols=e.get_section_by_name('.symtab');syms=list(symbols.iter_symbols());sec=list(e.iter_sections());text=e.get_section_by_name('.text');rel=e.get_section_by_name('.rel.text');bss=next(i for i,s in enumerate(sec) if s.name=='.bss');ids={s.name:i for i,s in enumerate(syms)}
 for name in ('Hal9_ReadHardEAX','Hal9_ReadHardECX'):
  at=symbols['sh_offset']+ids[name]*symbols['sh_entsize'];struct.pack_into('<IIIBBH',data,at,syms[ids[name]]['st_name'],0,0,0x11,0,0)
 refs=[]
 for i,r in enumerate(rel.iter_relocations()):
  at=r['r_offset'];s=syms[r['r_info_sym']]
  if s['st_shndx']==bss and struct.unpack_from('<I',text.data(),at)[0]==0x1e64:refs.append(at)
 assert refs==[0x10e41,0x10f3d,0x10f69,0x10fff,0x121da,0x12301],refs
 for disp,start,n,target,cmp_offset in [(0x10fff,0x10ffe,5,'Hal9_ReadHardEAX',0x11009),(0x121da,0x121d9,5,'Hal9_ReadHardEAX',0x121e4),(0x12301,0x122ff,6,'Hal9_ReadHardECX',0x1230c)]:
  original=text.data()[start:start+n];assert original==(b'\xa1\x64\x1e\0\0' if n==5 else b'\x8b\x0d\x64\x1e\0\0');assert any(r['r_offset']==cmp_offset and syms[r['r_info_sym']].name=='AiScript_Run' for r in rel.iter_relocations())
  patch=b'\xe8\xfc\xff\xff\xff'+(b'\x90' if n==6 else b'');data[text['sh_offset']+start:text['sh_offset']+start+n]=patch
  k=next(i for i,r in enumerate(rel.iter_relocations()) if r['r_offset']==disp);at=rel['sh_offset']+k*rel['sh_entsize'];struct.pack_into('<II',data,at,start+1,(ids[target]<<8)|2);changes.append(dict(start=start,bytes=n,old_operand_offset=disp,target=target,comparison_relocation_unchanged=cmp_offset))
 patched=a.output/'patched-core.o';patched.write_bytes(data)
 assert all(data[i]==aliased.read_bytes()[i] for i in range(text['sh_offset'],text['sh_offset']+text['sh_size']) if not any(text['sh_offset']+x['start']<=i<text['sh_offset']+x['start']+x['bytes'] for x in changes))
# Preserve all original aliases and legacy wrapper init chain.
renamed=a.output/'renamed-core.o';cmd=['objcopy','--redefine-sym','MemoriesModInit=TimeWizardLegacyDescriptionInit','--redefine-sym','TimeWizardV023MemoriesModInit=TimeWizardLegacyCompatInit'];aliases={'tw_ext_phase':0x830,'tw_ext_won':0x844,'tw_ext_plan_count':0x84c,'tw_ext_plan':0x88c,'tw_original_retail_card_use':0x97c,'tw_ext_chosen':0x840,'tw_ext_spin_id':0x9a8,'tw_ext_rng':0x82c,'tw_ext_chance_50':0x998,'tw_ext_test_result':0x9a0}
for name,offset in aliases.items():cmd+=['--add-symbol',f'{name}=.bss:{offset:#x},global']
run(cmd+[patched,renamed],'rename-core');objects=[renamed]
# Only the available Dual adapter changes: obtain a qualified active callback
# from the core, then retain the original exact signature/state checks.
f=src/'time_wizard_dual_play_compat.c';before=f.read_text();start=before.index('static DualPlayStatePrefix *resolve_dual_state(void)\n{');end=before.index('\n}',start)+2;old=before[start:end];needle='    fn = (const uint8_t *)compat_original_draw;';assert needle in old
cut=old.index('    if (!compat_original_draw');tail=old[old.index('    if (fn[0x23]'):];new=old[:cut]+'''    fn = (const uint8_t *)Hal9_CoreDualDrawV1();
    if (!fn) { cached_dual_draw=0;cached_dual_state=0;diag_dual_seen=0;return 0; }
    if (cached_dual_draw==(void*)fn) {diag_dual_seen=cached_dual_state!=0;return cached_dual_state;}
    cached_dual_draw=(void*)fn;cached_dual_state=0;
'''+tail
s=before[:start]+'extern const void *Hal9_CoreDualDrawV1(void);\n'+new+before[end:];f.write_text(s);(a.output/'dual-source.patch').write_text(''.join(difflib.unified_diff(before.splitlines(True),s.splitlines(True),fromfile='a/source/time_wizard_dual_play_compat.c',tofile='b/source/time_wizard_dual_play_compat.c')))
cc,flags,ld=b.compiler('i386');flags=b.target_flags('i386',flags)+['-ffile-prefix-map='+str(dst.resolve())+'=/hal9/package','-ffile-prefix-map='+str(root.resolve())+'=/hal9/repo','-fdebug-compilation-dir=/hal9/build']
for name,entry in [('time_wizard_dual_play_compat','TimeWizardV023MemoriesModInit'),('time_wizard_description_fit','TimeWizardV0231OriginalMemoriesModInit'),('time_wizard_evolution','TimeWizardV0232OriginalInit'),('time_wizard_encore','TimeWizardContractOriginalInit')]:
 obj=a.output/(name+'.o');run(cc+flags+[f'-DMemoriesModInit={entry}','-c',src/(name+'.c'),'-o',obj],name);objects.append(obj)
for name,source in [('contracts_entry',root/'experiments/hal9/contracts_entry.c'),('ai_probe_i386',root/'experiments/hal9/ai_probe_i386.S')]:
 obj=a.output/(name+'.o');run(cc+flags+['-c',source,'-o',obj],name);objects.append(obj)
run(ld+['-o',dst/'time-wizard.o']+objects,'link');undefined={x.split()[-1] for x in subprocess.check_output(['nm','-u',str(dst/'time-wizard.o')],text=True).splitlines()};extra={'Hal9_CoreHardStateV1','Hal9_CoreDualDrawV1','Hal9_CoreOwnersReadyV1'};missing=undefined-set((a.sdk/'exports.i386.txt').read_text().split());assert missing==extra,missing
(a.output/'BUILD.json').write_text(json.dumps(dict(scope='Pinned legacy core compatibility candidate, NOT full rebuild. Requires explicit experimental core bridge; never an unmodified SDK24 admission claim.',core_sha256=sha(core),candidate_sha256=sha(dst/'time-wizard.o'),bytes=(dst/'time-wizard.o').stat().st_size,patches=changes,unchanged_delegation_relocations=[0x10e41,0x10f3d,0x10f69],required_experimental_core_imports=sorted(extra),official_imports=len(undefined-extra),steps=rows),indent=2)+'\n');print('PASS pinned candidate',sha(dst/'time-wizard.o'))
