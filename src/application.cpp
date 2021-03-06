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

#include "animation/frame-scheduler.hpp"
#include "application.hpp"
#include "debug/logger.hpp"
#include "debug/performance-sampler.hpp"
#include "event/event-dispatcher.hpp"
#include "event/window-events.hpp"
#include "input/scancode.hpp"
#include "input/sdl-game-controller-tables.hpp"
#include "input/sdl-scancode-table.hpp"
#include "resources/image.hpp"
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <stdexcept>
#include <utility>
#include <thread>
#include <stb/stb_image_write.h>
#include <iostream>
#include <iomanip>

application::application():
	closed(false),
	exit_status(EXIT_SUCCESS),
	current_state{std::string(), nullptr, nullptr},
	queued_state{std::string(), nullptr, nullptr},
	update_callback(nullptr),
	render_callback(nullptr),
	fullscreen(true),
	vsync(true),
	cursor_visible(true),
	display_dimensions({0, 0}),
	window_dimensions({0, 0}),
	viewport_dimensions({0, 0}),
	mouse_position({0, 0}),
	update_rate(60.0),
	logger(nullptr),
	sdl_window(nullptr),
	sdl_gl_context(nullptr)
{
	// Setup logging
	logger = new debug::logger();
	
	// Get SDL compiled version
	SDL_version sdl_compiled_version;
	SDL_VERSION(&sdl_compiled_version);
	std::string sdl_compiled_version_string = std::to_string(sdl_compiled_version.major) + "." + std::to_string(sdl_compiled_version.minor) + "." + std::to_string(sdl_compiled_version.patch);
	logger->log("Compiled against SDL " + sdl_compiled_version_string);

	// Get SDL linked version
	SDL_version sdl_linked_version;
	SDL_GetVersion(&sdl_linked_version);
	std::string sdl_linked_version_string = std::to_string(sdl_linked_version.major) + "." + std::to_string(sdl_linked_version.minor) + "." + std::to_string(sdl_linked_version.patch);
	logger->log("Linking against SDL " + sdl_linked_version_string);

	// Init SDL
	logger->push_task("Initializing SDL");
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		logger->pop_task(EXIT_FAILURE);
		throw std::runtime_error("Failed to initialize SDL");
	}
	else
	{
		logger->pop_task(EXIT_SUCCESS);
	}

	// Load default OpenGL library
	logger->push_task("Loading OpenGL library");
	if (SDL_GL_LoadLibrary(nullptr) != 0)
	{
		logger->pop_task(EXIT_FAILURE);
	}
	else
	{
		logger->pop_task(EXIT_SUCCESS);
	}

	// Set window creation hints
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	// Get display dimensions
	SDL_DisplayMode sdl_desktop_display_mode;
	if (SDL_GetDesktopDisplayMode(0, &sdl_desktop_display_mode) != 0)
	{
		logger->error("Failed to get desktop display mode: \"" + std::string(SDL_GetError()) + "\"");
		throw std::runtime_error("Failed to detect desktop display mode");
	}
	else
	{
		logger->log("Detected " + std::to_string(sdl_desktop_display_mode.w) + "x" + std::to_string(sdl_desktop_display_mode.h) + " display");
		display_dimensions = {sdl_desktop_display_mode.w, sdl_desktop_display_mode.h};
	}
	
	// Create a hidden fullscreen window
	logger->push_task("Creating " + std::to_string(display_dimensions[0]) + "x" + std::to_string(display_dimensions[1]) + " window");
	sdl_window = SDL_CreateWindow
	(
		"",
    	SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
	    display_dimensions[0], display_dimensions[1],
		SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_HIDDEN
	);
	
	if (!sdl_window)
	{
		logger->pop_task(EXIT_FAILURE);
		throw std::runtime_error("Failed to create SDL window");
	}
	else
	{
		logger->pop_task(EXIT_SUCCESS);
	}

	// Create OpenGL context
	logger->push_task("Creating OpenGL 3.3 context");
	sdl_gl_context = SDL_GL_CreateContext(sdl_window);
	if (!sdl_gl_context)
	{
		logger->pop_task(EXIT_FAILURE);
		throw std::runtime_error("Failed to create OpenGL context");
	}
	else
	{
		logger->pop_task(EXIT_SUCCESS);
	}
	
	// Update window size and viewport size
	SDL_GetWindowSize(sdl_window, &window_dimensions[0], &window_dimensions[1]);
	SDL_GL_GetDrawableSize(sdl_window, &viewport_dimensions[0], &viewport_dimensions[1]);

	// Load OpenGL functions via GLAD
	logger->push_task("Loading OpenGL functions");
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
	{
		logger->pop_task(EXIT_FAILURE);
		throw std::runtime_error("Failed to load OpenGL functions");
	}
	else
	{
		logger->pop_task(EXIT_SUCCESS);
	}

	// Set v-sync mode
	int swap_interval = (vsync) ? 1 : 0;
	logger->push_task((swap_interval) ? "Enabling v-sync" : "Disabling v-sync");
	if (SDL_GL_SetSwapInterval(swap_interval) != 0)
	{
		logger->pop_task(EXIT_FAILURE);
	}
	else
	{
		logger->pop_task(EXIT_SUCCESS);
	}

	// Init SDL joystick and game controller subsystems
	logger->push_task("Initializing SDL Joystick and Game Controller subsystems");
	if (SDL_InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) != 0)
	{
		logger->pop_task(EXIT_FAILURE);
	}
	else
	{
		logger->pop_task(EXIT_SUCCESS);
	}

	// Load SDL game controller mappings
	/*
	logger->push_task("Loading SDL game controller mappings from database");
	std::string gamecontrollerdb_path = data_path + "controls/gamecontrollerdb.txt";
	if (SDL_GameControllerAddMappingsFromFile(gamecontrollerdb_path.c_str()) == -1)
	{
		logger->pop_task(EXIT_FAILURE);
	}
	else
	{
		logger->pop_task(EXIT_SUCCESS);
	}
	*/
	
	// Setup rasterizer
	rasterizer = new gl::rasterizer();
	
	// Setup events
	event_dispatcher = new ::event_dispatcher();
	
	// Setup input
	keyboard = new input::keyboard();
	keyboard->set_event_dispatcher(event_dispatcher);
	mouse = new input::mouse();
	mouse->set_event_dispatcher(event_dispatcher);

	// Setup frame scheduler
	frame_scheduler = new ::frame_scheduler();
	frame_scheduler->set_update_callback(std::bind(&application::update, this, std::placeholders::_1, std::placeholders::_2));
	frame_scheduler->set_render_callback(std::bind(&application::render, this, std::placeholders::_1));
	frame_scheduler->set_update_rate(update_rate);
	frame_scheduler->set_max_frame_duration(0.25);

	// Setup performance sampling
	performance_sampler = new debug::performance_sampler();
	performance_sampler->set_sample_size(15);
}

