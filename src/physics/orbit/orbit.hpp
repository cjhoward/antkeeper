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

#ifndef ANTKEEPER_PHYSICS_ORBIT_HPP
#define ANTKEEPER_PHYSICS_ORBIT_HPP

namespace physics {

/**
 * Orbital mechanics.
 *
 * @see Curtis, H. D. (2005). *Orbital mechanics for engineering students*. Amsterdam: Elsevier Butterworth Heinemann.
 */
namespace orbit {}

} // namespace physics

#include "physics/orbit/elements.hpp"
#include "physics/orbit/frames.hpp"
#include "physics/orbit/kepler.hpp"
#include "physics/orbit/state.hpp"

#endif // ANTKEEPER_PHYSICS_ORBIT_HPP
