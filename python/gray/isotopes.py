
def read_isotopes(lines):
    ret = {}
    default_set = False
    for line in lines:
        comment = line.find('#')
        if comment < 0:
            comment = len(line)
        line = line[:comment]
        if line:
            data = line.split()
            name = data[0]
            isotope = {
                'acolinearity_deg_fwhm': float(data[1]),
                'half_life_s': float(data[2]),
                'positron_emiss_prob': float(data[3]),
                'prompt_gamma_energy_mev': float(data[4]),
                'model': data[5]
            }
            if not default_set:
                # Set the first isotope we find in the file to be the default
                isotope['default'] = True
                default_set = True
            if isotope['model'] == 'levin_exp':
                isotope['prob_c'] = float(data[6])
                isotope['k1'] = float(data[7])
                isotope['k2'] = float(data[8])
                isotope['max_range_mm'] = float(data[9])
            elif isotope['model'] == 'gauss':
                isotope['fwhm_mm'] = float(data[6])
                isotope['max_range_mm'] = float(data[9])
            ret[name] = isotope
    return ret


def read_isotopes_file(filename):
    with open(filename, 'r') as fid:
        lines = fid.read().splitlines()
    return read_isotopes(lines)