application::~application()
{
	// Destroy the OpenGL context
	SDL_GL_DeleteContext(sdl_gl_context);
	
	// Destroy the SDL window
	SDL_DestroyWindow(sdl_window);

	// Shutdown SDL
	SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);
	SDL_Quit();
}

void application::close(int status)
{
	closed = true;
	exit_status = status;
}

int application::execute(bootloader_type bootloader)
{
	// Execute bootloader
	if (bootloader)
	{
		exit_status = bootloader(this);
		if (exit_status != EXIT_SUCCESS)
		{
			return exit_status;
		}
	}
	
	// Show window
	SDL_ShowWindow(sdl_window);
	
	// Clear window
	rasterizer->clear_framebuffer(true, false, false);
	SDL_GL_SwapWindow(sdl_window);
	
	// Perform initial update
	update(0.0, 0.0);
	
	// Reset frame scheduler
	frame_scheduler->reset();

	// Schedule frames until closed
	while (!closed)
	{
		// Tick frame scheduler
		frame_scheduler->tick();

		// Sample frame duration
		performance_sampler->sample(frame_scheduler->get_frame_duration());
	}
	
	// Exit current state
	change_state({std::string(), nullptr, nullptr});
	
	return exit_status;
}

void application::change_state(const application::state& next_state)
{
	// Exit current state
	if (current_state.exit)
	{
		logger->push_task("Exiting application state \"" + current_state.name + "\"");
		
		try
		{
			current_state.exit();
		}
		catch (...)
		{
			logger->pop_task(EXIT_FAILURE);
			throw;
		}
		logger->pop_task(EXIT_SUCCESS);
	}
	
	current_state = next_state;
	
	// Enter next state
	if (current_state.enter)
	{
		logger->push_task("Entering application state \"" + current_state.name + "\"");
		
		try
		{
			current_state.enter();
		}
		catch (...)
		{
			logger->pop_task(EXIT_FAILURE);
			throw;
		}
		logger->pop_task(EXIT_SUCCESS);
	}
	
	// Enter queued state (if any)
	if (queued_state.enter != nullptr || queued_state.exit != nullptr)
	{
		// Make a copy of the queued state
		application::state queued_state_copy = queued_state;
		
		// Clear the queued state
		queued_state = {std::string(), nullptr, nullptr};
		
		// Enter the queued state
		change_state(queued_state_copy);
	}
}

