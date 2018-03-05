/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include "gtest/gtest.h"
#include <cstdio>
#include <fstream>
#include <memory>
#include <string>
#include <sstream>
#include <vector>
#include "Gray/Gray/Syntax.h"

TEST(SplitTest, EmptyString) {
    std::vector<std::string> val = Syntax::Split("", " ");
    std::vector<std::string> exp;
    EXPECT_EQ(val, exp);
}

TEST(SplitTest, Basic) {
    std::vector<std::string> val = Syntax::Split("hello world", " ");
    std::vector<std::string> exp({"hello", "world"});
    EXPECT_EQ(val, exp);
}

TEST(SplitTest, EmptySplit) {
    std::vector<std::string> val = Syntax::Split("hello  world", " ");
    std::vector<std::string> exp({"hello", "world"});
    EXPECT_EQ(val, exp);
}

TEST(SplitTest, MultipleDelim) {
    std::vector<std::string> val = Syntax::Split("hello, world", " ,");
    std::vector<std::string> exp({"hello", "world"});
    EXPECT_EQ(val, exp);
}

TEST(SplitTest, ManyVal) {
    std::vector<std::string> val = Syntax::Split("  it was the best of times  ", " ,");
    std::vector<std::string> exp({"it", "was", "the", "best", "of", "times"});
    EXPECT_EQ(val, exp);
}

TEST(CommandTest, BasicSplit) {
    Command cmd("    begin_repeat 5 # ignored comment");
    ASSERT_EQ(cmd.tokens.size(), 2);
    EXPECT_EQ(cmd.tokens[0], "begin_repeat");
    EXPECT_EQ(cmd.tokens[1], "5");
}

TEST(CommandTest, MarkError) {
    std::string err_msg("there are four lights!");
    Command cmd("begin_repeat 5 # ignored comment");
    cmd.MarkError(err_msg);
    EXPECT_EQ(cmd.ErrorMsg(), err_msg);
    EXPECT_TRUE(cmd.IsError());
}

TEST(SyntaxTest, SingleCommand) {
    std::stringstream ss;
    ss << "log_positron\n";

    std::string filename = "test.dat";
    auto cmds = Syntax::ParseCommands(ss, filename);
    ASSERT_EQ(cmds.size(), 1);
    const Command& cmd = cmds.front();
    EXPECT_EQ(cmd.filename, filename);
    EXPECT_EQ(cmd.line, 1);
    EXPECT_FALSE(cmd.IsError());
    std::vector<std::string> exp({"log_positron"});
    EXPECT_EQ(cmd.tokens, exp);
}

TEST(SyntaxTest, LastBeginRepeatEmpty) {
    std::list<Command> cmds;
    auto iter = Syntax::LastBeginRepeat(cmds.rbegin(), cmds.rend());
    ASSERT_EQ(cmds.rend(), iter);
}

TEST(SyntaxTest, LastBeginRepeatSingle) {
    std::list<Command> cmds;
    cmds.emplace_back("begin_repeat 3");
    auto iter = Syntax::LastBeginRepeat(cmds.rbegin(), cmds.rend());
    EXPECT_EQ((*iter).tokens[0], "begin_repeat");
    EXPECT_EQ((*iter).tokens[1], "3");
}

TEST(SyntaxTest, LastBeginRepeatMultiple) {
    std::list<Command> cmds;
    cmds.emplace_back("begin_repeat 2");
    cmds.emplace_back("begin_repeat 3");
    auto iter = Syntax::LastBeginRepeat(cmds.rbegin(), cmds.rend());
    EXPECT_EQ((*iter).tokens[0], "begin_repeat");
    EXPECT_EQ((*iter).tokens[1], "3");
}

TEST(SyntaxTest, LastBeginRepeatDifferent) {
    std::list<Command> cmds;
    cmds.emplace_back("begin_repeat 2");
    cmds.emplace_back("begin_repeat 3");
    cmds.emplace_back("test");
    cmds.emplace_back("check");
    cmds.emplace_back("include");
    auto iter = Syntax::LastBeginRepeat(cmds.rbegin(), cmds.rend());
    EXPECT_EQ((*iter).tokens[0], "begin_repeat");
    EXPECT_EQ((*iter).tokens[1], "3");
}

TEST(SyntaxTest, LastBeginRepeatNone) {
    std::list<Command> cmds;
    cmds.emplace_back("test");
    cmds.emplace_back("check");
    cmds.emplace_back("include");
    auto iter = Syntax::LastBeginRepeat(cmds.rbegin(), cmds.rend());
    EXPECT_EQ(iter, cmds.rend());
}

