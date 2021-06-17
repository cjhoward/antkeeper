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

#include "animation/ease.hpp"
#include "animation/screen-transition.hpp"
#include "configuration.hpp"
#include "debug/logger.hpp"
#include "entity/archetype.hpp"
#include "entity/components/cavity.hpp"
#include "entity/components/copy-transform.hpp"
#include "entity/components/copy-translation.hpp"
#include "entity/components/model.hpp"
#include "entity/components/genome.hpp"
#include "entity/components/snap.hpp"
#include "entity/components/terrain.hpp"
#include "entity/components/tool.hpp"
#include "entity/components/transform.hpp"
#include "entity/components/camera-follow.hpp"
#include "entity/components/orbit.hpp"
#include "entity/components/blackbody.hpp"
#include "entity/components/celestial-body.hpp"
#include "entity/components/atmosphere.hpp"
#include "entity/components/light.hpp"
#include "entity/components/observer.hpp"
#include "entity/commands.hpp"
#include "game/game-context.hpp"
#include "game/states/game-states.hpp"
#include "math/math.hpp"
#include "nest.hpp"
#include "renderer/material.hpp"
#include "gl/texture-2d.hpp"
#include "gl/texture-filter.hpp"
#include "gl/texture-wrapping.hpp"
#include "renderer/model.hpp"
#include "renderer/passes/sky-pass.hpp"
#include "renderer/passes/shadow-map-pass.hpp"
#include "resources/resource-manager.hpp"
#include "scene/model-instance.hpp"
#include "scene/collection.hpp"
#include "scene/camera.hpp"
#include "scene/ambient-light.hpp"
#include "scene/directional-light.hpp"
#include "entity/systems/control.hpp"
#include "entity/systems/camera.hpp"
#include "entity/systems/render.hpp"
#include "entity/systems/tool.hpp"
#include "entity/systems/orbit.hpp"
#include "entity/systems/astronomy.hpp"
#include "game/biome.hpp"
#include "utility/fundamental-types.hpp"
#include "utility/bit-math.hpp"
#include "genetics/genetics.hpp"
#include "math/random.hpp"
#include <iostream>
#include <bitset>
#include <ctime>

