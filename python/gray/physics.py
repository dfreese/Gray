import numpy as np

def classical_electron_radius_cm():
    """
    https://en.wikipedia.org/wiki/Classical_electron_radius
    """
    return 2.8179403227e-13

def hc_MeV_cm():
    """ Planck's contant times the speed of light in MeV*cm
    https://en.wikipedia.org/wiki/Planck_constant
    """
    return 1.23984193e-10

def electron_mass_MeVc2():
    """The rest mass of the electron in MeV/c**2
    https://en.wikipedia.org/wiki/Electron
    """
    return 0.5109989461

def inv_p_energy_ratio(energy_mev, theta):
    return (1 + (energy_mev / electron_mass_MeVc2()) * (1 - np.cos(theta)))

def p_energy_ratio(energy_mev, theta):
    return inv_p_energy_ratio(energy_mev, theta) ** -1.0

def thompson_diff_cs(theta):
    theta = np.asanyarray(theta)
    thomp = (classical_electron_radius_cm() ** 2 *
             0.5 * (1 + np.cos(theta) ** 2))
    return thomp

def klein_nishina_diff_cs(energy_mev, theta):
    theta = np.asanyarray(theta)
    p = p_energy_ratio(energy_mev, theta)
    inv_p = inv_p_energy_ratio(energy_mev, theta)
    compt = (0.5 * classical_electron_radius_cm() ** 2 *
             p ** 2 * (p + inv_p - np.sin(theta)**2))
    return compt

def energy_to_inv_wavelength(energy_mev):
    energy_mev = np.asanyarray(energy_mev)
    return energy_mev / hc_MeV_cm()

def form_factor_x_param(energy_mev, theta):
    return np.sin(theta / 2.0) * energy_to_inv_wavelength(energy_mev)

def rayleigh_cs(material_x, material_form_factor, energy_mev, theta):
    """Rayleigh Scattering cross section in cm**2 based on the EPDL form factor
    functions.  This neglects the anomalous scattering factors.
    """
    x = form_factor_x_param(energy_mev, theta)
    fx = np.interp(x, material_x, material_form_factor)
    return thompson_diff_cs(theta) * fx ** 2

def compton_cs(material_x, material_scattering_func, energy_mev, theta):
    """Compton Scattering cross section in cm**2 based on the EPDL scattering
    functions.
    """
    x = form_factor_x_param(energy_mev, theta)
    sx = np.interp(x, material_x, material_scattering_func)
    return klein_nishina_diff_cs(energy_mev, theta) * sx
