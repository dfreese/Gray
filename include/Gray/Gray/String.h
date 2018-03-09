/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef STRING_H
#define STRING_H

#include <exception>
#include <sstream>
#include <string>
#include <vector>

namespace String {
/*!
 * https://stackoverflow.com/a/7408245/2465202
 *
 */
inline std::vector<std::string> Split(
        const std::string& text, const std::string& delims)
{
    std::vector<std::string> tokens;
    std::size_t start = text.find_first_not_of(delims);
    std::size_t end = 0;

    while ((end = text.find_first_of(delims, start)) != std::string::npos) {
        tokens.push_back(text.substr(start, end - start));
        start = text.find_first_not_of(delims, end);
    }
    if (start != std::string::npos) {
        tokens.push_back(text.substr(start));
    }

    return tokens;
}

inline bool ToDouble(const std::string& text, double& val) {
    try {
        val = std::stod(text);
        return (true);
    } catch (std::exception& e) {
        return (false);
    }
}

inline bool ToInt(const std::string& text, int& val) {
    try {
        val = std::stoi(text);
        return (true);
    } catch (std::exception& e) {
        return (false);
    }
}

/*!
 * Base case of Parse that only checks that the iterators now equal each other
 * indicating we succesfully used the entire range.
 */
template<typename Iter>
bool Parse(Iter begin, Iter end) {
    // Make sure we used all of our values
    return (begin == end);
}

/*!
 * Parsing all of the values individually from the input files was going to be
 * tedious, so here we have a variadic function that takes a range of strings
 * specified by two iterators, and uses stringstream to try and bludgeon those
 * strings into all of the parameters given.
 *
 * The range of the iterators must match the number of parameters given.  If
 * any conversion fails, false is returned immediately.  All the parameters up
 * to the failure will be modified.
 *
 * This works by recursively calling/generating itself until it hits the base
 * case above.
 *
 * If the number of parameters does not equal the range distance, false will
 * also be returned, so don't give it too many strings either.
 *
 * Useful resources on variadic templates and functions:
 * - https://florianjw.de/en/variadic_templates.html
 * - http://jguegant.github.io/blogs/tech/thread-safe-multi-type-map.html
 */
template<typename Iter, typename Head, typename... Tail>
bool Parse(Iter begin, Iter end, Head& head, Tail&... tail) {
    if (begin == end) {
        return (false);
    }
    std::stringstream ss;
    ss << *begin;
    ss >> head;
    if (ss.fail()) {
        return (false);
    }
    return (Parse(std::next(begin), end, tail...));
}

}

#endif // STRING_H
