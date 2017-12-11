import unittest
import numpy as np
from nema.nema import sinogram_space

class TestSinogramSpace(unittest.TestCase):
    def test_x_axis(self):
        p0 = np.array((
            (1.0, 1.0, 0.0),
            (1.0, -1.0, 0.0),
            (-1.0, 1.0, 0.0),
            (-1.0, -1.0, 0.0),
            (1.0, 1.0, 1.0),
            (1.0, -1.0, 1.0),
            (-1.0, 1.0, 1.0),
            (-1.0, -1.0, 1.0),
        ))
        p1 = np.array((
            (1.0, -1.0, 0.0),
            (1.0, 1.0, 0.0),
            (-1.0, -1.0, 0.0),
            (-1.0, 1.0, 0.0),
            (1.0, -1.0, 1.0),
            (1.0, 1.0, 1.0),
            (-1.0, -1.0, 1.0),
            (-1.0, 1.0, 1.0),
        ))
        exp_thetas = np.array((
            0.0,
            0.0,
            0.0,
            0.0,
            0.0,
            0.0,
            0.0,
            0.0,
        ))
        exp_dists = np.array((
            1.0,
            1.0,
            -1.0,
            -1.0,
            1.0,
            1.0,
            -1.0,
            -1.0,
        ))
        thetas, dists = sinogram_space(p0, p1)
        for exp_theta, theta in zip(exp_thetas, thetas):
            self.assertEqual(exp_theta, theta)
        for exp_dist, dist in zip(exp_dists, dists):
            self.assertEqual(exp_dist, dist)

    def test_y_axis(self):
        p0 = np.array((
            (-1.0, 1.0, 0.0),
            (1.0, 1.0, 0.0),
            (-1.0, -1.0, 0.0),
            (1.0, -1.0, 0.0),
        ))
        p1 = np.array((
            (1.0, 1.0, 0.0),
            (-1.0, 1.0, 0.0),
            (1.0, -1.0, 0.0),
            (-1.0, -1.0, 0.0),
        ))
        exp_thetas = np.array((
            -np.pi / 2,
            -np.pi / 2,
            -np.pi / 2,
            -np.pi / 2,
        ))
        exp_dists = np.array((
            -1.0,
            -1.0,
            1.0,
            1.0,
        ))
        thetas, dists = sinogram_space(p0, p1)
        for exp_theta, theta in zip(exp_thetas, thetas):
            self.assertEqual(exp_theta, theta)
        for exp_dist, dist in zip(exp_dists, dists):
            self.assertEqual(exp_dist, dist)
