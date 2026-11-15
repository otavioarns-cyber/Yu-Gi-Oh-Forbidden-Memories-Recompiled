"""Read-only checks over the actual raw callback traces; never inject state."""
from pathlib import Path
import argparse,json,re,hashlib
p=argparse.ArgumentParser();p.add_argument('--logs',type=Path,required=True);a=p.parse_args();rows=[];positive={51,52,53,67,71}
for group in ('final-primary','final-boundaries','final-absence'):
 src=a.logs/group;j=json.loads((src/'RESULTADOS.json').read_text());assert j['profiles'] and j['summary']['processes']==len(j['profiles']) and not j['summary']['incomplete'];assert all(x['returncode']==0 for x in j['builds'])
 for q in j['profiles']:
  f=src/q['log'];s=f.read_text();m=re.search(r'^HANDOFF_STOP mode=(\d+) phase=(\d+) id=(\d+) hand=(\d+) exchanges=(\d+) pending=(\d+) gate=(\d+) RD=(\d+) H=(\d+) hf_state=(\d+) consumed=(\d+) reason=(-?\d+)$',s,re.M);assert m,q['name'];v=list(map(int,m.groups()));mode=v[0];ok=mode in positive;assert v[3]==(4 if ok else 5) and v[4]<=(1 if ok else 0) and v[10]==int(ok),q['name']
  quota='NOT_APPLICABLE'
  if q['dual']:
   ledger=re.findall(r'^SUPPORT_HAND_LEDGER stage=exit owner=ritual-card-effects-test .*?used=(\d+) ',s,re.M);assert ledger,q['name'];used=int(ledger[-1]);expected=(1 if mode==53 else 2) if ok else 0;assert used==expected,(q['name'],used,expected);quota='PASS'
  rows.append(dict(group=group,case=q['name'],classification=q['classification'],physical='PASS',quota_after_actual_callbacks=quota,raw_sha256=hashlib.sha256(f.read_bytes()).hexdigest()))
result=dict(scope=__doc__,profiles=len(rows),physical_PASS=len(rows),quota_PASS=sum(r['quota_after_actual_callbacks']=='PASS' for r in rows),quota_not_applicable=sum(r['quota_after_actual_callbacks']=='NOT_APPLICABLE' for r in rows),definitive_Support_category_at_Dual_preconsumption_gate='NOT_DEMONSTRATED: native type16 remains Monster; original adapter final ledger agrees; no handoff bonus qualification implied.',rows=rows)
(a.logs/'TRACE_INVARIANTS.json').write_text(json.dumps(result,indent=2)+'\n');print(result['profiles'],result['physical_PASS'],result['quota_PASS'])
