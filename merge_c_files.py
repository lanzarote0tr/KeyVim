#!/usr/bin/env python3
import re
from pathlib import Path

# match   #include "foo.h"
STD_INC   = re.compile(r'^\s*#include\s+"([^"]+)\.h"\s*$')
# match   :#include "foo.h":   (delete-only)
COLON_INC = re.compile(r'^\s*:\s*#include\s+"[^"]+\.h"\s*:\s*$')

src      = Path("main.c")
out_file = Path("merged.c")

with src.open() as fin, out_file.open("w") as fout:
    for line in fin:
        if COLON_INC.match(line):          # drop :#include ...:
            continue

        m = STD_INC.match(line)
        if not m:                          # ordinary line
            fout.write(line)
            continue

        name      = m.group(1)             # "foo"
        c_path    = src.with_name(name + ".c")
        self_inc  = re.compile(rf'^\s*#include\s+"{re.escape(name)}\.h"\s*$')

        fout.write(f"/* begin {c_path.name} */\n")
        with c_path.open() as cf:
            for cl in cf:
                if self_inc.match(cl) or COLON_INC.match(cl):
                    continue               # skip self-include & colon-includes
                fout.write(cl)
        fout.write(f"\n/* end {c_path.name} */\n")

