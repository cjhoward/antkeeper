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

#include "game/states/splash.hpp"
#include "game/states/play.hpp"
#include "animation/screen-transition.hpp"
#include "animation/ease.hpp"
#include "animation/timeline.hpp"
#include "application.hpp"

namespace game {
namespace state {
namespace splash {

void enter(game::context* ctx)
{
	// Add splash billboard to UI scene
	ctx->ui_scene->add_object(ctx->splash_billboard);
	
	// Setup timing
	const float splash_fade_in_duration = 0.5f;
	const float splash_hang_duration = 2.0f;
	const float splash_fade_out_duration = 0.5f;
	
	// Start fade in
	ctx->fade_transition->transition(splash_fade_in_duration, true, ease<float>::in_quad);
	
	// Crate fade out function
	auto fade_out = [ctx, splash_fade_out_duration]()
	{
		ctx->fade_transition->transition(splash_fade_out_duration, false, ease<float>::out_quad);
	};
	
	// Create change state function
	auto change_state = [ctx]()
	{
		application::state next_state;
		next_state.name = "play";
		next_state.enter = std::bind(game::state::play::enter, ctx);
		next_state.exit = std::bind(game::state::play::exit, ctx);
		
		ctx->app->change_state(next_state);
	};
	
	// Schedule fade out and change state events
	timeline* timeline = ctx->timeline;
	float t = timeline->get_position();
	timeline::sequence splash_sequence =
	{
		{t + splash_fade_in_duration + splash_hang_duration, fade_out},
		{t + splash_fade_in_duration + splash_hang_duration + splash_fade_out_duration, change_state}
	};
	timeline->add_sequence(splash_sequence);
	
	// Set up splash skipper
	ctx->input_listener->set_callback
	(
		[ctx](const event_base& event)
		{
			auto id = event.get_event_type_id();
			if (id != mouse_moved_event::event_type_id && id != mouse_wheel_scrolled_event::event_type_id && id != game_controller_axis_moved_event::event_type_id)
			{
				ctx->timeline->clear();
				ctx->fade_transition->get_animation()->stop();
				ctx->rasterizer->set_clear_color(0.0f, 0.0f, 0.0f, 1.0f);
				ctx->rasterizer->clear_framebuffer(true, false, false);
				ctx->app->swap_buffers();
				
				application::state next_state;
				next_state.name = "play";
				next_state.enter = std::bind(game::state::play::enter, ctx);
				next_state.exit = std::bind(game::state::play::exit, ctx);
				
				ctx->app->change_state(next_state);
			}
		}
	);
	ctx->input_listener->set_enabled(true);
}

void exit(game::context* ctx)
{
	// Disable splash skipper
	ctx->input_listener->set_enabled(false);
	ctx->input_listener->set_callback(nullptr);
	
	// Remove splash billboard from UI scene
	ctx->ui_scene->remove_object(ctx->splash_billboard);
}

} // namespace splash
} // namespace state
} // namespace game
