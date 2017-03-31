#!/usr/bin/env python
import math

def read_float(v):
    """
    Convert ENDF6 string to float
    (the ENDF6 float representation omits the e for exponent and may contain
     blanks)
    """
    try:
        val = float(v[0] +
                    v[1:].replace(' ', '').replace('+', 'e+').replace('-', 'e-'))
    except ValueError:
        val = None
    return val

def parse_int(v):
    """Convert ENDF6 string to int"""
    return int(read_float(v))

_data_slices = tuple((slice(x, x + 11) for x in range(0, 66, 11)))

def parse_data(line):
    """Read first 6*11 characters of a line as floats"""
    return [read_float(line[s]) for s in _data_slices]

_head_funcs = zip(_data_slices, (parse_int, read_float, int, int, int, int))

def parse_head_record(line):
    """
    Takes a string of a HEAD record in the ENDF format and transforms it into a
    list of values with types based on the six record locations.

    Parameters
    ----------
    line : str
        a line representing an ENDF HEAD record.

    Returns
    ----------
    values : list
        A list containing the six values in the head record ZA (int), AWR
        (float), L1 (int), L2 (int), N1 (int), N2 (int).
    """
    return [f(line[s]) for s, f in _head_funcs]

_cont_funcs = zip(_data_slices, (read_float, read_float, int, int, int, int))

def parse_cont_record(line, skipC=False):
    """
    Takes a string of a CONT record in the ENDF format and transforms it into a
    list of values with types based on the six/four record locations.

    Parameters
    ----------
    line : str
        a line representing an ENDF CONT record.
    skipC : str, default=False
        Skips the first two records C1 and C2, which can be neglected by the
        format.

    Returns
    ----------
    values : list
        A list containing the six values in the cont record C1 (float), C2
        (float), L1 (int), L2 (int), N1 (int), N2 (int).  Four values L1, L2,
        N1, N2 are returned if skipC is True.
    """
    if skipC:
        return [f(line[s]) for s, f in _cont_funcs[2:]]
    else:
        return [f(line[s]) for s, f in _cont_funcs]

def parse_list_record(lines):
    """
    Takes a list of lines, starting with the beginning of a LIST record and
    parses it by reading the first CONT control record and then
    the subsequent LIST records, using readline() to process through the file.

    Parameters
    ----------
    lines : list of str
        a set of lines with the list record starting at lines[0] and being
        fully contained in lines.

    Returns
    ----------
    items : list
        output of get_cont_record() on the first CONT record line.
    itemsList : list
        generally 6 floats for each line in the list.
    no_lines : int
        number of lines in the list
    """
    # determine how many items are in list
    items = parse_cont_record(lines[0])
    NPL = items[4]
    # read items
    itemsList = []
    m = 0
    no_lines = (NPL-1)//6 + 1
    for i in range(1, no_lines + 1):
        line = lines[i]
        data = parse_data(lines[i])
        toRead = min(6, NPL-m)
        for val in data[:toRead]:
            itemsList.append(val)
        m = m + toRead
    return items, itemsList, no_lines

def parse_tab1_record(lines):
    """
    Takes a list of lines, starting with the beginning of a LIST record and
    parses it by reading the first CONT control record and then
    the subsequent LIST records, using readline() to process through the file.

    Parameters
    ----------
    lines : list of str
        a set of lines with the list record starting at lines[0] and being
        fully contained in lines.

    Returns
    ----------
    items : list
        output of get_cont_record() on the first CONT record line.
    itemsList : list
        generally 6 floats for each line in the list.
    no_lines : int
        number of lines in the list
    """
    # determine how many items are in list
    items = parse_cont_record(lines[0])
    NR = items[4]
    NP = items[5]

    # Read interpolation values
    NBT = []
    INT = []
    m = 0
    no_lines = (NR - 1) // 3 + 1
    for i in range(1, no_lines + 1):
        data = parse_data(lines[i])
        toRead = min(6, (NR - m) * 2)
        for xval, yval in zip(data[:toRead:2], data[1:toRead:2]):
            NBT.append(xval)
            INT.append(yval)

    # Read 1-d paired list
    x = []
    y = []
    m = 0
    start_line = 1 + no_lines
    no_lines = (NP - 1) // 3 + 1
    for i in range(start_line, start_line + no_lines):
        data = parse_data(lines[i])
        toRead = min(6, (NP - m) * 2)
        for xval, yval in zip(data[:toRead:2], data[1:toRead:2]):
            x.append(xval)
            y.append(yval)
        m = m + toRead / 2
    no_lines_total = start_line + no_lines
    return items, NBT, INT, x, y, no_lines_total

