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

#include "entity/systems/atmosphere.hpp"
#include "physics/atmosphere.hpp"

namespace entity {
namespace system {

atmosphere::atmosphere(entity::registry& registry):
	updatable(registry),
	rgb_wavelengths_nm{0, 0, 0},
	rgb_wavelengths_m{0, 0, 0}
{
	registry.on_construct<entity::component::atmosphere>().connect<&atmosphere::on_atmosphere_construct>(this);
	registry.on_replace<entity::component::atmosphere>().connect<&atmosphere::on_atmosphere_replace>(this);
}

void atmosphere::update(double t, double dt)
{}

void atmosphere::set_rgb_wavelengths(const double3& wavelengths)
{
	rgb_wavelengths_nm = wavelengths;
	rgb_wavelengths_m = wavelengths * 1e-9;
}

void atmosphere::update_coefficients(entity::id entity_id)
{
	// Abort if entity has no atmosphere component
	if (!registry.has<component::atmosphere>(entity_id))
		return;
	
	// Get atmosphere component of the entity
	component::atmosphere& atmosphere = registry.get<component::atmosphere>(entity_id);
	
	// Calculate polarization factors
	const double rayleigh_polarization = physics::atmosphere::polarization(atmosphere.index_of_refraction, atmosphere.rayleigh_density);
	const double mie_polarization = physics::atmosphere::polarization(atmosphere.index_of_refraction, atmosphere.mie_density);
	
	// Calculate Rayleigh scattering coefficients
	atmosphere.rayleigh_scattering =
	{
		physics::atmosphere::scattering_rayleigh(rgb_wavelengths_m.x, atmosphere.rayleigh_density, rayleigh_polarization),
		physics::atmosphere::scattering_rayleigh(rgb_wavelengths_m.y, atmosphere.rayleigh_density, rayleigh_polarization),
		physics::atmosphere::scattering_rayleigh(rgb_wavelengths_m.z, atmosphere.rayleigh_density, rayleigh_polarization)
	};
	
	// Calculate Mie scattering coefficients
	const double mie_scattering = physics::atmosphere::scattering_mie(atmosphere.mie_density, mie_polarization);
	atmosphere.mie_scattering = 
	{
		mie_scattering,
		mie_scattering,
		mie_scattering
	};
}

void atmosphere::on_atmosphere_construct(entity::registry& registry, entity::id entity_id, entity::component::atmosphere& atmosphere)
{
	update_coefficients(entity_id);
}

void atmosphere::on_atmosphere_replace(entity::registry& registry, entity::id entity_id, entity::component::atmosphere& atmosphere)
{
	update_coefficients(entity_id);
}

} // namespace system
} // namespace entity
