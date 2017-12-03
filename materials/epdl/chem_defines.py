symbol_to_z = {
    'H': 1,
    'He': 2,
    'Li': 3,
    'Be': 4,
    'B': 5,
    'C': 6,
    'N': 7,
    'O': 8,
    'F': 9,
    'Ne': 10,
    'Na': 11,
    'Mg': 12,
    'Al': 13,
    'Si': 14,
    'P': 15,
    'S': 16,
    'Cl': 17,
    'Ar': 18,
    'K': 19,
    'Ca': 20,
    'Sc': 21,
    'Ti': 22,
    'V': 23,
    'Cr': 24,
    'Mn': 25,
    'Fe': 26,
    'Co': 27,
    'Ni': 28,
    'Cu': 29,
    'Zn': 30,
    'Ga': 31,
    'Ge': 32,
    'As': 33,
    'Se': 34,
    'Br': 35,
    'Kr': 36,
    'Rb': 37,
    'Sr': 38,
    'Y': 39,
    'Zr': 40,
    'Nb': 41,
    'Mo': 42,
    'Tc': 43,
    'Ru': 44,
    'Rh': 45,
    'Pd': 46,
    'Ag': 47,
    'Cd': 48,
    'In': 49,
    'Sn': 50,
    'Sb': 51,
    'Te': 52,
    'I': 53,
    'Xe': 54,
    'Cs': 55,
    'Ba': 56,
    'La': 57,
    'Ce': 58,
    'Pr': 59,
    'Nd': 60,
    'Pm': 61,
    'Sm': 62,
    'Eu': 63,
    'Gd': 64,
    'Tb': 65,
    'Dy': 66,
    'Ho': 67,
    'Er': 68,
    'Tm': 69,
    'Yb': 70,
    'Lu': 71,
    'Hf': 72,
    'Ta': 73,
    'W': 74,
    'Re': 75,
    'Os': 76,
    'Ir': 77,
    'Pt': 78,
    'Au': 79,
    'Hg': 80,
    'Tl': 81,
    'Pb': 82,
    'Bi': 83,
    'Po': 84,
    'At': 85,
    'Rn': 86,
    'Fr': 87,
    'Ra': 88,
    'Ac': 89,
    'Th': 90,
    'Pa': 91,
    'U': 92,
    'Np': 93,
    'Pu': 94,
    'Am': 95,
    'Cm': 96,
    'Bk': 97,
    'Cf': 98,
    'Es': 99,
    'Fm': 100,
    'Md': 101,
    'No': 102,
    'Lr': 103,
    'Rf': 104,
    'Db': 105,
    'Sg': 106,
    'Bh': 107,
    'Hs': 108,
    'Mt': 109,
    'Ds': 110,
    'Rg': 111,
    'Cn': 112,
    'Nh': 113,
    'Fl': 114,
    'Mc': 115,
    'Lv': 116,
    'Ts': 117,
    'Og': 118,
}

z_to_symbol = {
    1: 'H',
    2: 'He',
    3: 'Li',
    4: 'Be',
    5: 'B',
    6: 'C',
    7: 'N',
    8: 'O',
    9: 'F',
    10: 'Ne',
    11: 'Na',
    12: 'Mg',
    13: 'Al',
    14: 'Si',
    15: 'P',
    16: 'S',
    17: 'Cl',
    18: 'Ar',
    19: 'K',
    20: 'Ca',
    21: 'Sc',
    22: 'Ti',
    23: 'V',
    24: 'Cr',
    25: 'Mn',
    26: 'Fe',
    27: 'Co',
    28: 'Ni',
    29: 'Cu',
    30: 'Zn',
    31: 'Ga',
    32: 'Ge',
    33: 'As',
    34: 'Se',
    35: 'Br',
    36: 'Kr',
    37: 'Rb',
    38: 'Sr',
    39: 'Y',
    40: 'Zr',
    41: 'Nb',
    42: 'Mo',
    43: 'Tc',
    44: 'Ru',
    45: 'Rh',
    46: 'Pd',
    47: 'Ag',
    48: 'Cd',
    49: 'In',
    50: 'Sn',
    51: 'Sb',
    52: 'Te',
    53: 'I',
    54: 'Xe',
    55: 'Cs',
    56: 'Ba',
    57: 'La',
    58: 'Ce',
    59: 'Pr',
    60: 'Nd',
    61: 'Pm',
    62: 'Sm',
    63: 'Eu',
    64: 'Gd',
    65: 'Tb',
    66: 'Dy',
    67: 'Ho',
    68: 'Er',
    69: 'Tm',
    70: 'Yb',
    71: 'Lu',
    72: 'Hf',
    73: 'Ta',
    74: 'W',
    75: 'Re',
    76: 'Os',
    77: 'Ir',
    78: 'Pt',
    79: 'Au',
    80: 'Hg',
    81: 'Tl',
    82: 'Pb',
    83: 'Bi',
    84: 'Po',
    85: 'At',
    86: 'Rn',
    87: 'Fr',
    88: 'Ra',
    89: 'Ac',
    90: 'Th',
    91: 'Pa',
    92: 'U',
    93: 'Np',
    94: 'Pu',
    95: 'Am',
    96: 'Cm',
    97: 'Bk',
    98: 'Cf',
    99: 'Es',
    100: 'Fm',
    101: 'Md',
    102: 'No',
    103: 'Lr',
    104: 'Rf',
    105: 'Db',
    106: 'Sg',
    107: 'Bh',
    108: 'Hs',
    109: 'Mt',
    110: 'Ds',
    111: 'Rg',
    112: 'Cn',
    113: 'Nh',
    114: 'Fl',
    115: 'Mc',
    116: 'Lv',
    117: 'Ts',
    118: 'Og',
}

