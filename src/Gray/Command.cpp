/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include "Gray/Gray/Command.h"
#include <algorithm>
#include <numeric>
#include <string>
#include <sstream>
#include <vector>
#include "Gray/Gray/String.h"

Command::Command(const std::string& line) :
    tokens(String::Split(line.substr(0, line.find_first_of("#")), " "))
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

bool Command::operator==(const std::string& val) const {
    if (tokens.empty()) {
        return (false);
    } else {
        return (tokens.front() == val);
    }
}

/*!
 * Concatenates tokens[1] through the end with a space.
 */
std::string Command::Join() const {
    if (tokens.size() < 2) {
        return ("");
    }
    return (std::accumulate(
                std::next(tokens.begin(), 2),
                tokens.end(), tokens[1],
                [](const std::string& a, const std::string& b) {
                    return (a + ' ' + b);
                    }));
}

std::string Command::JoinAll() const {
    if (tokens.size() < 1) {
        return ("");
    }
    return (std::accumulate(
                std::next(tokens.begin()),
                tokens.end(), tokens[0],
                [](const std::string& a, const std::string& b) {
                    return (a + ' ' + b);
                    }));
}
