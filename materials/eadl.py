#!/usr/bin/env python
import urllib2

def endftod(string):
    """
    Takes a string of a number in the ENDF format and transforms it into a
    python float.  The ENDF format has scientific notation as number+/-exponent
    with no 'e' indicator making it not compatible with float().  We replace
    '+' with 'e' and '-' with 'e-' and then passing to float()

    Parameters
    ----------
    string : str
        an ENDF float represented as a string

    Returns
    ----------
    value : float
        a python float of the scientific notation value.
    """
    string = string.replace('+', 'e')
    string = string.replace('-', 'e-')
    return(float(string))

def get_head_record(line):
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
    ZA = int(endftod(line[:11]))
    AWR = endftod(line[11:22])
    L1 = int(line[22:33])
    L2 = int(line[33:44])
    N1 = int(line[44:55])
    N2 = int(line[55:66])
    return [ZA, AWR, L1, L2, N1, N2]

def get_cont_record(line, skipC=False):
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
    if not skipC:
        C1 = endftod(line[:11])
        C2 = endftod(line[11:22])
    L1 = int(line[22:33])
    L2 = int(line[33:44])
    N1 = int(line[44:55])
    N2 = int(line[55:66])
    if skipC:
        return [L1, L2, N1, N2]
    else:
        return [C1, C2, L1, L2, N1, N2]

def get_text_record(line):
    """
    Takes a string of a TEXT record line in the ENDF format and returns the
    appropriate section of the line representing the actual text record and not
    information at the end of the line.

    Parameters
    ----------
    line : str
        a line representing an ENDF TEXT record line.

    Returns
    ----------
    line : str
        The text portion of the TEXT record line.
    """
    return line[0:66]

def get_list_record(fid):
    """
    Takes an open file that is currently pointing to the beginning of a LIST
    record and processes it by reading the first CONT control record and then
    the subsequent LIST records, using readline() to process through the file.

    Parameters
    ----------
    fid : file object
        an open file object currently point to the start of a LIST record

    Returns
    ----------
    items : list
        output of get_cont_record() on the first CONT record line.
    itemsList : list
        generally 6 floats for each line in the list.
    """
    # determine how many items are in list
    items = get_cont_record(fid.readline())
    NPL = items[4]
    # read items
    itemsList = []
    m = 0
    for i in range((NPL-1)//6 + 1):
        line = fid.readline()
        toRead = min(6, NPL-m)
        for j in range(toRead):
            val = endftod(line[0:11])
            itemsList.append(val)
            line = line[11:]
        m = m + toRead
    return items, itemsList

def read_header(fid):
    """
    Processes the information in the header of the element and returns a dict
    with the follwoing fields:

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
    items = get_head_record(fid.readline())
    target = {}
    info = {}
    projectile = {}
    target['ZA'] = items[0]
    target['mass'] = items[1]
    target['fissionable'] = (items[3] == 1)

    libraries = {0: 'ENDF/B', 1: 'ENDF/A', 2: 'JEFF', 3: 'EFF',
                  4: 'ENDF/B High Energy', 5: 'CENDL', 6: 'JENDL',
                  31: 'INDL/V', 32: 'INDL/A', 33: 'FENDL', 34: 'IRDF',
                  35: 'BROND', 36: 'INGDB-90', 37: 'FENDL/A', 41: 'BROND'}
    try:
        library = libraries[items[4]]
    except KeyError:
        library = 'Unknown'
    info['modification'] = items[5]

    # Control record 1
    items = get_cont_record(fid.readline())
    target['excitation_energy'] = items[0]
    target['stable'] = (int(items[1]) == 0)
    target['state'] = items[2]
    target['isomeric_state'] = items[3]
    info['format'] = items[5]
    assert info['format'] == 6

    # Control record 2
    items = get_cont_record(fid.readline())
    projectile['mass'] = items[0]
    info['energy_max'] = items[1]
    library_release = items[2]
    info['sublibrary'] = items[4]
    library_version = items[5]
    info['library'] = (library, library_version, library_release)

    # Control record 3
    items = get_cont_record(fid.readline())
    target['temperature'] = items[0]
    info['derived'] = (items[2] > 0)
    NWD = items[4]
    NXC = items[5]

    # Text records
    text = [get_text_record(fid.readline()) for i in range(NWD)]
    if len(text) >= 5:
        target['symbol'] = text[0][4:6].split()[0]
        target['atomic_number'] = int(text[0][0:3])
        info['laboratory'] = text[0][11:22]
        info['date'] = text[0][22:32]
        info['author'] = text[0][32:66]
        info['reference'] = text[1][1:22]
        info['date_distribution'] = text[1][22:32]
        info['date_release'] = text[1][33:43]
        info['date_entry'] = text[1][55:63]
        info['identifier'] = text[2:5]
        info['description'] = text[5:]
    # Discard reaction information
    get_cont_record(fid.readline(), skipC=True)
    get_cont_record(fid.readline(), skipC=True)
    return target

def read_atomic_relaxation(fid):
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
    params = get_head_record(fid.readline())
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
    for i in range(n_subshells):
        params, list_items = get_list_record(fid)
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
    fid.readline()
    return atomic_relaxation

def retreive_eadl(filename, url=None):
    """
    Download EADL from the IAEA website, by blatantly spoofing the User-Agent.
    Spoofing based on this reference:
        http://stackoverflow.com/a/802246/2465202
    Downloading based on this one:
        http://stackoverflow.com/a/22721/2465202
    """
    if url is None:
        url = 'https://www-nds.iaea.org/epdl97/data/endfb6/eadl/eadl.all'

    opener = urllib2.build_opener()
    opener.addheaders = [('User-Agent', 'Mozilla/5.0')]
    remote_fid = opener.open(url)
    with open(filename, 'w') as local_fid:
        local_fid.write(remote_fid.read())
    remote_fid.close()

def check_eadl(filename=None, url=None):
    """
    Check if the EADL library exists, and if it doesn't, then use
    retreive_eadl() to grab it.
    """
    try:
        with open(filename, 'r') as fid:
            exists = True
    except IOError:
        exists = False

    if not exists:
        retreive_eadl(filename, url)

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
        check_eadl(filename, url)
        self._elements = dict()
        self._num_to_symbol = dict()
        with open(filename, 'r') as fid:
            # Initialize by getting the first line and the position prior to
            # the read.  Calls to read_header() and read_atomic_relaxation()
            # will require the full first line, so we need to seek backwards.
            # readline will return None when the file is done, so we loop with
            # line as a condition.
            position = fid.tell()
            line = fid.readline()
            while line:
                # The ENDF format specifies that the MF and MT numbers shall be
                # found in these parts of the line.
                MF = int(line[70:72])
                MT = int(line[72:75])
                if MF == 1 and MT == 451:
                    fid.seek(position)
                    element_header = read_header(fid)
                if MF == 28 and MT == 533:
                    fid.seek(position)
                    if element_header is None:
                        element_header = dict()
                    element_header['shells'] = read_atomic_relaxation(fid)
                    self._elements[element_header['symbol']] = element_header
                    self._num_to_symbol[element_header['atomic_number']] = \
                        element_header['symbol']
                    element_header = None
                position = fid.tell()
                line = fid.readline()

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
            return self._elements[key]
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
