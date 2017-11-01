#include  "gtest/gtest.h"
#include "VrMath/LinearR3.h"
#include "Random/Transform.h"
#include <cmath>

TEST(DeflectionTest, RangeWraps) {
    const VectorR3 unit_z(0, 0, 1);
    const double theta = 0.3; // pick arbitrary angle
    const VectorR3 start = Transform::Deflection(unit_z, std::cos(theta), 0);
    const VectorR3 end = Transform::Deflection(unit_z, std::cos(theta), 1);
    // There may be some numerical error at 1.0.
    EXPECT_LT((end - start).Norm(), 1e-16);
}

TEST(DeflectionTest, CorrectAngle) {
    VectorR3 unit_z(0, 0, 1);
    const double rot_variable = 0.3; // pick arbitrary number [0,1]
    for (double angle: {0.0, 0.04, 1.0, 20.0}) {
        VectorR3 ret = Transform::Deflection(unit_z, std::cos(angle), rot_variable);
        // get the cos of the angle between the vectors from the dot product
        double cal_angle = unit_z ^ ret;
        EXPECT_DOUBLE_EQ(ret.Norm(), 1.0);
        EXPECT_LT(std::abs(cal_angle - std::cos(angle)), 1e-6);
    }
}

TEST(AcolinearityTest, ZeroPreservesAndNegates) {
    VectorR3 unit_z(0, 0, 1);
    VectorR3 neg_z(0, 0, -1);

    for (double angle: {0.0, 0.04, 1.0, 20.0}) {
        for (double rotation: {0.0, 0.01, 0.5, 1.0}) {
            VectorR3 ret = Transform::Acolinearity(unit_z, angle, rotation, 0);
            EXPECT_DOUBLE_EQ(ret.Norm(), 1.0);
            EXPECT_EQ(neg_z, ret);
        }
    }
}

TEST(AcolinearityTest, CorrectAngle) {
    VectorR3 unit_z(0, 0, 1);
    VectorR3 neg_z(0, 0, -1);

    const double gauss_variable = 1.0;
    const double rot_variable = 0.3; // pick arbitrary number [0,1]
    for (double angle: {0.0, 0.04, 1.0, 20.0}) {
        VectorR3 ret = Transform::Acolinearity(unit_z, angle, rot_variable,
                                               gauss_variable);
        // get the cos of the angle between the vectors from the dot product
        double cal_angle = unit_z ^ ret.Negate();
        EXPECT_DOUBLE_EQ(ret.Norm(), 1.0);
        EXPECT_LT(std::abs(cal_angle - std::cos(angle)), 1e-6);
    }
}

/*!
 * Since the angle and the gaussian random variable are just multiplied
 * together, but kept separate to indicate the different functionality, run
 * essentially the CorrectAngle test again.
 */
TEST(AcolinearityTest, GaussScaling) {
    VectorR3 unit_z(0, 0, 1);
    VectorR3 neg_z(0, 0, -1);

    const double angle = 1.0;
    const double rot_variable = 0.3; // pick arbitrary number [0,1]
    for (double gauss_variable: {0.0, 0.04, 1.0, 20.0}) {
        VectorR3 ret = Transform::Acolinearity(unit_z, angle, rot_variable,
                                               gauss_variable);
        // get the cos of the angle between the vectors from the dot product
        double cal_angle = unit_z ^ ret.Negate();
        EXPECT_DOUBLE_EQ(ret.Norm(), 1.0);
        EXPECT_LT(std::abs(cal_angle - std::cos(gauss_variable)), 1e-6);
    }
}

TEST(AcolinearityTest, VaryingReferences) {
    VectorR3 ref(0, 0, 1);
    const double gauss_variable = 1.0;
    const double angle = 0.5;
    const double rot_variable = 0.1;
    for (size_t ii = 0; ii < 10; ++ii) {
        VectorR3 ret = Transform::Acolinearity(ref, angle, rot_variable,
                                               gauss_variable);
        // get the cos of the angle between the vectors from the dot product
        double cal_angle = ref ^ ret.Negate();

        // Allow some inaccuracy as we are performing this with the same data
        // many times.
        ASSERT_LT(std::abs(ret.Norm() - 1.0), 1e-14);
        ASSERT_LT(std::abs(cal_angle - std::cos(angle)), 1e-14);

        // Restart the process from where we are currently at to move around
        // the unit circle.
        ref = ret;
    }
}

