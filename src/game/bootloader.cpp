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

#include "animation/animation.hpp"
#include "animation/animator.hpp"
#include "animation/ease.hpp"
#include "animation/screen-transition.hpp"
#include "animation/timeline.hpp"
#include "application.hpp"
#include "debug/cli.hpp"
#include "debug/console-commands.hpp"
#include "debug/logger.hpp"
#include "game/context.hpp"
#include "gl/framebuffer.hpp"
#include "gl/pixel-format.hpp"
#include "gl/pixel-type.hpp"
#include "gl/rasterizer.hpp"
#include "gl/texture-2d.hpp"
#include "gl/texture-filter.hpp"
#include "gl/texture-wrapping.hpp"
#include "gl/vertex-array.hpp"
#include "gl/vertex-attribute-type.hpp"
#include "gl/vertex-buffer.hpp"
#include "renderer/material-flags.hpp"
#include "renderer/material-property.hpp"
#include "renderer/passes/bloom-pass.hpp"
#include "renderer/passes/clear-pass.hpp"
#include "renderer/passes/final-pass.hpp"
#include "renderer/passes/material-pass.hpp"
#include "renderer/passes/outline-pass.hpp"
#include "renderer/passes/shadow-map-pass.hpp"
#include "renderer/passes/sky-pass.hpp"
#include "renderer/simple-render-pass.hpp"
#include "renderer/vertex-attributes.hpp"
#include "renderer/compositor.hpp"
#include "renderer/renderer.hpp"
#include "resources/config-file.hpp"
#include "resources/resource-manager.hpp"
#include "resources/resource-manager.hpp"
#include "scene/scene.hpp"
#include "game/states/loading.hpp"
#include "entity/systems/behavior.hpp"
#include "entity/systems/camera.hpp"
#include "entity/systems/collision.hpp"
#include "entity/systems/constraint.hpp"
#include "entity/systems/control.hpp"
#include "entity/systems/locomotion.hpp"
#include "entity/systems/nest.hpp"
#include "entity/systems/snapping.hpp"
#include "entity/systems/render.hpp"
#include "entity/systems/samara.hpp"
#include "entity/systems/subterrain.hpp"
#include "entity/systems/terrain.hpp"
#include "entity/systems/tool.hpp"
#include "entity/systems/ui.hpp"
#include "entity/systems/vegetation.hpp"
#include "entity/systems/spatial.hpp"
#include "entity/systems/tracking.hpp"
#include "entity/systems/painting.hpp"
#include "entity/systems/astronomy.hpp"
#include "entity/systems/blackbody.hpp"
#include "entity/systems/atmosphere.hpp"
#include "entity/systems/orbit.hpp"
#include "entity/systems/proteome.hpp"
#include "entity/components/marker.hpp"
#include "entity/commands.hpp"
#include "utility/paths.hpp"
#include "event/event-dispatcher.hpp"
#include "input/event-router.hpp"
#include "input/mapper.hpp"
#include "input/listener.hpp"
#include "input/game-controller.hpp"
#include "input/mouse.hpp"
#include "input/keyboard.hpp"
#include "pheromone-matrix.hpp"
#include "configuration.hpp"
#include "input/scancode.hpp"
#include <cxxopts.hpp>
#include <dirent.h>
#include <entt/entt.hpp>
#include <filesystem>
#include <functional>
#include <string>
#include <vector>
#include "utility/timestamp.hpp"

static constexpr double seconds_per_day = 24.0 * 60.0 * 60.0;

static void parse_options(game::context* ctx, int argc, char** argv);
static void setup_resources(game::context* ctx);
static void load_config(game::context* ctx);
static void load_strings(game::context* ctx);
static void setup_window(game::context* ctx);
static void setup_rendering(game::context* ctx);
static void setup_scenes(game::context* ctx);
static void setup_animation(game::context* ctx);
static void setup_entities(game::context* ctx);
static void setup_systems(game::context* ctx);
static void setup_controls(game::context* ctx);
static void setup_cli(game::context* ctx);
static void setup_callbacks(game::context* ctx);

int bootloader(application* app, int argc, char** argv)
{
	// Get application logger
	debug::logger* logger = app->get_logger();
	
	logger->push_task("Running application bootloader");
	
	// Allocate game context
	game::context* ctx = new game::context();
	ctx->app = app;
	ctx->logger = logger;
	
	// Init game context
	try
	{
		parse_options(ctx, argc, argv);
		setup_resources(ctx);
		load_config(ctx);
		load_strings(ctx);
		setup_window(ctx);
		setup_rendering(ctx);
		setup_scenes(ctx);
		setup_animation(ctx);
		setup_entities(ctx);
		setup_systems(ctx);
		setup_controls(ctx);
		setup_cli(ctx);
		setup_callbacks(ctx);
	}
	catch (const std::exception& e)
	{
		logger->error("Caught exception: \"" + std::string(e.what()) + "\"");
		logger->pop_task(EXIT_FAILURE);
		return EXIT_FAILURE;
	}
	
	logger->pop_task(EXIT_SUCCESS);
	
	// Setup initial application state
	application::state initial_state;
	initial_state.name = "loading";
	initial_state.enter = std::bind(game::state::loading::enter, ctx);
	initial_state.exit = std::bind(game::state::loading::exit, ctx);
	
	// Enter initial application state
	app->change_state(initial_state);
	
	return EXIT_SUCCESS;
}

void parse_options(game::context* ctx, int argc, char** argv)
{
	debug::logger* logger = ctx->logger;
	logger->push_task("Parsing command line options");
	
	try
	{
		cxxopts::Options options("Antkeeper", "Ant colony simulation game");
		options.add_options()
			("b,biome", "Selects the biome to load", cxxopts::value<std::string>())
			("c,continue", "Continues from the last save")
			("d,data", "Sets the data package path", cxxopts::value<std::string>())
			("f,fullscreen", "Starts in fullscreen mode")
			("n,new-game", "Starts a new game")
			("q,quick-start", "Skips to the main menu")
			("r,reset", "Restores all settings to default")
			("v,vsync", "Enables or disables v-sync", cxxopts::value<int>())
			("w,windowed", "Starts in windowed mode");
		auto result = options.parse(argc, argv);
		
		// --biome
		if (result.count("biome"))
			ctx->option_biome = result["biome"].as<std::string>();
		
		// --continue
		if (result.count("continue"))
			ctx->option_continue = true;
		
		// --data
		if (result.count("data"))
			ctx->option_data = result["data"].as<std::string>();
		
		// --fullscreen
		if (result.count("fullscreen"))
			ctx->option_fullscreen = true;
		
		// --new-game
		if (result.count("new-game"))
			ctx->option_new_game = true;
		
		// --quick-start
		if (result.count("quick-start"))
			ctx->option_quick_start = true;
		
		// --reset
		if (result.count("reset"))
			ctx->option_reset = true;
		
		// --vsync
		if (result.count("vsync"))
			ctx->option_vsync = (result["vsync"].as<int>()) ? true : false;
		
		// --windowed
		if (result.count("windowed"))
			ctx->option_windowed = true; 
	}
	catch (const std::exception& e)
	{
		logger->error("Exception caught: \"" + std::string(e.what()) + "\"");
		logger->pop_task(EXIT_FAILURE);
		return;
	}
	
	logger->pop_task(EXIT_SUCCESS);
}

