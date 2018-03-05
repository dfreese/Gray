/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include "Gray/Gray/Syntax.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

Command::Command(const std::string& line) :
    tokens(Syntax::Split(line.substr(0, line.find_first_of("#")), " "))
{}

std::string Command::ErrorMsg() const {
    return (err_msg);
}

void Command::MarkError(const std::string& err_msg) {
    this->err_msg = err_msg;
    this->is_error = true;
}

bool Command::IsError() const {
    return (is_error);
}

/*!
 * https://stackoverflow.com/a/7408245/2465202
 *
 */
std::vector<std::string> Syntax::Split(
        const std::string& text, const std::string& delims)
{
    std::vector<std::string> tokens;
    std::size_t start = text.find_first_not_of(delims), end = 0;

    while((end = text.find_first_of(delims, start)) != std::string::npos) {
        tokens.push_back(text.substr(start, end - start));
        start = text.find_first_not_of(delims, end);
    }
    if(start != std::string::npos) {
        tokens.push_back(text.substr(start));
    }

    return tokens;
}

std::list<Command> Syntax::ParseCommands(
        std::istream& input, const std::string& filename)
{
    std::list<Command> commands;
    std::string line;
    int line_no = 0;
    while (std::getline(input, line)) {
        ++line_no;
        commands.emplace_back(Command(line));
        commands.back().line = line_no;
        commands.back().filename = filename;
    }
    Syntax::PruneEmpty(commands);
    Syntax::HandleIncludes(commands);
    Syntax::HandleRepeats(commands);
    return (commands);
}

std::list<Command> Syntax::ParseCommands(const Command& include) {
    if (include.tokens.size() == 1) {
        std::list<Command> err({include});
        err.back().MarkError("Include filename not specified");
        return (err);
    } else if (include.tokens.size() > 2) {
        std::list<Command> err({include});
        err.back().MarkError("Too make options for include specified");
        return (err);
    }
    // TODO: make include filename relative to file it was in.
    std::string filename = include.tokens[1];
    std::ifstream input(filename);
    if (!input) {
        std::list<Command> err({include});
        err.back().MarkError("Unable to open " + filename);
        return (err);
    }
    return (Syntax::ParseCommands(input, filename));
}

/*!
 * Convinience function for the top level scene file.  Could be a list return,
 * but since there will no longer be insertions into the middle, then we try
 * to keep it simple.
 */
std::vector<Command> Syntax::ParseCommands(const std::string& filename) {
    // Generate a dummy upper level include command.
    auto cmds = Syntax::ParseCommands(Command("include " + filename));
    return (std::vector<Command>(cmds.begin(), cmds.end()));
}

void Syntax::PruneEmpty(std::list<Command>& commands) {
    commands.remove_if([](const Command& c) {
            return (c.tokens.empty()); });
}

void Syntax::HandleIncludes(std::list<Command>& commands) {
    for (auto it = commands.begin(); it != commands.end(); ++it) {
        const Command& cmd = *it;
        // ParseCommands(Command include) returns a list with a single include
        // command marked as an error if the include fails.  Make sure we don't
        // get into an infinite loop by checking that the command isn't an
        // error first.
        // TODO: make sure that circular includes aren't followed.  They'll
        // crash the program currently.
        if ((cmd.tokens.front() == "include") && (!cmd.IsError())) {
            // Delete the current include and insert whatever is included
            // in it's place.
            // Create the new list to be inserted.
            auto include = Syntax::ParseCommands(cmd);
            // Delete the include command used from the list.  Then insert the
            // newly generated list into that same position, and reset it to be
            // the first element of the newly inserted commands, since we
            // deleted the current it.
            it = commands.insert(commands.erase(it),
                    include.begin(), include.end());
        }
    }
}

std::list<Command>::reverse_iterator Syntax::LastBeginRepeat(
        std::list<Command>::reverse_iterator rbeg,
        std::list<Command>::reverse_iterator rend)
{
    auto riter = std::find_if(rbeg, rend, [](const Command& cmd) {
            return (cmd.tokens.front() == "begin_repeat"); });
    return (riter);
}

std::list<Command>::iterator Syntax::FirstEndRepeat(
        std::list<Command>::iterator beg,
        std::list<Command>::iterator end)
{
    return (std::find_if(beg, end, [](const Command& cmd) {
                return (cmd.tokens.front() == "end_repeat"); }));
}

