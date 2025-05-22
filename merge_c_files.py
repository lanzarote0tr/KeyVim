# merge_clean_with_test_comments.py
import re

files = ["main.c", "helper.c", "inputhandler.c"]
included_headers = set()
output = []

def inline_includes(lines):
    result = []
    for line in lines:
        match = re.match(r'#include\s+"(.+)"', line)
        if match:
            hdr = match.group(1)
            if hdr not in included_headers:
                included_headers.add(hdr)
                with open(hdr) as f:
                    result += inline_includes(f.readlines())
        else:
            result.append(line)
    return result

def remove_test_main(lines):
    new_lines = []
    in_main = False
    brace_depth = 0
    for line in lines:
        if not in_main:
            if re.match(r'\s*int\s+main\s*\(', line):
                in_main = True
                brace_depth = 0
                continue
            new_lines.append(line)
        else:
            brace_depth += line.count('{') - line.count('}')
            if brace_depth <= 0:
                in_main = False
    return new_lines

def comment_test_blocks(lines):
    commented = []
    in_test_block = False
    for line in lines:
        if '/* TEST_START */' in line:
            in_test_block = True
            commented.append('/* TEST_START */\n')
            continue
        elif '/* TEST_END */' in line:
            in_test_block = False
            commented.append('/* TEST_END */\n')
            continue

        if in_test_block:
            commented.append('/* ' + line.rstrip('\n') + ' */\n')
        else:
            commented.append(line)
    return commented

for fname in files:
    with open(fname) as f:
        lines = f.readlines()
        lines = inline_includes(lines)
        lines = remove_test_main(lines)
        lines = comment_test_blocks(lines)
        output += lines

with open("merged.c", "w") as f:
    f.writelines(output)

