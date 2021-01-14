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

#include "render-system.hpp"
#include "ecs/components/transform-component.hpp"
#include "renderer/renderer.hpp"

namespace ecs {

render_system::render_system(ecs::registry& registry):
	entity_system(registry),
	renderer(nullptr)
{
	registry.on_construct<model_component>().connect<&render_system::on_model_construct>(this);
	registry.on_replace<model_component>().connect<&render_system::on_model_replace>(this);
	registry.on_destroy<model_component>().connect<&render_system::on_model_destroy>(this);
}

void render_system::update(double t, double dt)
{
	registry.view<transform_component, model_component>().each
	(
		[this](ecs::entity entity, auto& transform, auto& model)
		{
			scene::model_instance* instance = model_instances[entity];

			instance->set_transform(transform.world);

			if (transform.warp)
			{
				instance->get_transform_tween().update();
				instance->update_tweens();
				transform.warp = false;
			}
		}
	);
}

void render_system::render(double alpha)
{
	if (renderer)
	{
		for (const scene::collection* collection: layers)
		{
			renderer->render(alpha, *collection);
		}
	}
}

void render_system::add_layer(scene::collection* layer)
{
	layers.push_back(layer);
}

void render_system::remove_layers()
{
	layers.clear();
}

void render_system::set_renderer(::renderer* renderer)
{
	this->renderer = renderer;
}

scene::model_instance* render_system::get_model_instance(ecs::entity entity)
{
	if (auto it = model_instances.find(entity); it != model_instances.end())
		return it->second;
	return nullptr;
}

void render_system::update_model_and_materials(ecs::entity entity, model_component& model)
{
	if (auto model_it = model_instances.find(entity); model_it != model_instances.end())
	{
		model_it->second->set_model(model.model);
		model_it->second->set_instanced((model.instance_count > 0), model.instance_count);
		
		for (auto material_it = model.materials.begin(); material_it != model.materials.end(); ++material_it)
		{
			model_it->second->set_material(material_it->first, material_it->second);
		}
		
		// Add model instance to its specified layers
		for (std::size_t i = 0; i < std::min<std::size_t>(layers.size(), (sizeof(model.layers) << 3)); ++i)
		{
			layers[i]->remove_object(model_it->second);
			
			if ((model.layers >> i) & 1)
			{
				layers[i]->add_object(model_it->second);
			}
		}
	}
}

void render_system::on_model_construct(ecs::registry& registry, ecs::entity entity, model_component& model)
{
	scene::model_instance* model_instance = new scene::model_instance();	
	model_instances[entity] = model_instance;
	update_model_and_materials(entity, model);
}

void render_system::on_model_replace(ecs::registry& registry, ecs::entity entity, model_component& model)
{
	update_model_and_materials(entity, model);
}

void render_system::on_model_destroy(ecs::registry& registry, ecs::entity entity)
{
	if (auto it = model_instances.find(entity); it != model_instances.end())
	{
		// Remove model instance from all layers
		for (std::size_t i = 0; i < layers.size(); ++i)
		{
			layers[i]->remove_object(it->second);
		}
		
		delete it->second;
		model_instances.erase(it);
	}
}

} // namespace ecs