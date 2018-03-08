/*
 * Gray: A Ray Tracing-based Monte Carlo Simulator for PET
 *
 * Copyright (c) 2018, David Freese, Peter Olcott, Sam Buss, Craig Levin
 *
 * This software is distributed under the terms of the MIT License unless
 * otherwise noted.  See LICENSE for further details.
 *
 */
#ifndef LOAD_H
#define LOAD_H
#include <vector>
#include "Gray/Gray/Syntax.h"

class Config;

namespace Load {
bool ConfigCommands(std::vector<Command>& cmds, Config& config);
bool ConfigCommand(Command& cmd, Config & config);
}

#endif // LOAD_H
