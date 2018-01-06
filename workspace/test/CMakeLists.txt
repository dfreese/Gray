add_executable(test_gray
    test_main.cpp
    test_daq.cpp
    test_io.cpp
    test_linear.cpp
    test_mapping.cpp
    test_math.cpp
    test_random.cpp
    test_source.cpp
    test_transform.cpp
    test_viewable.cpp
    )
target_link_libraries(test_gray gammaray)
target_link_libraries(test_gray gtest_main)


add_test(NAME test COMMAND test_gray)