TEST(UniformSphereTest, IsUnit) {
    VectorR3 z = Transform::UniformSphere(0.0, 1.0);
    EXPECT_DOUBLE_EQ(z.Norm(), 1.0);
    EXPECT_DOUBLE_EQ(z.z, 1.0);

    VectorR3 neg_z = Transform::UniformSphere(0.0, 0.0);
    EXPECT_DOUBLE_EQ(neg_z.Norm(), 1.0);
    EXPECT_DOUBLE_EQ(neg_z.z, -1.0);

    VectorR3 x = Transform::UniformSphere(0.0, 0.5);
    EXPECT_DOUBLE_EQ(x.Norm(), 1.0);
    EXPECT_DOUBLE_EQ(x.x, 1.0);

    VectorR3 y = Transform::UniformSphere(1.0, 0.5);
    EXPECT_DOUBLE_EQ(y.Norm(), 1.0);
    EXPECT_DOUBLE_EQ(y.x, 1.0);
}

TEST(UniformSphereFilledTest, CubicScale) {
    const double theta = 0;
    const double cos_phi = 1.0;

    for (const double radius_cubed: {0.0, 0.1, 0.5, 1.0}) {
        const VectorR3 ret = Transform::UniformSphereFilled(theta, cos_phi, radius_cubed);
        const VectorR3 exp(0.0, 0.0, std::pow(radius_cubed, 1.0 / 3));
        EXPECT_EQ(ret, exp);
    }
}

TEST(UniformCylinderTest, Radius) {
    const double height = 1;
    const double height_rand = 1.0;
    const double rad_rand = 1.0;
    const double theta_rand = 0.0;

    for (double radius: {0.1, 0.5, 30.0}) {
        const VectorR3 ret = Transform::UniformCylinder(height, radius,
                                                        height_rand, rad_rand,
                                                        theta_rand);
        const VectorR3 exp(radius, 0, height / 2);
        EXPECT_EQ(ret, exp);
    }
}

TEST(UniformCylinderTest, RadiusSampling) {
    const double height = 30.0;
    const double height_rand = 0.0;
    const double theta_rand = 0.25; // Transformed to pi / 2
    const double radius = 30.0;

    for (double rad_rand: {0.0, 0.1, 0.5, 1.0}) {
        const VectorR3 ret = Transform::UniformCylinder(height, radius,
                                                        height_rand, rad_rand,
                                                        theta_rand);
        const VectorR3 exp(0, radius * std::sqrt(rad_rand), -height / 2);
        // There may be some error in sin(pi/2) not being exactly zero and the
        // square root.
        EXPECT_LT((ret - exp).Norm(), 1e-14);
    }
}

TEST(UniformAnnulusCylinderTest, Height) {
    const double theta = 0;
    const double radius = 1;
    const double height_rand = 1;
    for (double height: {0.1, 0.5, 30.0}) {
        VectorR3 ret = Transform::UniformAnnulusCylinder(height, radius, theta,
                                                         height_rand);
        EXPECT_EQ(ret, VectorR3(radius, 0, height/2));
    }
}

TEST(UniformAnnulusCylinderTest, HeightRandVar) {
    const double theta = 0.25; // Transformed to PI/2
    const double radius = 1;
    const double height = 30;
    for (double height_rand: {0.0, 0.1, 0.5, 1.0}) {
        VectorR3 ret = Transform::UniformAnnulusCylinder(height, radius, theta,
                                                         height_rand);
        const VectorR3 exp(0, radius, height_rand * height - height/2);
        // There may be some error in sin(pi/2) not being exactly zero.
        EXPECT_LT((ret - exp).Norm(), 1e-16);
    }
}

