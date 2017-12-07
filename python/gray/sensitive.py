
def read_sensitive(lines):
    ret = set()
    for line in lines:
        comment = line.find('#')
        if comment < 0:
            comment = len(line)
        line = line[:comment]
        if line:
            ret.add(line)
    return ret


def read_sensitive_file(filename):
    with open(filename, 'r') as fid:
        lines = fid.read().splitlines()
    return read_sensitive(lines)

def mark_sensitve(materials, sensitive):
    for mat in materials.values():
        mat['sensitive'] = False
    for mat in sensitive:
        if mat in materials:
            materials[mat]['sensitive'] = True
        else:
            raise KeyError('''{0} given as sensitive material does not exist
                           in the materials database'''.format(mat))