void setup_resources(game::context* ctx)
{
	debug::logger* logger = ctx->logger;
	
	// Setup resource manager
	ctx->resource_manager = new resource_manager(logger);
	
	// Determine application name
	std::string application_name;
	#if defined(_WIN32) || defined(__APPLE__)
		application_name = "Antkeeper";
	#else
		application_name = "antkeeper";
	#endif
	
	// Detect paths
	ctx->data_path = get_data_path(application_name);
	ctx->config_path = get_config_path(application_name);
	ctx->mods_path = ctx->config_path + "mods/";
	ctx->saves_path = ctx->config_path + "saves/";
	ctx->screenshots_path = ctx->config_path + "screenshots/";
	
	// Log resource paths
	logger->log("Detected data path as \"" + ctx->data_path + "\"");
	logger->log("Detected config path as \"" + ctx->config_path + "\"");
	
	// Create nonexistent config directories
	std::vector<std::string> config_paths;
	config_paths.push_back(ctx->config_path);
	config_paths.push_back(ctx->mods_path);
	config_paths.push_back(ctx->saves_path);
	config_paths.push_back(ctx->screenshots_path);
	for (const std::string& path: config_paths)
	{
		if (!path_exists(path))
		{
			logger->push_task("Creating directory \"" + path + "\"");
			if (create_directory(path))
			{
				logger->pop_task(EXIT_SUCCESS);
			}
			else
			{
				logger->pop_task(EXIT_FAILURE);
			}
		}
	}
	
	// Redirect logger output to log file on non-debug builds
	#if defined(NDEBUG)
		std::string log_filename = config_path + "log.txt";
		ctx->log_filestream.open(log_filename.c_str());
		ctx->log_filestream << logger->get_history();
		logger->redirect(&log_filestream);
	#endif
	
	// Scan for mods
	std::vector<std::string> mods;
	struct dirent** files = nullptr;
	if (int n = scandir(ctx->mods_path.c_str(), &files, NULL, alphasort); n >= 0)
	{
		for (int i = 0; i < n; ++i)
		{
			struct dirent* file = files[i];
			
			switch (file->d_type)
			{
				case DT_REG:
				case DT_DIR:
				{
					std::string mod_name = file->d_name;
					
					// Skip hidden files and directories
					if (mod_name.front() == '.')
						break;
					
					mods.push_back(mod_name);
				}
				
				default:
					break;
			}
		}
	}
	
	// Determine data package path
	if (ctx->option_data.has_value())
	{
		ctx->data_package_path = ctx->option_data.value();
		if (std::filesystem::path(ctx->data_package_path).is_relative())
			ctx->data_package_path = ctx->data_path + ctx->data_package_path;
	}
	else
	{
		ctx->data_package_path = ctx->data_path + "data.zip";
	}
	
	// Mount mods
	for (const std::string& mod_name: mods)
		ctx->resource_manager->mount(ctx->mods_path + mod_name);
	
	// Mount config path
	ctx->resource_manager->mount(ctx->config_path);
	
	// Mount data package
	ctx->resource_manager->mount(ctx->data_package_path);


	// Include resource search paths in order of priority
	ctx->resource_manager->include("/shaders/");
	ctx->resource_manager->include("/models/");
	ctx->resource_manager->include("/images/");
	ctx->resource_manager->include("/textures/");
	ctx->resource_manager->include("/materials/");
	ctx->resource_manager->include("/entities/");
	ctx->resource_manager->include("/behaviors/");
	ctx->resource_manager->include("/controls/");
	ctx->resource_manager->include("/localization/");
	ctx->resource_manager->include("/biomes/");
	ctx->resource_manager->include("/traits/");
	ctx->resource_manager->include("/");
}

void load_config(game::context* ctx)
{
	debug::logger* logger = ctx->logger;
	logger->push_task("Loading config");
	
	// Load config file
	ctx->config = ctx->resource_manager->load<config_file>("config.txt");
	if (!ctx->config)
	{
		logger->pop_task(EXIT_FAILURE);
		return;
	}
	
	logger->pop_task(EXIT_SUCCESS);
}

void load_strings(game::context* ctx)
{
	debug::logger* logger = ctx->logger;
	logger->push_task("Loading strings");
		
	ctx->string_table = ctx->resource_manager->load<string_table>("strings.csv");
	
	build_string_table_map(&ctx->string_table_map, *ctx->string_table);
	
	ctx->language_code = ctx->config->get<std::string>("language");
	ctx->language_index = -1;
	for (int i = 2; i < (*ctx->string_table)[0].size(); ++i)
	{
		if ((*ctx->string_table)[0][i] == ctx->language_code)
			ctx->language_index = i;
	}
	
	logger->log("lang index: " + std::to_string(ctx->language_index));
	
	ctx->strings = &ctx->string_table_map[ctx->language_code];
	
	logger->pop_task(EXIT_SUCCESS);
}

void setup_window(game::context* ctx)
{
	debug::logger* logger = ctx->logger;
	logger->push_task("Setting up window");
	
	application* app = ctx->app;
	config_file* config = ctx->config;
	
	// Set fullscreen or windowed mode
	bool fullscreen = true;
	if (ctx->option_fullscreen.has_value())
		fullscreen = true;
	else if (ctx->option_windowed.has_value())
		fullscreen = false;
	else if (config->has("fullscreen"))
		fullscreen = (config->get<int>("fullscreen") != 0);
	app->set_fullscreen(fullscreen);
	
	// Set resolution
	const auto& display_dimensions = ctx->app->get_display_dimensions();
	int2 resolution = {display_dimensions[0], display_dimensions[1]};
	if (fullscreen)
	{
		if (config->has("fullscreen_resolution"))
			resolution = config->get<int2>("fullscreen_resolution");
	}
	else
	{
		if (config->has("windowed_resolution"))
			resolution = config->get<int2>("windowed_resolution");
	}
	app->resize_window(resolution.x, resolution.y);
	
	// Set v-sync
	bool vsync = true;
	if (ctx->option_vsync.has_value())
		vsync = (ctx->option_vsync.value() != 0);
	else if (config->has("vsync"))
		vsync = (config->get<int>("vsync") != 0);
	app->set_vsync(vsync);
	
	// Set title
	app->set_title((*ctx->strings)["title"]);
	
	logger->pop_task(EXIT_SUCCESS);
}

