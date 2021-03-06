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

#ifndef ANTKEEPER_ENTITY_SYSTEM_UPDATABLE_HPP
#define ANTKEEPER_ENTITY_SYSTEM_UPDATABLE_HPP

#include "entity/registry.hpp"

namespace entity {
namespace system {

/**
 * Abstract base class for updatable systems.
 */
class updatable
{
public:
	/**
	 * Creates an updatable system.
	 *
	 * @param registry Reference to the registry on which the system will operate.
	 */
	updatable(entity::registry& registry);

	/**
	 * Perform's a system's update() function.
	 *
	 * @param t Total elapsed time, in seconds.
	 * @param dt Delta time, in seconds.
	 * @param registry Entity registry.
	 */
	virtual void update(double t, double dt) = 0;
	
protected:
	/// Registry on which the system operate
	entity::registry& registry;
};

} // namespace system
} // namespace entity

#endif // ANTKEEPER_ENTITY_SYSTEM_UPDATABLE_HPP
