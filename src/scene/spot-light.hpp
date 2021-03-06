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

#ifndef ANTKEEPER_SCENE_SPOT_LIGHT_HPP
#define ANTKEEPER_SCENE_SPOT_LIGHT_HPP

#include "scene/light.hpp"
#include "utility/fundamental-types.hpp"

namespace scene {

/**
 * Directional cone light source.
 */
class spot_light: public light
{
public:
	/// Creates a spot light.
	spot_light();
	
	/// Returns light_type::spot
	virtual light_type get_light_type() const;
	
	/**
	 * Sets the attenuation factors of the light.
	 *
	 * @param attenuation Vector containing the constant, linear, and quadratic attenuation factors, as x, y, and z, respectively.
	 */
	void set_attenuation(const float3& attenuation);
	
	/**
	 * Sets the spot light cutoff angles.
	 *
	 * @param cutoff Vector containing the inner and outer cutoff angles, as x and y, respectively.
	 */
	void set_cutoff(const float2& cutoff);
	
	/// Returns the direction vector.
	const float3& get_direction() const;
	
	/// Returns the attenuation factors of the light.
	const float3& get_attenuation() const;
	
	/// Returns the spot light cutoff angles.
	const float2& get_cutoff() const;
	
	/// Returns the cosine of the spot light cutoff angles.
	const float2& get_cosine_cutoff() const;
	
	/// Returns the direction tween.
	const tween<float3>& get_direction_tween() const;
	
	/// Returns the attenuation tween.
	const tween<float3>& get_attenuation_tween() const;
	
	/// Returns the cutoff tween.
	const tween<float2>& get_cutoff_tween() const;
	
	/// Returns the cosine cutoff tween.
	const tween<float2>& get_cosine_cutoff_tween() const;

	/// @copydoc object_base::update_tweens();
	virtual void update_tweens();

private:
	virtual void transformed();

	tween<float3> direction;
	tween<float3> attenuation;
	tween<float2> cutoff;
	tween<float2> cosine_cutoff;
};

inline light_type spot_light::get_light_type() const
{
	return light_type::spot;
}

inline const float3& spot_light::get_direction() const
{
	return direction[1];
}

inline const float3& spot_light::get_attenuation() const
{
	return attenuation[1];
}

inline const float2& spot_light::get_cutoff() const
{
	return cutoff[1];
}

inline const float2& spot_light::get_cosine_cutoff() const
{
	return cosine_cutoff[1];
}

inline const tween<float3>& spot_light::get_direction_tween() const
{
	return direction;
}

inline const tween<float3>& spot_light::get_attenuation_tween() const
{
	return attenuation;
}

inline const tween<float2>& spot_light::get_cutoff_tween() const
{
	return cutoff;
}

inline const tween<float2>& spot_light::get_cosine_cutoff_tween() const
{
	return cosine_cutoff;
}

} // namespace scene

#endif // ANTKEEPER_SCENE_SPOT_LIGHT_HPP