void setup_rendering(game::context* ctx)
{
	debug::logger* logger = ctx->logger;
	logger->push_task("Setting up rendering");
	
	// Get rasterizer from application
	ctx->rasterizer = ctx->app->get_rasterizer();
	
	// Get default framebuffer
	const gl::framebuffer& default_framebuffer = ctx->rasterizer->get_default_framebuffer();
	const auto& viewport_dimensions = default_framebuffer.get_dimensions();
	
	// Create HDR framebuffer (32F color, 32F depth)
	ctx->framebuffer_hdr_color = new gl::texture_2d(viewport_dimensions[0], viewport_dimensions[1], gl::pixel_type::float_32, gl::pixel_format::rgb);
	ctx->framebuffer_hdr_color->set_wrapping(gl::texture_wrapping::extend, gl::texture_wrapping::extend);
	ctx->framebuffer_hdr_color->set_filters(gl::texture_min_filter::linear, gl::texture_mag_filter::linear);
	ctx->framebuffer_hdr_color->set_max_anisotropy(0.0f);
	ctx->framebuffer_hdr_depth = new gl::texture_2d(viewport_dimensions[0], viewport_dimensions[1], gl::pixel_type::float_32, gl::pixel_format::ds);
	ctx->framebuffer_hdr_depth->set_wrapping(gl::texture_wrapping::extend, gl::texture_wrapping::extend);
	ctx->framebuffer_hdr_depth->set_filters(gl::texture_min_filter::linear, gl::texture_mag_filter::linear);
	ctx->framebuffer_hdr_depth->set_max_anisotropy(0.0f);
	ctx->framebuffer_hdr = new gl::framebuffer(viewport_dimensions[0], viewport_dimensions[1]);
	ctx->framebuffer_hdr->attach(gl::framebuffer_attachment_type::color, ctx->framebuffer_hdr_color);
	ctx->framebuffer_hdr->attach(gl::framebuffer_attachment_type::depth, ctx->framebuffer_hdr_depth);
	ctx->framebuffer_hdr->attach(gl::framebuffer_attachment_type::stencil, ctx->framebuffer_hdr_depth);
	
	// Create shadow map framebuffer
	int shadow_map_resolution = 4096;
	if (ctx->config->has("shadow_map_resolution"))
	{
		shadow_map_resolution = ctx->config->get<int>("shadow_map_resolution");
	}
	ctx->shadow_map_depth_texture = new gl::texture_2d(shadow_map_resolution, shadow_map_resolution, gl::pixel_type::float_32, gl::pixel_format::d);
	ctx->shadow_map_depth_texture->set_wrapping(gl::texture_wrapping::extend, gl::texture_wrapping::extend);
	ctx->shadow_map_depth_texture->set_filters(gl::texture_min_filter::linear, gl::texture_mag_filter::linear);
	ctx->shadow_map_depth_texture->set_max_anisotropy(0.0f);
	ctx->shadow_map_framebuffer = new gl::framebuffer(shadow_map_resolution, shadow_map_resolution);
	ctx->shadow_map_framebuffer->attach(gl::framebuffer_attachment_type::depth, ctx->shadow_map_depth_texture);
	
	// Create bloom pingpong framebuffers (16F color, no depth)
	int bloom_width = viewport_dimensions[0] >> 1;
	int bloom_height = viewport_dimensions[1] >> 1;
	ctx->bloom_texture = new gl::texture_2d(bloom_width, bloom_height, gl::pixel_type::float_16, gl::pixel_format::rgb);
	ctx->bloom_texture->set_wrapping(gl::texture_wrapping::extend, gl::texture_wrapping::extend);
	ctx->bloom_texture->set_filters(gl::texture_min_filter::linear, gl::texture_mag_filter::linear);
	ctx->bloom_texture->set_max_anisotropy(0.0f);
	ctx->framebuffer_bloom = new gl::framebuffer(bloom_width, bloom_height);
	ctx->framebuffer_bloom->attach(gl::framebuffer_attachment_type::color, ctx->bloom_texture);
	
	// Load blue noise texture
	gl::texture_2d* blue_noise_map = ctx->resource_manager->load<gl::texture_2d>("blue-noise.tex");
	
	// Load fallback material
	ctx->fallback_material = ctx->resource_manager->load<material>("fallback.mtl");
	
	// Setup overworld compositor
	ctx->overworld_shadow_map_clear_pass = new clear_pass(ctx->rasterizer, ctx->shadow_map_framebuffer);
	ctx->overworld_shadow_map_clear_pass->set_cleared_buffers(false, true, false);
	ctx->overworld_shadow_map_clear_pass->set_clear_depth(1.0f);
	ctx->overworld_shadow_map_pass = new shadow_map_pass(ctx->rasterizer, ctx->shadow_map_framebuffer, ctx->resource_manager);
	ctx->overworld_shadow_map_pass->set_split_scheme_weight(0.75f);
	ctx->overworld_clear_pass = new clear_pass(ctx->rasterizer, ctx->framebuffer_hdr);
	ctx->overworld_clear_pass->set_cleared_buffers(true, true, true);
	ctx->overworld_clear_pass->set_clear_depth(0.0f);
	ctx->overworld_sky_pass = new sky_pass(ctx->rasterizer, ctx->framebuffer_hdr, ctx->resource_manager);
	ctx->app->get_event_dispatcher()->subscribe<mouse_moved_event>(ctx->overworld_sky_pass);
	ctx->overworld_sky_pass->set_enabled(true);
	ctx->overworld_material_pass = new material_pass(ctx->rasterizer, ctx->framebuffer_hdr, ctx->resource_manager);
	ctx->overworld_material_pass->set_fallback_material(ctx->fallback_material);
	ctx->overworld_material_pass->shadow_map_pass = ctx->overworld_shadow_map_pass;
	ctx->overworld_material_pass->shadow_map = ctx->shadow_map_depth_texture;
	ctx->app->get_event_dispatcher()->subscribe<mouse_moved_event>(ctx->overworld_material_pass);
	ctx->overworld_outline_pass = new outline_pass(ctx->rasterizer, ctx->framebuffer_hdr, ctx->resource_manager);
	ctx->overworld_outline_pass->set_outline_width(0.25f);
	ctx->overworld_outline_pass->set_outline_color(float4{1.0f, 1.0f, 1.0f, 1.0f});
	ctx->overworld_outline_pass->set_enabled(false);
	ctx->overworld_bloom_pass = new bloom_pass(ctx->rasterizer, ctx->framebuffer_bloom, ctx->resource_manager);
	ctx->overworld_bloom_pass->set_source_texture(ctx->framebuffer_hdr_color);
	ctx->overworld_bloom_pass->set_brightness_threshold(1.0f);
	ctx->overworld_bloom_pass->set_blur_iterations(5);
	ctx->overworld_bloom_pass->set_enabled(true);
	ctx->overworld_final_pass = new ::final_pass(ctx->rasterizer, &ctx->rasterizer->get_default_framebuffer(), ctx->resource_manager);
	ctx->overworld_final_pass->set_color_texture(ctx->framebuffer_hdr_color);
	ctx->overworld_final_pass->set_bloom_texture(ctx->bloom_texture);
	ctx->overworld_final_pass->set_blue_noise_texture(blue_noise_map);
	ctx->overworld_compositor = new compositor();
	ctx->overworld_compositor->add_pass(ctx->overworld_shadow_map_clear_pass);
	ctx->overworld_compositor->add_pass(ctx->overworld_shadow_map_pass);
	ctx->overworld_compositor->add_pass(ctx->overworld_clear_pass);
	ctx->overworld_compositor->add_pass(ctx->overworld_sky_pass);
	ctx->overworld_compositor->add_pass(ctx->overworld_material_pass);
	//ctx->overworld_compositor->add_pass(ctx->overworld_outline_pass);
	ctx->overworld_compositor->add_pass(ctx->overworld_bloom_pass);
	ctx->overworld_compositor->add_pass(ctx->overworld_final_pass);
	
	// Setup underworld compositor
	ctx->underworld_clear_pass = new clear_pass(ctx->rasterizer, ctx->framebuffer_hdr);
	ctx->underworld_clear_pass->set_cleared_buffers(true, true, false);
	ctx->underworld_material_pass = new material_pass(ctx->rasterizer, ctx->framebuffer_hdr, ctx->resource_manager);
	ctx->underworld_material_pass->set_fallback_material(ctx->fallback_material);
	ctx->app->get_event_dispatcher()->subscribe<mouse_moved_event>(ctx->underworld_material_pass);
	gl::shader_program* underworld_final_shader = ctx->resource_manager->load<gl::shader_program>("underground-final.glsl");
	ctx->underworld_final_pass = new simple_render_pass(ctx->rasterizer, &ctx->rasterizer->get_default_framebuffer(), underworld_final_shader);
	ctx->underground_color_texture_property = ctx->underworld_final_pass->get_material()->add_property<const gl::texture_2d*>("color_texture");
	ctx->underground_color_texture_property->set_value(ctx->framebuffer_hdr_color);
	ctx->underworld_final_pass->get_material()->update_tweens();
	ctx->underworld_compositor = new compositor();
	ctx->underworld_compositor->add_pass(ctx->underworld_clear_pass);
	ctx->underworld_compositor->add_pass(ctx->underworld_material_pass);
	ctx->underworld_compositor->add_pass(ctx->underworld_final_pass);
	
	// Setup UI camera compositor
	ctx->ui_clear_pass = new clear_pass(ctx->rasterizer, &ctx->rasterizer->get_default_framebuffer());
	ctx->ui_clear_pass->set_cleared_buffers(false, true, false);
	ctx->ui_clear_pass->set_clear_depth(0.0f);
	ctx->ui_material_pass = new material_pass(ctx->rasterizer, &ctx->rasterizer->get_default_framebuffer(), ctx->resource_manager);
	ctx->ui_material_pass->set_fallback_material(ctx->fallback_material);
	ctx->ui_compositor = new compositor();
	ctx->ui_compositor->add_pass(ctx->ui_clear_pass);
	ctx->ui_compositor->add_pass(ctx->ui_material_pass);
	
	// Create billboard VAO
	{
		const float billboard_vertex_data[] =
		{
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f
		};

		std::size_t billboard_vertex_size = 8;
		std::size_t billboard_vertex_stride = sizeof(float) * billboard_vertex_size;
		std::size_t billboard_vertex_count = 6;

		ctx->billboard_vbo = new gl::vertex_buffer(sizeof(float) * billboard_vertex_size * billboard_vertex_count, billboard_vertex_data);
		ctx->billboard_vao = new gl::vertex_array();
		ctx->billboard_vao->bind_attribute(VERTEX_POSITION_LOCATION, *ctx->billboard_vbo, 3, gl::vertex_attribute_type::float_32, billboard_vertex_stride, 0);
		ctx->billboard_vao->bind_attribute(VERTEX_TEXCOORD_LOCATION, *ctx->billboard_vbo, 2, gl::vertex_attribute_type::float_32, billboard_vertex_stride, sizeof(float) * 3);
		ctx->billboard_vao->bind_attribute(VERTEX_BARYCENTRIC_LOCATION, *ctx->billboard_vbo, 3, gl::vertex_attribute_type::float_32, billboard_vertex_stride, sizeof(float) * 5);
	}
	
	// Load marker albedo textures
	ctx->marker_albedo_textures = new gl::texture_2d*[8];
	ctx->marker_albedo_textures[0] = ctx->resource_manager->load<gl::texture_2d>("marker-clear-albedo.tex");
	ctx->marker_albedo_textures[1] = ctx->resource_manager->load<gl::texture_2d>("marker-yellow-albedo.tex");
	ctx->marker_albedo_textures[2] = ctx->resource_manager->load<gl::texture_2d>("marker-green-albedo.tex");
	ctx->marker_albedo_textures[3] = ctx->resource_manager->load<gl::texture_2d>("marker-blue-albedo.tex");
	ctx->marker_albedo_textures[4] = ctx->resource_manager->load<gl::texture_2d>("marker-purple-albedo.tex");
	ctx->marker_albedo_textures[5] = ctx->resource_manager->load<gl::texture_2d>("marker-pink-albedo.tex");
	ctx->marker_albedo_textures[6] = ctx->resource_manager->load<gl::texture_2d>("marker-red-albedo.tex");
	ctx->marker_albedo_textures[7] = ctx->resource_manager->load<gl::texture_2d>("marker-orange-albedo.tex");
	
	// Create renderer
	ctx->renderer = new renderer();
	ctx->renderer->set_billboard_vao(ctx->billboard_vao);
	
	logger->pop_task(EXIT_SUCCESS);
}

