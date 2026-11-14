#!/usr/bin/env python3
"""Independent table-level assertions for v0.3.3. No game/runtime required."""
import csv,re,sys,hashlib
from pathlib import Path
base=Path(__file__).resolve().parents[1]
s=(base/'profile_data.h').read_text(encoding='utf-8')

names=['isis','jono','jono2','weevil','shadi','heishin','heishin2','rex','martis','desert','bakura','pegasus','mage_soldier','tea2','ocean','secmeton','forest','anubisius','mountain','atenza','meadow','kepura','labyrinth','sebek','neku','seto3rd','darknite','nitemare','mai']
pools={}
for name in names:
 match=re.search(r'static const RdWeightedCard rd_'+name+r'_pool\[\]\s*=\s*\{(.*?)\};',s,re.S)
 assert match,name
 pool={int(a):int(b) for a,b in re.findall(r'\{\s*(\d+)\s*,\s*(\d+)\s*\}',match.group(1))}
 count=len(re.findall(r'\{\s*(\d+)\s*,\s*(\d+)\s*\}',match.group(1)))
 assert len(pool)==count,(name,'duplicate')
 assert sum(pool.values())==(2048 if name=='isis' else 2018),(name,sum(pool.values()))
 pools[name]=pool
expected_counts=dict(zip(names,[101,23,51,73,118,63,74,26,121,57,129,101,151,69,78,90,83,139,60,148,70,165,97,76,87,63,91,70,118]))
for n in names:assert len(pools[n])==expected_counts[n],(n,len(pools[n]))
rex=pools['rex'];martis=pools['martis'];desert=pools['desert'];weevil=pools['weevil']
rex_dino={105:130,218:125,568:115,573:110,59:100,570:100,80:80,32:65,81:65,11:45,79:40,482:39,509:20,705:7}
rex_other={98:57,135:77,154:77,203:38,556:38,237:38,591:72,122:77,200:71,82:20}
rex_misc={331:163,326:249}
assert rex=={**rex_dino,**rex_other,**rex_misc}
assert (sum(rex_dino.values()),sum(rex_other.values()),sum(rex_misc.values()))==(1041,565,412)
assert all(martis[x]==1 for x in (42,112,115,128,162,164,169,171,194,233))
assert martis[705]==30 and martis[719]==50
assert weevil[16]==25 and weevil[72]==30 and weevil[339]==12 and [weevil[i] for i in (50,54,397,476,477)]==[57,49,49,39,39]
assert [weevil[i] for i in (478,479,485)]==[40,40,40]
assert [weevil[i] for i in (717,57,67,367,52,533)]==[50,10,5,48,31,21]
assert all(x not in desert for x in (479,485,699))
assert desert[98]==35 and desert[719]==30 and desert[146]==25
zombie_ids=[564,99,545,368,97,596,36,96,98,719,351,470,241,30,132,146,359,539,228,215,153,139,108]
dino_ids=[509,482,79,11,32,81,80,570,568,59,573,218]
rock_ids=[366,531,632,454,74,517,456,457,623,515,455,244,257,28,505,263]
magic_ids=[331,348,656,663,688,690]
assert set(desert)==set(zombie_ids+dino_ids+rock_ids+magic_ids)
assert tuple(sum(desert[x] for x in ids) for ids in (zombie_ids,dino_ids,rock_ids,magic_ids))==(932,283,219,584)
# New profiles use exactly 12 approved -5 cuts + 30 ready-made result and
# 30 virtual points. No ritual magic in their normal pools.
new_profiles={
 'bakura':(365,674,{493:59,128:43,111:67,131:27,362:27,349:75,106:43,633:43,170:27,89:27,542:27,323:27}),
 'pegasus':(715,695,{577:59,382:43,528:43,357:43,712:43,42:43,337:155,668:115,672:115,329:115,684:115,690:115}),
 'mage_soldier':(362,665,{233:43,25:37,164:35,281:35,600:27,171:19,60:19,64:19,430:19,363:19,234:19,647:19}),
}
for name,(ready,magic,cuts) in new_profiles.items():
 assert len(cuts)==12
 assert pools[name][ready]==30 and magic not in pools[name]
 assert all(pools[name][id]==weight for id,weight in cuts.items())
