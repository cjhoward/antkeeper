/*
 * Copyright (C) 2021  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ANTKEEPER_DEBUG_CONSOLE_COMMANDS_HPP
#define ANTKEEPER_DEBUG_CONSOLE_COMMANDS_HPP

#include "game/context.hpp"
#include <string>

namespace debug {

/// Console commands
namespace cc {

std::string echo(std::string text);

std::string exit(game::context* ctx);

std::string scrot(game::context* ctx);

std::string cue(game::context* ctx, float t, std::string command);

} // namespace cc
} // namespace debug

#endif // ANTKEEPER_DEBUG_CONSOLE_COMMANDS_HPP
