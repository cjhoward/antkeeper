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

#ifndef ANTKEEPER_ENTITY_SYSTEM_CONTROL_HPP
#define ANTKEEPER_ENTITY_SYSTEM_CONTROL_HPP

#include "entity/systems/updatable.hpp"
#include "entity/id.hpp"
#include "event/event-handler.hpp"
#include "event/input-events.hpp"
#include "event/window-events.hpp"
#include "input/control.hpp"
#include "input/control-set.hpp"
#include "scene/model-instance.hpp"
#include "utility/fundamental-types.hpp"
#include "scene/camera.hpp"

class nest;

namespace entity {
namespace system {

class camera;

class control:
	public updatable,
	public event_handler<mouse_moved_event>,
	public event_handler<window_resized_event>
{
public:
	control(entity::registry& registry);
	
	virtual void update(double t, double dt);
	
	void set_invert_mouse_x(bool invert);
	void set_invert_mouse_y(bool invert);

	void set_camera_system(system::camera* camera_system);
	void set_nest(::nest* nest);
	void set_tool(scene::model_instance* tool);
	void set_flashlight(entity::id entity_id);
	void set_camera_subject(entity::id entity_id);
	
	void set_viewport(const float4& viewport);
	void set_underworld_camera(scene::camera* camera);

	input::control_set* get_control_set();
	input::control* get_move_forward_control();
	input::control* get_move_back_control();
	input::control* get_move_left_control();
	input::control* get_move_right_control();
	input::control* get_rotate_ccw_control();
	input::control* get_rotate_cw_control();
	input::control* get_tilt_up_control();
	input::control* get_tilt_down_control();
	input::control* get_zoom_in_control();
	input::control* get_zoom_out_control();
	input::control* get_adjust_camera_control();
	input::control* get_ascend_control();
	input::control* get_descend_control();
	input::control* get_toggle_view_control();
	input::control* get_tool_menu_control();
	input::control* get_equip_lens_control();
	input::control* get_equip_brush_control();
	input::control* get_equip_forceps_control();
	input::control* get_equip_marker_control();
	input::control* get_equip_container_control();
	input::control* get_equip_twig_control();
	input::control* get_next_marker_control();
	input::control* get_previous_marker_control();
	input::control* get_use_tool_control();
	input::control* get_fast_forward_control();
	input::control* get_rewind_control();
	input::control* get_exposure_increase_control();
	input::control* get_exposure_decrease_control();

private:
	virtual void handle_event(const mouse_moved_event& event);
	virtual void handle_event(const window_resized_event& event);
	
	input::control_set control_set;
	input::control move_forward_control;
	input::control move_back_control;
	input::control move_left_control;
	input::control move_right_control;
	input::control rotate_ccw_control;
	input::control rotate_cw_control;
	input::control tilt_up_control;
	input::control tilt_down_control;
	input::control zoom_in_control;
	input::control zoom_out_control;
	input::control adjust_camera_control;
	input::control ascend_control;
	input::control descend_control;
	input::control toggle_view_control;
	input::control tool_menu_control;
	input::control equip_lens_control;
	input::control equip_brush_control;
	input::control equip_forceps_control;
	input::control equip_marker_control;
	input::control equip_container_control;
	input::control equip_twig_control;
	input::control next_marker_control;
	input::control previous_marker_control;
	input::control use_tool_control;
	input::control fast_forward_control;
	input::control rewind_control;
	input::control exposure_increase_control;
	input::control exposure_decrease_control;

	float zoom_speed;
	float min_elevation;
	float max_elevation;
	float near_focal_distance;
	float far_focal_distance;
	float near_movement_speed;
	float far_movement_speed;
	float near_fov;
	float far_fov;
	float near_clip_near;
	float far_clip_near;
	float near_clip_far;
	float far_clip_far;
	
	float timestep;
	float zoom;
	system::camera* camera_system;
	::nest* nest;
	scene::model_instance* tool;
	float2 mouse_position;
	float4 viewport;
	
	entity::id flashlight_entity;
	entity::id camera_subject_entity;
	scene::camera* underworld_camera;
	
	float mouse_angle;
	float old_mouse_angle;
	float flashlight_turns;
	float flashlight_turns_i;
	float flashlight_turns_f;
};

inline input::control_set* control::get_control_set()
{
	return &control_set;
}

inline input::control* control::get_move_forward_control()
{
	return &move_forward_control;
}

inline input::control* control::get_move_back_control()
{
	return &move_back_control;
}

inline input::control* control::get_move_left_control()
{
	return &move_left_control;
}

inline input::control* control::get_move_right_control()
{
	return &move_right_control;
}

inline input::control* control::get_rotate_ccw_control()
{
	return &rotate_ccw_control;
}

inline input::control* control::get_rotate_cw_control()
{
	return &rotate_cw_control;
}

inline input::control* control::get_tilt_up_control()
{
	return &tilt_up_control;
}

inline input::control* control::get_tilt_down_control()
{
	return &tilt_down_control;
}

inline input::control* control::get_zoom_in_control()
{
	return &zoom_in_control;
}

inline input::control* control::get_zoom_out_control()
{
	return &zoom_out_control;
}

inline input::control* control::get_adjust_camera_control()
{
	return &adjust_camera_control;
}

inline input::control* control::get_ascend_control()
{
	return &ascend_control;
}

inline input::control* control::get_descend_control()
{
	return &descend_control;
}

inline input::control* control::get_toggle_view_control()
{
	return &toggle_view_control;
}

inline input::control* control::get_tool_menu_control()
{
	return &tool_menu_control;
}

inline input::control* control::get_equip_lens_control()
{
	return &equip_lens_control;
}

inline input::control* control::get_equip_brush_control()
{
	return &equip_brush_control;
}

inline input::control* control::get_equip_forceps_control()
{
	return &equip_forceps_control;
}

inline input::control* control::get_equip_marker_control()
{
	return &equip_marker_control;
}

inline input::control* control::get_equip_container_control()
{
	return &equip_container_control;
}

inline input::control* control::get_equip_twig_control()
{
	return &equip_twig_control;
}

inline input::control* control::get_next_marker_control()
{
	return &next_marker_control;
}

inline input::control* control::get_previous_marker_control()
{
	return &previous_marker_control;
}

inline input::control* control::get_use_tool_control()
{
	return &use_tool_control;
}

inline input::control* control::get_fast_forward_control()
{
	return &fast_forward_control;
}

inline input::control* control::get_rewind_control()
{
	return &rewind_control;
}

inline input::control* control::get_exposure_increase_control()
{
	return &exposure_increase_control;
}

inline input::control* control::get_exposure_decrease_control()
{
	return &exposure_decrease_control;
}

} // namespace system
} // namespace entity

#endif // ANTKEEPER_ENTITY_SYSTEM_CONTROL_HPP

