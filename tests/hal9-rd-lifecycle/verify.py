"""Read-only cross-checks of actual runtime logs; not Windows certification."""
from pathlib import Path
import argparse, json, re, hashlib
p=argparse.ArgumentParser();p.add_argument('--work',type=Path,required=True);a=p.parse_args();rows=[]
for group in ('owner','extra','bonus','blocked','regressions','absence'):
 src=a.work/'logs'/('final-'+group);j=json.loads((src/'RESULTADOS.json').read_text())
 assert not j['summary']['incomplete'] and all(b['returncode']==0 for b in j['builds'])
 for q in j['profiles']:
  if group=='owner' and q['mode'] in (314,319):continue # persistent fault control; focal repeat in extra
  m=q['mode'];s=(src/q['log']).read_text();failed=[c for c in q['checks'] if c['status']=='FAIL']
  assert not failed or group=='regressions' and m==34 and len(failed)==1 and failed[0]['name']=='reset-rejects-stale-wheel-completion',(q['name'],failed)
  assert q['complete']
  if group in ('owner','extra','bonus','blocked'):
   assert 'RESULT actual-Main-entry-no-ModulesReset PASS' in s and 'RESULT end-controller-does-not-recover-owner PASS' in s,q['name']
   first=re.search(r'OWNER_FIRST .*?RD=(\d+) generation=(\d+) connection=(\d+) serial=(\d+)',s)
   new=re.search(r'OWNER_NEW .*?reset_delta=(\d+) RD=(\d+) generation=(\d+) connection=(\d+) serial=(\d+) pending=(\d+) monitor=(\d+) safe=(\d+) attempts=(\d+)',s)
   assert first and new and int(new[1])==0,q['name']
   if m not in (311,315,316,318):
    assert int(new[3])>int(first[2]) and int(new[4])>int(first[3]) and new[5]==first[4],q['name']
    assert all(int(new[n])==0 for n in (6,7,8,9)),q['name']
   if m in (300,304,310,321):assert 'RESULT unscoped-InitScene-does-not-invalidate-owner PASS' in s and 'RESULT scene-settings-shuffle-are-not-new-duel-proof PASS' in s
   refused=m in (300,307,308,313,315,316,317,318)
   not_reached=m==319 and not q['dual']
   assert q['classification']==('REFUSED' if refused else 'NOT_REACHED' if not_reached else 'PASS'),(q['name'],q['classification'])
   if m not in (300,307,308,313,315,316,317,318):
    assert 'RESULT new-duel-real-fresh-physical-action PASS' in s,q['name']
    if q['rd'] and 'T' in q['active']:assert 'RESULT old-client-callbacks-during-new-owner-transaction PASS' in s and 'RESULT new-handoff-full-context PASS' in s,q['name']
   if m in (315,316,317,318):assert 'SAFE_REFUSAL ' in s
  rows.append(dict(group=group,case=q['name'],classification=q['classification'],raw_sha256=hashlib.sha256((src/q['log']).read_bytes()).hexdigest(),checks='HISTORICAL_FAIL' if failed else 'PASS'))
(a.work/'logs/INVARIANTS.json').write_text(json.dumps(dict(scope=__doc__,profiles=len(rows),rows=rows),indent=2)+'\n');print('INVARIANTS_PASS',len(rows))
