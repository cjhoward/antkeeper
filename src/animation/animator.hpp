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

#ifndef ANTKEEPER_ANIMATOR_HPP
#define ANTKEEPER_ANIMATOR_HPP

#include <set>

class animation_base;

class animator
{
public:
	/**
	 * Progresses all active animations by @p dt.
	 *
	 * @param dt Delta time by which the animations will be progressed.
	 */
	void animate(double dt);
	
	void add_animation(animation_base* animation);
	void remove_animation(animation_base* animation);
	void remove_animations();
	
private:
	std::set<animation_base*> animations;
};

#endif // ANTKEEPER_ANIMATOR_HPP
