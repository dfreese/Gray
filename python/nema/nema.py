import numpy as np

def sinogram_space(p1, p2):
    """Transforms two detector coordinates (x,y,z) into an angle, theta, and
    a displacement.  Theta = 0 is the x axis.  The range of theta is
    [-pi/2, pi/2).
    """
    p1 = np.atleast_2d(np.asarray(p1, dtype=float))
    p2 = np.atleast_2d(np.asarray(p2, dtype=float))
    if p1.shape[1] != 3:
        p1 = p1.T
    if p2.shape[1] != 3:
        p2 = p2.T

    delta = p1 - p2
    mask = np.array((1.0, 1.0, 0.0))
    length = np.linalg.norm(delta * mask, axis=1, keepdims=True)
    delta /= length;

    # the angle we use is perpendicular to delta.  For this we use
    # (y, -x).  To get distance, project either of the points onto the angle.
    s = p1[:, 1] * delta[:, 0] - p1[:, 0] * delta[:, 1]
    alpha = np.arctan2(delta[:, 0], -delta[:, 1])
    # Force the output to be [-pi/2, pi/2) so we don't duplicate the vertical
    # angle.
    s[alpha < -np.pi / 2] *= -1
    alpha[alpha < -np.pi / 2] += np.pi
    s[alpha >= np.pi / 2] *= -1
    alpha[alpha >= np.pi / 2] -= np.pi
    return alpha, s

def displacement(point, theta):
    """Takes an (x, y) coordinate and translates it into a displacement within
    the sinogram for a given set of theta in radians.
    """
    point = np.atleast_2d(point)
    if point.shape[0] != 2:
        point = point.T
    theta = np.asanyarray(theta).ravel()
    # This makes the unit vector that we are projecting onto consistent with
    # sinogram_space, that theta = 0 points towards (x, y) = (1, 0), and the
    # projection unit vector for theta = 0 is towards theta = -pi / 2.
    unit_vec = np.stack((np.cos(theta), -np.sin(theta)))
    disp = (point * unit_vec).sum(0)
    return disp

def aligned_sinogram(sino):
    sino = np.asarray(sino).copy()
    mid = sino.shape[1] / 2
    for idx, max_idx in zip(range(sino.shape[0]), sino.argmax(1)):
        sino[idx, :] = np.roll(sino[idx, :], mid - max_idx)
    return sino

def aligned_sum(sino):
    return aligned_sinogram(sino).sum(0)

def nema_counts(prompt_sino, delay_sino):
    true_cut_cm = 2.0
    # Subtract out the randoms
    corrected_sum = prompt_sino.aligned_sum() - delay_sino.aligned_sum()
    # Add in the 20.0mm point where we cut between definitely scatter and
    # scatter/true
    centers_with_cut = np.sort(np.concatenate(
        (prompt_sino.centers_dist, (-true_cut_cm, true_cut_cm,))))
    # This will give us the exact values back for all of the points, plus the
    # linearly interpolated points for the values on the true cut
    counts_with_cut = np.interp(
        centers_with_cut, prompt_sino.centers_dist, corrected_sum)

    # Then add the first point onto the end of the array, with the appropriate
    # step, so that we can use trapz to integrate over the array.
    centers_with_cut = np.concatenate(
        (centers_with_cut, (centers_with_cut[-1] + prompt_sino.step_dist,)))
    counts_with_cut = np.concatenate((counts_with_cut, (counts_with_cut[0],)))
    # Calculate the pixel number, since we're dealing with bin counts, instead
    # of a pdf so we can't integrate using trapz with distance units.
    centers_vox_with_cut = centers_with_cut / prompt_sino.step_dist

    # This will select all of the points that not in the center of the
    # sinogram.  It includes the edges to give us the linear interoplation
    # scatter estimate across the center section, as specified by the standard.
    # np.trapz will take care of the uneven spacing of bins.
    scat_select = ((centers_with_cut <= -true_cut_cm) |
                   (centers_with_cut >= true_cut_cm))

    # This should just be the same as corrected_sum.sum()
    prompt = np.trapz(counts_with_cut, centers_vox_with_cut)
    # Now get the scatter estimate for the entire bin
    scat = np.trapz(counts_with_cut[scat_select],
                    centers_vox_with_cut[scat_select])
    true = prompt - scat
    rand = delay_sino.data.sum()

    return true, scat, rand


class Sinogram(object):
    def __init__(self, p1, p2, bins_theta, bins_dist, range_dist=None):
        """
        """
        alpha, s = sinogram_space(p1, p2)
        self.range_theta = (-np.pi / 2, np.pi / 2)
        if range_dist is None:
            range_dist = np.abs(s).max()
        else:
            range_dist = float(range_dist)
        # enforce that there is a center bin
        if (bins_dist % 2) == 0:
            bins_dist += 1
        self.range_dist = (-range_dist, range_dist)
        self.data, self.edges_theta, self.edges_dist = np.histogram2d(
            alpha, s,
            bins=(bins_theta, bins_dist),
            range=(self.range_theta, self.range_dist))
        self.bins_theta = bins_theta
        self.bins_dist = bins_dist
        self.centers_theta = (
            self.edges_theta[:-1] + np.diff(self.edges_theta) / 2.0)
        self.centers_dist = (
            self.edges_dist[:-1] + np.diff(self.edges_dist) / 2.0)
        self.extent = (self.range_dist +
                       tuple(x * 180 / np.pi for x in self.range_theta))
        self.aspect = ((self.extent[1] - self.extent[0]) /
                       (self.extent[3] - self.extent[2]))
        self.step_theta = self.centers_theta[1] - self.centers_theta[0]
        self.step_dist = self.centers_dist[1] - self.centers_dist[0]

    def aligned(self):
        return aligned_sinogram(self.data)

    def aligned_sum(self):
        return aligned_sum(self.data)
