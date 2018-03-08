/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */
#include <string>

namespace File {
std::string DirWindows(const std::string& file);
std::string DirPosix(const std::string& file);
std::string Dir(const std::string& file);
std::string JoinWindows(const std::string& dir, const std::string& file);
std::string JoinPosix(const std::string& dir, const std::string& file);
std::string Join(const std::string& dir, const std::string& file);
}

