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

#ifndef ANTKEEPER_MATH_VECTOR_OPERATORS_HPP
#define ANTKEEPER_MATH_VECTOR_OPERATORS_HPP

#include "math/vector-type.hpp"
#include "math/vector-functions.hpp"

/// @copydoc math::add(const math::vector<T, N>&, const math::vector<T, N>&)
template <class T, std::size_t N>
math::vector<T, N> operator+(const math::vector<T, N>& x, const math::vector<T, N>& y);

/// @copydoc math::div(const math::vector<T, N>&, const math::vector<T, N>&)
template <class T, std::size_t N>
math::vector<T, N> operator/(const math::vector<T, N>& x, const math::vector<T, N>& y);

/// @copydoc math::div(const math::vector<T, N>&, T)
template <class T, std::size_t N>
math::vector<T, N> operator/(const math::vector<T, N>& v, T s);

/// @copydoc math::mul(const math::vector<T, N>&, const math::vector<T, N>&)
template <class T, std::size_t N>
math::vector<T, N> operator*(const math::vector<T, N>& x, const math::vector<T, N>& y);

/// @copydoc math::mul(const math::vector<T, N>&, T)
template <class T, std::size_t N>
math::vector<T, N> operator*(const math::vector<T, N>& v, T s);

/// @copydoc math::mul(const math::vector<T, N>&, T)
template <class T, std::size_t N>
math::vector<T, N> operator*(T s, const math::vector<T, N>& v);

/// @copydoc math::negate(const math::vector<T, N>&)
template <class T, std::size_t N>
math::vector<T, N> operator-(const math::vector<T, N>& x);

/// @copydoc math::sub(const math::vector<T, N>&, const math::vector<T, N>&)
template <class T, std::size_t N>
math::vector<T, N> operator-(const math::vector<T, N>& x, const math::vector<T, N>& y);

/**
 * Adds two vectors and stores the result in the first vector.
 *
 * @param x First vector.
 * @param y Second vector.
 * @return Reference to the first vector.
 */
template <class T, std::size_t N>
math::vector<T, N>& operator+=(math::vector<T, N>& x, const math::vector<T, N>& y);

/**
 * Subtracts two vectors and stores the result in the first vector.
 *
 * @param x First vector.
 * @param y Second vector.
 * @return Reference to the first vector.
 */
template <class T, std::size_t N>
math::vector<T, N>& operator-=(math::vector<T, N>& x, const math::vector<T, N>& y);

/**
 * Multiplies two vectors and stores the result in the first vector.
 *
 * @param x First vector.
 * @param y Second vector.
 * @return Reference to the first vector.
 */
template <class T, std::size_t N>
math::vector<T, N>& operator*=(math::vector<T, N>& x, const math::vector<T, N>& y);

/**
 * Multiplies a vector and a scalar and stores the result in the vector.
 *
 * @param v Vector.
 * @param s Scalar.
 * @return Reference to the vector.
 */
template <class T, std::size_t N>
math::vector<T, N>& operator*=(math::vector<T, N>& v, T s);

/**
 * Divides the first vector by the second vector the result in the first vector.
 *
 * @param x First vector.
 * @param y Second vector.
 * @return Reference to the first vector.
 */
template <class T, std::size_t N>
math::vector<T, N>& operator/=(math::vector<T, N>& x, const math::vector<T, N>& y);

/**
 * Divides a vector by a scalar and stores the result in the vector.
 *
 * @param v Vector.
 * @param s Scalar.
 * @return Reference to the vector.
 */
template <class T, std::size_t N>
math::vector<T, N>& operator/=(math::vector<T, N>& v, T s);

template <class T, std::size_t N>
inline math::vector<T, N> operator+(const math::vector<T, N>& x, const math::vector<T, N>& y)
{
	return add(x, y);
}

template <class T, std::size_t N>
inline math::vector<T, N> operator-(const math::vector<T, N>& x)
{
	return negate(x);
}

template <class T, std::size_t N>
inline math::vector<T, N> operator-(const math::vector<T, N>& x, const math::vector<T, N>& y)
{
	return sub(x, y);
}

template <class T, std::size_t N>
inline math::vector<T, N> operator*(const math::vector<T, N>& x, const math::vector<T, N>& y)
{
	return mul(x, y);
}

template <class T, std::size_t N>
inline math::vector<T, N> operator*(const math::vector<T, N>& v, T s)
{
	return mul(v, s);
}

template <class T, std::size_t N>
inline math::vector<T, N> operator*(T s, const math::vector<T, N>& v)
{
	return mul(v, s);
}

template <class T, std::size_t N>
inline math::vector<T, N> operator/(const math::vector<T, N>& x, const math::vector<T, N>& y)
{
	return div(x, y);
}

template <class T, std::size_t N>
inline math::vector<T, N> operator/(const math::vector<T, N>& v, T s)
{
	return div(v, s);
}

template <class T, std::size_t N>
inline math::vector<T, N>& operator+=(math::vector<T, N>& x, const math::vector<T, N>& y)
{
	return (x = x + y);
}

template <class T, std::size_t N>
inline math::vector<T, N>& operator-=(math::vector<T, N>& x, const math::vector<T, N>& y)
{
	return (x = x - y);
}

template <class T, std::size_t N>
inline math::vector<T, N>& operator*=(math::vector<T, N>& x, const math::vector<T, N>& y)
{
	return (x = x * y);
}

template <class T, std::size_t N>
inline math::vector<T, N>& operator*=(math::vector<T, N>& v, T s)
{
	return (v = v * s);
}

template <class T, std::size_t N>
inline math::vector<T, N>& operator/=(math::vector<T, N>& x, const math::vector<T, N>& y)
{
	return (x = x * y);
}

template <class T, std::size_t N>
inline math::vector<T, N>& operator/=(math::vector<T, N>& v, T s)
{
	return (v = v / s);
}

#endif // ANTKEEPER_MATH_VECTOR_OPERATORS_HPP

