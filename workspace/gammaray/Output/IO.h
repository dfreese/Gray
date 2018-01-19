/*
 * Gray: a Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */

#ifndef io_h
#define io_h

#include <iostream>
#include <string>

namespace IO {
    std::istream& GetLineCommented(std::istream& is,
                                   std::string& str,
                                   char comment = '#');
    std::istream& GetLineCommentLeadWs(std::istream& is,
                                       std::string& str,
                                       char comment = '#');
}

#endif // io_h