void setup_scenes(game::context* ctx)
{
	debug::logger* logger = ctx->logger;
	logger->push_task("Setting up scenes");
	
	// Get default framebuffer
	const auto& viewport_dimensions = ctx->rasterizer->get_default_framebuffer().get_dimensions();
	float viewport_aspect_ratio = static_cast<float>(viewport_dimensions[0]) / static_cast<float>(viewport_dimensions[1]);
	
	// Create infinite culling mask
	float inf = std::numeric_limits<float>::infinity();
	ctx->no_cull = {{-inf, -inf, -inf}, {inf, inf, inf}};
	
	// Setup overworld camera
	ctx->overworld_camera = new scene::camera();
	ctx->overworld_camera->set_perspective(math::radians<float>(45.0f), viewport_aspect_ratio, 0.1f, 1000.0f);
	ctx->overworld_camera->set_compositor(ctx->overworld_compositor);
	ctx->overworld_camera->set_composite_index(0);
	ctx->overworld_camera->set_active(true);
	
	// Setup underworld camera
	ctx->underworld_camera = new scene::camera();
	ctx->underworld_camera->set_perspective(math::radians<float>(45.0f), viewport_aspect_ratio, 0.1f, 1000.0f);
	ctx->underworld_camera->look_at({0, 50, 0}, {0, 0, 0}, {0, 0, -1});
	ctx->underworld_camera->set_compositor(ctx->underworld_compositor);
	ctx->underworld_camera->set_composite_index(0);
	ctx->underworld_camera->set_active(false);
	
	// Setup UI camera
	ctx->ui_camera = new scene::camera();
	ctx->ui_camera->set_compositor(ctx->ui_compositor);
	
	// Setup lights
	ctx->moon_light = new scene::directional_light();
	ctx->moon_light->set_intensity(0.0f);
	ctx->moon_light->update_tweens();
	
	ctx->subterrain_light = new scene::point_light();
	ctx->subterrain_light->set_color({1, 1, 1});
	ctx->subterrain_light->set_intensity(1.0f);
	ctx->subterrain_light->set_attenuation({1.0f, 0.09f, 0.032f});
	ctx->subterrain_light->update_tweens();
	
	ctx->underworld_ambient_light = new scene::ambient_light();
	ctx->underworld_ambient_light->set_color({1, 1, 1});
	ctx->underworld_ambient_light->set_intensity(0.1f);
	ctx->underworld_ambient_light->update_tweens();
	
	ctx->lens_spot_light = new scene::spot_light();
	ctx->lens_spot_light->set_color({1, 1, 1});
	ctx->lens_spot_light->set_intensity(20.0f);
	ctx->lens_spot_light->set_attenuation({1.0f, 0.0f, 0.0f});
	ctx->lens_spot_light->set_cutoff({math::radians(1.25f), math::radians(1.8f)});
	
	ctx->flashlight_spot_light = new scene::spot_light();
	ctx->flashlight_spot_light->set_color({1, 1, 1});
	ctx->flashlight_spot_light->set_intensity(1.0f);
	ctx->flashlight_spot_light->set_attenuation({1.0f, 0.0f, 0.0f});
	ctx->flashlight_spot_light->set_cutoff({math::radians(10.0f), math::radians(19.0f)});


	
	const gl::texture_2d* splash_texture = ctx->resource_manager->load<gl::texture_2d>("splash.tex");
	auto splash_dimensions = splash_texture->get_dimensions();
	ctx->splash_billboard_material = new material();
	ctx->splash_billboard_material->set_shader_program(ctx->resource_manager->load<gl::shader_program>("ui-element-textured.glsl"));
	ctx->splash_billboard_material->add_property<const gl::texture_2d*>("background")->set_value(splash_texture);
	ctx->splash_billboard_material->add_property<float4>("tint")->set_value(float4{1, 1, 1, 1});
	ctx->splash_billboard_material->update_tweens();
	ctx->splash_billboard = new scene::billboard();
	ctx->splash_billboard->set_material(ctx->splash_billboard_material);
	ctx->splash_billboard->set_scale({(float)std::get<0>(splash_dimensions) * 0.5f, (float)std::get<1>(splash_dimensions) * 0.5f, 1.0f});
	ctx->splash_billboard->set_translation({0.0f, 0.0f, 0.0f});
	ctx->splash_billboard->update_tweens();
	
	
	// Create depth debug billboard
	/*
	material* depth_debug_material = new material();
	depth_debug_material->set_shader_program(ctx->resource_manager->load<gl::shader_program>("ui-element-textured.glsl"));
	depth_debug_material->add_property<const gl::texture_2d*>("background")->set_value(shadow_map_depth_texture);
	depth_debug_material->add_property<float4>("tint")->set_value(float4{1, 1, 1, 1});
	billboard* depth_debug_billboard = new billboard();
	depth_debug_billboard->set_material(depth_debug_material);
	depth_debug_billboard->set_scale({128, 128, 1});
	depth_debug_billboard->set_translation({-960 + 128, 1080 * 0.5f - 128, 0});
	depth_debug_billboard->update_tweens();
	ui_system->get_scene()->add_object(depth_debug_billboard);
	*/
	
	// Setup overworld scene
	ctx->overworld_scene = new scene::collection();
	ctx->overworld_scene->add_object(ctx->overworld_camera);
	//ctx->overworld_scene->add_object(ctx->moon_light);
	//ctx->overworld_scene->add_object(ctx->spot_light);
	
	// Setup underworld scene
	ctx->underworld_scene = new scene::collection();
	ctx->underworld_scene->add_object(ctx->underworld_camera);
	ctx->underworld_scene->add_object(ctx->underworld_ambient_light);
	//ctx->underworld_scene->add_object(ctx->lantern);
	//ctx->underworld_scene->add_object(ctx->subterrain_light);
	//ctx->underworld_scene->add_object(ctx->portal_billboard);
	//model_instance* larva = new scene::model_instance(ctx->resource_manager->load<model>("larva.mdl"));
	//ctx->underworld_scene->add_object(larva);
	
	// Setup UI scene
	ctx->ui_scene = new scene::collection();
	ctx->ui_scene->add_object(ctx->ui_camera);
	
	//ctx->overworld_scene->add_object(ctx->lens_spot_light);
	ctx->underworld_scene->add_object(ctx->flashlight_spot_light);
	
	// Set overworld as active scene
	ctx->active_scene = ctx->overworld_scene;
	
	logger->pop_task(EXIT_SUCCESS);
}

