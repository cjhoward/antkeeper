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

#include "renderer/simple-render-pass.hpp"
#include "rasterizer/rasterizer.hpp"
#include "rasterizer/framebuffer.hpp"
#include "rasterizer/shader-program.hpp"
#include "rasterizer/shader-input.hpp"
#include "rasterizer/vertex-buffer.hpp"
#include "rasterizer/vertex-array.hpp"
#include "rasterizer/vertex-attribute-type.hpp"
#include "rasterizer/drawing-mode.hpp"
#include "rasterizer/texture-2d.hpp"
#include "rasterizer/texture-wrapping.hpp"
#include "rasterizer/texture-filter.hpp"
#include "renderer/vertex-attributes.hpp"
#include "renderer/render-context.hpp"
#include "renderer/material.hpp"
#include "renderer/material-property.hpp"
#include <vmq/vmq.hpp>
#include <glad/glad.h>

using namespace vmq;

simple_render_pass::simple_render_pass(::rasterizer* rasterizer, const ::framebuffer* framebuffer, ::shader_program* shader_program):
	render_pass(rasterizer, framebuffer),
	shader_program(shader_program),
	time_tween(nullptr)
{
	// Create material
	material = new ::material(shader_program);
	
	// Add standard properties to material
	time_property = material->add_property<float>("time");
	resolution_property = material->add_property<float2>("resolution");
	
	const float vertex_data[] =
	{
		-1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f
	};

	std::size_t vertex_size = 3;
	std::size_t vertex_stride = sizeof(float) * vertex_size;
	std::size_t vertex_count = 6;

	quad_vbo = new vertex_buffer(sizeof(float) * vertex_size * vertex_count, vertex_data);
	quad_vao = new vertex_array();
	quad_vao->bind_attribute(VERTEX_POSITION_LOCATION, *quad_vbo, 3, vertex_attribute_type::float_32, vertex_stride, 0);
}

simple_render_pass::~simple_render_pass()
{
	delete material;
	delete quad_vao;
	delete quad_vbo;
}

void simple_render_pass::render(render_context* context) const
{
	// Bind framebuffer
	rasterizer->use_framebuffer(*framebuffer);
	
	// Setup graphics context
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Setup viewport
	auto viewport = framebuffer->get_dimensions();
	rasterizer->set_viewport(0, 0, std::get<0>(viewport), std::get<1>(viewport));
	
	// Change shader program
	rasterizer->use_program(*shader_program);
	
	// Get interpolated time
	float time = (time_tween) ? time_tween->interpolate(context->alpha) : 0.0f;
	
	// Update material properties
	time_property->set_value(time);
	resolution_property->set_value({static_cast<float>(std::get<0>(viewport)), static_cast<float>(std::get<1>(viewport))});
	
	// Upload material properties
	material->upload();

	// Draw quad
	rasterizer->draw_arrays(*quad_vao, drawing_mode::triangles, 0, 6);
}

void simple_render_pass::set_time_tween(const tween<double>* time)
{
	this->time_tween = time;
}