# Used for MF = 23, Photon Interaction Cross Sections
_photon_cross_section_types = {
    501: 'total',
    502: 'coherent_scattering',
    504: 'incoherent_scattering',
    515: 'pp_electron',
    516: 'pp_total',
    517: 'pp_nuclear',
    522: 'total_photoionization',
    534: 'K', 535: 'L1', 536: 'L2', 537: 'L3', 538: 'M1', 539: 'M2',
    540: 'M3', 541: 'M4', 542: 'M5', 543: 'N1', 544: 'N2', 545: 'N3',
    546: 'N4',547: 'N5', 548: 'N6', 549: 'N7', 550: 'O1', 551: 'O2',
    552: 'O3', 553: 'O4', 554: 'O5', 555: 'O6', 556: 'O7', 559: 'P1',
    560: 'P2', 561: 'P3', 570: 'Q1'
}

def parse_cross_section(lines):
    (ZA, AWR) = parse_head_record(lines[0])[:2]
    tab1_out = parse_tab1_record(lines[1:])
    binding_energy, fluorescene_yield = tab1_out[0][:2]
    energy, xsection = tab1_out[3:5]
    return ZA, AWR, binding_energy, fluorescene_yield, energy, xsection

def parse_cross_sections(lines_dict):
    """
    Processes the cross section information of an isotope (ENDF format MF=23,
    MT=5XX) and returns a dict of dicts, each with the with at least the
    following fields:

    =================  ====================================================
      Field              Description
    =================  ====================================================
    energy             The energy at which the factor was evaluated (eV)
    xsec               The cross section value (barns)
    =================  ====================================================

    MT values 534 and higher represent subshell cross sections.  These are put
    into a separate dict, under the 'shells' key.  Inside 'subshells' each
    there is a dict under the subshell's name (i.e. 'K') that additionally has
    the following fields:

    =================  ====================================================
      Field              Description
    =================  ====================================================
    binding_energy     The binding energy of the subshell (eV)
    fluorescene_yield  The fluorescene yield of the subshell if any (eV)
    =================  ====================================================

    The keys of the main dict recturned are set by the allowable MT numbers,
    which are:
    === ===========================================
    MT  Description
    === ===========================================
    501 Total cross sections
    502 Coherent scattering cross sections
    504 Incoherent scattering cross sections
    515 Pair production cross sections, Electron field
    516 Pair production cross sections, Total
    517 Pair production cross sections, Nuclear field
    522 Total photoionization cross section
    534 K subshell photoionization
    535 L1 subshell photoionization
    536 L2 subshell photoionization
    537 L3 subshell photoionization
    538 M1 subshell photoionization
    539 M2 subshell photoionization
    540 M3 subshell photoionization
    541 M4 subshell photoionization
    542 M5 subshell photoionization
    543 N1 subshell photoionization
    544 N2 subshell photoionization
    545 N3 subshell photoionization
    546 N4 subshell photoionization
    547 N5 subshell photoionization
    548 N6 subshell photoionization
    549 N7 subshell photoionization
    550 O1 subshell photoionization
    551 O2 subshell photoionization
    552 O3 subshell photoionization
    553 O4 subshell photoionization
    554 O5 subshell photoionization
    555 O6 subshell photoionization
    556 O7 subshell photoionization
    559 P1 subshell photoionization
    560 P2 subshell photoionization
    561 P3 subshell photoionization
    570 Q1 subshell photoionization
    === ===========================================

    These are listed in _photon_cross_section_types along with the key they are
    given in the returned data format

    Parameters
    ----------
    lines_dict : dict(int:list[str])
        A dict with the keys being the MT values as ints, and the values being
        a list of strings with all of the lines associated with that MF/MT
        combination.

    Returns
    -------
    data : dict of dicts
        parameters described above

    """
    data = {'shells': {}}
    for mt, mt_lines in lines_dict.iteritems():
        if mt not in _photon_cross_section_types:
            continue
        xsection_out = parse_cross_section(mt_lines)
        if mt < 534:
            data[_photon_cross_section_types[mt]] = {'energy': xsection_out[4],
                                                     'xsec': xsection_out[5]}
        else:
            data['shells'][_photon_cross_section_types[mt]] = {
                'binding_energy': xsection_out[2],
                'fluorescene_yield': xsection_out[3],
                'energy': xsection_out[4],
                'xsec': xsection_out[5]}
    return data