assert {335:12,341:24,346:12}.items() <= pools['mage_soldier'].items()
assert len(pools)==29 and all(sum(p.values())+(0 if name=='isis' else 30)==2048 for name,p in pools.items())

assert pools['mai'][701]==30 and all(cid not in pools['mai'] for cid in (54,70,416,676))
assert pools['shadi'][720]==20 and not pools['shadi'].get(716) and pools['weevil'][16]==25
for name,sha in [('mai','c22b3597e49467d1b5e7cade3e8d4367d1ba939be94f1d6e710ebc08ad1bd912'),
                 ('shadi','15e79dfb5f5b26d60559375ac978e911859b14ec42e24604d093bdda60e2375b'),
                 ('weevil','a1581f43c26eec34d41f6f8acf8bfc4929f9134e9326350f22b17038e307ec2c')]:
 actual=hashlib.sha256(' '.join(f'{cid}:{weight}' for cid,weight in sorted(pools[name].items())).encode()).hexdigest()
 assert actual==sha,(name,actual)

# The three exact approved new weighted lists. Digests cover every individual
# card ID and weight in ascending card ID order, not only the total and count.
approved_new_pool_sha256={
 'tea2':'67ca6cfe2c0748c48ae57bf13a88ca65a1e039434d0bedabb216c5acaeb18da3',
 'ocean':'27f078b03274f82497292ca0829a401a53d2218ac0e2878f0db06b928bc21a08',
 'secmeton':'24249f9451e5d83bb4c10d62dfa2a166c8bc62ef2ed947fa8a1a298c6a63f852',
 'forest':'4c07cd6da9a01d4bc1890b6dda63ec6b11257336673dd5898094505fb526628e',
 'anubisius':'119f0d9fe131860421a2fe2b6fc63e56402f34d777a885f49580fa74dec8ded1',
 'mountain':'7465844f88d20674edf8ddd63269c1f986d5afbb3c33364dabbc81e2a2cce9b7',
 'atenza':'d93df0a0754eb5e52db48671d981a2edf56a5098f28df52c1343505741f96e20',
 'meadow':'4318324c876c57888a7c40ab91001fdcf2ae07d3b36cb843cee25569f5c3a313',
 'kepura':'569d6bbee892a58f14f3bcdfc9e0ea7d8c95b3e8746a2bc739b9dcb035d47785',
}
for name,digest in approved_new_pool_sha256.items():
 actual=hashlib.sha256(','.join(f'{id}:{w}' for id,w in sorted(pools[name].items())).encode()).hexdigest()
 assert actual==digest,(name,'unapproved weight or ID change',actual)
assert all(cid not in pools['tea2'] for cid in [62,63,330,331,332,333,334,335,348,698])
assert all(cid not in pools['ocean'] for cid in [400,51,77,630,566,76,569,381,590,700])
assert pools['ocean'][718]==30 and pools['ocean'][334]==160
assert pools['secmeton'][710]==48 and 692 not in pools['secmeton']
assert all(cid not in pools['secmeton'] for cid in [35,372,441,508,614,632])

assert pools['mountain'].get(37,0)==0 and pools['mountain'][358]==144
assert pools['atenza'][37]==32 and pools['atenza'].get(358,0)==32
assert pools['atenza'].get(680,0)==0 and pools['atenza'][327]==50 and pools['atenza'][706]==32
assert pools['meadow'][653]==98 and pools['meadow'][677]==40 and 716 not in pools['meadow']
assert pools['kepura'].get(697,0)==0 and pools['kepura'][302]==92 and pools['kepura'][364]==30
assert all(ritual_magic not in pools[name] for name,ritual_magic in [('atenza',691),('meadow',697),('kepura',670)])

