add_executable(test_gray
    test_main.cpp
    test_linear.cpp
    test_math.cpp
    test_transform.cpp
    )
target_link_libraries(test_gray gammaray)
target_link_libraries(test_gray gtest_main)


add_test(NAME test COMMAND test_gray)
