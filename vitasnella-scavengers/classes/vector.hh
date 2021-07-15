#pragma once
#include <algorithm>
#include <cstdio>
#include <xorstr.hh>
#include <cmath>

using matrix_t = struct {
	union {
		struct {
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;

		};

		float m[4][4];
	};
};

struct vec2_t {
	float x, y;

	void print( ) const {
		printf( _( "%f | %f \n" ), x, y );
	}
};

struct vec3_t {
	float x, y, z;

	void print( ) const {
		printf( _( "%f | %f | %f \n" ), x, y, z );
	}

	vec3_t& operator+=( const vec3_t& v ) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	vec3_t& operator-=( const vec3_t& v ) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	vec3_t operator*( const float v ) const {
		return vec3_t{ x * v, y * v, z * v };
	}

	vec3_t operator-( const vec3_t& v ) const {
		return vec3_t{ x - v.x, y - v.y, z - v.z };
	}

	vec3_t& operator+( const float& v ) {
		x = x + v;
		y = y + v;
		z = z + v;
		return *this;
	}

	vec3_t operator/( const float fl ) const {
		return vec3_t{ x / fl, y / fl, z / fl };
	}

	vec3_t& operator*=( const float v ) {
		x *= v;
		y *= v;
		z *= v;
		return *this;
	}

	vec3_t& operator*=( const vec3_t& vec ) {
		x *= vec.x;
		y *= vec.y;
		z *= vec.z;
		return *this;
	}

	vec3_t& operator/=( const float fl ) {
		x /= fl;
		y /= fl;
		z /= fl;
		return *this;
	}

	vec3_t operator+( const vec3_t& v ) const {
		return vec3_t{ x + v.x, y + v.y, z + v.z };
	}

	void make_abs( ) {
		x = std::abs( x );
		y = std::abs( y );
		z = std::abs( z );
	}

	[[nodiscard]] bool empty( ) const {
		return x == 0.f && y == 0.f && z == 0.f || std::isnan( x ) || std::isnan( y ) || std::isnan( z );
	}

	[[nodiscard]] bool isinf( ) const {
		return std::isinf( x ) && std::isinf( y ) && std::isinf( z );
	}

	[[nodiscard]] float length( ) const {
		return sqrtf( x * x + y * y + z * z );
	}

	[[nodiscard]] float dist_to( const vec3_t target ) const {
		vec3_t delta = *this;
		delta -= target;

		return delta.length( );
	}

	float distance_3d( const vec3_t& v2 ) const {
		const auto v1 = *this;

		vec3_t out;
		out.x = v1.x - v2.x;
		out.y = v1.y - v2.y;
		out.z = v1.z - v2.z;
		return sqrt( out.x * out.x + out.y * out.y + out.z * out.z );
	}
	
	vec3_t normalize( ) {
		if ( this->x > 180.0f )
			this->x -= 360.f;
		else if ( this->x < -180.0f )
			this->x += 360.0f;

		if ( this->y > 180.0f )
			this->y -= 360.0f;
		else if ( this->y < -180.0f )
			this->y += 360.0f;

		this->z = 0.0f;
		return *this;
	}

	[[nodiscard]] float dot( const vec3_t& other ) const {
		return x * other.x + y * other.y + z * other.z;
	}

	[[nodiscard]] vec3_t cross( const vec3_t rhs ) const {
		const auto lhs = *this;
		return vec3_t{
			lhs.y * rhs.z - lhs.z * rhs.y,
			lhs.z * rhs.x - lhs.x * rhs.z,
			lhs.x * rhs.y - lhs.y * rhs.x
		};
	}

	void make_absolute( ) {
		x = std::abs( x );
		y = std::abs( y );
		z = std::abs( z );
	}
	
	void clamp( ) {
		normalize( );

		x = std::clamp( x, -89.0f, 89.0f );
		y = std::clamp( y, -180.0f, 180.0f );
		z = 0.0f;
	}
};

struct vec4_t {
	float x, y, z, w;
};

inline matrix_t matrix_multiplication( const matrix_t pM1, const matrix_t pM2 ) {
	matrix_t out;
	out._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41;
	out._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42;
	out._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43;
	out._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44;
	out._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41;
	out._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42;
	out._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43;
	out._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44;
	out._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41;
	out._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42;
	out._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43;
	out._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44;
	out._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41;
	out._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42;
	out._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43;
	out._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44;

	return out;
}
