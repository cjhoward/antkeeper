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

#ifndef ANTKEEPER_CELESTIAL_MECHANICS_HPP
#define ANTKEEPER_CELESTIAL_MECHANICS_HPP

#include "utility/fundamental-types.hpp"

namespace ast
{

/**
 * Approximates the obliquity of the ecliptic.
 *
 * @param jd Julian date.
 * @return Obliquity of the ecliptic, in radians.
 */
double approx_ecliptic_obliquity(double jd);

/**
 * Approximates the ecliptic coordinates of the Earth's sun.
 *
 * @param jd Julian date.
 * @return Ecliptic rectangular geocentric coordinates of the Earth's sun, with distance in AU.
 */
double3 approx_sun_ecliptic(double jd);

/**
 * Approximates the ecliptic coordinates of the Earth's moon.
 *
 * @param jd Julian date.
 * @return Ecliptic rectangular geocentric coordinates of the Earth's moon, with distance in Earth radii.
 */
double3 approx_moon_ecliptic(double jd);

/**
 * Approximates the ecliptic rotation of the Earth's moon.
 *
 * @param jd Julian date.
 * @return Rotation matrix representing the moon's rotation in ecliptic space.
 */
double3x3 approx_moon_ecliptic_rotation(double jd);

struct orbital_elements
{
	double a; ///< Semi-major axis (km)
	double e; ///< Eccentricity
	double w; ///< Argument of periapsis (radians)
	double m; ///< Mean anomaly (radians)
	double i; ///< Inclination to the ecliptic (radians)
	double n; ///< Longitude of the ascending node (radians)
};

//constexpr orbital_elements j2k_orbit_moon = {384400.0, 0.0554, 318.15, 135.275.16, 125.08};

struct orbital_state
{
	double3 r; ///< Cartesian position
	double3 v; ///< Cartesian velocity
};

} // namespace ast

#endif // ANTKEEPER_CELESTIAL_MECHANICS_HPP