# Used for MF = 27, Atomic Form Factors or Scattering Functions
_photon_form_factor_types = {
    502: 'coherent_scattering',
    504: 'incoherent_scattering',
    505: 'imaginary_anomalous_scattering',
    506: 'real_anomalous_scattering',
}

def parse_form_factor(lines):
    (ZA, AWR) = parse_head_record(lines[0])[:2]
    tab1_out = parse_tab1_record(lines[1:])
    Z = tab1_out[0][1]
    energy, factor = tab1_out[3:5]
    return ZA, AWR, Z, energy, factor

def parse_form_factors(lines_dict):
    """
    Processes the form factor information of an isotope (ENDF format MF=27,
    MT=502, 504, 505, 506) and returns a dict of dicts, each with the with the
    following fields:

    =================  ====================================================
      Field              Description
    =================  ====================================================
    Z                  The atomic number of the element
    energy             The energy at which the factor was evaluated
    factor             The atomic symbol of the element
    =================  ====================================================

    The keys of the main dict recturned are set by the allowable MT numbers,
    which are:
    === ===========================================
    MT  Description
    === ===========================================
    502 Coherent Scattering Form Factors
    504 Incoherent Scattering Functions
    505 Imaginary Anomalous Scattering Form Factors
    506 Real Anomalous Scattering Form Factors
    === ===========================================

    These are listed in _photon_form_factor_types along with the key they are
    given in the returned data format

    Parameters
    ----------
    lines_dict : dict(int:list[str])
        A dict with the keys being the MT values as ints, and the values being
        a list of strings with all of the lines associated with that MF/MT
        combination.

    Returns
    -------
    data : dict of dicts
        parameters described above

    """
    data = {}
    for mt, mt_lines in lines_dict.iteritems():
        if mt not in _photon_form_factor_types:
            continue
        ff_out = parse_form_factor(mt_lines)
        data[_photon_form_factor_types[mt]] = {
            'Z': ff_out[2],
            'energy': ff_out[3],
            'factor': ff_out[4]}
    return data

def parse_header(lines):
    """
    Processes the information in the header of the element (ENDF format MF=1,
    MT=451) and returns a dict with the following fields:

    =================  ====================================================
      Field              Description
    =================  ====================================================
    temperature        Temperature of the measurement
    atomic_number      The atomic number of the element
    symbol             The atomic symbol of the element
    ZA                 The ZA number, deliniating isotopes = Z * 1000 + A
    isomeric_state     Unknown
    state              Unknown
    mass               Atomic mass of the element
    fissionable        Boolean if the element is fissile
    stable             Boolean if the element is stable
    excitation_energy  The excitation energy used in the experiment
    =================  ====================================================

    Processes the following records HEAD, CONT, CONT, CONT, n * TEXT, CONT,
    CONT.  The number of TEXT records is defined in the previous control line.

    More information is processed from the text fields than is returned.
    """
    # First HEAD record
    items = parse_head_record(lines[0])
    target = {}
    target['ZA'] = items[0]
    target['mass'] = items[1]
    target['fissionable'] = (items[3] == 1)

    # Control record 1
    items = parse_cont_record(lines[1])
    target['excitation_energy'] = items[0]
    target['stable'] = (int(items[1]) == 0)
    target['state'] = items[2]
    target['isomeric_state'] = items[3]
    file_format = items[5]
    assert file_format == 6

    # Ignore Control record 2
    # Control record 3
    items = parse_cont_record(lines[3])
    target['temperature'] = items[0]
    NWD = items[4]
    NXC = items[5]

    # Parse some information out of the text description
    target['symbol'] = lines[4][4:6].split()[0]
    target['atomic_number'] = int(lines[4][0:3])
    # ignore remaining description information
    return target

