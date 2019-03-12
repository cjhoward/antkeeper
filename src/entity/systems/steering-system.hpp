/*
 * Copyright (C) 2017-2019  Christopher J. Howard
 *
 * This file is part of Antkeeper Source Code.
 *
 * Antkeeper Source Code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper Source Code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper Source Code.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef STEERING_SYSTEM_HPP
#define STEERING_SYSTEM_HPP

#include "../entity-group.hpp"
#include "../components/steering-component.hpp"
#include "../system.hpp"

#include <emergent/emergent.hpp>
using namespace Emergent;

typedef EntityGroup<SteeringComponent> SteeringGroup;

class SteeringSystem: public
	System,
	SteeringGroup::Observer
{
public:
	SteeringSystem(ComponentManager* componentManager);
	virtual ~SteeringSystem();
	
	/**
	 * Calculates the steering force for each steering component using weighted truncated running sums with prioritization.
	 */
	virtual void update(float t, float dt);

private:
	SteeringGroup boids;

	virtual void memberRegistered(const SteeringGroup::Member* member);
	virtual void memberUnregistered(const SteeringGroup::Member* member);
};

#endif // STEERING_SYSTEM_HPP
