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

#ifndef ANTKEEPER_ENTITY_SYSTEM_TOOL_HPP
#define ANTKEEPER_ENTITY_SYSTEM_TOOL_HPP

#include "entity/systems/updatable.hpp"
#include "entity/id.hpp"
#include "event/event-handler.hpp"
#include "event/input-events.hpp"
#include "event/window-events.hpp"
#include "utility/fundamental-types.hpp"
#include "animation/spring.hpp"
#include "animation/animation.hpp"
#include "scene/camera.hpp"

class orbit_cam;
class event_dispatcher;

namespace entity {
namespace system {

class tool:
	public updatable,
	public event_handler<mouse_moved_event>,
	public event_handler<window_resized_event>
{
public:
	tool(entity::registry& registry, event_dispatcher* event_dispatcher);
	virtual ~tool();
	virtual void update(double t, double dt);

	void set_camera(const scene::camera* camera);
	void set_orbit_cam(const orbit_cam* camera);
	void set_viewport(const float4& viewport);
	void set_pick(bool enabled);
	void set_sun_direction(const float3& direction);
	
	void set_active_tool(entity::id entity_id);
	
	void set_tool_active(bool active);
	
	entity::id get_active_tool() const;
	
private:
	virtual void handle_event(const mouse_moved_event& event);
	virtual void handle_event(const window_resized_event& event);

	event_dispatcher* event_dispatcher;
	const scene::camera* camera;
	const orbit_cam* orbit_cam;
	float4 viewport;
	float2 mouse_position;
	bool was_pick_enabled;
	bool pick_enabled;
	float3 sun_direction;
	entity::id active_tool;
	bool warp;
	bool tool_active;
	
	
	numeric_spring<float, float> hand_angle_spring;
	numeric_spring<float3, float> pick_spring;
	
	animation<float> descend_animation;
	animation<float> ascend_animation;
	float active_tool_distance;
};

inline entity::id tool::get_active_tool() const
{
	return active_tool;
}

} // namespace system
} // namespace entity

#endif // ANTKEEPER_ENTITY_SYSTEM_TOOL_HPP