TEST(SyntaxTest, FirstEndRepeatNone) {
    std::list<Command> cmds;
    cmds.emplace_back("test");
    cmds.emplace_back("check");
    cmds.emplace_back("include");
    cmds.emplace_back("log_positron");
    auto iter = Syntax::FirstEndRepeat(cmds.begin(), cmds.end());
    EXPECT_EQ(iter, cmds.end());
}

TEST(SyntaxTest, FirstEndRepeatDifferent) {
    std::list<Command> cmds;
    cmds.emplace_back("test");
    cmds.emplace_back("check");
    cmds.emplace_back("include");
    // end_repeat shouldn't have fields after it necessarily, but we don't
    // exclude it explicitly
    cmds.emplace_back("end_repeat 2");
    cmds.emplace_back("end_repeat 3");
    cmds.emplace_back("log_positron");
    auto iter = Syntax::FirstEndRepeat(cmds.begin(), cmds.end());
    EXPECT_EQ((*iter).tokens[0], "end_repeat");
    EXPECT_EQ((*iter).tokens[1], "2");
}

TEST(SyntaxTest, UnrollRepeatEmpty) {
    std::list<Command> cmds;
    cmds.emplace_back("begin_repeat 2");
    cmds.emplace_back("end_repeat");
    Command push = Command("push");
    Command pop = Command("pop");

    auto unrolled = Syntax::UnrollRepeatBasic(cmds.begin(), cmds.end());
    ASSERT_EQ(unrolled.size(), 4);
    std::list<Command>::iterator it = unrolled.begin();
    for (int ii = 0; ii < 2; ++ii) {
        EXPECT_EQ((*it++).tokens, push.tokens);
        EXPECT_EQ((*it++).tokens, pop.tokens);
    }
}

TEST(SyntaxTest, UnrollRepeatBasic) {
    std::list<Command> cmds;
    cmds.emplace_back("begin_repeat 2");
    cmds.emplace_back("k 0.0 0.0 0.0 1.0 1.0 1.0");
    cmds.emplace_back("end_repeat");

    Command push = Command("push");
    Command ref = *std::next(cmds.begin());
    Command pop = Command("pop");

    auto unrolled = Syntax::UnrollRepeatBasic(cmds.begin(), cmds.end());
    ASSERT_EQ(unrolled.size(), 6);
    std::list<Command>::iterator it = unrolled.begin();
    for (int ii = 0; ii < 2; ++ii) {
        EXPECT_EQ((*it++).tokens, push.tokens);
        EXPECT_EQ((*it++).tokens, ref.tokens);
        EXPECT_EQ((*it++).tokens, pop.tokens);
    }
}

TEST(SyntaxTest, UnrollRepeatGridNone) {
    std::list<Command> cmds;
    cmds.emplace_back("begin_repeat grid 1 2 2 1.0 1.0 1.0");
    cmds.emplace_back("end_repeat");

    Command push = Command("push");
    Command pop = Command("pop");

    auto unrolled = Syntax::UnrollRepeatGrid(cmds.begin(), cmds.end());
    ASSERT_EQ(unrolled.size(), 12);
    std::list<Command>::iterator it = unrolled.begin();
    for (int ii = 0; ii < 2; ++ii) {
        for (int jj = 0; jj < 2; ++jj) {
            EXPECT_EQ((*it++).tokens, push.tokens);
            // should be the translate command
            EXPECT_EQ((*it++).tokens.front(), "t");
            EXPECT_EQ((*it++).tokens, pop.tokens);
        }
    }
}

TEST(SyntaxTest, UnrollRepeatGrid) {
    std::list<Command> cmds;
    cmds.emplace_back("begin_repeat grid 1 2 2 1.0 1.0 1.0");
    cmds.emplace_back("k 0.0 0.0 0.0 1.0 1.0 1.0");
    cmds.emplace_back("end_repeat");

    Command push = Command("push");
    Command ref = *std::next(cmds.begin());
    Command pop = Command("pop");

    auto unrolled = Syntax::UnrollRepeatGrid(cmds.begin(), cmds.end());
    ASSERT_EQ(unrolled.size(), 2 * 2 * 4);
    std::list<Command>::iterator it = unrolled.begin();
    for (int jj = 0; jj < 2; ++jj) {
        for (int ii = 0; ii < 2; ++ii) {
            EXPECT_EQ((*it++).tokens, push.tokens);
            // should be the translate command
            EXPECT_EQ((*it).tokens[0], "t");
            EXPECT_EQ(std::stod((*it).tokens[1]), 0.0);
            EXPECT_EQ(std::stod((*it).tokens[2]), -0.5 + ii * 1.0);
            EXPECT_EQ(std::stod((*it).tokens[3]), -0.5 + jj * 1.0);
            ++it;
            EXPECT_EQ((*it++).tokens, ref.tokens);
            EXPECT_EQ((*it++).tokens, pop.tokens);
        }
    }
}

