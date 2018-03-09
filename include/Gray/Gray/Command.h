/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef COMMAND_H
#define COMMAND_H

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
    bool operator==(const std::string& val) const;
    std::string Join() const;
    std::string JoinAll() const;
private:
    bool is_error = false;
    std::string err_msg;
};

#endif // COMMAND_H