void application::queue_state(const application::state& next_state)
{
	queued_state = next_state;
	logger->log("Queued application state \"" + queued_state.name + "\"");
}

std::shared_ptr<image> application::capture_frame() const
{
	int w = viewport_dimensions[0];
	int h = viewport_dimensions[1];
	
	std::shared_ptr<image> frame = std::make_shared<image>();
	frame->format(3, false);
	frame->resize(w, h);

	// Read pixel data from framebuffer into image
	glReadBuffer(GL_BACK);
	glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, frame->get_pixels());
	
	return std::move(frame);
}

void application::save_frame(const std::string& path) const
{
	logger->push_task("Saving screenshot to \"" + path + "\"");
	
	auto frame = capture_frame();
	
	std::thread
	(
		[frame, path]
		{
			stbi_flip_vertically_on_write(1);
			stbi_write_png(path.c_str(), frame->get_width(), frame->get_height(), frame->get_channels(), frame->get_pixels(), frame->get_width() * frame->get_channels());
		}
	).detach();
	
	logger->pop_task(EXIT_SUCCESS);
}

void application::set_update_callback(const update_callback_type& callback)
{
	update_callback = callback;
}

void application::set_render_callback(const render_callback_type& callback)
{
	render_callback = callback;
}

void application::set_update_rate(double frequency)
{
	update_rate = frequency;
	frame_scheduler->set_update_rate(update_rate);
}

void application::set_title(const std::string& title)
{
	SDL_SetWindowTitle(sdl_window, title.c_str());
}

void application::set_cursor_visible(bool visible)
{
	SDL_ShowCursor((visible) ? SDL_ENABLE : SDL_DISABLE);
	cursor_visible = visible;
}

void application::set_relative_mouse_mode(bool enabled)
{
	if (enabled)
	{
		SDL_GetMouseState(&mouse_position[0], &mouse_position[1]);
		SDL_ShowCursor(SDL_DISABLE);
		SDL_SetRelativeMouseMode(SDL_TRUE);
	}
	else
	{
		SDL_SetRelativeMouseMode(SDL_FALSE);
		SDL_WarpMouseInWindow(sdl_window, mouse_position[0], mouse_position[1]);
		if (cursor_visible)
			SDL_ShowCursor(SDL_ENABLE);
	}
}

