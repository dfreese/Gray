import os
import unittest
import numpy as np
import epdl
import epdl.epdl

class TestEPDL(unittest.TestCase):
    @classmethod
    def setUpClass(self):
        self.data = epdl.epdl._data()

    def test_load(self):
        self.assertEqual(len(self.data._elements), 100)
        self.assertSetEqual(set(self.data._elements.keys()),
                            set(range(1, 101)))
        for Z, element in self.data._elements.items():
            self.assertIsInstance(Z, int)
            self.assertGreaterEqual(Z, 1)
            self.assertLessEqual(Z, 100)
            self.assertIsInstance(element.energy, np.ndarray)
            self.assertIsInstance(element.matten_rayl, np.ndarray)
            self.assertIsInstance(element.matten_comp, np.ndarray)
            self.assertIsInstance(element.matten_phot, np.ndarray)
            self.assertIsInstance(element.x, np.ndarray)
            self.assertIsInstance(element.form_factor, np.ndarray)
            self.assertIsInstance(element.scattering_func, np.ndarray)
            self.assertIsInstance(element.anom_scat_fact_imag, np.ndarray)
            self.assertIsInstance(element.anom_scat_fact_real, np.ndarray)
            self.assertIsInstance(element.edges, np.ndarray)
            self.assertIsInstance(element.edge_select, np.ndarray)
            self.assertIsInstance(element.atomic_weight, float)

    def test_z_conversion(self):
        self.assertEqual(epdl.convert_to_z('Lu'), 71)
        self.assertEqual(epdl.convert_to_z('71'), 71)
        self.assertEqual(epdl.convert_to_z(71), 71)

    def test_z_description(self):
        self.assertEqual(epdl.z_description_dict({'3': 5, 'Bi': 42.}),
                         {3: 5, 83: 42.})

    def test_composition_to_mass_fraction(self):
        lso_comp = {'Lu': 2, 'Si': 1, 'O': 5}
        lso_mass = {71: 0.76402276741561,
                    14: 0.06132006471290742,
                    8: 0.17465716787148242}
        result = self.data.composition_to_mass_fraction(lso_comp)
        self.assertEqual(result, lso_mass)

class TestEpdlFreeFunc(unittest.TestCase):
    def test_standard_grid(self):
        val = epdl.standard_grid(0.001, 0.01)
        exp = np.array((0.001, 0.002, 0.003, 0.004, 0.005, 0.006, 0.008, 0.01))
        self.assertTrue((val == exp).all())

        val = epdl.standard_grid(0.002, 0.01)
        self.assertTrue((val == exp[1:]).all())

        val = epdl.standard_grid(0.0023, 0.02)
        exp = np.array((0.0023, 0.003, 0.004, 0.005, 0.006, 0.008, 0.01, 0.02))
        self.assertTrue((val == exp).all())

class TestEpdlMaterial(unittest.TestCase):
    def test_types(self):
        lso = epdl.material_by_formula('Lu2SiO5')
        self.assertIsInstance(lso.z_eff, float)
        self.assertIsInstance(lso.atomic_weight, float)
        self.assertIsInstance(lso.energy, np.ndarray)
        self.assertIsInstance(lso.matten_rayl, np.ndarray)
        self.assertIsInstance(lso.matten_comp, np.ndarray)
        self.assertIsInstance(lso.matten_phot, np.ndarray)
        self.assertIsInstance(lso.x, np.ndarray)
        self.assertIsInstance(lso.form_factor, np.ndarray)
        self.assertIsInstance(lso.scattering_func, np.ndarray)
        self.assertIsInstance(lso.edges, np.ndarray)
        # Not implemented currently
        self.assertIsNone(lso.anom_scat_fact_imag)
        self.assertIsNone(lso.anom_scat_fact_real)

class TestDatabase(unittest.TestCase):
    @classmethod
    def setUpClass(self):
        gate_file = os.path.join(os.path.dirname(__file__), 'GateMaterials.db')
        self.data = epdl.gate_database_materials(gate_file, 0.001, 1.5)

    def test_lso(self):
        lso = self.data['LSO']
        self.assertAlmostEqual(lso.z_eff, 56.5013547)
        self.assertEqual(lso.energy[0], 0.001)
        self.assertEqual(lso.energy[-1], 1.5)
