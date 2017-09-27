#include  "gtest/gtest.h"
#include "VrMath/LinearR3.h"
#include "Random/Transform.h"
#include <cmath>

TEST(AcolinearityTest, ZeroPreservesAndNegates) {
    VectorR3 unit_z(0, 0, 1);
    VectorR3 neg_z(0, 0, -1);

    for (double angle: {0.0, 0.04, 1.0, 20.0}) {
        for (double rotation: {0.0, 0.01, 0.5, 1.0}) {
            VectorR3 ret = Transform::Acolinearity(unit_z, angle, rotation, 0);
            ASSERT_DOUBLE_EQ(ret.Norm(), 1.0);
            ASSERT_EQ(neg_z, ret);
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
        ASSERT_DOUBLE_EQ(ret.Norm(), 1.0);
        ASSERT_EQ(cal_angle, std::cos(angle));
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
        ASSERT_DOUBLE_EQ(ret.Norm(), 1.0);
        ASSERT_EQ(cal_angle, std::cos(gauss_variable));
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
    ASSERT_DOUBLE_EQ(z.Norm(), 1.0);
    ASSERT_DOUBLE_EQ(z.z, 1.0);

    VectorR3 neg_z = Transform::UniformSphere(0.0, 0.0);
    ASSERT_DOUBLE_EQ(neg_z.Norm(), 1.0);
    ASSERT_DOUBLE_EQ(neg_z.z, -1.0);

    VectorR3 x = Transform::UniformSphere(0.0, 0.5);
    ASSERT_DOUBLE_EQ(x.Norm(), 1.0);
    ASSERT_DOUBLE_EQ(x.x, 1.0);

    VectorR3 y = Transform::UniformSphere(1.0, 0.5);
    ASSERT_DOUBLE_EQ(y.Norm(), 1.0);
    ASSERT_DOUBLE_EQ(y.x, 1.0);
}

TEST(UniformAnnulusCylinderTest, Height) {
    const double theta = 0;
    const double radius = 1;
    const double height_rand = 1;
    for (double height: {0.1, 0.5, 30.0}) {
        VectorR3 ret = Transform::UniformAnnulusCylinder(height, radius, theta,
                                                         height_rand);
        ASSERT_EQ(ret, VectorR3(radius, 0, height/2));
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
        ASSERT_LT((ret - exp).Norm(), 1e-16);
    }
}
