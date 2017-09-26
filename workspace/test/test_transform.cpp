#include  "gtest/gtest.h"
#include "VrMath/LinearR3.h"
#include "Random/Transform.h"

TEST(AcolinearityTest, PreservesNorm) {
    ASSERT_TRUE(true);    
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
