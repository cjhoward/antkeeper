/*
 * Copyright (C) 2020  Christopher J. Howard
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

#include "control-system.hpp"
#include "input/control.hpp"
#include "orbit-cam.hpp"
#include "scene/camera.hpp"
#include "geometry/intersection.hpp"
#include "animation/easings.hpp"
#include "nest.hpp"
#include <vmq/vmq.hpp>

using namespace vmq::operators;

template <class T>
static inline T lerp(const T& x, const T& y, float a)
{
	return x * (1.0f - a) + y * a;
}

template <class T>
static inline T log_lerp(const T& x, const T& y, float a)
{
	T log_x = std::log(x);
	T log_y = std::log(y);
	return std::exp(lerp(log_x, log_y, a));
}

control_system::control_system():
	timestep(0.0f),
	zoom(0.0f)
{
	control_set.add_control(&move_forward_control);
	control_set.add_control(&move_back_control);
	control_set.add_control(&move_right_control);
	control_set.add_control(&move_left_control);
	control_set.add_control(&rotate_ccw_control);
	control_set.add_control(&rotate_cw_control);
	control_set.add_control(&tilt_up_control);
	control_set.add_control(&tilt_down_control);
	control_set.add_control(&zoom_in_control);
	control_set.add_control(&zoom_out_control);
	control_set.add_control(&adjust_camera_control);
	control_set.add_control(&ascend_control);
	control_set.add_control(&descend_control);
	control_set.add_control(&toggle_view_control);
	control_set.add_control(&tool_menu_control);
	
	// Set deadzone at 15% for all controls
	const std::list<control*>* controls = control_set.get_controls();
	for (control* control: *controls)
	{
		control->set_deadzone(0.15f);
	}

	zoom_speed = 4.0f; //1
	min_elevation = vmq::radians(-85.0f);
	max_elevation = vmq::radians(85.0f);
	near_focal_distance = 2.0f;
	far_focal_distance = 200.0f;
	near_movement_speed = 10.0f;
	far_movement_speed = 80.0f;
	near_fov = vmq::radians(80.0f);
	far_fov = vmq::radians(35.0f);
	near_clip_near = 0.1f;
	far_clip_near = 5.0f;
	near_clip_far = 100.0f;
	far_clip_far = 2000.0f;

	nest = nullptr;
	orbit_cam = nullptr;
}

void control_system::update(float dt)
{
	this->timestep = dt;

	if (adjust_camera_control.is_active() && !adjust_camera_control.was_active())
	{
	}

	// Determine zoom
	if (zoom_in_control.is_active())
		zoom += zoom_speed * dt * zoom_in_control.get_current_value();
	if (zoom_out_control.is_active())
		zoom -= zoom_speed * dt * zoom_out_control.get_current_value();
	zoom = std::max<float>(0.0f, std::min<float>(1.0f, zoom));

	float focal_distance = log_lerp(near_focal_distance, far_focal_distance, 1.0f - zoom);

	float fov = log_lerp(near_fov, far_fov, 1.0f - zoom);

	//float elevation_factor = (orbit_cam->get_target_elevation() - min_elevation) / max_elevation;
	//fov = log_lerp(near_fov, far_fov, elevation_factor);
	float clip_near = log_lerp(near_clip_near, far_clip_near, 1.0f - zoom);
	float clip_far = log_lerp(near_clip_far, far_clip_far, 1.0f - zoom);
	float movement_speed = log_lerp(near_movement_speed * dt, far_movement_speed * dt, 1.0f - zoom);

	orbit_cam->set_target_focal_distance(focal_distance);
	orbit_cam->get_camera()->set_perspective(fov, orbit_cam->get_camera()->get_aspect_ratio(), clip_near, clip_far);

	const float rotation_speed = 2.0f * dt;
	float rotation = 0.0f;
	if (rotate_ccw_control.is_active())
		rotation += rotate_ccw_control.get_current_value() * rotation_speed;
	if (rotate_cw_control.is_active())
		rotation -= rotate_cw_control.get_current_value() * rotation_speed;
	if (rotation)
	{
		orbit_cam->rotate(rotation);
	}

	const float tilt_speed = 2.0f * dt;
	float tilt = 0.0f;
	if (tilt_up_control.is_active())
		tilt -= tilt_up_control.get_current_value() * tilt_speed;
	if (tilt_down_control.is_active())
		tilt += tilt_down_control.get_current_value() * tilt_speed;
	if (tilt)
	{
		orbit_cam->tilt(tilt);
		float elevation = orbit_cam->get_target_elevation();
		elevation = std::min<float>(max_elevation, std::max<float>(min_elevation, elevation));
		orbit_cam->set_target_elevation(elevation);
	}

	float2 movement{0.0f, 0.0f};
	if (move_right_control.is_active())
		movement[0] += move_right_control.get_current_value();
	if (move_left_control.is_active())
		movement[0] -= move_left_control.get_current_value();
	if (move_forward_control.is_active())
		movement[1] -= move_forward_control.get_current_value();
	if (move_back_control.is_active())
		movement[1] += move_back_control.get_current_value();
	
	const float deadzone = 0.01f;
	float magnitude_squared = vmq::length_squared(movement);

	if (magnitude_squared > deadzone)
	{
		if (magnitude_squared > 1.0f)
		{
			movement = vmq::normalize(movement);
		}

		orbit_cam->move(movement * movement_speed);
	}

	float ascention = 0.0f;
	if (ascend_control.is_active())
		ascention += ascend_control.get_current_value();
	if (descend_control.is_active())
		ascention -= descend_control.get_current_value();
	
	if (ascention)
	{
		float old_depth = -orbit_cam->get_target_focal_point()[1];
		
		orbit_cam->set_target_focal_point(orbit_cam->get_target_focal_point() + float3{0.0f, ascention * movement_speed, 0.0f});

		if (nest)
		{
			float3 focal_point = orbit_cam->get_target_focal_point();
			float depth = -focal_point[1];
			
			float delta_shaft_angle = nest->get_shaft_angle(*nest->get_central_shaft(), depth) - nest->get_shaft_angle(*nest->get_central_shaft(), old_depth);
			
			//orbit_cam->set_target_azimuth(orbit_cam->get_target_azimuth() - delta_shaft_angle);
			orbit_cam->set_target_focal_point(nest->get_shaft_position(*nest->get_central_shaft(), depth));
		}
	}

	orbit_cam->update(dt);

	camera* camera = orbit_cam->get_camera();

	float3 pick_near = camera->unproject({mouse_position[0], viewport[3] - mouse_position[1], 0.0f}, viewport);
	float3 pick_far = camera->unproject({mouse_position[0], viewport[3] - mouse_position[1], 1.0f}, viewport);
	float3 pick_direction = vmq::normalize(pick_far - pick_near);
	ray<float> picking_ray = {pick_near, pick_direction};
	plane<float> ground_plane = {float3{0, 1, 0}, 0.0f};

	auto intersection_result = ray_plane_intersection(picking_ray, ground_plane);
	if (std::get<0>(intersection_result))
	{
		float3 pick = picking_ray.extrapolate(std::get<1>(intersection_result));
		if (tool)
			tool->set_translation(pick);
	}

	if (toggle_view_control.is_active() && !toggle_view_control.was_active())
	{
	}
}

void control_system::set_orbit_cam(::orbit_cam* orbit_cam)
{
	this->orbit_cam = orbit_cam;
}

void control_system::set_nest(::nest* nest)
{
	this->nest = nest;
}

void control_system::set_tool(model_instance* tool)
{
	this->tool = tool;
}

void control_system::set_viewport(const float4& viewport)
{
	this->viewport = viewport;
}

void control_system::handle_event(const mouse_moved_event& event)
{
	if (adjust_camera_control.is_active())
	{
		bool invert_x = true;
		bool invert_y = false;

		float rotation_factor = event.dx;
		float elevation_factor = event.dy;
		
		if (invert_x)
		{
			rotation_factor *= -1.0f;
		}
		if (invert_y)
		{
			elevation_factor *= -1.0f;
		}

		float rotation = vmq::radians(22.5f) * rotation_factor * timestep;
		float elevation = orbit_cam->get_target_elevation() + elevation_factor * 0.25f * timestep;
		elevation = std::min<float>(max_elevation, std::max<float>(min_elevation, elevation));

		orbit_cam->rotate(rotation);
		orbit_cam->set_target_elevation(elevation);
	}
	else if (!adjust_camera_control.was_active())
	{
		mouse_position[0] = event.x;
		mouse_position[1] = event.y;
	}
}