def parse_atomic_relaxation(lines):
    """
    Processes ENDF format MF=28, MT=533 into its constituent transitions.  This
    consists of a HEAD record, which gives the number of subshells, as well as
    the ZA number and AWR, which are current pulled from the info/header
    sections.  Then each shell with transitions has LIST record.  This is
    returned as a dictionary shells, the name of the shell being associate with
    a dictionary containing 'binding_energy', 'number_electrons', and
    'transitions', which are a float, float, and a tuple, respectively.  See
    the EADL class __getitem__ description for more information.
    """
    # Read HEAD record
    params = parse_head_record(lines[0])
    n_subshells = params[4]

    # Read list of data
    atomic_relaxation = {}
    subshells = {1: 'K', 2: 'L1', 3: 'L2', 4: 'L3', 5: 'M1',
                 6: 'M2', 7: 'M3', 8: 'M4', 9: 'M5', 10: 'N1',
                 11: 'N2', 12: 'N3', 13: 'N4', 14: 'N5', 15: 'N6',
                 16: 'N7', 17: 'O1', 18: 'O2', 19: 'O3', 20: 'O4',
                 21: 'O5', 22: 'O6', 23: 'O7', 24: 'O8', 25: 'O9',
                 26: 'P1', 27: 'P2', 28: 'P3', 29: 'P4', 30: 'P5',
                 31: 'P6', 32: 'P7', 33: 'P8', 34: 'P9', 35: 'P10',
                 36: 'P11', 37: 'Q1', 38: 'Q2', 39: 'Q3', 0: None}
    start_line = 1
    for i in range(n_subshells):
        params, list_items, no_lines = parse_list_record(lines[start_line:])
        # add in the CONT lines and the LIST records lines
        start_line += (1 + no_lines)
        subi = subshells[int(params[0])]
        n_transitions = int(params[5])
        ebi = list_items[0]
        eln = list_items[1]
        data = {'binding_energy': ebi, 'number_electrons': eln, 'transitions': []}
        for j in range(1, n_transitions + 1):
            subj = subshells[int(list_items[6 * j])]
            subk = subshells[int(list_items[6 * j + 1])]
            etr = list_items[6 * j + 2]
            ftr = list_items[6 * j + 3]
            data['transitions'].append((subj, subk, etr, ftr))
            atomic_relaxation[subi] = data
    # Skip SEND record
    return {'shells': atomic_relaxation}

_line_process = {
    'MAT' : (slice(66,70), int),
    'MF'  : (slice(70,72), int),
    'MT'  : (slice(72,75), int),
    'line': (slice(75,80), int),
    'content' : (slice(0,66), str),
    }

def parse_line(l):
    return dict(zip(_line_process.iterkeys(),
                    (s[1](l[s[0]]) for s in _line_process.itervalues())))

def parse_endf_format(filename):
    with open(filename, 'r') as fid:
        lines = fid.read().splitlines()
    data = {}
    for line in lines:
        parsed_line = parse_line(line)
        mat = parsed_line['MAT']
        mf = parsed_line['MF']
        mt = parsed_line['MT']
        if mat == 0 or mf == 0 or mt == 0:
            continue
        if mat not in data:
            data[mat] = {}
        if mf not in data[mat]:
            data[mat][mf] = {}
        if mt not in data[mat][mf]:
            data[mat][mf][mt] = []
        data[mat][mf][mt].append(parsed_line['content'])
    return data

def recursive_update(d, u):
    from collections import Mapping
    for k, v in u.items():
        # this condition handles the problem
        if not isinstance(d, Mapping):
            d = u
        elif isinstance(v, Mapping):
            d[k] = recursive_update(d.get(k, {}), v)
        else:
            d[k] = u[k]
    return d

