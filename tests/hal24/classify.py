"""Classify existing HAL24 results by executed worker; never modify test outcomes."""
from pathlib import Path
import argparse,collections,json
p=argparse.ArgumentParser(description=__doc__);p.add_argument('results',type=Path);p.add_argument('output',type=Path);args=p.parse_args()
r=json.loads(args.results.read_text());groups=collections.defaultdict(list)
for q in r['profiles']:
 first=next((s for s in q['observations'] if s.startswith('ACTION first ')),'')
 bonus=next((s for s in q['observations'] if s.startswith('ACTION bonus ')),'')
 failures=[c for c in q['checks'] if c['status']=='FAIL']
 if failures and 'staged=1 ' in first and 'gate=0 rd=1 ai=0 ' in first:group='I1_RD_missed_gate'
 elif failures and ('gate=0 rd=0 ai=1 ' in first or 'gate=0 rd=0 ai=1 ' in bonus):group='I2_AI_external_separate'
 elif failures:group='UNCLASSIFIED_FAILURE'
 else:group='NO_FAILURE_EXERCISED'
 groups[group].append(q)
summary={g:{'processes':len(qs),'checks':dict(collections.Counter(c['status'] for q in qs for c in q['checks'])),'cases':[q['case']+':'+q['phase'] for q in qs]} for g,qs in groups.items()}
args.output.write_text(json.dumps({'method':'Read-only classification from actual worker traces and failed invariants; I2 is not counted as RD I1. Does not modify results.','groups':summary},indent=2)+'\n')
print({g:{k:v for k,v in data.items() if k!='cases'} for g,data in summary.items()})
raise SystemExit('UNCLASSIFIED_FAILURE' in groups)
