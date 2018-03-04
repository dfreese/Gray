/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include  "gtest/gtest.h"
#include "Gray/Output/IO.h"
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