std::list<Command> Syntax::UnrollRepeatBasic(
        std::list<Command>::iterator beg,
        std::list<Command>::iterator end)
{
    std::list<Command> unrolled;
    const Command& desc = *beg;
    if (desc.tokens.size() != 2) {
        unrolled.emplace_back(*beg);
        unrolled.back().MarkError("Invalid repeat syntax");
        return (unrolled);
    }
    int no_repeats;
    try {
        no_repeats = std::stoi(desc.tokens[1]);
    } catch (std::exception& e) {
        unrolled.emplace_back(*beg);
        unrolled.back().MarkError("Invalid repeat number");
        return (unrolled);
    }
    for (int ii = 0; ii < no_repeats; ++ii) {
        // Wrap the inside of the repeats in a push/pop
        unrolled.emplace_back("push");
        // And add in all of the values inside of begin_repeat/end_repeat
        unrolled.insert(unrolled.end(), std::next(beg), std::prev(end));
        unrolled.emplace_back("pop");
    }
    return (unrolled);
}

std::list<Command> Syntax::UnrollRepeatGrid(
        std::list<Command>::iterator beg,
        std::list<Command>::iterator end)
{
    std::list<Command> unrolled;
    const Command& desc = *beg;
    if (desc.tokens.size() != 8) {
        unrolled.emplace_back(*beg);
        unrolled.back().MarkError("Invalid repeat syntax");
        return (unrolled);
    }
    int no_repeats[3];
    try {
        no_repeats[0] = std::stoi(desc.tokens[2]);
        no_repeats[1] = std::stoi(desc.tokens[3]);
        no_repeats[2] = std::stoi(desc.tokens[4]);
    } catch (std::exception& e) {
        unrolled.emplace_back(*beg);
        unrolled.back().MarkError("Invalid repeat number");
        return (unrolled);
    }
    double step[3];
    try {
        step[0] = std::stod(desc.tokens[5]);
        step[1] = std::stod(desc.tokens[6]);
        step[2] = std::stod(desc.tokens[7]);
    } catch (std::exception& e) {
        unrolled.emplace_back(*beg);
        unrolled.back().MarkError("Invalid repeat step");
        return (unrolled);
    }
    if ((no_repeats[0] < 1) || (no_repeats[1] < 1) || (no_repeats[2] < 1)) {
        unrolled.emplace_back(*beg);
        unrolled.back().MarkError("Number of repeats should be positive");
        return (unrolled);
    }
    double offset[3] = {
        -0.5 * step[0] * (no_repeats[0] - 1),
        -0.5 * step[1] * (no_repeats[1] - 1),
        -0.5 * step[2] * (no_repeats[2] - 1)
    };
    for (int kk = 0; kk < no_repeats[2]; ++kk) {
        for (int jj = 0; jj < no_repeats[1]; ++jj) {
            for (int ii = 0; ii < no_repeats[0]; ++ii) {
                double step_ii[3] = {
                    offset[0] + ii * step[0],
                    offset[1] + jj * step[1],
                    offset[2] + kk * step[2],
                };
                std::stringstream ss;
                ss << "t " << step_ii[0] << " " << step_ii[1] << " "
                   << step_ii[2];
                // Wrap the inside of the repeats in a push/pop
                unrolled.emplace_back("push");
                unrolled.emplace_back(ss.str());
                // And add in all of the values inside of begin_repeat/end_repeat
                unrolled.insert(unrolled.end(), std::next(beg), std::prev(end));
                unrolled.emplace_back("pop");
            }
        }
    }
    return (unrolled);
}

