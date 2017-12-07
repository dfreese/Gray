import unittest
import os
from gray.gate import (
    Element, Material, Database, parse_density_unit_str,
    parse_density_molar_mass_str, database_epdl
    )

_flourine_test_str = 'Fluorine:   S= F   ; Z=  9. ; A=  18.998 g/mole'

class TestElementGrams(unittest.TestCase):
    @classmethod
    def setUpClass(self):
        self.data = Element(_flourine_test_str)

    def test_name(self):
        self.assertEqual(self.data.name, 'Fluorine')

    def test_symbol(self):
        self.assertEqual(self.data.symbol, 'F')

    def test_atomic_weight(self):
        self.assertEqual(self.data.awg, 18.998)

class TestElementMilligrams(unittest.TestCase):
    def test_atomic_weight(self):
        element = Element(
            _flourine_test_str.replace('18.998 g/mole', '18998. mg/mole'))
        self.assertEqual(element.awg, 18.998)

_lso_test_str = (
    'LSO: d=7.4 g/cm3; n=3 ; state=Solid',
    '        +el: name=Lutetium   ; n=2',
    '        +el: name=Silicon    ; n=1',
    '        +el: name=Oxygen     ; n=4',
    '',
    )
_lso_mg_test_str = tuple(s.replace('g/cm3', 'mg/cm3') for s in _lso_test_str)
_lso_no_state_test_str = tuple(
    s.replace(' ; state=Solid', '') for s in _lso_test_str)
_lso_massfrac_test_str = tuple(
    s.replace(' ; n=', ' ; f=0.') for s in _lso_test_str)

class TestMaterialBase(unittest.TestCase):
    @classmethod
    def setUpClass(self):
        self.data = Material(_lso_test_str)

    def test_name(self):
        self.assertEqual(self.data.name, 'LSO')

    def test_density(self):
        self.assertEqual(self.data.density, 7.4)

    def test_no_elements(self):
        self.assertEqual(self.data.no_elements, 3)

    def test_description(self):
        self.assertEqual(self.data.description, 'composition')

    def test_elements(self):
        self.assertEqual(self.data.elements, {
                'Lutetium': 2,
                'Silicon': 1,
                'Oxygen': 4,
            })

class TestMaterialOther(unittest.TestCase):
    def test_no_state(self):
        mat = Material(_lso_no_state_test_str)
        self.assertIsNone(mat.state)

    def test_mgcm3(self):
        mat = Material(_lso_mg_test_str)
        self.assertAlmostEqual(7.4e-3, mat.density)

    def test_mass_frac(self):
        mat = Material(_lso_massfrac_test_str)
        self.assertEqual(mat.description, 'massfraction')

class TestDensityUnitParses(unittest.TestCase):
    def test_gcm3(self):
        self.assertEqual(parse_density_unit_str('1.19 g/cm3'), 1.19)

    def test_mgcm3(self):
        self.assertAlmostEqual(parse_density_unit_str('1.19 mg/cm3'), 0.00119)

    def test_gml(self):
        self.assertEqual(parse_density_unit_str('1.19 g/ml'), 1.19)

    def test_mgml(self):
        self.assertAlmostEqual(parse_density_unit_str('1.19 mg/ml'), 0.00119)

    def test_unknown(self):
        self.assertRaises(
            ValueError, parse_density_molar_mass_str, '1.19 kg/cm3')

class TestMolarMassUnitParses(unittest.TestCase):
    def test_gmol(self):
        self.assertEqual(parse_density_molar_mass_str('1.19 g/mol'), 1.19)

    def test_gmole(self):
        self.assertEqual(parse_density_molar_mass_str('1.19 g/mole'), 1.19)

    def test_gmol(self):
        self.assertAlmostEqual(
            parse_density_molar_mass_str('1.19 mg/mol'), 1.19e-3)

    def test_gmole(self):
        self.assertAlmostEqual(
            parse_density_molar_mass_str('1.19 mg/mole'), 1.19e-3)

    def test_unknown(self):
        self.assertRaises(
            ValueError, parse_density_molar_mass_str, '1.19 kg/mole')

class TestDatabase(unittest.TestCase):
    @classmethod
    def setUpClass(self):
        gate_file = os.path.join(os.path.dirname(__file__), 'GateMaterials.db')
        self.data = Database(gate_file)

    def test_lu(self):
        self.assertEqual(self.data.element(71).symbol, 'Lu')

    def test_h(self):
        self.assertEqual(self.data.element('H').awg, 1.01)

    def test_u(self):
        self.assertEqual(self.data.element('Uranium').Z, 92)

    def test_vacuum(self):
        self.assertAlmostEqual(
            self.data.material('Vacuum').density, 0.000001e-3)

    def test_carbide(self):
        self.assertEqual(
            self.data.material('Carbide').elements, {6: 1, 74: 1})

    def test_lso(self):
        self.assertEqual(
            self.data.material('LSO').elements, {8: 5, 14: 1, 71: 2})

class TestDatabaseEPDLIntegration(unittest.TestCase):
    @classmethod
    def setUpClass(self):
        gate_file = os.path.join(os.path.dirname(__file__), 'GateMaterials.db')
        self.data = database_epdl(gate_file, 0.001, 1.5)

    def test_lso(self):
        lso = self.data['LSO']
        self.assertAlmostEqual(lso.z_eff, 56.5013547)
        self.assertEqual(lso.energy[0], 0.001)
        self.assertEqual(lso.energy[-1], 1.5)