z_to_name = {
    1: 'Hydrogen',
    2: 'Helium',
    3: 'Lithium',
    4: 'Beryllium',
    5: 'Boron',
    6: 'Carbon',
    7: 'Nitrogen',
    8: 'Oxygen',
    9: 'Fluorine',
    10: 'Neon',
    11: 'Sodium',
    12: 'Magnesium',
    13: 'Aluminum, Aluminium',
    14: 'Silicon',
    15: 'Phosphorus',
    16: 'Sulfur',
    17: 'Chlorine',
    18: 'Argon',
    19: 'Potassium',
    20: 'Calcium',
    21: 'Scandium',
    22: 'Titanium',
    23: 'Vanadium',
    24: 'Chromium',
    25: 'Manganese',
    26: 'Iron',
    27: 'Cobalt',
    28: 'Nickel',
    29: 'Copper',
    30: 'Zinc',
    31: 'Gallium',
    32: 'Germanium',
    33: 'Arsenic',
    34: 'Selenium',
    35: 'Bromine',
    36: 'Krypton',
    37: 'Rubidium',
    38: 'Strontium',
    39: 'Yttrium',
    40: 'Zirconium',
    41: 'Niobium',
    42: 'Molybdenum',
    43: 'Technetium',
    44: 'Ruthenium',
    45: 'Rhodium',
    46: 'Palladium',
    47: 'Silver',
    48: 'Cadmium',
    49: 'Indium',
    50: 'Tin',
    51: 'Antimony',
    52: 'Tellurium',
    53: 'Iodine',
    54: 'Xenon',
    55: 'Cesium',
    56: 'Barium',
    57: 'Lanthanum',
    58: 'Cerium',
    59: 'Praseodymium',
    60: 'Neodymium',
    61: 'Promethium',
    62: 'Samarium',
    63: 'Europium',
    64: 'Gadolinium',
    65: 'Terbium',
    66: 'Dysprosium',
    67: 'Holmium',
    68: 'Erbium',
    69: 'Thulium',
    70: 'Ytterbium',
    71: 'Lutetium',
    72: 'Hafnium',
    73: 'Tantalum',
    74: 'Tungsten',
    75: 'Rhenium',
    76: 'Osmium',
    77: 'Iridium',
    78: 'Platinum',
    79: 'Gold',
    80: 'Mercury',
    81: 'Thallium',
    82: 'Lead',
    83: 'Bismuth',
    84: 'Polonium',
    85: 'Astatine',
    86: 'Radon',
    87: 'Francium',
    88: 'Radium',
    89: 'Actinium',
    90: 'Thorium',
    91: 'Protactinium',
    92: 'Uranium',
    93: 'Neptunium',
    94: 'Plutonium',
    95: 'Americium',
    96: 'Curium',
    97: 'Berkelium',
    98: 'Californium',
    99: 'Einsteinium',
    100: 'Fermium',
    101: 'Mendelevium',
    102: 'Nobelium',
    103: 'Lawrencium',
    104: 'Rutherfordium',
    105: 'Dubnium',
    106: 'Seaborgium',
    107: 'Bohrium',
    108: 'Hassium',
    109: 'Meitnerium',
    110: 'Darmstadtium',
    111: 'Roentgenium',
    112: 'Copernicium',
    113: 'Nihonium',
    114: 'Flerovium',
    115: 'Moscovium',
    116: 'Livermorium',
    117: 'Tennessine',
    118: 'Oganesson',
}