void setup_animation(game::context* ctx)
{
	// Setup timeline system
	ctx->timeline = new timeline();
	ctx->timeline->set_autoremove(true);

	// Setup animator
	ctx->animator = new animator();
	
	// Initialize time tween
	ctx->time_tween = new tween<double>(0.0);
	ctx->time_tween->set_interpolator(math::lerp<double, double>);
	
	// Create fade transition
	ctx->fade_transition = new screen_transition();
	ctx->fade_transition->get_material()->set_shader_program(ctx->resource_manager->load<gl::shader_program>("fade-transition.glsl"));
	ctx->fade_transition_color = ctx->fade_transition->get_material()->add_property<float3>("color");
	ctx->fade_transition_color->set_value({0, 0, 0});
	ctx->ui_scene->add_object(ctx->fade_transition->get_billboard());
	ctx->animator->add_animation(ctx->fade_transition->get_animation());
	
	// Create inner radial transition
	ctx->radial_transition_inner = new screen_transition();
	ctx->radial_transition_inner->get_material()->set_shader_program(ctx->resource_manager->load<gl::shader_program>("radial-transition-inner.glsl"));
	ctx->ui_scene->add_object(ctx->radial_transition_inner->get_billboard());
	ctx->animator->add_animation(ctx->radial_transition_inner->get_animation());
	
	// Create outer radial transition
	ctx->radial_transition_outer = new screen_transition();
	ctx->radial_transition_outer->get_material()->set_shader_program(ctx->resource_manager->load<gl::shader_program>("radial-transition-outer.glsl"));
	ctx->ui_scene->add_object(ctx->radial_transition_outer->get_billboard());
	ctx->animator->add_animation(ctx->radial_transition_outer->get_animation());
	
	// Setup tweens
	ctx->focal_point_tween = new tween<float3>();
	ctx->focal_point_tween->set_interpolator(math::lerp<float3, float>);
	
	// Set material pass tweens
	ctx->overworld_sky_pass->set_time_tween(ctx->time_tween);
	ctx->overworld_material_pass->set_time_tween(ctx->time_tween);
	ctx->overworld_material_pass->set_focal_point_tween(ctx->focal_point_tween);
	ctx->overworld_final_pass->set_time_tween(ctx->time_tween);
	ctx->underworld_material_pass->set_time_tween(ctx->time_tween);
	ctx->underworld_material_pass->set_focal_point_tween(ctx->focal_point_tween);
	ctx->underworld_final_pass->set_time_tween(ctx->time_tween);
	ctx->underworld_material_pass->set_focal_point_tween(ctx->focal_point_tween);
	ctx->ui_material_pass->set_time_tween(ctx->time_tween);
}

void setup_entities(game::context* ctx)
{
	// Create entity registry
	ctx->entity_registry = new entt::registry();
	
	// Reserve named entities
	ctx->brush_entity = ctx->entity_registry->create();
	ctx->flashlight_entity = ctx->entity_registry->create();
	ctx->forceps_entity = ctx->entity_registry->create();
	ctx->lens_entity = ctx->entity_registry->create();
	ctx->marker_entity = ctx->entity_registry->create();
	ctx->container_entity = ctx->entity_registry->create();
	ctx->twig_entity = ctx->entity_registry->create();
	ctx->focal_point_entity = ctx->entity_registry->create();
}

