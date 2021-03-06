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

#include "snapping.hpp"
#include "entity/components/collision.hpp"
#include "entity/components/snap.hpp"
#include "entity/components/transform.hpp"
#include "entity/id.hpp"
#include "utility/fundamental-types.hpp"

namespace entity {
namespace system {

snapping::snapping(entity::registry& registry):
	updatable(registry)
{}

void snapping::update(double t, double dt)
{
	registry.view<component::transform, component::snap>().each(
		[&](entity::id entity_id, auto& snap_transform, auto& snap)
		{
			bool intersection = false;
			float a = std::numeric_limits<float>::infinity();
			float3 pick;
			
			geom::ray<float> snap_ray = snap.ray;
			if (snap.relative)
			{
				snap_ray.origin += snap_transform.local.translation;
				snap_ray.direction = snap_transform.local.rotation * snap_ray.direction;
			}

			this->registry.view<component::transform, component::collision>().each(
				[&](entity::id entity_id, auto& collision_transform, auto& collision)
				{
					// Transform ray into local space of collision component
					math::transform<float> inverse_transform = math::inverse(collision_transform.local);
					float3 origin = inverse_transform * snap_ray.origin;
					float3 direction = math::normalize(math::conjugate(collision_transform.local.rotation) * snap_ray.direction);
					geom::ray<float> transformed_ray = {origin, direction};

					// Broad phase AABB test
					auto aabb_result = geom::ray_aabb_intersection(transformed_ray, collision.bounds);
					if (!std::get<0>(aabb_result))
					{
						return;
					}

					// Narrow phase mesh test
					auto mesh_result = collision.mesh_accelerator.query_nearest(transformed_ray);
					if (mesh_result)
					{
						intersection = true;
						if (mesh_result->t < a)
						{
							a = mesh_result->t;
							pick = snap_ray.extrapolate(a);
						}
					}
				});

			if (intersection)
			{
				snap_transform.local.translation = pick;
				snap_transform.warp = snap.warp;
				
				if (snap.autoremove)
				{
					this->registry.remove<component::snap>(entity_id);
				}
			}
		});
}

} // namespace system
} // namespace entity