TEST(SyntaxTest, UnrollRepeatRotate) {
    std::list<Command> cmds;
    cmds.emplace_back("begin_repeat rotate 3 30.0 0.0 0.0 1.0");
    cmds.emplace_back("k 0.0 0.0 0.0 1.0 1.0 1.0");
    cmds.emplace_back("end_repeat");

    Command push = Command("push");
    Command ref = *std::next(cmds.begin());
    Command pop = Command("pop");

    auto unrolled = Syntax::UnrollRepeatRotate(cmds.begin(), cmds.end());
    ASSERT_EQ(unrolled.size(), 3 * 4);
    std::list<Command>::iterator it = unrolled.begin();
    for (int ii = 0; ii < 3; ++ii) {
        EXPECT_EQ((*it++).tokens, push.tokens);
        // should be the rotate command
        EXPECT_EQ((*it).tokens[0], "raxis");
        EXPECT_EQ(std::stod((*it).tokens[1]), 0.0);
        EXPECT_EQ(std::stod((*it).tokens[2]), 0.0);
        EXPECT_EQ(std::stod((*it).tokens[3]), 1.0);
        EXPECT_EQ(std::stod((*it).tokens[4]), ii * 30.0);
        ++it;
        EXPECT_EQ((*it++).tokens, ref.tokens);
        EXPECT_EQ((*it++).tokens, pop.tokens);
    }
}

TEST(SyntaxTest, HandleRepeatsNone) {
    std::list<Command> cmds;
    Syntax::HandleRepeats(cmds);
    ASSERT_EQ(cmds.size(), 0);
}

TEST(SyntaxTest, HandleRepeatsUnpaired) {
    std::list<Command> cmds;
    cmds.emplace_back("begin_repeat 2");
    Syntax::HandleRepeats(cmds);
    ASSERT_EQ(cmds.size(), 1);
    EXPECT_TRUE(cmds.front().IsError());
}

TEST(SyntaxTest, HandleRepeatsUnpairedEnd) {
    std::list<Command> cmds;
    cmds.emplace_back("end_repeat");
    Syntax::HandleRepeats(cmds);
    ASSERT_EQ(cmds.size(), 1);
    EXPECT_TRUE(cmds.front().IsError());
}

TEST(SyntaxTest, HandleRepeatsPairedEmpty) {
    std::list<Command> cmds;
    cmds.emplace_back("log_positron");
    cmds.emplace_back("begin_repeat 2");
    cmds.emplace_back("end_repeat");
    Command ref = cmds.front();
    Syntax::HandleRepeats(cmds);
    ASSERT_EQ(cmds.size(), 1);
    EXPECT_EQ(cmds.front().tokens, ref.tokens);
}

TEST(SyntaxTest, HandleRepeatsPaired) {
    std::list<Command> cmds;
    cmds.emplace_back("log_positron");
    cmds.emplace_back("begin_repeat 2");
    cmds.emplace_back("k 0.0 0.0 0.0 1.0 1.0 1.0");
    cmds.emplace_back("end_repeat");
    Command push = Command("push");
    // Copy the "k" command
    Command ref = *std::next(cmds.begin(), 2);
    Command pop = Command("pop");

    Syntax::HandleRepeats(cmds);
    ASSERT_EQ(cmds.size(), 7);
    std::list<Command>::iterator it = cmds.begin();
    EXPECT_EQ((*it++).tokens, std::vector<std::string>({"log_positron"}));
    for (int ii = 0; ii < 2; ++ii) {
        EXPECT_EQ((*it++).tokens, push.tokens);
        EXPECT_EQ((*it++).tokens, ref.tokens);
        EXPECT_EQ((*it++).tokens, pop.tokens);
    }
}

