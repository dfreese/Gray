#!/usr/bin/env python
import os
import re
import sys
import numpy as np
from .chem_defines import symbol_to_z

try:
    # For Python 3.0 and later
    from urllib.request import build_opener
except ImportError:
    # Fall back to Python 2's urllib2
    from urllib2 import build_opener

def loglog_interp(zz, xx, yy):
    logz = np.log10(zz)
    logx = np.log10(xx)
    logy = np.log10(yy)
    return np.power(10.0, np.interp(logz, logx, logy))

def standard_grid(start, end):
    start_mag = np.log10(start).astype(int)
    end_mag = np.log10(end).astype(int)
    points = np.array((1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 8.0))
    mag = 10.0 ** np.arange(start_mag - 1, end_mag)
    vals = (points[None, :] * mag[:, None]).ravel()
    vals = np.unique(np.concatenate(((start,),
                                     vals[(vals >= start) & (vals <= end)],
                                     (end,))))
    return vals

def convert_to_z(key):
    if key in symbol_to_z:
        return symbol_to_z[key]
    else:
        return int(key)

def z_description_dict(desc):
    return {convert_to_z(k) : v for k, v in desc.items()}

def formula_to_composition(formula):
    """
    Breaks a molecular formula into its constituent elements.  Returns a
    dictionary of element keys and an integer formula
    """
    # Ignore the first value in the split, because it will be blank from
    # trying to split on the first element symbol.
    parts = [re.split('([0-9]+)', test) for test in
    re.findall('[A-Z][a-z]?[0-9]*', formula)]
    values = [int(part[1]) if len(part) > 1 else 1 for part in parts]
    elements = [part[0] for part in parts]
    return {k:int(v) for k, v in zip(elements, values)}

def retreive_dataset(filename, url):
    """
    Download datasets, like the EADL or EPDL on the IAEA website, by blatantly
    spoofing the User-Agent. Spoofing based on this reference:
        http://stackoverflow.com/a/802246/2465202
    Downloading based on this one:
        http://stackoverflow.com/a/22721/2465202
    """
    opener = build_opener()
    opener.addheaders = [('User-Agent', 'Mozilla/5.0')]
    remote_fid = opener.open(url)
    with open(filename, 'w') as local_fid:
        local_fid.write(remote_fid.read())
    remote_fid.close()

def check_dataset(filename=None, url=None):
    """
    Check if a dataset, such as the EADL library, exists, and if it doesn't,
    then use retreive_dataset() to grab it.
    """
    try:
        with open(filename, 'r'):
            exists = True
    except IOError:
        exists = False

    if not exists:
        retreive_dataset(filename, url)

_END_OF_TABLE_LINE = ' ' * 71 + '1\r\n'

_BARN_CM = 1.0e-24
_ALVOGADRO = 6.0221409e23

_EPDL_URL = 'https://www-nds.iaea.org/epdl97/data/endfb6/epdl97/epdl97.all'

