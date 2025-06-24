#!/usr/bin/env python3
import csv
import sys
import re
from collections import defaultdict

PAIR_RE = re.compile(r'\{\s*(-?\d+)\s*,\s*(-?\d+)\s*\}')

def parse_pairs(cells):
    """Given list of 7 strings like '{1,0}', returns flat tuple of 14 ints."""
    exps = []
    for i, cell in enumerate(cells, 1):
        m = PAIR_RE.match(cell.strip())
        if not m:
            sys.exit(f"Error parsing column {i+1}: expected '{{num,den}}', got {cell!r}")
        exps.extend([int(m.group(1)), int(m.group(2))])
    return tuple(exps)

def to_camel(s: str) -> str:
    parts = re.split(r'[^0-9A-Za-z]+', s)
    return ''.join(p.capitalize() for p in parts if p)

def macro_for(q: str) -> str:
    return f'kSIQuantity{to_camel(q)}'

def read_rows(path):
    rows = []
    with open(path, newline='', encoding='utf-8') as f:
        rdr = csv.reader(f)
        for lineno, row in enumerate(rdr, 1):
            if not row or not row[0].strip():
                continue
            if len(row) != 8:
                sys.exit(f"Line {lineno}: expected 8 columns, got {len(row)}")
            qty = row[0].strip()
            exps = parse_pairs(row[1:])
            rows.append((qty, exps))
    return rows

def group_by_exponents(rows):
    d = defaultdict(list)
    for qty, exps in rows:
        d[exps].append(qty)
    return d

def emit(groups):
    indent = '    '
    print("void DimensionalityLibraryBuild() {")
    print(f"{indent}dimLibrary = OCDictionaryCreateMutable(0);")
    print(f"{indent}dimQuantitiesLibrary = OCDictionaryCreateMutable(0);")
    print(f"{indent}SIDimensionalityRef dim;")
    print()
    for exps, qtys in groups.items():
        first = qtys[0]
        print(f"#pragma mark {macro_for(first)}")
        if len(qtys) > 1:
            others = ', '.join(macro_for(q) for q in qtys[1:])
            print(f"{indent}// {others}")
        nums = ', '.join(str(e) for e in exps)
        print(f"{indent}dim = AddDimensionalityToLibrary({nums});")
        for q in qtys:
            print(f"{indent}OCDictionaryAddValue(dimQuantitiesLibrary, {macro_for(q)}, dim);")
        print()
    print("}")
    
def main():
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} SIDimensionality.csv", file=sys.stderr)
        sys.exit(1)
    rows = read_rows(sys.argv[1])
    groups = group_by_exponents(rows)
    emit(groups)

if __name__ == '__main__':
    main()