def parse_endf(filename, pull_header=True):
    data = parse_endf_format(filename)
    parsed_data = {}
    for mat, mat_data in data.iteritems():
        parsed_data[mat] = {}
        if 1 in mat_data and pull_header:
            if 451 in mat_data[1]:
                header_data = parse_header(mat_data[1][451])
                parsed_data[mat].update(header_data)
        if 23 in mat_data:
            xsections = parse_cross_sections(mat_data[23])
            parsed_data[mat].update(xsections)
        if 27 in mat_data:
            form_fact = parse_form_factors(mat_data[27])
            parsed_data[mat].update(form_fact)
        if 28 in mat_data:
            if 533 in mat_data[28]:
                atomic_relax = parse_atomic_relaxation(mat_data[28][533])
                parsed_data[mat].update(atomic_relax)
    return parsed_data

class EADL(object):
    """
    Parsing EADL information in a ENDF material evaluation with multiple
    files/sections.

    The EADL class provides a means to parse data from an ENDF-6 format
    file and access it as stored internal Python objects. The class is able to
    process files with the following formats:
    parsing capabilities is as follows:

    == === ==============================
    MF MT  Description
    == === ==============================
    1  451 Descriptive data and directory
    28 533 Atomic relaxation data
    == === ==============================

    All other formats are ignored.  This is meant to process the data found in
    the full Livermore Evaulated Attribute Data Library (EADL) that can be
    found here:

    This file should contain all of the emission data for all atoms from an
    excitation.  This class is based entirely off of the work done by PyNE:
    the Nuclear Engineering Toolkit, in the evaluation class, found here:

    https://github.com/pyne/pyne/blob/master/pyne/endf.pyx

    This class was built with processing the full EADL library file available
    here (8.6Mb):

    https://www-nds.iaea.org/epdl97/data/endfb6/eadl/eadl.all

    Much of the information on the ENDF-6 format can be found in the user
    manual, found here:

    https://www.bnl.gov/isd/documents/70393.pdf

    """

    def __init__(self, filename, url=None):
        """
        Creates a EADL class by opening a file named filename.  Each atom found
        in the ENDF formatted file is placed into an _atoms attribute of the
        class, which can be accessed using the [] operator.   If the filename
        given doesn't exist, it will attempt to download it from url.  If url
        is None, the default is attempted.

        Parameters
        ----------
        filename : str
            a filename string pointing to the EADL file.
        url : str, default in class description
            the url pointing to the full EADL library
        """
        self._data = parse_endf(filename)
        self._elements = dict()
        self._num_to_symbol = dict()
        for mat, mat_data in self._data.iteritems():
            self._elements[mat_data['symbol']] = mat
            self._num_to_symbol[mat_data['atomic_number']] = mat_data['symbol']

    def read(self, filename):
        new_data = parse_endf(filename, pull_header=False)
        self._data = recursive_update(self._data, new_data)

    def __getitem__(self, key):
        """
        Returns the dict associated with the element specified by the key.  The
        key can be the atomic symbol or number.  The provided class should have
        at least the property 'shells'.  Generally speaking it will contain the
        following information:

        =================  ====================================================
          Field              Description
        =================  ====================================================
        temperature        Temperature of the measurement
        shells             A dict of shells and their emissions
        atomic_number      The atomic number of the element
        symbol             The atomic symbol of the element
        ZA                 The ZA number, deliniating isotopes = Z * 1000 + A
        isomeric_state     Unknown
        state              Unknown
        mass               Atomic mass of the element
        fissionable        Boolean if the element is fissile
        stable             Boolean if the element is stable
        excitation_energy  The excitation energy used in the experiment
        =================  ====================================================

        Each of the shells listed in shells will have a dict associated with it
        which has the following fields:

        =================  ====================================================
          Field              Description
        =================  ====================================================
        binding_energy     Binding energy for subshell (eV)
        number_electrons   Number of electrons in subshell when neutral
        transitions        A list tuples for each transistion from that state
        =================  ====================================================

        The transistion tuple has the following fields:

        =========  ============================================================
          Index      Description
        =========  ============================================================
        0          The subshell from which an electron decays
        1          The subshell from which an electron is ejected (Is
                   None if the transistion is radiative)
        2          Energy of the transition in eV
        3          Fractional probability of transition
        =========  ============================================================

        Parameters
        ----------
        key : str or int
            The atomic symbol or number of the element to get information on.

        Raises
        ------
        KeyError
            If the element was not found, or never loaded.
        """
        if isinstance(key, int):
            if key in self._num_to_symbol:
                key = self._num_to_symbol[key]
            else:
                raise IndexError('Unknown atomic number: {}'.format(key))
        if key in self._elements:
            return self._data[self._elements[key]]
        else:
            raise KeyError('Unknown atomic name')

    def __iter__(self):
        """
        Implement iteration through the elements in order of atomic number.
        """
        sorted_keys = self._num_to_symbol.keys()
        sorted_keys.sort()
        for number in sorted_keys:
            yield self._elements[self._num_to_symbol[number]]

    def iteritems(self):
        """
        Implement iteration through the elements in order of atomic number.
        """
        sorted_keys = self._num_to_symbol.keys()
        sorted_keys.sort()
        for number in sorted_keys:
            yield (self._num_to_symbol[number],
                   self._elements[self._num_to_symbol[number]])