void application::resize_window(int width, int height)
{
	int x = (display_dimensions[0] >> 1) - (width >> 1);
	int y = (display_dimensions[1] >> 1) - (height >> 1);
	
	// Resize and center window
	SDL_SetWindowPosition(sdl_window, x, y);
	SDL_SetWindowSize(sdl_window, width, height);
	
	window_resized();
}

void application::set_fullscreen(bool fullscreen)
{
	if (this->fullscreen != fullscreen)
	{
		this->fullscreen = fullscreen;
		
		if (fullscreen)
		{
			SDL_HideWindow(sdl_window);
			SDL_SetWindowFullscreen(sdl_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
			SDL_ShowWindow(sdl_window);
		}
		else
		{
			SDL_SetWindowFullscreen(sdl_window, 0);
			SDL_SetWindowBordered(sdl_window, SDL_TRUE);
			SDL_SetWindowResizable(sdl_window, SDL_TRUE);
		}
	}
}

void application::set_vsync(bool vsync)
{
	if (this->vsync != vsync)
	{
		this->vsync = vsync;
		SDL_GL_SetSwapInterval((vsync) ? 1 : 0);
	}
}

void application::set_window_opacity(float opacity)
{
	SDL_SetWindowOpacity(sdl_window, opacity);
}

void application::swap_buffers()
{
	SDL_GL_SwapWindow(sdl_window);
}

void application::update(double t, double dt)
{
	translate_sdl_events();
	event_dispatcher->update(t);
	
	if (update_callback)
	{
		update_callback(t, dt);
	}
	
	/*
	static int frame =  0;
	if (frame % 60 == 0)
	{
		std::cout << std::fixed;
		std::cout << std::setprecision(2);
		std::cout << performance_sampler->mean_frame_duration() * 1000.0 << "\n";
	}
	++frame;
	*/
}

void application::render(double alpha)
{
	/*
	std::cout << std::fixed;
	std::cout << std::setprecision(2);
	std::cout << performance_sampler->mean_frame_duration() * 1000.0 << std::endl;
	*/
	
	if (render_callback)
	{
		render_callback(alpha);
	}
	
	SDL_GL_SwapWindow(sdl_window);
}

void application::translate_sdl_events()
{
	SDL_Event sdl_event;
	while (SDL_PollEvent(&sdl_event))
	{
		switch (sdl_event.type)
		{
			case SDL_KEYDOWN:
			case SDL_KEYUP:
			{
				if (sdl_event.key.repeat == 0)
				{
					input::scancode scancode = input::scancode::unknown;
					if (sdl_event.key.keysym.scancode <= SDL_SCANCODE_APP2)
					{
						scancode = input::sdl_scancode_table[sdl_event.key.keysym.scancode];
					}

					if (sdl_event.type == SDL_KEYDOWN)
						keyboard->press(scancode);
					else
						keyboard->release(scancode);
				}
				break;
			}

			case SDL_MOUSEMOTION:
			{
				mouse->move(sdl_event.motion.x, sdl_event.motion.y, sdl_event.motion.xrel, sdl_event.motion.yrel);
				break;
			}

			case SDL_MOUSEBUTTONDOWN:
			{
				mouse->press(sdl_event.button.button, sdl_event.button.x, sdl_event.button.y);
				break;
			}
			
			case SDL_MOUSEBUTTONUP:
			{
				mouse->release(sdl_event.button.button, sdl_event.button.x, sdl_event.button.y);
				break;
			}
			
			case SDL_MOUSEWHEEL:
			{
				int direction = (sdl_event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED) ? -1 : 1;
				mouse->scroll(sdl_event.wheel.x * direction, sdl_event.wheel.y * direction);
				break;
			}

			case SDL_CONTROLLERBUTTONDOWN:
			{
				if (sdl_event.cbutton.button != SDL_CONTROLLER_BUTTON_INVALID)
				{
					if (auto it = game_controller_map.find(sdl_event.cdevice.which); it != game_controller_map.end())
					{
						input::game_controller_button button = input::sdl_button_table[sdl_event.cbutton.button];
						it->second->press(button);
					}
				}
				break;
			}

			case SDL_CONTROLLERBUTTONUP:
			{
				if (sdl_event.cbutton.button != SDL_CONTROLLER_BUTTON_INVALID)
				{
					if (auto it = game_controller_map.find(sdl_event.cdevice.which); it != game_controller_map.end())
					{
						input::game_controller_button button = input::sdl_button_table[sdl_event.cbutton.button];
						it->second->release(button);
					}
				}
				break;
			}

			case SDL_CONTROLLERAXISMOTION:
			{
				if (sdl_event.caxis.axis != SDL_CONTROLLER_AXIS_INVALID)
				{
					if (auto it = game_controller_map.find(sdl_event.cdevice.which); it != game_controller_map.end())
					{
						input::game_controller_axis axis = input::sdl_axis_table[sdl_event.caxis.axis];
						float value = sdl_event.caxis.value;
						value /= (value < 0.0f) ? 32768.0f : 32767.0f;
						it->second->move(axis, value);
					}
				}
				break;
			}

			case SDL_CONTROLLERDEVICEADDED:
			{
				if (SDL_IsGameController(sdl_event.cdevice.which))
				{
					SDL_GameController* sdl_controller = SDL_GameControllerOpen(sdl_event.cdevice.which);

					std::string controller_name = SDL_GameControllerNameForIndex(sdl_event.cdevice.which);
					if (sdl_controller)
					{
						if (auto it = game_controller_map.find(sdl_event.cdevice.which); it != game_controller_map.end())
						{
							logger->log("Reconnected game controller  \"" + controller_name + "\"");
							
							it->second->connect(true);
						}
						else
						{
							logger->log("Connected game controller \"" + controller_name + "\"");
							
							input::game_controller* controller = new input::game_controller();
							controller->set_event_dispatcher(event_dispatcher);
							game_controllers.push_back(controller);
							game_controller_map[sdl_event.cdevice.which] = controller;
							
							controller->connect(false);
						}
					}
					else
					{
						logger->error("Failed to connected game controller \"" + controller_name + "\"");
					}
				}

				break;
			}

			case SDL_CONTROLLERDEVICEREMOVED:
			{
				SDL_GameController* sdl_controller = SDL_GameControllerFromInstanceID(sdl_event.cdevice.which);

				if (sdl_controller)
				{
					SDL_GameControllerClose(sdl_controller);
					logger->log("Disconnected game controller");
					
					if (auto it = game_controller_map.find(sdl_event.cdevice.which); it != game_controller_map.end())
					{
						it->second->disconnect();
					}
				}

				break;
			}

			case SDL_WINDOWEVENT:
			{
				if (sdl_event.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					window_resized();
				}

				break;
			}

			case SDL_QUIT:
			{
				close(EXIT_SUCCESS);
				break;
			}
		}
	}
}

void application::window_resized()
{
	// Update window size and viewport size
	SDL_GetWindowSize(sdl_window, &window_dimensions[0], &window_dimensions[1]);
	SDL_GL_GetDrawableSize(sdl_window, &viewport_dimensions[0], &viewport_dimensions[1]);
	
	rasterizer->context_resized(viewport_dimensions[0], viewport_dimensions[1]);
	
	window_resized_event event;
	event.w = window_dimensions[0];
	event.h = window_dimensions[1];
	
	event_dispatcher->queue(event);

	/*
	rasterizer->window_resized(width, height);
	overworld_camera.set_perspective(overworld_camera.get_fov(), aspect_ratio, overworld_camera.get_clip_near(), overworld_camera.get_clip_far());
	underworld_camera.set_perspective(underworld_camera.get_fov(), aspect_ratio, underworld_camera.get_clip_near(), underworld_camera.get_clip_far());
	control_system->set_viewport(viewport);
	camera_system->set_viewport(viewport);
	tool_system->set_viewport(viewport);
	ui_system->set_viewport(viewport);
	*/
}
