"""Independent read-only checks of tokens, owner boundaries and physical traces."""
from pathlib import Path
import argparse,json,re,hashlib
p=argparse.ArgumentParser();p.add_argument('--logs',type=Path,required=True);a=p.parse_args();rows=[]
groups=('final-lifecycle','final-end-boundary','final-projections','final-off-projection','final-primary','final-boundaries','final-regressions','final-absence')
for group in groups:
 src=a.logs/group;j=json.loads((src/'RESULTADOS.json').read_text());assert j['profiles'] and not j['summary']['incomplete'];assert all(x['returncode']==0 for x in j['builds'])
 for q in j['profiles']:
  s=(src/q['log']).read_text();m=q['mode']
  fails=[c for c in q['checks'] if c['status']=='FAIL']
  assert not fails or group=='final-regressions' and m==34 and len(fails)==1 and fails[0]['name']=='reset-rejects-stale-wheel-completion',(q['name'],fails)
  if group in ('final-lifecycle','final-end-boundary'):
   assert 'RESULT new-duel-without-ModulesReset PASS' in s and 'reset_delta=0' in s,q['name']
   assert 'RESULT old-ticket-cannot-continue PASS' in s and 'RESULT old-client-callbacks-never-authorize PASS' in s,q['name']
   blocked=m in (202,206,207,212)
   assert q['classification']==('REFUSED' if blocked else 'PASS'),q['name']
   if not blocked:
    assert 'RESULT new-duel-new-explicit-transaction PASS' in s and 'RESULT old-callback-during-fresh-negotiation-refused PASS' in s,q['name']
    z=re.search(r'RECOVERY_FINAL .*?old_action=(\d+) action=(\d+) old_client=(\d+) client=(\d+) offered_nonce=(\d+) caller_nonce=(\d+) new_nonce=(\d+) resets=(\d+)',s);assert z,q['name'];v=list(map(int,z.groups()));assert v[1]>v[0] and v[3]>v[2] and v[6]>v[4] and v[7]==0,q['name']
   if m in (202,207):assert re.search(r'RECOVERY_OWNER_BLOCK .*?state=4 .*?hand=5',s),q['name']
  if group in ('final-projections','final-off-projection') and 'PROJECTION_END' in s:
   block=int(re.search(r'PROJECTION_END .*?block=(\d+)',s).group(1))
   assert q['classification']==('REFUSED' if block and m!=231 else 'NOT_REACHED'),q['name']
  rows.append(dict(group=group,case=q['name'],classification=q['classification'],checks='PASS' if not fails else 'HISTORICAL_FAIL',raw_sha256=hashlib.sha256((src/q['log']).read_bytes()).hexdigest()))
(a.logs/'INVARIANTS.json').write_text(json.dumps(dict(scope=__doc__,profiles=len(rows),rows=rows),indent=2)+'\n');print('INVARIANTS_PASS',len(rows))