# This V3 annex uses SHA256 of space-separated sorted ID:weight pairs.
round6={
'labyrinth':('f5d841ec8431b49bf4c5296c8edc5bd8321eb98471e4e31abd7116d3f6eb4fc8',97),
'sebek':('cb5167958e781026e94cf758a8d30ec24460861c52de5bce16dde275758f3033',76),
'neku':('d468255958ba0e72af366921b2fcdd363824a3e7450313f9488172bc31de8bc1',87),
'seto3rd':('e44e68e98d6eb2140ee9774232cf870fce551451d4a1184c213371f24a01b2e3',63),
'darknite':('eaeaf88dfcbcfcc999bf498cda106b4f30c71d31eb7bc268040905aa862c10d8',91),
'nitemare':('377abdde08a3357a62166f5d1eab116188404339b0c5668017a400f265952cbf',70),
'isis':('776e44b8e3c8c27de3ce618fe518258bc551829484a5ef252f77bed89b67aaaf',101)}
for name,(digest,size) in round6.items():
 pool=pools[name];assert len(pool)==size
 actual=hashlib.sha256(' '.join(f'{a}:{b}' for a,b in sorted(pool.items())).encode()).hexdigest()
 assert actual==digest,(name,actual)
assert pools['isis'].get(679,0)==0 and pools['isis'].get(704,0)==0
assert pools['sebek'].get(335,0)==0 and pools['sebek'].get(535,0)==0
assert pools['sebek'][82]==30 and pools['sebek'][22]==116 and pools['sebek'][217]==72
assert pools['labyrinth'][374]==30 and pools['labyrinth'][660]==60
assert pools['seto3rd'][672]==130 and pools['seto3rd'][380]==80 and pools['seto3rd'][1]==120 and 675 not in pools['seto3rd']
assert [pools['darknite'][cid] for cid in range(330,336)]==[74,74,74,74,74,80] and 694 not in pools['darknite'] and 709 not in pools['darknite']
assert all(pools['nitemare'][cid]==3 for cid in (79,90,99,424,518,555))
assert pools['nitemare'][709]==16 and pools['nitemare'][613]==148 and 694 not in pools['nitemare']
assert pools['neku'][360]==30 and pools['neku'][335]==160 and pools['neku'].get(694,0)==0

path=base/'spec'/'Ritual-Duelists-Pools.csv'
with path.open('w',newline='',encoding='utf-8') as f:
 writer=csv.writer(f,delimiter=';');writer.writerow(['Duelista','Duelist_ID','Carta_ID','Peso','Tipo_entrada'])
 display=dict(zip(names,['Isis','Jono 1st','Jono 2nd','Weevil','Shadi','Heishin 1st','Heishin 2nd','Rex Raptor','High Mage Martis','Desert Mage','Yami Bakura','Pegasus','Mage Soldier','Teana 2nd','Ocean Mage','High Mage Secmeton','Forest Mage','High Mage Anubisius','Mountain Mage','High Mage Atenza','Meadow Mage','High Mage Kepura','Labyrinth Mage','Guardian Sebek','Guardian Neku','Seto 3rd','DarkNite','Nitemare','Mai Valentine']))
 duel_id=dict(zip(names,[16,3,19,10,13,8,35,9,28,27,14,15,18,20,21,22,23,24,25,26,29,30,31,33,34,36,37,38,11]))
 for name in names:
  for cid,weight in sorted(pools[name].items()):writer.writerow([display[name],duel_id[name],cid,weight,'Carta real'])
  if name!='isis':writer.writerow([display[name],duel_id[name],'RITUAL_PACKAGE',30,'Virtual'])
print('PASS audit: 29 pools (28 packages at 2018+30; Isis 2048 real); Rex 1041/565/412; Desert 932/283/219/584; Martis approved cuts; Weevil reconciliation; exact card counts')
print('CSV:',path.name)