std::list<Command> Syntax::UnrollRepeatRotate(
        std::list<Command>::iterator beg,
        std::list<Command>::iterator end)
{
    std::list<Command> unrolled;
    const Command& desc = *beg;
    if (desc.tokens.size() != 7) {
        unrolled.emplace_back(*beg);
        unrolled.back().MarkError("Invalid repeat syntax");
        return (unrolled);
    }
    int no_repeats;
    try {
        no_repeats = std::stoi(desc.tokens[2]);
    } catch (std::exception& e) {
        unrolled.emplace_back(*beg);
        unrolled.back().MarkError("Invalid repeat number");
        return (unrolled);
    }
    double angle;
    try {
        angle = std::stod(desc.tokens[3]);
    } catch (std::exception& e) {
        unrolled.emplace_back(*beg);
        unrolled.back().MarkError("Invalid repeat angle");
        return (unrolled);
    }
    double axis[3];
    try {
        axis[0] = std::stod(desc.tokens[4]);
        axis[1] = std::stod(desc.tokens[5]);
        axis[2] = std::stod(desc.tokens[6]);
    } catch (std::exception& e) {
        unrolled.emplace_back(*beg);
        unrolled.back().MarkError("Invalid repeat axis");
        return (unrolled);
    }
    for (int ii = 0; ii < no_repeats; ++ii) {
        double angle_ii = ii * angle;
        std::stringstream ss;
        ss << "raxis " << axis[0] << " " << axis[1] << " " << axis[2] << " "
           << angle_ii;
        // Wrap the inside of the repeats in a push/pop
        unrolled.emplace_back("push");
        unrolled.emplace(unrolled.end(), ss.str());
        // And add in all of the values inside of begin_repeat/end_repeat
        unrolled.insert(unrolled.end(), std::next(beg), std::prev(end));
        unrolled.emplace_back("pop");
    }
    return (unrolled);
}

std::list<Command> Syntax::UnrollRepeat(
        std::list<Command>::iterator beg,
        std::list<Command>::iterator end)
{
    const Command& desc = *beg;
    if (desc.tokens.size() == 1) {
        std::list<Command> err;
        err.emplace_back(*beg);
        err.back().MarkError("begin_repeat does not specify repeats");
        return (err);
    }
    // Create the unrolled set of commands
    const std::string& type = desc.tokens[1];
    if (std::next(beg, 2) == end) {
        // If repeat command is empty (i.e. only a begin and end) then return
        // an empty list.
        return (std::list<Command>());
    } else if (type == "grid") {
        return (UnrollRepeatGrid(beg, end));
    } else if (type == "rotate") {
        return (UnrollRepeatRotate(beg, end));
    } else {
        return (UnrollRepeatBasic(beg, end));
    }
}

void Syntax::HandleRepeats(std::list<Command>& commands) {
    // Work from the end of the list to find the most inner repeats first.
    // Unroll those, and then work to the next one until we no longer have
    // repeats in the structure.
    auto beg = Syntax::LastBeginRepeat(commands.rbegin(), commands.rend());
    while (beg != commands.rend()) {
        // In order to convert from a reverse iterator to a forward iterator we
        // need to advance it by one, and then take its base.  We wait until
        // now to convert it into a forward iterator, as we need the rend()
        // condition to make sure there are no begin_repeats left in the list.
        // Converting first would require checking against begin(), which
        // misses begin_repeats at the start of the list.
        auto beg_fwd = std::next(beg).base();
        auto end = Syntax::FirstEndRepeat(beg_fwd, commands.end());
        if (end == commands.end()) {
            (*beg_fwd).MarkError("begin_repeat not paired with end_repeat");
            // Bail because we have an error
            return;
        }
        // Unroll repeat, advancing end so that we, in iterator terms, specify
        // the entire range of the repeat from begin/end.
        auto unrolled = Syntax::UnrollRepeat(beg_fwd, std::next(end));
        // Delete the repeat from the list and add in the unrolled version,
        // but hold onto the position before the insert to continue searching
        // for begin_repeats.
        auto next_beg = commands.insert(commands.erase(beg_fwd, std::next(end)),
                unrolled.begin(), unrolled.end());
        // LastBeginRepeat requires reverse iterators, so convert the fwd iter
        beg = std::reverse_iterator<std::list<Command>::iterator>(next_beg);
        // commands.splice(commands.erase(beg_fwd, std::next(end)), unrolled);
        // Find next repeat
        beg = Syntax::LastBeginRepeat(beg, commands.rend());
    }
    // All valid repeats should have been unrolled.  Since we unroll based on
    // begin_repeat, and mark any of thoe unpaired if needed, the only thing
    // left to check should be unpaired end_repeats.
    auto end = Syntax::FirstEndRepeat(commands.begin(), commands.end());
    while (end != commands.end()) {
        (*end).MarkError("end_repeat not paired with begin_repeat");
        // starting at the next element, look for more unpaired end_repeats
        end = Syntax::FirstEndRepeat(std::next(end), commands.end());
    }
}
