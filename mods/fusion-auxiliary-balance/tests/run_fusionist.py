#!/usr/bin/env python3
import subprocess,tempfile,os
from pathlib import Path
root=Path(__file__).resolve().parent.parent
source=(root/'mod.c').read_text()
def function(name):
    at=source.index(name+'(');start=source.rfind('\nstatic ',0,at)+1
    brace=source.index('{',at);depth=1;end=brace+1
    while depth:
        depth+=(source[end]=='{')-(source[end]=='}');end+=1
    return source[start:end]+'\n'
with tempfile.TemporaryDirectory() as tmp:
    inc=Path(tmp)/'finalize_under_test.h'
    inc.write_text(function('aux_fusion_special')+function('aux_card_finalize'))
    binary=Path(tmp)/'runtime-test'
    subprocess.run(['gcc','-std=c11','-O1','-g','-fsanitize=address,undefined',
        '-I',tmp,str(root/'tests/test_fusionist_runtime.c'),'-o',str(binary)],check=True)
    subprocess.run([str(binary)],check=True,env={**os.environ,"ASAN_OPTIONS":"detect_leaks=0"})
