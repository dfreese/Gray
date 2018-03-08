/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#include "Gray/Gray/File.h"
#include <string>

std::string File::DirWindows(const std::string& file) {
    // Windows can never have a forward or backslash
    auto last_slash = file.find_last_of("\\");
    if (last_slash == std::string::npos) {
        return ("");
    } else {
        return (file.substr(0, last_slash));
    }
}

std::string File::DirPosix(const std::string& file) {
    // POSIX systems can have a backslash in file paths
    auto last_slash = file.find_last_of("/");
    if (last_slash == std::string::npos) {
        return ("");
    } else {
        return (file.substr(0, last_slash));
    }
}

/*!
 * Get the directory of a file on the local platform.  A blank string is
 * returned for a locally referenced file.  Assume it will break with anything
 * complicated on Windows.
 */
std::string File::Dir(const std::string& file) {
#ifdef _WIN32
    return (File::DirWindows(file));
#else
    return (File::DirPosix(file));
#endif
}

std::string File::JoinWindows(const std::string& dir, const std::string& file) {
    if (dir.empty()) {
        return (file);
    } else if (dir.back() == '\\') {
        return (dir + file);
    } else {
        return (dir + "\\" + file);
    }
}

std::string File::JoinPosix(const std::string& dir, const std::string& file) {
    if (dir.empty()) {
        return (file);
    } else if (dir.back() == '/') {
        return (dir + file);
    } else {
        return (dir + "/" + file);
    }
}

/*!
 * Join a directory and file on the local platform.  Assume it will break with
 * anything complicated on Windows.  Replace with std::filesystem when c++17
 * support is more universal, or boost::filesystem if it's causing problems
 * and you want the extra dependency.
 */
std::string File::Join(const std::string& dir, const std::string& file) {
#ifdef _WIN32
    return (File::JoinWindows(dir, file));
#else
    return (File::JoinPosix(dir, file));
#endif
}