TEST(SyntaxTest, HandleRepeatsNested) {
    std::list<Command> cmds;
    cmds.emplace_back("log_positron");
    cmds.emplace_back("begin_repeat 1");
    cmds.emplace_back("begin_repeat 2");
    cmds.emplace_back("k 0.0 0.0 0.0 1.0 1.0 1.0");
    cmds.emplace_back("end_repeat");
    cmds.emplace_back("end_repeat");
    cmds.emplace_back("log_positron");
    Command push = Command("push");
    // Copy the "k" command
    Command ref = *std::next(cmds.begin(), 3);
    Command pop = Command("pop");

    Syntax::HandleRepeats(cmds);
    ASSERT_EQ(cmds.size(), 10);
    auto it = std::next(cmds.begin());
    for (int ii = 0; ii < 1; ++ii) {
        EXPECT_EQ((*it++).tokens, push.tokens);
        for (int jj = 0; jj < 2; ++jj) {
            EXPECT_EQ((*it++).tokens, push.tokens);
            EXPECT_EQ((*it++).tokens, ref.tokens);
            EXPECT_EQ((*it++).tokens, pop.tokens);
        }
        EXPECT_EQ((*it++).tokens, pop.tokens);
    }
}

TEST(SyntaxTest, HandleIncludesNotThere) {
    std::string test_file = "tmp_xyz_test_file.dff";
    // Don't create the file, in fact, make sure it isn't there.
    // If you're depending on a file with a name like that for something
    // important, shame on you.
    std::remove(test_file.c_str());

    std::list<Command> cmds;
    cmds.emplace_back("include " + test_file);

    Syntax::HandleIncludes(cmds);
    ASSERT_EQ(cmds.size(), 1);
    EXPECT_TRUE(cmds.front().IsError());
}

TEST(SyntaxTest, HandleIncludes) {
    std::string test_file = "tmp_xyz_test_file.dff";
    {
        std::ofstream output(test_file);
        output << "log_positron\n\n\n sphere 0.0 0.0 0.0 1.0\n m LSO\n\n\n";
    }

    std::list<Command> cmds;
    cmds.emplace_back("include " + test_file);

    Syntax::HandleIncludes(cmds);
    std::remove(test_file.c_str());

    ASSERT_EQ(cmds.size(), 3);
    std::vector<std::string> exp_first({"log_positron", "sphere", "m"});
    std::vector<int> exp_line({1, 4, 5});
    std::vector<std::string> first;
    std::vector<int> line;
    for (auto& cmd : cmds) {
        first.emplace_back(cmd.tokens.front());
        line.emplace_back(cmd.line);
        EXPECT_EQ(cmd.filename, test_file);
    }
    EXPECT_EQ(first, exp_first);
    EXPECT_EQ(line, exp_line);
}

TEST(SyntaxTest, SingleCommandLeadingBlankLine) {
    std::stringstream ss;
    ss << "\n\nlog_positron\n";

    std::string filename = "test.dat";
    auto cmds = Syntax::ParseCommands(ss, filename);
    ASSERT_EQ(cmds.size(), 1);
    ASSERT_EQ(cmds.front().tokens.size(), 1);
    EXPECT_EQ(cmds.front().tokens[0], "log_positron");
    EXPECT_FALSE(cmds.front().IsError());
    EXPECT_EQ(cmds.front().filename, filename);
    EXPECT_EQ(cmds.front().line, 3);
}

TEST(SyntaxTest, MultipleCommandsLeadingBlankLine) {
    std::stringstream ss;
    ss << "\n\n\nlog_positron\n   \n  \n  sphere 0.0 0.0 0.0 1.0\n m LSO";

    std::string filename = "test.dat";
    auto cmds = Syntax::ParseCommands(ss, filename);

    std::vector<std::string> first;
    std::vector<int> line;
    for (auto& cmd : cmds) {
        first.emplace_back(cmd.tokens.front());
        line.emplace_back(cmd.line);
    }

    std::vector<std::string> exp_first({"log_positron", "sphere", "m"});
    std::vector<int> exp_line({4, 7, 8});
    EXPECT_EQ(first, exp_first);
    EXPECT_EQ(line, exp_line);
}

TEST(SyntaxTest, ParseCommandsWithRepeat) {
    std::stringstream ss;
    ss << "\n\n\n begin_repeat 3\n echo    test\n  end_repeat  \n\n\n";

    std::string filename = "test.dat";
    auto cmds = Syntax::ParseCommands(ss, filename);

    std::vector<std::string> first;
    std::vector<int> line;
    for (auto& cmd : cmds) {
        first.emplace_back(cmd.tokens.front());
        line.emplace_back(cmd.line);
    }

    std::vector<std::string> exp_first({
            "push", "echo", "pop",
            "push", "echo", "pop",
            "push", "echo", "pop"});
    std::vector<int> exp_line({
            -1, 5, -1,
            -1, 5, -1,
            -1, 5, -1});
    EXPECT_EQ(first, exp_first);
    EXPECT_EQ(line, exp_line);
}