def no_holes(t):
    n = 0
    for el_dict in t.itervalues():
        for shell_dict in el_dict.itervalues():
            for trans_id in shell_dict.iterkeys():
                n += len(trans_id[1])
    return n

def calculate_shell_cascades(elements, round_to = 1000.0, threshold = 15000.0):
    """Calculates the unique emissions cascades that are possible from all of
    the subshells of the different based on an EADL class elements
    """
    # For each subshell, in each element, we have a dict of transitions. The
    # key at the subshell level is a tuple of tuples, the first being the
    # emissions, the second tuple being the holes.
    transitions = {}
    for element_name, element_dict in elements.iteritems():
        new_element_dict = {}
        transitions[element_name] = new_element_dict
        for shell_name, shell_info in elements[element_name]['shells'].iteritems():
            # We have crosssection information on some shells that don't have
            # transition information.
            if 'transitions' not in shell_info:
                continue
            shell_dict = {}
            new_element_dict[shell_name] = shell_dict
            for transition in shell_info['transitions']:
                transition_prob = transition[3]
                if transition[1] is None:
                    # Round the emissions to whatever the user specifies
                    emissions = (transition[2] // round_to * round_to,)
                    # And drop any transistion under the threshold
                    if emissions[0] < threshold:
                        emissions = ()
                    holes = (transition[0],)
                else:
                    emissions = ()
                    holes = (transition[0], transition[1],)
                transition_id = (emissions, holes)
                if transition_id in shell_dict:
                    shell_dict[transition_id] += transition_prob
                else:
                    shell_dict[transition_id] = transition_prob

    # The simplest way to do things is start from the outer shells and work in
    # so that we can trim the tree backwards rather than finding every
    # combination and then sorting out if it's radiative or not.  These are all
    # of the possible shell combinations from EADL, from lowest to highest
    # energy.
    rev_shell_names = ('Q3', 'Q2', 'Q1', 'P11', 'P10', 'P9', 'P8', 'P7', 'P6',
                       'P5', 'P4', 'P3', 'P2', 'P1', 'O9', 'O8', 'O7', 'O6',
                       'O5', 'O4', 'O3', 'O2', 'O1', 'N7', 'N6', 'N5', 'N4',
                       'N3', 'N2', 'N1', 'M5', 'M4', 'M3', 'M2', 'M1', 'L3',
                       'L2', 'L1', 'K')

    # Process the set of transitions to remove all of the holes and find all of
    # the possible emissions.  For each shell in each element, basically do a
    # depth first search on the transitions until we arrive at a transition
    # without a hole.
    while no_holes(transitions) > 0:
        for element_name, element_dict in transitions.iteritems():
            for shell_name in rev_shell_names:
                if shell_name not in element_dict:
                    continue
                shell_dict = element_dict[shell_name]
                # Create a dictionary of transitions without holes, that at the
                # end, we will use to replace shell_dict, assuming there are
                # some radiative transitions.
                new_shell_dict = {}
                for transition_id, transition_prob in shell_dict.iteritems():
                    trans_stack = [(transition_id, transition_prob)]
                    while len(trans_stack) > 0:
                        t = trans_stack.pop()
                        t_id = t[0]
                        t_prob = t[1]
                        emissions = t_id[0]
                        holes = t_id[1]
                        if len(holes) == 0:
                            if t_id in new_shell_dict:
                                new_shell_dict[t_id] += t_prob
                            else:
                                new_shell_dict[t_id] = t_prob
                        elif holes[0] not in element_dict:
                            # The first hole doesn't have any information.
                            # Eliminate it and add it back onto the stack
                            new_trans_id = (emissions, holes[1:])
                            trans_stack.append((new_trans_id, t_prob))
                        else:
                            # The first hole has transition information
                            # associated with that subshell.  For each trans
                            # in that shell, add a new transition onto the
                            # stack with all of the current and new emissions
                            # as well as all of the current and new holes,
                            # minus the hole we are currently looking at.  The
                            # probability of the new transition will be the
                            # probability of the current transition times the
                            # secondary transition, because it is a markov
                            # process.
                            for sec_trans_id, sec_trans_prob in element_dict[holes[0]].iteritems():
                                new_trans_id = (emissions + sec_trans_id[0],
                                                holes[1:] + sec_trans_id[1])
                                new_trans_prob = t_prob * sec_trans_prob
                                trans_stack.append((new_trans_id, new_trans_prob))
                # Remove shells that are not radiative
                if len(new_shell_dict.keys()) == 1 and new_shell_dict.keys()[0] == ((), ()):
                    del element_dict[shell_name]
                else:
                    element_dict[shell_name] = new_shell_dict
    # Remove the holes section from the dictionary, because they should all
    # be blank now.
    for element_name, element_dict in transitions.iteritems():
        for shell_name, shell_dict in element_dict.iteritems():
            new_shell_dict = {}
            for transition_id, transition_prob in shell_dict.iteritems():
                new_shell_dict[transition_id[0]] = transition_prob
            element_dict[shell_name] = new_shell_dict
    return transitions

def lower_bound(x, y):
    return [x >= z for z in y].index(False) - 1

def upper_bound(x, y):
    return len(y) - [x < z for z in y[::-1]].index(False)

def log_interp(x_new, x, y):
    i0 = lower_bound(x_new, x)
    i1 = upper_bound(x_new, x)
    x0 = math.log10(x[i0])
    x1 = math.log10(x[i1])
    y0 = math.log10(y[i0])
    y1 = math.log10(y[i1])
    x_new = math.log10(x_new)
    delta = (x_new - x0) / (x1 - x0)
    y_new = y0 + delta * (y1 - y0)
    return 10 ** y_new

def shell_probabilities(elements, interp_e=511.0e3):
    xsections = {}
    for element_name, element_dict in elements.iteritems():
        new_element_dict = {}
        xsections[element_name] = new_element_dict
        element_shells = elements[element_name]['shells']
        for shell_name, shell_info in element_shells.iteritems():
            energies = shell_info['energy']
            xsec = shell_info['xsec']
            interp_xsec = log_interp(interp_e, energies, xsec)
            new_element_dict[shell_name] = {
                'binding_energy': shell_info['binding_energy'],
                'xsec': interp_xsec}

    for element_name, element_dict in xsections.iteritems():
        xsection_sum = 0.0
        for shell_name, shell_dict in element_dict.iteritems():
            xsection_sum += shell_dict['binding_energy']
        for shell_name, shell_dict in element_dict.iteritems():
            shell_dict['prob'] = shell_dict['binding_energy'] / xsection_sum
    return xsections

def element_probabilities(transitions, shell_probs):
    full_trans = {}
    for element_name, element_dict in transitions.iteritems():
        full_trans[element_name] = {}
        element_shell_probs = shell_probs[element_name]
        for shell_name, shell_dict in element_dict.iteritems():
            shell_prob = element_shell_probs[shell_name]['prob']
            shell_bind_e = element_shell_probs[shell_name]['binding_energy']
            new_shell_dict = {}
            full_trans[element_name][shell_bind_e] = new_shell_dict
            for transition_id, transition_prob in shell_dict.iteritems():
                new_prob = transition_prob * shell_prob
                new_shell_dict[transition_id] = new_prob
    return full_trans

def add_atomic_probabilities(gray_mats, elements):
    for name, data in gray_mats.iteritems():
        total_mass = 0.0
        for element, frac in data['composition'].iteritems():
            total_mass += frac * elements[element]['mass']
        data['prob'] = data['composition'].copy()
        for element, frac in data['composition'].iteritems():
            mass_frac = frac * elements[element]['mass'] / total_mass
            data['prob'][element] = mass_frac
    return gray_mats

def material_probabilities(gray_mats, element_probs):
    mat_probs = {}
    for mat_name, mat_dict in gray_mats.iteritems():
        new_mat_dict = {}
        mat_probs[mat_name]  = new_mat_dict
        for ele_name, ele_prob in mat_dict['prob'].iteritems():
            for bind_e, bind_e_dict in element_probs[ele_name].iteritems():
                if bind_e not in new_mat_dict:
                    new_mat_dict[bind_e] = {}
                new_bind_e_dict = new_mat_dict[bind_e]
                for emis, emis_prob in bind_e_dict.iteritems():
                    new_prob = emis_prob * ele_prob
                    if emis not in new_bind_e_dict:
                        new_bind_e_dict[emis] = 0
                    new_bind_e_dict[emis] += new_prob
    return mat_probs

def filter_material_probabilities(mat_probs, threshold):
    new_mat_probs = {}
    for mat_name, mat_dict in mat_probs.iteritems():
        new_mat_dict = {}
        new_mat_probs[mat_name]  = new_mat_dict
        for bind_e, bind_e_dict in mat_dict.iteritems():
            new_bind_e_dict = {}
            for emis, emis_prob in bind_e_dict.iteritems():
                if emis_prob > threshold:
                    if emis not in new_bind_e_dict:
                        new_bind_e_dict[emis] = 0
                    new_bind_e_dict[emis] += emis_prob
                else:
                    if () not in new_bind_e_dict:
                        new_bind_e_dict[()] = 0
                    new_bind_e_dict[()] += emis_prob
            # If it's just the zero emission case, drop that binding energy
            if len(new_bind_e_dict.keys()) > 1:
                new_mat_dict[bind_e]  = new_bind_e_dict
    return new_mat_probs

def add_no_emission_prob(mat_probs):
    for mat_name, mat_dict in mat_probs.iteritems():
        total_prob = 0.0
        for bind_e, bind_e_dict in mat_dict.iteritems():
            for emis, emis_prob in bind_e_dict.iteritems():
                total_prob += emis_prob
        mat_dict[0.0] = {(): 1.0 - total_prob}
    return mat_probs

def take_largest_emission(mat_probs):
    new_mat_probs = {}
    for mat_name, mat_dict in mat_probs.iteritems():
        new_mat_dict = {}
        new_mat_probs[mat_name]  = new_mat_dict
        for bind_e, bind_e_dict in mat_dict.iteritems():
            new_bind_e_dict = {}
            new_mat_dict[bind_e] = new_bind_e_dict
            for emis, emis_prob in bind_e_dict.iteritems():
                if len(emis) == 0:
                    new_emis = None
                elif len(emis) == 1:
                    new_emis = emis[0]
                else:
                    new_emis = list(emis)
                    new_emis.sort()
                    new_emis = new_emis[-1]
                new_bind_e_dict[new_emis] = emis_prob
    return new_mat_probs

def full_material_emission_probs(gray_mats, elements):
    transitions = calculate_shell_cascades(elements)
    shell_probs = shell_probabilities(elements)
    element_probs = element_probabilities(transitions, shell_probs)
    gray_mats = add_atomic_probabilities(gray_mats, elements)
    mat_probs = material_probabilities(gray_mats, element_probs)
    mat_probs = filter_material_probabilities(mat_probs, 0.5e-2)
    mat_probs = add_no_emission_prob(mat_probs)
    mat_probs = take_largest_emission(mat_probs)
    return mat_probs
