/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef SYNTAX_H
#define SYNTAX_H

#include <iosfwd>
#include <list>
#include <memory>
#include <string>
#include <vector>

class Command {
public:
    std::string filename;
    int line = -1;
    std::vector<std::string> tokens;
    Command(const std::string& line);
    std::string ErrorMsg() const;
    void MarkError(const std::string& err_msg);
    bool IsError() const;
private:
    bool is_error = false;
    std::string err_msg;
};

namespace Syntax {
std::vector<std::string> Split(
        const std::string& text, const std::string& delims);
std::list<Command> ParseCommands(
        std::istream& input, const std::string& filename);
std::list<Command> ParseCommands(const Command& include);
std::vector<Command> ParseCommands(const std::string& filename);
void PruneEmpty(std::list<Command>& commands);
void HandleIncludes(std::list<Command>& commands);

std::list<Command>::reverse_iterator LastBeginRepeat(
        std::list<Command>::reverse_iterator rbeg,
        std::list<Command>::reverse_iterator rend);
std::list<Command>::iterator FirstEndRepeat(
        std::list<Command>::iterator beg,
        std::list<Command>::iterator end);
void HandleRepeats(std::list<Command>& commands);
std::list<Command> UnrollRepeat(
        std::list<Command>::iterator beg,
        std::list<Command>::iterator end);
std::list<Command> UnrollRepeatBasic(
        std::list<Command>::iterator beg,
        std::list<Command>::iterator end);
std::list<Command> UnrollRepeatGrid(
        std::list<Command>::iterator beg,
        std::list<Command>::iterator end);
std::list<Command> UnrollRepeatRotate(
        std::list<Command>::iterator beg,
        std::list<Command>::iterator end);
}

#endif // SYNTAX_H
