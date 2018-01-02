#include  "gtest/gtest.h"
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include "Daq/Process.h"
#include "Daq/Mapping.h"

TEST(LoadMappingTest, Basic) {
    using DetIdT = Mapping::DetIdT;
    std::stringstream ss("detector block bx by bz\n"
                         "0 0 0 0 0\n"
                         "1 0 1 0 0\n"
                         "2 0 0 1 0\n"
                         "3 0 1 1 0 # has a comment\n"
                         "4 0 0 0 1\n"
                         "   5 0 1 0 1 # has leading whitespace too\n"
                         "6 0 0 1 1\n"
                         "7 0 1 1 1\n");
    const std::vector<DetIdT> det_exp = {0, 1, 2, 3, 4, 5, 6, 7};
    const std::vector<DetIdT> blk_exp = {0, 0, 0, 0, 0, 0, 0, 0};
    const std::vector<DetIdT> bx_exp = {0, 1, 0, 1, 0, 1, 0, 1};
    const std::vector<DetIdT> by_exp = {0, 0, 1, 1, 0, 0, 1, 1};
    const std::vector<DetIdT> bz_exp = {0, 0, 0, 0, 1, 1, 1, 1};
    const std::map<std::string, std::vector<DetIdT>> exp_mapping = {
        {"detector", det_exp},
        {"block", blk_exp},
        {"bx", bx_exp},
        {"by", by_exp},
        {"bz", bz_exp},
    };
    std::map<std::string, std::vector<DetIdT>> mapping;
    const int no_detectors = static_cast<int>(det_exp.size());
    int status = Mapping::LoadMapping(ss, mapping);
    ASSERT_EQ(status, no_detectors);
    EXPECT_EQ(exp_mapping, mapping);
}
