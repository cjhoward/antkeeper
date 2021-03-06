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

#include "renderer/shader-template.hpp"
#include <algorithm>
#include <sstream>

shader_template::shader_template(const std::string& source_code)
{
	source(source_code);
}

shader_template::shader_template()
{}

void shader_template::source(const std::string& source)
{
	// Reset template
	template_source.clear();
	vertex_directives.clear();
	fragment_directives.clear();
	geometry_directives.clear();
	define_directives.clear();
	
	// Iterate through source line-by-line
	std::istringstream source_stream(source);
	std::string line;
	while (std::getline(source_stream, line))
	{
		std::string token;
		std::istringstream line_stream(line);
		
		// Detect `#pragma` directives
		if (line_stream >> token && token == "#pragma")
		{
			if (line_stream >> token)
			{
				// Map line numbers of supported directives
				if (token == "define")
				{
					if (line_stream >> token)
						define_directives.insert({token, template_source.size()});
				}
				else if (token == "vertex")
					vertex_directives.insert(template_source.size());
				else if (token == "fragment")
					fragment_directives.insert(template_source.size());
				else if (token == "geometry")
					geometry_directives.insert(template_source.size());
			}
		}
		
		// Append line to template source
		template_source.push_back(line);
	}
}

std::string shader_template::configure(gl::shader_stage stage, const dictionary_type& definitions) const
{
	replace_stage_directives(stage);
	replace_define_directives(definitions);
	
	// Join vector of source lines into single string
	std::ostringstream stream;
	std::copy(template_source.begin(), template_source.end(), std::ostream_iterator<std::string>(stream, "\n"));
	return stream.str();
}

gl::shader_object* shader_template::compile(gl::shader_stage stage, const dictionary_type& definitions) const
{
	// Generate shader object source
	std::string object_source = configure(stage, definitions);
	
	// Create new shader object
	gl::shader_object* object = new gl::shader_object(stage);
	
	// Set shader object source
	object->source(object_source);
	
	// Compile shader object
	object->compile();
	
	return object;
}

gl::shader_program* shader_template::build(const dictionary_type& definitions) const
{
	gl::shader_object* vertex_object = nullptr;
	gl::shader_object* fragment_object = nullptr;
	gl::shader_object* geometry_object = nullptr;
	
	// Create shader program
	gl::shader_program* program = new gl::shader_program();
	
	if (has_vertex_directive())
	{
		// Compile vertex shader object and attach to shader program
		vertex_object = compile(gl::shader_stage::vertex, definitions);
		program->attach(vertex_object);
	}
	
	if (has_fragment_directive())
	{
		// Compile fragment shader object and attach to shader program
		fragment_object = compile(gl::shader_stage::fragment, definitions);
		program->attach(fragment_object);
	}
	
	if (has_geometry_directive())
	{
		// Compile fragment shader object and attach to shader program
		geometry_object = compile(gl::shader_stage::geometry, definitions);
		program->attach(geometry_object);
	}
	
	// Link attached shader objects into shader program
	program->link();
	
	if (vertex_object)
	{
		// Detach and delete vertex shader object
		program->detach(vertex_object);
		delete vertex_object;
	}
	
	if (fragment_object)
	{
		// Detach and delete fragment shader object
		program->detach(fragment_object);
		delete fragment_object;
	}
	
	if (geometry_object)
	{
		// Detach and delete geometry shader object
		program->detach(geometry_object);
		delete geometry_object;
	}
	
	return program;
}

void shader_template::replace_stage_directives(gl::shader_stage stage) const
{
	// Determine stage directives according to the shader stage being generated
	const std::string vertex_directive = (stage == gl::shader_stage::vertex) ? "#define __VERTEX__" : "/* #undef __VERTEX__ */";
	const std::string fragment_directive = (stage == gl::shader_stage::fragment) ? "#define __FRAGMENT__" : "/* #undef __FRAGMENT__ */";
	const std::string geometry_directive = (stage == gl::shader_stage::geometry) ? "#define __GEOMETRY__" : "/* #undef __GEOMETRY__ */";
	
	// Handle `#pragma <stage>` directives
	for (std::size_t i: vertex_directives)
		template_source[i] = vertex_directive;
	for (std::size_t i: fragment_directives)
		template_source[i] = fragment_directive;
	for (std::size_t i: geometry_directives)
		template_source[i] = geometry_directive;
}

void shader_template::replace_define_directives(const dictionary_type& definitions) const
{
	// For each `#pragma define <key>` directive
	for (const auto& define_directive: define_directives)
	{
		// Get a reference to the directive line
		std::string& line = template_source[define_directive.second];
		
		// Check if the corresponding definition was given by the configuration
		auto definitions_it = definitions.find(define_directive.first);
		if (definitions_it != definitions.end())
		{
			// Definition found, Replace `#pragma define <key>` with `#define <key>` or `#define <key> <value>`
			line = "#define " + define_directive.first;
			if (!definitions_it->second.empty())
				line += " " + definitions_it->second;
		}
		else
		{
			// Definition not found, replace `#pragma define <key>` with the comment `/* #undef <key> */`.
			line = "/* #undef " + define_directive.first + " */";
		}
	}
}

bool shader_template::has_vertex_directive() const
{
	return !vertex_directives.empty();
}

bool shader_template::has_fragment_directive() const
{
	return !fragment_directives.empty();
}

bool shader_template::has_geometry_directive() const
{
	return !geometry_directives.empty();
}

bool shader_template::has_define_directive(const std::string& key) const
{
	return (define_directives.find(key) != define_directives.end());
}