TEST(UniformRectangleTest, Scaling) {
    const VectorR3 size(1, 1, 1);
    const VectorR3 offset = size / 2.0;
    auto rand_pt_test = {0.0, 0.1, 0.5, 1.0};
    for (double rand_x: rand_pt_test) {
        for (double rand_y: rand_pt_test) {
            for (double rand_z: rand_pt_test) {
                VectorR3 ret = Transform::UniformRectangle(size, rand_x, rand_y, rand_z);
                VectorR3 exp = VectorR3(rand_x, rand_y, rand_z) - offset;
                EXPECT_EQ(ret, exp);
            }
        }
    }
}

TEST(UniformRectangleTest, Size) {
    // Zero to test negative.
    const double rand_x = 0.0;
    const double rand_y = 0.0;
    const double rand_z = 0.0;
    auto size_pt_test = {0.1, 0.5, 30.0};
    for (double size_x: size_pt_test) {
        for (double size_y: size_pt_test) {
            for (double size_z: size_pt_test) {
                const VectorR3 size(size_x, size_y, size_z);
                const VectorR3 offset = size / 2.0;
                VectorR3 ret = Transform::UniformRectangle(size, rand_x, rand_y, rand_z);
                VectorR3 exp = offset - size;
                EXPECT_EQ(ret, exp);
            }
        }
    }
}

TEST(GaussianEnergyBlurTest, ZeroEnergy) {
    const double energy = 0;
    const double rand_gauss = 0;
    for (double eres: {0.0, 0.1, 0.5, 1.0}) {
        const double result = Transform::GaussianEnergyBlur(energy, eres, rand_gauss);
        EXPECT_DOUBLE_EQ(result, 0.0);
    }
}

TEST(GaussianEnergyBlurTest, Scaling) {
    const double energy = 0.100;
    for (const double eres: {0.0, 0.1, 0.5, 1.0}) {
        for (const double rand_gauss: {0.0, 0.1, 0.5, 1.0}) {
            const double result = Transform::GaussianEnergyBlur(energy, eres, rand_gauss);
            const double exp = energy * (1.0 + eres * Transform::fwhm_to_sigma * rand_gauss);
            EXPECT_DOUBLE_EQ(result, exp);
        }
    }
}

TEST(GaussianEnergyBlurInverseSqrtTest, EresScaling) {
    const double ref_energy = 0.511;
    for (const double energy: {0.0, 0.1}) {
        for (const double eres: {0.0, 0.1, 0.5, 1.0}) {
            for (const double rand_gauss: {0.0, 0.1, 0.5, 1.0}) {
                const double exp_eres = (energy > 0) ? eres * (std::sqrt(ref_energy) / std::sqrt(energy)) : 1.0;
                const double exp = Transform::GaussianEnergyBlur(energy, exp_eres, rand_gauss);
                const double result = Transform::GaussianEnergyBlurInverseSqrt(energy, eres,
                                                                               ref_energy, rand_gauss);
                EXPECT_DOUBLE_EQ(result, exp);
            }
        }
    }
}

TEST(GaussianTimeBlurTest, Scaling) {
    const double time = 0;
    for (const double tres: {0.0, 0.1, 0.5, 1.0}) {
        for (const double rand_gauss: {0.0, 0.1, 0.5, 1.0}) {
            const double result = Transform::GaussianBlurTime(time, tres, rand_gauss);
            const double exp = tres * Transform::fwhm_to_sigma * rand_gauss;
            EXPECT_DOUBLE_EQ(result, exp);
        }
    }
}

TEST(GaussianTimeBlurTest, Offset) {
    const double tres = 0;
    const double rand_gauss = 1.0;
    for (const double time: {-30.0, 0.0, 30.0}) {
        const double result = Transform::GaussianBlurTime(time, tres, rand_gauss);
        EXPECT_DOUBLE_EQ(result, time);
    }
}

TEST(SelectionTest, LessThan) {
    EXPECT_EQ(Transform::Selection(0.5, 0.4), true);
}

TEST(SelectionTest, Equal) {
    EXPECT_EQ(Transform::Selection(0.5, 0.5), true);
}

TEST(SelectionTest, Greater) {
    EXPECT_EQ(Transform::Selection(0.5, 0.6), false);
}
