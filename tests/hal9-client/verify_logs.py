"""Independent read-only oracles over actual controller/gate traces."""
from pathlib import Path
import argparse,json,re,hashlib
p=argparse.ArgumentParser();p.add_argument('--logs',type=Path,required=True);a=p.parse_args();rows=[]
for group in ('delivery-primary','delivery-boundaries','delivery-absence','delivery-regressions'):
 src=a.logs/group;j=json.loads((src/'RESULTADOS.json').read_text());assert j['profiles'] and not j['summary']['incomplete'];assert all(x['returncode']==0 for x in j['builds'])
 for q in j['profiles']:
  f=src/q['log'];s=f.read_text();mode=q['mode'];gates=[]
  for stage,source,kind,dest,used,bonus,last in re.findall(r'^CLIENT_GATE stage=(\w+) source=(\d+) type=(-?\d+) destination=(\d+) used=(\d+) bonus=(\d+) last=(\d+)$',s,re.M):
   if stage!='enter' or int(source)==0:continue
   category=2 if int(kind)>=20 else 1;gates.append(dict(category=category,destination=int(dest),used=int(used),bonus=int(bonus)))
   if group=='delivery-primary' and mode in (102,113,120,124):
    assert category==(2 if int(dest)>=6 else 1),(q['name'],gates[-1])
    assert not category&int(used),(q['name'],gates[-1])
  for name,status,detail in re.findall(r'^RESULT (\S+) (PASS|FAIL) (.*)$',s,re.M):
   if status=='FAIL':assert group=='delivery-regressions' and mode==34 and name=='reset-rejects-stale-wheel-completion',(q['name'],name)
  stop=re.search(r'^CLIENT_STOP .*?hand=(\d+) exchanges=(\d+).*?consumed=(\d+)',s,re.M)
  if group in ('delivery-primary','delivery-boundaries','delivery-absence') and mode!=120:
   assert stop,q['name'];hand,exchanges,consumed=map(int,stop.groups())
   positive=mode in (101,102,103,113,118,119,121,124)
   assert hand==(4 if positive else 5) and consumed==int(positive) and exchanges<=int(mode==101),(q['name'],stop.group())
  if mode==120 and q['dual']:
   second=re.search(r'^CLIENT_SECOND_SUPPORT phase=(\d+) id=(\d+) used=(\d+) hand=(\d+) views=(\d+)$',s,re.M)
   assert second and tuple(map(int,second.groups()))[:4] in ((7,16,3,3),(6,16,3,3)),q['name']
  if mode==121 and q['dual']:
   assert 'RESULT real-Dual-rejects-spent-Support PASS' in s and any(g['used']==2 and g['category']==2 for g in gates),q['name']
  rows.append(dict(group=group,case=q['name'],classification=q['classification'],physical_or_regression='PASS',gates=gates,raw_sha256=hashlib.sha256(f.read_bytes()).hexdigest()))
result=dict(scope=__doc__,profiles=len(rows),rows=rows)
(a.logs/'CLIENT_INVARIANTS.json').write_text(json.dumps(result,indent=2)+'\n');print('TRACE_ORACLES_PASS',len(rows))
