#include  "gtest/gtest.h"
#include "Output/IO.h"
#include <sstream>
#include <string>

TEST(CommentLineTest, PoundComment) {
    std::stringstream ss("  testing should be here # should be gone");
    std::string exp("  testing should be here ");
    std::string result;
    IO::GetLineCommented(ss, result);
    EXPECT_EQ(exp, result);
}

TEST(CommentLineWsTest, PoundCommentWs) {
    std::stringstream ss("  testing should be here # should be gone");
    std::string exp("testing should be here ");
    std::string result;
    IO::GetLineCommentLeadWs(ss, result);
    EXPECT_EQ(exp, result);
}