void play_state_enter(game_context* ctx)
{
	debug::logger* logger = ctx->logger;
	logger->push_task("Entering play state");
	
	resource_manager* resource_manager = ctx->resource_manager;
	entt::registry& entity_registry = *ctx->entity_registry;
	
	// Load biome
	if (ctx->option_biome.has_value())
	{
		ctx->biome = resource_manager->load<biome>(ctx->option_biome.value() + ".bio");
	}
	else
	{
		ctx->biome = resource_manager->load<biome>("forest.bio");
	}
	
	// Apply biome parameters to scene
	sky_pass* sky_pass = ctx->overworld_sky_pass;
	sky_pass->set_enabled(true);
	sky_pass->set_sky_model(ctx->resource_manager->load<model>("sky-dome.mdl"));
	sky_pass->set_moon_model(ctx->resource_manager->load<model>("moon.mdl"));
	
	// Create sun
	auto sun_entity = entity_registry.create();
	{
		entity::component::celestial_body body;
		body.radius = 6.957e+8;
		body.axial_tilt = math::radians(0.0);
		body.axial_rotation = math::radians(0.0);
		body.angular_frequency = math::radians(0.0);
		
		entity::component::orbit orbit;
		orbit.elements.a = 0.0;
		orbit.elements.e = 0.0;
		orbit.elements.i = math::radians(0.0);
		orbit.elements.raan = math::radians(0.0);
		orbit.elements.w = math::radians(0.0);
		orbit.elements.ta = math::radians(0.0);
		
		entity::component::blackbody blackbody;
		blackbody.temperature = 5778.0;
		
		entity::component::transform transform;
		transform.local = math::identity_transform<float>;
		transform.warp = true;
		
		entity_registry.assign<entity::component::celestial_body>(sun_entity, body);
		entity_registry.assign<entity::component::orbit>(sun_entity, orbit);
		entity_registry.assign<entity::component::blackbody>(sun_entity, blackbody);
		entity_registry.assign<entity::component::transform>(sun_entity, transform);
	}
	
	// Create Earth
	auto earth_entity = entity_registry.create();
	{
		entity::component::celestial_body body;
		body.radius = 6.3781e6;
		body.axial_tilt = math::radians(23.4393);
		body.axial_rotation = math::radians(280.46061837504);
		body.angular_frequency = math::radians(360.9856122880876128);
		
		entity::component::orbit orbit;
		orbit.elements.a = 1.496e+11;
		orbit.elements.e = 0.01671123;
		orbit.elements.i = math::radians(-0.00001531);
		orbit.elements.raan = math::radians(0.0);
		const double longitude_periapsis = math::radians(102.93768193);
		orbit.elements.w = longitude_periapsis - orbit.elements.raan;
		orbit.elements.ta = math::radians(100.46457166) - longitude_periapsis;
		
		entity::component::terrain terrain;
		terrain.elevation = [](double, double) -> double
		{
			//return math::random<double>(0.0, 1.0);
			return 0.0;
		};
		terrain.max_lod = 18;
		terrain.patch_material = resource_manager->load<material>("desert-terrain.mtl");
		
		entity::component::atmosphere atmosphere;
		atmosphere.exosphere_altitude = 65e3;
		atmosphere.index_of_refraction = 1.000293;
		atmosphere.rayleigh_density = 2.545e25;
		atmosphere.rayleigh_scale_height = 8000.0;
		atmosphere.mie_density = 14.8875;
		atmosphere.mie_scale_height = 1200.0;
		atmosphere.mie_anisotropy = 0.8;
		
		entity::component::transform transform;
		transform.local = math::identity_transform<float>;
		transform.warp = true;
		
		entity_registry.assign<entity::component::celestial_body>(earth_entity, body);
		entity_registry.assign<entity::component::orbit>(earth_entity, orbit);
		entity_registry.assign<entity::component::atmosphere>(earth_entity, atmosphere);
		entity_registry.assign<entity::component::terrain>(earth_entity, terrain);
		entity_registry.assign<entity::component::transform>(earth_entity, transform);
	}
	
	// Create observer
	auto observer_eid = entity_registry.create();
	{
		entity::component::observer observer;
		observer.reference_body_eid = earth_entity;
		observer.elevation = 0.0;
		observer.latitude = 0.0;
		observer.longitude = 0.0;
		observer.camera = ctx->overworld_camera;
		
		entity_registry.assign<entity::component::observer>(observer_eid, observer);
	}
	
	scene::ambient_light* ambient = new scene::ambient_light();
	ambient->set_color({1, 1, 1});
	ambient->set_intensity(0.0f);
	ambient->update_tweens();
	ctx->overworld_scene->add_object(ambient);
	
	scene::directional_light* sun = new scene::directional_light();
	//sun->set_intensity(1000.0f);
	//sun->set_light_texture(resource_manager->load<gl::texture_2d>("forest-gobo.tex"));
	//sun->set_light_texture_scale({2000, 2000});
	//sun->set_light_texture_opacity(0.925f);
	//sun->look_at({2, 1, 0}, {0, 0, 0}, {0, 0, 1});
	//sun->update_tweens();
	ctx->overworld_scene->add_object(sun);
	ctx->overworld_shadow_map_pass->set_light(sun);
	
	// Set universal time
	const double universal_time = 0.0;
	ctx->astronomy_system->set_universal_time(universal_time);
	ctx->orbit_system->set_universal_time(universal_time);
	
	// Set astronomy system observation parameters
	ctx->astronomy_system->set_reference_body(earth_entity);
	ctx->astronomy_system->set_observer_location(double3{0.0, math::radians(0.0f), math::radians(0.0f)});
	ctx->astronomy_system->set_sun_light(sun);
	ctx->astronomy_system->set_sky_pass(ctx->overworld_sky_pass);

	// Load entity archetypes
	entity::archetype* ant_hill_archetype = resource_manager->load<entity::archetype>("ant-hill.ent");
	entity::archetype* nest_archetype = resource_manager->load<entity::archetype>("harvester-nest.ent");
	entity::archetype* redwood_archetype = resource_manager->load<entity::archetype>("redwood.ent");
	entity::archetype* forceps_archetype = resource_manager->load<entity::archetype>("forceps.ent");
	entity::archetype* lens_archetype = resource_manager->load<entity::archetype>("lens.ent");
	entity::archetype* brush_archetype = resource_manager->load<entity::archetype>("brush.ent");
	entity::archetype* marker_archetype = resource_manager->load<entity::archetype>("marker.ent");
	entity::archetype* container_archetype = resource_manager->load<entity::archetype>("container.ent");
	entity::archetype* twig_archetype = resource_manager->load<entity::archetype>("twig.ent");
	entity::archetype* larva_archetype = resource_manager->load<entity::archetype>("ant-larva.ent");
	entity::archetype* flashlight_archetype = resource_manager->load<entity::archetype>("flashlight.ent");
	entity::archetype* flashlight_light_cone_archetype = resource_manager->load<entity::archetype>("flashlight-light-cone.ent");
	entity::archetype* lens_light_cone_archetype = resource_manager->load<entity::archetype>("lens-light-cone.ent");
	entity::archetype* cube_archetype = resource_manager->load<entity::archetype>("unit-cube.ent");
	entity::archetype* color_checker_archetype = resource_manager->load<entity::archetype>("color-checker.ent");
	
	// Create tools
	/*
	forceps_archetype->assign(entity_registry, ctx->forceps_entity);
	lens_archetype->assign(entity_registry, ctx->lens_entity);
	brush_archetype->assign(entity_registry, ctx->brush_entity);
	marker_archetype->assign(entity_registry, ctx->marker_entity);
	container_archetype->assign(entity_registry, ctx->container_entity);
	twig_archetype->assign(entity_registry, ctx->twig_entity);
	*/
	
	// Create flashlight and light cone, set light cone parent to flashlight, and move both to underworld scene
	/*
	flashlight_archetype->assign(entity_registry, ctx->flashlight_entity);
	auto flashlight_light_cone = flashlight_light_cone_archetype->create(entity_registry);
	entity::command::parent(entity_registry, flashlight_light_cone, ctx->flashlight_entity);
	entity::command::assign_render_layers(entity_registry, ctx->flashlight_entity, 2);
	entity::command::assign_render_layers(entity_registry, flashlight_light_cone, 2);
	*/
	
	// Make lens tool's model instance unculled, so its shadow is always visible.
	//scene::model_instance* lens_model_instance = ctx->render_system->get_model_instance(ctx->lens_entity);
	//if (lens_model_instance)
	//{
		//lens_model_instance->set_culling_mask(&ctx->no_cull);
	//}
	
	// Create lens light cone and set its parent to lens
	//auto lens_light_cone = lens_light_cone_archetype->create(entity_registry);
	//entity::command::bind_transform(entity_registry, lens_light_cone, ctx->lens_entity);
	//entity::command::parent(entity_registry, lens_light_cone, ctx->lens_entity);
	
	// Hide inactive tools
	/*
	entity::command::assign_render_layers(entity_registry, ctx->forceps_entity, 0);
	entity::command::assign_render_layers(entity_registry, ctx->brush_entity, 0);
	entity::command::assign_render_layers(entity_registry, ctx->lens_entity, 0);
	entity::command::assign_render_layers(entity_registry, ctx->marker_entity, 0);
	entity::command::assign_render_layers(entity_registry, ctx->container_entity, 0);
	entity::command::assign_render_layers(entity_registry, ctx->twig_entity, 0);
	*/
	
	// Activate brush tool
	//ctx->tool_system->set_active_tool(ctx->brush_entity);

	// Create ant-hill
	//auto ant_hill_entity = ant_hill_archetype->create(entity_registry);
	//entity::command::place(entity_registry, ant_hill_entity, earth_entity, 0.0, 0.0, 0.0);
	
	// Create color checker
	/*
	auto color_checker = color_checker_archetype->create(entity_registry);
	entity::command::place(entity_registry, color_checker, {-10, -10});
	auto& cc_transform = entity_registry.get<entity::component::transform>(color_checker);
	cc_transform.local.scale *= 10.0f;
	cc_transform.local.rotation = math::angle_axis(math::radians(-90.0f), {1, 0, 0});
	*/
	
	// Setup camera focal point
	entity::component::transform focal_point_transform;
	focal_point_transform.local = math::identity_transform<float>;
	//focal_point_transform.local.translation = {0, 6.3781e6, 0};
	focal_point_transform.warp = true;
	entity::component::camera_follow focal_point_follow;
	entity::component::snap focal_point_snap;
	focal_point_snap.ray = {float3{0, 10000, 0}, float3{0, -1, 0}};
	focal_point_snap.warp = false;
	focal_point_snap.relative = true;
	focal_point_snap.autoremove = false;
	entity_registry.assign_or_replace<entity::component::transform>(ctx->focal_point_entity, focal_point_transform);
	entity_registry.assign_or_replace<entity::component::camera_follow>(ctx->focal_point_entity, focal_point_follow);
	//entity_registry.assign_or_replace<entity::component::snap>(ctx->focal_point_entity, focal_point_snap);
	
	// Setup camera
	ctx->overworld_camera->look_at({0, 0, 1}, {0, 0, 0}, {0, 1, 0});
	ctx->overworld_camera->set_exposure(-14.5f);
	ctx->camera_system->set_camera(ctx->overworld_camera);
	
	ctx->overworld_scene->update_tweens();
	
	// Allocate a nest
	nest* nest = new ::nest();
	
	// Setup initial nest parameters
	float tunnel_radius = 1.15f;
	nest->set_tunnel_radius(tunnel_radius);
	nest::shaft* central_shaft = nest->get_central_shaft();
	central_shaft->chirality = 1.0f;
	central_shaft->rotation = math::radians(0.0f);
	central_shaft->depth = {0.0f, 200.0f};
	central_shaft->radius = {15.0f, 15.0f};
	central_shaft->pitch = {40.0f, 40.0f};
	central_shaft->translation = {{{0.0f, 0.0f}, {0.0f, 0.0f}}};
	central_shaft->current_depth = 0.0f;
	for (std::size_t i = 0; i < 4; ++i)
	{
		nest::chamber chamber;
		chamber.shaft = central_shaft;
		chamber.depth = (i + 1) * 50.0f;
		chamber.rotation = math::radians(0.0f);
		chamber.inner_radius = 4.0f;
		chamber.outer_radius = 10.0f;
		central_shaft->chambers.push_back(chamber);
	}
	
	// Dig nest shafts
	float shift = 0.1f;
	for (int i = 0; i < 800; ++i)
	{
		entity::component::cavity cavity;
		cavity.position = nest->extend_shaft(*nest->get_central_shaft());
		cavity.position += float3{math::random(-shift, shift), math::random(-shift, shift), math::random(-shift, shift)};
		cavity.radius = tunnel_radius * math::random(1.0f, 1.1f);

		entity_registry.assign<entity::component::cavity>(entity_registry.create(), cavity);
	}

	// Dig nest chambers
	/*
	for (int i = 0; i < central_shaft->chambers.size(); ++i)
	{
		for (int j = 0; j < 150; ++j)
		{
			entity::component::cavity cavity;
			cavity.position = nest->expand_chamber(central_shaft->chambers[i]);
			cavity.position += float3{math::random(-shift, shift), math::random(-shift, shift), math::random(-shift, shift)};
			cavity.radius = tunnel_radius * math::random(1.0f, 1.1f);

			entity_registry.assign<entity::component::cavity>(entity_registry.create(), cavity);
		}
	}
	*/
	
	// Place larva in chamber
	{
		auto larva_eid = larva_archetype->create(entity_registry);
		entity::command::assign_render_layers(entity_registry, larva_eid, 1);
		entity::command::warp_to(entity_registry, larva_eid, {50, 0.1935f, 0});
		//auto& transform = entity_registry.get<entity::component::transform>(larva_entity);
		//transform.transform = math::identity_transform<float>;
		//transform.transform.translation = nest->get_shaft_position(*central_shaft, central_shaft->depth[1]);
		//transform.transform.translation.y -= 1.0f;
		
		// Construct larva genome
		entity::component::genome genome;
		genome.ploidy = 2;
		const std::string antp = "ATGACCATGAGCACCAACAACTGCGAAAGCATGACCAGCTATTTTACCAACAGCTATATGGGCGCGGATATGCATCATGGCCATTATCCGGGCAACGGCGTGACCGATCTGGATGCGCAGCAGATGCATCATTATAGCCAGAACGCGAACCATCAGGGCAACATGCCGTATCCGCGCTTTCCGCCGTATGATCGCATGCCGTATTATAACGGCCAGGGCATGGATCAGCAGCAGCAGCATCAGGTGTATAGCCGCCCGGATAGCCCGAGCAGCCAGGTGGGCGGCGTGATGCCGCAGGCGCAGACCAACGGCCAGCTGGGCGTGCCGCAGCAGCAGCAGCAGCAGCAGCAGCAGCCGAGCCAGAACCAGCAGCAGCAGCAGGCGCAGCAGGCGCCGCAGCAGCTGCAGCAGCAGCTGCCGCAGGTGACCCAGCAGGTGACCCATCCGCAGCAGCAGCAGCAGCAGCCGGTGGTGTATGCGAGCTGCAAACTGCAGGCGGCGGTGGGCGGCCTGGGCATGGTGCCGGAAGGCGGCAGCCCGCCGCTGGTGGATCAGATGAGCGGCCATCATATGAACGCGCAGATGACCCTGCCGCATCATATGGGCCATCCGCAGGCGCAGGTGCATCAGAACCATCATAACATGGGCATGTATCAGCAGCAGAGCGGCGTGCCGCCGGTGGGCGCGCCGCCGCAGGGCATGATGCATCAGGGCCAGGGCCCGCCGCAGATGCATCAGGGCCATCCGGGCCAGCATACCCCGCCGAGCCAGAACCCGAACAGCCAGAGCAGCGGCATGCCGAGCCCGCTGTATCCGTGGATGCGCAGCCAGTTTGAACGCAAACGCGGCCGCCAGACCTATACCCGCTATCAGACCCTGGAACTGGAAAAAGAATTTCATTTTAACCGCTATCTGACCCGCCGCCGCCGCATTGAAATTGCGCATGCGCTGTGCCTGACCGAACGCCAGATTAAAATTTGGTTTCAGAACCGCCGCATGAAATGGAAAAAAGAAAACAAAACCAAAGGCGAACCGGGCAGCGGCGGCGAAGGCGATGAAATTACCCCGCCGAACAGCCCGCAGTAG";
		genome.chromosomes.push_back(antp);
		entity_registry.assign<entity::component::genome>(larva_eid, genome);
	}
	
	entity::system::control* control_system = ctx->control_system;
	control_system->update(0.0, 0.0);
	control_system->set_nest(nest);
	
	// Start fade in
	ctx->fade_transition->transition(1.0f, true, ease<float>::in_quad);

	logger->pop_task(EXIT_SUCCESS);
	
	std::string biome_name = (*ctx->strings)[ctx->biome->name];
	logger->log("Entered biome \"" + biome_name + "\"");
}

void play_state_exit(game_context* ctx)
{
	debug::logger* logger = ctx->logger;
	logger->push_task("Exiting play state");
	
	logger->pop_task(EXIT_SUCCESS);
}