class EPDL(object):
    class Element(object):
        def __init__(self, atomic_weight):
            self.atomic_weight = atomic_weight
            self.energy = None
            self.matten_rayl = None
            self.matten_comp = None
            self.matten_phot = None
            self.x = None
            self.form_factor = None
            self.scattering_func = None
            self.anom_scat_fact_imag = None
            self.anom_scat_fact_real = None
            self.edges = None
            self.edge_select = None

        def get_val_edges(process=None):
            energy = self.energy[self.edge_select]
            if process is None:
                return energy
            elif process == 'rayl':
                val = self.matten_rayl[edge_select]
            elif process == 'comp':
                val = self.matten_comp[edge_select]
            elif process == 'phot':
                val = self.matten_phot[edge_select]
            return energy, val

        def get_val_nonedges(type):
            energy = self.energy[~self.edge_select]
            if process is None:
                return energy
            elif process == 'rayl':
                val = self.matten_rayl[~self.edge_select]
            elif process == 'comp':
                val = self.matten_comp[~self.edge_select]
            elif process == 'phot':
                val = self.matten_phot[~self.edge_select]
            return energy, val


    class Material(object):
        def __init__(self):
            self.z_eff = None
            self.atomic_weight = None
            self.index = None
            self.density = None
            self.energy = None
            self.matten_rayl = None
            self.matten_comp = None
            self.matten_phot = None
            self.x = None
            self.form_factor = None
            self.scattering_func = None
            self.anom_scat_fact_imag = None
            self.anom_scat_fact_real = None
            self.edges = None

        def to_dict(self):
            data = {
                'z_eff': self.z_eff,
                'atomic_weight': self.atomic_weight,
                'index': self.index,
                'density': self.density,
                'energy': tuple(self.energy),
                'matten_rayl': tuple(self.matten_rayl),
                'matten_comp': tuple(self.matten_comp),
                'matten_phot': tuple(self.matten_phot),
                'x': tuple(self.x),
                'form_factor': tuple(self.form_factor),
                'scattering_func': tuple(self.scattering_func),
                'edges': tuple(self.edges),
            }
            return data

    def __init__(self, filename=None):
        self._elements = {}
        if filename is None:
            d = os.path.dirname(sys.modules['epdl.epdl'].__file__)
            epdl_file = 'epdl.all'
            epdl_file_full = os.path.join(d, epdl_file)
            check_dataset(epdl_file_full, _EPDL_URL)
            filename = epdl_file_full
        with open(filename, 'rb') as fid:
            self._read_file(fid)

    def _init_element_entry(self, Z, atomic_weight):
        if Z not in self._elements:
            self._elements[Z] = EPDL.Element(atomic_weight)
        return self._elements[Z]

    @staticmethod
    def _read_table_elements(fid, start, no_lines):
        data = np.empty((no_lines, 2))
        fid.seek(start)
        for idx in range(no_lines):
            line = fid.readline()
            data[idx, 0] = float(line[0:16])
            data[idx, 1] = float(line[16:32])
        fid.readline() # Read out the end of section line
        return data

    @staticmethod
    def _crosssection_to_linatt(crosssection, atomic_weight):
        return crosssection * _BARN_CM * _ALVOGADRO / atomic_weight

    def _read_file(self, fid):
        while True:
            # get header lines
            line1 = fid.readline()
            line2 = fid.readline()
            if len(line2) == 0:
                break
            table_file_start = fid.tell()

            nuc_zzzaaa = int(line1[0:6].strip())
            Z = nuc_zzzaaa / 1000

            yi = int(line1[7:9].strip() or -1)
            yo = int(line1[10:12].strip() or -1)
            atomic_weight = float(line1[13:24].strip() or -1.)
            date = line1[25:31].strip()
            iflag = int(line1[31].strip() or 0)

            element = self._init_element_entry(Z, atomic_weight)

            # parse the second header line
            rdesc = int(line2[0:2].strip() or -1)
            rprop = int(line2[2:5].strip() or -1)
            rmod = int(line2[5:8].strip() or -1)
            x1 = int(float(line2[21:32].strip() or -1.))

            end_of_table_found = False
            table_lines = -1 # Don't count the end of the table
            while not end_of_table_found:
                line = fid.readline()
                end_of_table_found = (line == _END_OF_TABLE_LINE)
                table_lines += 1

            table_file_end = fid.tell()

            if rdesc == 71 and rprop == 0 and rmod == 0:
                element.matten_rayl = EPDL._read_table_elements(
                    fid, table_file_start, table_lines)
                element.matten_rayl[:, 1] = EPDL._crosssection_to_linatt(
                    element.matten_rayl[:, 1], element.atomic_weight
                )
            if rdesc == 72 and rprop == 0 and rmod == 0:
                element.matten_comp = EPDL._read_table_elements(
                    fid, table_file_start, table_lines)
                element.matten_comp[:, 1] = EPDL._crosssection_to_linatt(
                    element.matten_comp[:, 1], element.atomic_weight
                )
            if rdesc == 73 and rprop == 0 and rmod == 0:
                element.matten_phot = EPDL._read_table_elements(
                    fid, table_file_start, table_lines)
                element.matten_phot[:, 1] = EPDL._crosssection_to_linatt(
                    element.matten_phot[:, 1], element.atomic_weight
                )
            if rdesc == 93 and rprop == 941 and rmod == 0:
                element.form_factor = EPDL._read_table_elements(
                    fid, table_file_start, table_lines)
            if rdesc == 93 and rprop == 942 and rmod == 0:
                element.scattering_func = EPDL._read_table_elements(
                    fid, table_file_start, table_lines)
            if rdesc == 93 and rprop == 943 and rmod == 0:
                element.anom_scat_fact_imag = EPDL._read_table_elements(
                    fid, table_file_start, table_lines)
            if rdesc == 93 and rprop == 944 and rmod == 0:
                element.anom_scat_fact_real = EPDL._read_table_elements(
                    fid, table_file_start, table_lines)

        # Now that we have all of the data loaded in, as we are not sure on the
        # ordering of the different elements, we can now convert all of the
        # linear attenuation distributions to use the same energy, basing on
        # the photoelectric energy, as it will have the most detail with the
        # edges.
        for Z, element in self._elements.items():
            # Use the energy from the photoelectric factors, as it will be
            # the most detailed, with the edges
            element.energy = element.matten_phot[:, 0]
            element.matten_phot = element.matten_phot[:, 1]
            # And then loglog interpolate the rayleigh and compton factors
            # onto the photoelectric
            element.matten_rayl = loglog_interp(
                element.energy, element.matten_rayl[:, 0],
                element.matten_rayl[:, 1])
            element.matten_comp = loglog_interp(
                element.energy, element.matten_comp[:, 0],
                element.matten_comp[:, 1])
            element.edges = element.energy[:-1][np.diff(element.energy) == 0]
            element.edge_select = (element.energy[:, None] ==
                                   element.edges[None, :]).any(1)

            element.x = element.scattering_func[:, 0]
            element.scattering_func = element.scattering_func[:, 1]
            element.form_factor = np.interp(
                element.x,element.form_factor[:, 0], element.form_factor[:, 1])

    def composition_to_mass_fraction(self, desc):
        desc = z_description_dict(desc)
        # Convert the number of atoms to an total atomic weight per element and
        # calculate the total weight of the material while we're at it.
        total_atomic_weight = 0
        for Z, n in desc.items():
            total_atomic_weight += n * self._elements[Z].atomic_weight
        # convert each of the weights to a mass fraction and pass it along it's
        # merry way
        return {z : n * self._elements[z].atomic_weight / total_atomic_weight
                for z, n in desc.items()}

    def material_by_formula(self, formula, energy_lo=None, energy_hi=None):
        return self.material_by_composition(
            formula_to_composition(formula), energy_lo, energy_hi)

    def material_by_composition(self, desc, energy_lo=None, energy_hi=None):
        return self.material_by_mass_fraction(
            self.composition_to_mass_fraction(desc), energy_lo, energy_hi)

    def material_by_mass_fraction(self, desc, energy_lo=None, energy_hi=None):
        desc = z_description_dict(desc)
        total_atomic_weight = 0
        for Z, w in desc.items():
            total_atomic_weight += w * self._elements[Z].atomic_weight

        material = EPDL.Material()
        material.atomic_weight = total_atomic_weight

        material.z_eff = 0.0
        for Z, w in desc.items():
            material.z_eff += w * Z

        # find the valid range for all elements by finding the highest minimum
        # and lowest max
        energy_lo_bound = np.array(
            [self._elements[Z].energy.min() for Z in desc.keys()]).max()
        energy_hi_bound = np.array(
            [self._elements[Z].energy.max() for Z in desc.keys()]).min()

        if energy_lo is None:
            energy_lo = energy_lo_bound
        else:
            energy_lo = np.array((energy_lo, energy_lo_bound)).max()

        if energy_hi is None:
            energy_hi = energy_hi_bound
        else:
            energy_hi = np.array((energy_hi, energy_hi_bound)).min()

        material.energy = standard_grid(energy_lo, energy_hi)
        material.matten_rayl = np.zeros(material.energy.shape)
        material.matten_comp = np.zeros(material.energy.shape)
        material.matten_phot = np.zeros(material.energy.shape)

        for Z, w in desc.items():
            element = self._elements[Z]
            material.matten_rayl += w * loglog_interp(
                material.energy, element.energy, element.matten_rayl)
            material.matten_comp += w * loglog_interp(
                material.energy, element.energy, element.matten_comp)
            material.matten_phot += w * loglog_interp(
                material.energy, element.energy, element.matten_phot)

        z_sorted = tuple(sorted(desc.keys()))
        e_sorted = tuple(self._elements[Z] for Z in z_sorted)
        edges_energy = {
            Z : e.energy[e.edge_select]
            for Z, e in zip(z_sorted, e_sorted)}
        edges_phot = {
            Z : desc[Z] * e.matten_phot[e.edge_select]
            for Z, e in zip(z_sorted, e_sorted)}

        for Z in desc.keys():
            edge_energy = edges_energy[Z]
            for z_internal, w_internal in desc.items():
                if Z == z_internal:
                    continue
                element = self._elements[z_internal]
                in_energy = self._elements[z_internal].energy
                edges_phot[Z] += w_internal * loglog_interp(
                    edge_energy, in_energy, element.matten_phot)

        # right now, assume the edges are distinct.  If there are doubles or
        # more this won't hurt anything as only the first and last values will
        # be looked at for interpolation, and graphing it will just go back and
        # forth.
        # TODO: check assumption, or find better method
        energy_cat = np.concatenate([edges_energy[Z] for Z in z_sorted])
        phot_cat = np.concatenate([edges_phot[Z] for Z in z_sorted])

        e_valid = (energy_lo <= energy_cat) & (energy_cat <= energy_hi)
        energy_cat = energy_cat[e_valid]
        phot_cat = phot_cat[e_valid]
        material.edges = np.unique(energy_cat)

        energy_pre_edge_cat = material.energy.copy()
        material.energy = np.concatenate((material.energy, energy_cat))
        material.matten_phot = np.concatenate((material.matten_phot, phot_cat))

        # Use merge sort to keep the edges stable with repect to each
        # other
        sort_idx = np.argsort(material.energy, kind='mergesort')
        material.energy = material.energy[sort_idx]
        material.matten_phot = material.matten_phot[sort_idx]

        material.matten_rayl = loglog_interp(
            material.energy, energy_pre_edge_cat, material.matten_rayl)
        material.matten_comp = loglog_interp(
            material.energy, energy_pre_edge_cat, material.matten_comp)


        # Now look at the scattering_func and form_factor.
        # Find the material with the most number of points, use that for x, and
        # then interpolate all of the elements onto that.
        max_size_z = None
        max_size_points = 0
        for Z in desc.keys():
            element = self._elements[Z]
            if max_size_z is None:
                max_size_z = Z
                max_size_points = self._elements[Z].x.size
            else:
                cmp_size_points = self._elements[Z].x.size
                if max_size_points < cmp_size_points:
                    max_size_z = Z
                    max_size_points = cmp_size_points
        material.x = self._elements[Z].x.copy()
        material.scattering_func = np.zeros(material.x.shape)
        material.form_factor = np.zeros(material.x.shape)

        for Z, w in desc.items():
            element = self._elements[Z]
            material.scattering_func += w * np.interp(
                material.x, element.x, element.scattering_func)
            material.form_factor += w * np.interp(
                material.x, element.x, element.form_factor)

        return material

__data = None
def _data():
    global __data
    if __data is None:
        __data = EPDL()
    return __data

def init():
    _data()

def element(sym_or_z):
    return _data()._elements[convert_to_z(sym_or_z)]

def elements():
    # TODO fix magic Z = 1 to 100 number range
    for Z in range(1, 101):
        yield _data()._elements[Z]

def z_elements():
    # TODO fix magic Z = 1 to 100 number range
    for Z in range(1, 101):
        yield Z, _data()._elements[Z]

def material_by_formula(formula, energy_lo=None, energy_hi=None):
    return _data().material_by_formula(formula, energy_lo, energy_hi)

def material_by_composition(desc, energy_lo=None, energy_hi=None):
    return _data().material_by_composition(desc, energy_lo, energy_hi)

def material_by_mass_fraction(desc, energy_lo=None, energy_hi=None):
    return _data().material_by_mass_fraction(desc, energy_lo, energy_hi)