void setup_systems(game::context* ctx)
{
	event_dispatcher* event_dispatcher = ctx->app->get_event_dispatcher();

	const auto& viewport_dimensions = ctx->app->get_viewport_dimensions();
	float4 viewport = {0.0f, 0.0f, static_cast<float>(viewport_dimensions[0]), static_cast<float>(viewport_dimensions[1])};
	
	// RGB wavelengths determined by matching wavelengths to XYZ, transforming XYZ to ACEScg, then selecting the max wavelengths for R, G, and B.
	const double3 rgb_wavelengths_nm = {602.224, 541.069, 448.143};
	
	// Setup terrain system
	ctx->terrain_system = new entity::system::terrain(*ctx->entity_registry);
	ctx->terrain_system->set_patch_subdivisions(30);
	ctx->terrain_system->set_patch_scene_collection(ctx->overworld_scene);
	ctx->terrain_system->set_max_error(200.0);
	
	// Setup vegetation system
	//ctx->vegetation_system = new entity::system::vegetation(*ctx->entity_registry);
	//ctx->vegetation_system->set_terrain_patch_size(TERRAIN_PATCH_SIZE);
	//ctx->vegetation_system->set_vegetation_patch_resolution(VEGETATION_PATCH_RESOLUTION);
	//ctx->vegetation_system->set_vegetation_density(1.0f);
	//ctx->vegetation_system->set_vegetation_model(ctx->resource_manager->load<model>("grass-tuft.mdl"));
	//ctx->vegetation_system->set_scene(ctx->overworld_scene);
	
	// Setup camera system
	ctx->camera_system = new entity::system::camera(*ctx->entity_registry);
	ctx->camera_system->set_viewport(viewport);
	event_dispatcher->subscribe<mouse_moved_event>(ctx->camera_system);
	event_dispatcher->subscribe<window_resized_event>(ctx->camera_system);
	
	// Setup tool system
	ctx->tool_system = new entity::system::tool(*ctx->entity_registry, event_dispatcher);
	ctx->tool_system->set_camera(ctx->overworld_camera);
	ctx->tool_system->set_orbit_cam(ctx->camera_system->get_orbit_cam());
	ctx->tool_system->set_viewport(viewport);
	
	// Setup subterrain system
	ctx->subterrain_system = new entity::system::subterrain(*ctx->entity_registry, ctx->resource_manager);
	ctx->subterrain_system->set_scene(ctx->underworld_scene);
	
	// Setup nest system
	ctx->nest_system = new entity::system::nest(*ctx->entity_registry, ctx->resource_manager);
	
	// Setup collision system
	ctx->collision_system = new entity::system::collision(*ctx->entity_registry);
	
	// Setup samara system
	ctx->samara_system = new entity::system::samara(*ctx->entity_registry);
	
	// Setup snapping system
	ctx->snapping_system = new entity::system::snapping(*ctx->entity_registry);
	
	// Setup behavior system
	ctx->behavior_system = new entity::system::behavior(*ctx->entity_registry);
	
	// Setup locomotion system
	ctx->locomotion_system = new entity::system::locomotion(*ctx->entity_registry);
	
	// Setup pheromone system
	ctx->pheromones = new pheromone_matrix();
	ctx->pheromones->rows = 256;
	ctx->pheromones->columns = 256;
	ctx->pheromones->buffers = new float*[2];
	ctx->pheromones->buffers[0] = new float[ctx->pheromones->rows * ctx->pheromones->columns];
	ctx->pheromones->buffers[1] = new float[ctx->pheromones->rows * ctx->pheromones->columns];
	ctx->pheromones->current = 0;
	//diffuse(ctx->pheromones);
	
	// Setup spatial system
	ctx->spatial_system = new entity::system::spatial(*ctx->entity_registry);
	
	// Setup constraint system
	ctx->constraint_system = new entity::system::constraint(*ctx->entity_registry);
	
	// Setup tracking system
	ctx->tracking_system = new entity::system::tracking(*ctx->entity_registry, event_dispatcher, ctx->resource_manager);
	ctx->tracking_system->set_scene(ctx->overworld_scene);
	
	// Setup painting system
	ctx->painting_system = new entity::system::painting(*ctx->entity_registry, event_dispatcher, ctx->resource_manager);
	ctx->painting_system->set_scene(ctx->overworld_scene);
	
	// Setup solar system
	ctx->orbit_system = new entity::system::orbit(*ctx->entity_registry);
	
	// Setup blackbody system
	ctx->blackbody_system = new entity::system::blackbody(*ctx->entity_registry);
	ctx->blackbody_system->set_rgb_wavelengths(rgb_wavelengths_nm);
	
	// Setup atmosphere system
	ctx->atmosphere_system = new entity::system::atmosphere(*ctx->entity_registry);
	ctx->atmosphere_system->set_rgb_wavelengths(rgb_wavelengths_nm);
	
	// Setup astronomy system
	ctx->astronomy_system = new entity::system::astronomy(*ctx->entity_registry);
	ctx->astronomy_system->set_sky_pass(ctx->overworld_sky_pass);
	
	// Setup proteome system
	ctx->proteome_system = new entity::system::proteome(*ctx->entity_registry);
	
	// Set time scale
	float time_scale = 60.0f;
	if (ctx->config->has("time_scale"))
	{
		time_scale = ctx->config->get<float>("time_scale");
	}
	
	ctx->orbit_system->set_time_scale(time_scale / seconds_per_day);
	ctx->astronomy_system->set_time_scale(time_scale / seconds_per_day);
	
	// Setup render system
	ctx->render_system = new entity::system::render(*ctx->entity_registry);
	ctx->render_system->add_layer(ctx->overworld_scene);
	ctx->render_system->add_layer(ctx->underworld_scene);
	ctx->render_system->add_layer(ctx->ui_scene);
	ctx->render_system->set_renderer(ctx->renderer);
	
	// Setup control system
	ctx->control_system = new entity::system::control(*ctx->entity_registry);
	ctx->control_system->set_viewport(viewport);
	ctx->control_system->set_underworld_camera(ctx->underworld_camera);
	ctx->control_system->set_tool(nullptr);
	//ctx->control_system->set_flashlight(flashlight, flashlight_light_cone);
	ctx->control_system->get_adjust_camera_control()->set_activated_callback([ctx](){ ctx->app->set_relative_mouse_mode(true); ctx->tool_system->set_pick(false); });
	ctx->control_system->get_adjust_camera_control()->set_deactivated_callback([ctx](){ ctx->app->set_relative_mouse_mode(false); ctx->tool_system->set_pick(true); });
	ctx->control_system->set_flashlight(ctx->flashlight_entity);
	ctx->control_system->set_camera_subject(ctx->focal_point_entity);
	ctx->control_system->set_camera_system(ctx->camera_system);
	event_dispatcher->subscribe<mouse_moved_event>(ctx->control_system);
	event_dispatcher->subscribe<window_resized_event>(ctx->control_system);
	
	// Setup UI system
	ctx->ui_system = new entity::system::ui(ctx->resource_manager);
	ctx->ui_system->set_camera(ctx->ui_camera);
	ctx->ui_system->set_scene(ctx->ui_scene);
	ctx->ui_system->set_viewport(viewport);
	ctx->ui_system->set_tool_menu_control(ctx->control_system->get_tool_menu_control());
	event_dispatcher->subscribe<mouse_moved_event>(ctx->ui_system);
	event_dispatcher->subscribe<window_resized_event>(ctx->ui_system);
}

