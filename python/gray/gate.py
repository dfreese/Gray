import re
import gray

def remove_whitespace(s):
    """
    https://stackoverflow.com/a/3739939/2465202
    """
    return ''.join(s.split())

def parse_density_unit_str(value):
    value = remove_whitespace(value)

    # order of the find is important, because find can work for
    # multiple units
    allowed_units = (('mg/cm3', 1e-3), ('g/cm3', 1.0),
                     ('mg/ml', 1e-3), ('g/ml', 1.0))
    for unit, scale in allowed_units:
        unit_idx = value.find(unit)
        if not unit_idx < 0:
            return scale * float(value[:unit_idx])
    raise ValueError('Unrecognized density unit: {0}'.format(value))

def parse_density_molar_mass_str(value):
    value = value.replace(' ', '').replace('\t', '')

    # order of the find is important, because find can work for
    # multiple units
    allowed_units = (('mg/mole', 1e-3), ('g/mole', 1.0),
                     ('mg/mol', 1e-3), ('g/mol', 1.0))
    for unit, scale in allowed_units:
        unit_idx = value.find(unit)
        if not unit_idx < 0:
            return scale * float(value[:unit_idx])
    raise ValueError('Unrecognized molar mass unit: {0}'.format(value))

class Element(object):
    def __init__(self, line):
        lidx = 0
        ridx = line.find(':')
        self.name = line[lidx:ridx]

        # Find the symbol S
        lidx = line.find('S', ridx)
        lidx = line.find('=', lidx)
        ridx = line.find(';', lidx)
        self.symbol = line[lidx + 1:ridx].strip()

        # Now find Z
        lidx = line.find('Z', ridx)
        lidx = line.find('=', lidx)
        ridx = line.find(';', lidx)
        self.Z = int(float(line[lidx + 1:ridx].strip()))

        lidx = line.find('A', ridx)
        lidx = line.find('=', lidx)
        self.awg = parse_density_molar_mass_str(line[lidx + 1:])

_material_line_state = re.compile('(.+):d=(.+);n=(.+);state=(.+)')
_material_line = re.compile('(.+):d=(.+);n=(.+)')
_element_line = re.compile('\+el:name=(.+);([n,f])=(.+)')

class Material(object):
    def __init__(self, lines):
        m = re.match(_material_line_state, remove_whitespace(lines[0]))
        if m is None:
            self.state = None
            m = re.match(_material_line, remove_whitespace(lines[0]))
            if m is None:
                raise ValueError(
                    '"{}" is not a valid Material'.format(lines[0]))
        else:
            self.state = m.group(4)

        self.name = m.group(1)
        self.index = -1
        self.density = parse_density_unit_str(m.group(2))
        self.no_elements = int(m.group(3))

        self.elements = {}
        self.description = None
        for line in lines[1:]:
            m = re.match(_element_line, remove_whitespace(line))
            if m is None:
                continue
            name = m.group(1)
            if name == 'auto':
                name = self.name

            if m.group(2) == 'n':
                if self.description is None:
                    self.description = 'composition'
                elif self.description != 'composition':
                    raise ValueError(
                        '{} description is not consistent'.format(self.name))
                val = int(m.group(3))
            elif m.group(2) == 'f':
                if self.description is None:
                    self.description = 'massfraction'
                elif self.description != 'massfraction':
                    raise ValueError(
                        '{} description is not consistent'.format(self.name))
                val = float(m.group(3))
            self.elements[name] = val

        if len(self.elements) != self.no_elements:
            raise RuntimeError(
                'No elements for {} does not match listed'.format(self.name))

class Database(object):
    def __init__(self, filename):
        lines = open(filename, 'r').read().splitlines()
        element_start = -1
        material_start = -1
        for idx, line in zip(range(len(lines)), lines):
            if line.find('[Elements]') != -1:
                element_start = idx
            if line.find('[Materials]') != -1:
                material_start = idx
        element_lines = lines[element_start + 1:material_start - 1]
        self._elements = (Element(line) for line in element_lines)
        self._elements = {e.name:e for e in self._elements}
        self._z_lookup = {e.Z:e.name for e in self._elements.values()}
        self._sym_lookup = {e.symbol:e.name for e in self._elements.values()}

        # Use lines that we identify as materials to split up the Materials
        # section of the database.
        self._materials = {}
        mat_start = None
        mat_index = 0
        for idx in range(material_start, len(lines)):
            m = re.match(_material_line, remove_whitespace(lines[idx]))
            if m is not None:
                if mat_start is None:
                    mat_start = idx
                    # First one, so we dont know where it ends yet.  Wait until
                    # the second material is found to process the first.
                    continue
                mat = Material(lines[mat_start:idx])
                mat.index = mat_index
                mat_index += 1
                self._materials[mat.name] = mat
                mat_start = idx
        # Handle the last material found
        mat = Material(lines[mat_start:])
        self._materials[mat.name] = mat
        mat.index = mat_index

        # Make sure all of the elements are described
        for material in self._materials.values():
            for element in material.elements.keys():
                if element not in self._elements:
                    raise ValueError(
                        'Element "{0}" listed in "{1}" not specified'.format(
                            element, material.name))
            material.elements = {
                self._elements[e].Z: v for e, v in material.elements.items()}

    def element(self, name_sym_z):
        if name_sym_z in self._z_lookup:
            return self._elements[self._z_lookup[name_sym_z]]
        elif name_sym_z in self._sym_lookup:
            return self._elements[self._sym_lookup[name_sym_z]]
        else:
            return self._elements[name_sym_z]

    def material(self, name):
        return self._materials[name]

    def items(self):
        for k, v in self._materials.items():
            yield k, v

    def keys(self):
        for k in self._materials.keys():
            yield k

    def values(self):
        for v in self._materials.values():
            yield v

def database_epdl(filename, energy_lo=None, energy_hi=None):
    '''
    Uses gray.gate.Database and the epdl module to parse a GATE style materials
    database and then generate the necessary information
    '''
    import epdl
    data = gray.Database(filename)
    epdl_data = {}
    for name, mat in data.items():
        if mat.description == 'composition':
            epdl_data[name] = epdl.material_by_composition(
                mat.elements, energy_lo, energy_hi)
        elif mat.description == 'massfraction':
            epdl_data[name] = epdl.material_by_mass_fraction(
                mat.elements, energy_lo, energy_hi)
        epdl_data[name].density = mat.density
        epdl_data[name].index = mat.index
    return epdl_data
