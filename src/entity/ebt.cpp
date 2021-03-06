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

#include "entity/ebt.hpp"
#include "entity/components/transform.hpp"
#include <iostream>

namespace entity {
namespace ebt {

status print(context& context, const std::string& text)
{
	std::cout << text;
	return status::success;
}

status print_eid(context& context)
{
	std::cout << static_cast<std::size_t>(context.entity_id) << std::endl;
	return status::success;
}

status warp_to(context& context, float x, float y, float z)
{
	auto& transform = context.registry->get<component::transform>(context.entity_id);
	transform.local.translation = {x, y, z};
	transform.warp = true;
	return status::success;
}

bool is_carrying_food(const context& context)
{
	return false;
}

} // namespace ebt
} // namespace entity