void setup_controls(game::context* ctx)
{
	event_dispatcher* event_dispatcher = ctx->app->get_event_dispatcher();
	
	// Setup input event routing
	ctx->input_event_router = new input::event_router();
	ctx->input_event_router->set_event_dispatcher(event_dispatcher);
	
	// Setup input mapper
	ctx->input_mapper = new input::mapper();
	ctx->input_mapper->set_event_dispatcher(event_dispatcher);
	
	// Setup input listener
	ctx->input_listener = new input::listener();
	ctx->input_listener->set_event_dispatcher(event_dispatcher);
	
	// Create toggle fullscreen control
	ctx->toggle_fullscreen_control = new input::control();
	ctx->toggle_fullscreen_control->set_activated_callback
	(
		[ctx]()
		{
			bool fullscreen = !ctx->app->is_fullscreen();
			
			ctx->app->set_fullscreen(fullscreen);
			
			if (!fullscreen)
			{
				int2 resolution = ctx->config->get<int2>("windowed_resolution");
				ctx->app->resize_window(resolution.x, resolution.y);
			}
			
			ctx->config->set<int>("fullscreen", (fullscreen) ? 1 : 0);
		}
	);
	
	// Create screenshot control
	ctx->screenshot_control = new input::control();
	ctx->screenshot_control->set_activated_callback
	(
		[ctx]()
		{
			std::string path = ctx->screenshots_path + "antkeeper-" + timestamp() + ".png";
			ctx->app->save_frame(path);
		}
	);
	
	// Create menu back control
	ctx->menu_back_control = new input::control();
	ctx->menu_back_control->set_activated_callback
	(
		std::bind(&application::close, ctx->app, 0)
	);
	
	// Create menu select control
	ctx->menu_select_control = new input::control();
	
	// Create application control set
	ctx->application_controls = new input::control_set();
	ctx->application_controls->add_control(ctx->toggle_fullscreen_control);
	ctx->application_controls->add_control(ctx->screenshot_control);
	
	// Create menu control set
	ctx->menu_controls = new input::control_set();
	ctx->menu_controls->add_control(ctx->menu_back_control);
	ctx->menu_controls->add_control(ctx->menu_select_control);

	


	ctx->camera_controls = ctx->control_system->get_control_set();

	// Application control mappings
	ctx->input_event_router->add_mapping(input::key_mapping(ctx->toggle_fullscreen_control, nullptr, input::scancode::f11));
	ctx->input_event_router->add_mapping(input::key_mapping(ctx->screenshot_control, nullptr, input::scancode::f12));
	
	// Add menu control mappings
	ctx->input_event_router->add_mapping(input::key_mapping(ctx->menu_back_control, nullptr, input::scancode::escape));
	ctx->input_event_router->add_mapping(input::key_mapping(ctx->menu_back_control, nullptr, input::scancode::backspace));
	ctx->input_event_router->add_mapping(input::game_controller_button_mapping(ctx->menu_back_control, nullptr, input::game_controller_button::b));
	ctx->input_event_router->add_mapping(input::key_mapping(ctx->control_system->get_tool_menu_control(), nullptr, input::scancode::left_shift));
	ctx->input_event_router->add_mapping(input::game_controller_button_mapping(ctx->control_system->get_tool_menu_control(), nullptr, input::game_controller_button::x));
	ctx->input_event_router->add_mapping(input::key_mapping(ctx->menu_select_control, nullptr, input::scancode::enter));
	ctx->input_event_router->add_mapping(input::key_mapping(ctx->menu_select_control, nullptr, input::scancode::space));
	
	
	
	ctx->input_event_router->add_mapping(input::key_mapping(ctx->control_system->get_toggle_view_control(), nullptr, input::scancode::tab));
	ctx->control_system->get_toggle_view_control()->set_activated_callback(
		[ctx]()
		{
			if (ctx->active_scene == ctx->overworld_scene)
			{
				ctx->active_scene = ctx->underworld_scene;
				ctx->radial_transition_inner->transition(0.5f, false, ease<float, double>::in_quad);
				
				auto switch_cameras = [ctx]()
				{
					ctx->overworld_camera->set_active(false);
					ctx->underworld_camera->set_active(true);
					ctx->fade_transition->transition(0.25f, true, ease<float, double>::out_quad);
				};
				
				float t = ctx->timeline->get_position();
				ctx->timeline->add_cue({t + 0.5f, switch_cameras});
			}
			else
			{
				ctx->active_scene = ctx->overworld_scene;
				ctx->fade_transition->transition(0.25f, false, ease<float, double>::out_quad);
				
				auto switch_cameras = [ctx]()
				{
					ctx->overworld_camera->set_active(true);
					ctx->underworld_camera->set_active(false);
					ctx->radial_transition_inner->transition(0.5f, true, ease<float, double>::out_quad);
				};
				
				float t = ctx->timeline->get_position();
				ctx->timeline->add_cue({t + 0.25f, switch_cameras});
			}
		});
	
	ctx->input_event_router->add_mapping(input::key_mapping(ctx->control_system->get_move_forward_control(), nullptr, input::scancode::w));
	ctx->input_event_router->add_mapping(input::game_controller_axis_mapping(ctx->control_system->get_move_forward_control(), nullptr, input::game_controller_axis::left_y, true));
	ctx->input_event_router->add_mapping(input::key_mapping(ctx->control_system->get_move_back_control(), nullptr, input::scancode::s));
	ctx->input_event_router->add_mapping(input::game_controller_axis_mapping(ctx->control_system->get_move_back_control(), nullptr, input::game_controller_axis::left_y, false));
	ctx->input_event_router->add_mapping(input::key_mapping(ctx->control_system->get_move_left_control(), nullptr, input::scancode::a));
	ctx->input_event_router->add_mapping(input::game_controller_axis_mapping(ctx->control_system->get_move_left_control(), nullptr, input::game_controller_axis::left_x, true));
	ctx->input_event_router->add_mapping(input::key_mapping(ctx->control_system->get_move_right_control(), nullptr, input::scancode::d));
	ctx->input_event_router->add_mapping(input::game_controller_axis_mapping(ctx->control_system->get_move_right_control(), nullptr, input::game_controller_axis::left_x, false));
	ctx->input_event_router->add_mapping(input::game_controller_axis_mapping(ctx->control_system->get_rotate_ccw_control(), nullptr, input::game_controller_axis::right_x, false));
	ctx->input_event_router->add_mapping(input::game_controller_axis_mapping(ctx->control_system->get_rotate_cw_control(), nullptr, input::game_controller_axis::right_x, true));
	ctx->input_event_router->add_mapping(input::game_controller_axis_mapping(ctx->control_system->get_tilt_up_control(), nullptr, input::game_controller_axis::right_y, false));
	ctx->input_event_router->add_mapping(input::game_controller_axis_mapping(ctx->control_system->get_tilt_down_control(), nullptr, input::game_controller_axis::right_y, true));
	ctx->input_event_router->add_mapping(input::mouse_wheel_mapping(ctx->control_system->get_zoom_in_control(), nullptr, input::mouse_wheel_axis::positive_y));
	ctx->input_event_router->add_mapping(input::mouse_wheel_mapping(ctx->control_system->get_zoom_out_control(), nullptr, input::mouse_wheel_axis::negative_y));
	ctx->input_event_router->add_mapping(input::mouse_button_mapping(ctx->control_system->get_adjust_camera_control(), nullptr, 3));
	ctx->input_event_router->add_mapping(input::game_controller_button_mapping(ctx->control_system->get_ascend_control(), nullptr, input::game_controller_button::y));
	ctx->input_event_router->add_mapping(input::game_controller_button_mapping(ctx->control_system->get_descend_control(), nullptr, input::game_controller_button::a));
	ctx->input_event_router->add_mapping(input::game_controller_axis_mapping(ctx->control_system->get_zoom_out_control(), nullptr, input::game_controller_axis::trigger_left, false));
	ctx->input_event_router->add_mapping(input::game_controller_axis_mapping(ctx->control_system->get_zoom_in_control(), nullptr, input::game_controller_axis::trigger_right, false));
	ctx->input_event_router->add_mapping(input::key_mapping(ctx->control_system->get_rotate_ccw_control(), nullptr, input::scancode::q));
	ctx->input_event_router->add_mapping(input::key_mapping(ctx->control_system->get_rotate_cw_control(), nullptr, input::scancode::e));
	ctx->input_event_router->add_mapping(input::key_mapping(ctx->control_system->get_fast_forward_control(), nullptr, input::scancode::dot));
	ctx->input_event_router->add_mapping(input::key_mapping(ctx->control_system->get_rewind_control(), nullptr, input::scancode::comma));
	
	ctx->input_event_router->add_mapping(input::key_mapping(ctx->control_system->get_exposure_increase_control(), nullptr, input::scancode::right_brace));
	ctx->input_event_router->add_mapping(input::key_mapping(ctx->control_system->get_exposure_decrease_control(), nullptr, input::scancode::left_brace));
	
	
	ctx->input_event_router->add_mapping(input::key_mapping(ctx->control_system->get_equip_brush_control(), nullptr, input::scancode::one));
	ctx->input_event_router->add_mapping(input::key_mapping(ctx->control_system->get_equip_twig_control(), nullptr, input::scancode::two));
	ctx->input_event_router->add_mapping(input::key_mapping(ctx->control_system->get_equip_forceps_control(), nullptr, input::scancode::three));
	ctx->input_event_router->add_mapping(input::key_mapping(ctx->control_system->get_equip_container_control(), nullptr, input::scancode::four));
	ctx->input_event_router->add_mapping(input::key_mapping(ctx->control_system->get_equip_lens_control(), nullptr, input::scancode::five));
	ctx->input_event_router->add_mapping(input::key_mapping(ctx->control_system->get_equip_marker_control(), nullptr, input::scancode::six));
	
	//ctx->input_event_router->add_mapping(input::key_mapping(ctx->control_system->get_next_marker_control(), nullptr, input::scancode::right_brace));
	//ctx->input_event_router->add_mapping(input::key_mapping(ctx->control_system->get_previous_marker_control(), nullptr, input::scancode::left_brace));
	
	ctx->input_event_router->add_mapping(input::mouse_button_mapping(ctx->control_system->get_use_tool_control(), nullptr, 1));
	ctx->control_system->get_use_tool_control()->set_activated_callback
	(
		[ctx]()
		{
			ctx->tool_system->set_tool_active(true);
		}
	);
	ctx->control_system->get_use_tool_control()->set_deactivated_callback
	(
		[ctx]()
		{
			ctx->tool_system->set_tool_active(false);
		}
	);
	
	ctx->control_system->get_equip_forceps_control()->set_activated_callback
	(
		[ctx]()
		{
			ctx->tool_system->set_active_tool(ctx->forceps_entity);
		}
	);
	ctx->control_system->get_equip_brush_control()->set_activated_callback
	(
		[ctx]()
		{
			ctx->tool_system->set_active_tool(ctx->brush_entity);
		}
	);
	ctx->control_system->get_equip_lens_control()->set_activated_callback
	(
		[ctx]()
		{
			ctx->tool_system->set_active_tool(ctx->lens_entity);
		}
	);
	ctx->control_system->get_equip_marker_control()->set_activated_callback
	(
		[ctx]()
		{
			ctx->tool_system->set_active_tool(ctx->marker_entity);
		}
	);
	ctx->control_system->get_equip_container_control()->set_activated_callback
	(
		[ctx]()
		{
			ctx->tool_system->set_active_tool(ctx->container_entity);
		}
	);
	ctx->control_system->get_equip_twig_control()->set_activated_callback
	(
		[ctx]()
		{
			ctx->tool_system->set_active_tool(ctx->twig_entity);
		}
	);
	
	ctx->control_system->get_next_marker_control()->set_activated_callback
	(
		[ctx]()
		{
			auto& marker = ctx->entity_registry->get<entity::component::marker>(ctx->marker_entity);
			marker.color = (marker.color + 1) % 8;			
			const gl::texture_2d* marker_albedo_texture = ctx->marker_albedo_textures[marker.color];
			
			model* marker_model = ctx->render_system->get_model_instance(ctx->marker_entity)->get_model();
			for (::model_group* group: *marker_model->get_groups())
			{
				material_property_base* albedo_property = group->get_material()->get_property("albedo_texture");
				if (albedo_property)
				{
					static_cast<material_property<const gl::texture_2d*>*>(albedo_property)->set_value(marker_albedo_texture);
				}
			}
		}
	);
	
	ctx->control_system->get_previous_marker_control()->set_activated_callback
	(
		[ctx]()
		{
			auto& marker = ctx->entity_registry->get<entity::component::marker>(ctx->marker_entity);
			marker.color = (marker.color + 7) % 8;			
			const gl::texture_2d* marker_albedo_texture = ctx->marker_albedo_textures[marker.color];
			
			model* marker_model = ctx->render_system->get_model_instance(ctx->marker_entity)->get_model();
			for (::model_group* group: *marker_model->get_groups())
			{
				material_property_base* albedo_property = group->get_material()->get_property("albedo_texture");
				if (albedo_property)
				{
					static_cast<material_property<const gl::texture_2d*>*>(albedo_property)->set_value(marker_albedo_texture);
				}
			}
		}
	);
	
	
	float time_scale = ctx->config->get<float>("time_scale");
	ctx->control_system->get_fast_forward_control()->set_activated_callback
	(
		[ctx, time_scale]()
		{
			ctx->orbit_system->set_time_scale(time_scale * 100.0f / seconds_per_day);
			ctx->astronomy_system->set_time_scale(time_scale * 100.0f / seconds_per_day);
		}
	);
	ctx->control_system->get_fast_forward_control()->set_deactivated_callback
	(
		[ctx, time_scale]()
		{
			ctx->orbit_system->set_time_scale(time_scale / seconds_per_day);
			ctx->astronomy_system->set_time_scale(time_scale / seconds_per_day);
		}
	);
	ctx->control_system->get_rewind_control()->set_activated_callback
	(
		[ctx, time_scale]()
		{
			ctx->orbit_system->set_time_scale(time_scale * -100.0f / seconds_per_day);
			ctx->astronomy_system->set_time_scale(time_scale * -100.0f / seconds_per_day);
		}
	);
	ctx->control_system->get_rewind_control()->set_deactivated_callback
	(
		[ctx, time_scale]()
		{
			ctx->orbit_system->set_time_scale(time_scale / seconds_per_day);
			ctx->astronomy_system->set_time_scale(time_scale / seconds_per_day);
		}
	);
	
	// Make lens tool's model instance unculled, so its shadow is always visible.
	scene::model_instance* lens_model_instance = ctx->render_system->get_model_instance(ctx->lens_entity);
	if (lens_model_instance)
	{
		lens_model_instance->set_culling_mask(&ctx->no_cull);
	}

}

