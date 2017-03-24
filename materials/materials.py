#!/usr/bin/env python
import re

def break_formula(formula):
    """
    Breaks a molecular formula into its constituent elements.  Returns a
    dictionary of element keys and a normalized formula
    """
    # Ignore the first value in the split, because it will be blank from trying
    # to split on the first element symbol.
    parts = [re.split('([0-9]+)', test) for test in
             re.findall('[A-Z][a-z]?[0-9]*', formula)]
    values = [int(part[1]) if len(part) > 1 else 1 for part in parts]
    elements = [part[0] for part in parts]
    total = sum((float(v) for v in values))
    return {k:float(v) / total for k, v in zip(elements, values)}

def load_materials(filename):
    """
    Loads a Gray Materials file, typically called Gray_Materials.txt.  Returns
    a dictionary of the materials, where the key is the material name and the
    value is a dict containing:

    =========== ======  ====================================================
     Field       Type    Description
    =========== ======  ====================================================
    formula     str     The chemical formula for the material
    density     float   Density of the material (g/cm^3)
    sensitive   bool    If it should be considered as a detector by Gray
    composition dict    Normalized formula.  Keys: elements Value: fraction
    =========== ======  ====================================================

    """
    materials = {}
    with open(filename, 'r') as fid:
        for line in fid.read().splitlines():
            # Ignore blank lines and lines that start with a #
            if len(line) == 0:
                continue
            elif line[0] == '#':
                continue
            # Assume five columns, separated by spaces  The first three are
            # relevant to this script
            name, formula, density, sensitive = line.split()
            materials[name] = {'formula': formula,
                               'density': float(density),
                               'sensitive': bool(sensitive),
                               'composition': break_formula(formula)}
    return materials