void setup_cli(game::context* ctx)
{
	ctx->cli = new debug::cli();
	ctx->cli->register_command("echo", debug::cc::echo);
	ctx->cli->register_command("exit", std::function<std::string()>(std::bind(&debug::cc::exit, ctx)));
	ctx->cli->register_command("scrot", std::function<std::string()>(std::bind(&debug::cc::scrot, ctx)));
	ctx->cli->register_command("cue", std::function<std::string(float, std::string)>(std::bind(&debug::cc::cue, ctx, std::placeholders::_1, std::placeholders::_2)));
	//std::string cmd = "cue 20 exit";
	//logger->log(cmd);
	//logger->log(cli.interpret(cmd));
}

void setup_callbacks(game::context* ctx)
{
	// Set update callback
	ctx->app->set_update_callback
	(
		[ctx](double t, double dt)
		{
			// Update tweens
			ctx->time_tween->update();
			ctx->overworld_sky_pass->update_tweens();
			ctx->overworld_scene->update_tweens();
			ctx->underworld_scene->update_tweens();
			ctx->ui_scene->update_tweens();
			ctx->focal_point_tween->update();
			ctx->underworld_final_pass->get_material()->update_tweens();
			
			// Set time tween time
			(*ctx->time_tween)[1] = t;
						
			ctx->timeline->advance(dt);
			
			ctx->control_system->update(t, dt);
			ctx->terrain_system->update(t, dt);
			//ctx->vegetation_system->update(t, dt);
			ctx->snapping_system->update(t, dt);
			ctx->nest_system->update(t, dt);
			ctx->subterrain_system->update(t, dt);
			ctx->collision_system->update(t, dt);
			ctx->samara_system->update(t, dt);
			ctx->behavior_system->update(t, dt);
			ctx->locomotion_system->update(t, dt);
			ctx->camera_system->update(t, dt);
			ctx->tool_system->update(t, dt);
			
			ctx->orbit_system->update(t, dt);
			ctx->blackbody_system->update(t, dt);
			ctx->atmosphere_system->update(t, dt);
			ctx->astronomy_system->update(t, dt);
			ctx->spatial_system->update(t, dt);
			ctx->constraint_system->update(t, dt);
			ctx->tracking_system->update(t, dt);
			ctx->painting_system->update(t, dt);
			ctx->proteome_system->update(t, dt);
			
			//(*ctx->focal_point_tween)[1] = ctx->orbit_cam->get_focal_point();
			
			auto xf = entity::command::get_world_transform(*ctx->entity_registry, ctx->lens_entity);
			//ctx->lens_spot_light->look_at(xf.translation, xf.translation + ctx->sun_direct->get_direction(), {0, 1, 0});
			
			xf = entity::command::get_world_transform(*ctx->entity_registry, ctx->flashlight_entity);
			//ctx->flashlight_spot_light->set_transform(xf);
			ctx->flashlight_spot_light->look_at(xf.translation, xf.translation + xf.rotation * float3{0, 0, 1}, {0, 0, -1});
			
			ctx->ui_system->update(dt);
			ctx->render_system->update(t, dt);
			ctx->animator->animate(dt);
			
			ctx->application_controls->update();
			ctx->menu_controls->update();
			ctx->camera_controls->update();
		}
	);
	
	// Set render callback
	ctx->app->set_render_callback
	(
		[ctx](double alpha)
		{
			ctx->render_system->draw(alpha);
		}
	);
}
