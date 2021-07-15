#pragma once
#include <chrono>
#include "../classes/vector.hh"
#include "../memory/driver.hh"
#include <d3d9types.h>
#include <unordered_map>

#define PI 3.14159265358979323846f
#define RAD2DEG( x  )  ( ( float )( x ) * ( float )( 180.f / PI ) )

#ifndef _DBG
#define GET_MODULE( x, module_name ) \
	while ( !( x ) ) { \
	std::this_thread::sleep_for( std::chrono::seconds( 2 ) ); \
	( x ) = reinterpret_cast<uintptr_t>( utils::globals::driver.get_module_base( module_name ) ); \
	if ( x ) \
	   break; \
    }

#define GET_WINDOW( x, window_class_name, window_name ) \
	while ( !( x ) ) { \
	std::this_thread::sleep_for( std::chrono::seconds( 2 ) ); \
	( x ) = LI_FN( FindWindowA ).cached( )( window_class_name, window_name ); \
	if ( x ) \
	   break; \
    }

#define GET_PROCESS_PID( x, process_name ) \
	while ( !( x ) ) { \
	std::this_thread::sleep_for( std::chrono::seconds( 2 ) ); \
	( x ) = utils::process_id_name( process_name ); \
	if ( x ) \
	   break; \
    }
#else
#define GET_MODULE( x, module_name ) \
	while ( !( x ) ) { \
	( x ) = reinterpret_cast<uintptr_t>( utils::globals::driver.get_module_base( module_name ) ); \
	if ( x ) \
	   break; \
    }

#define GET_WINDOW( x, window_class_name, window_name ) \
	while ( !( x ) ) { \
	( x ) = LI_FN( FindWindowA ).cached( )( window_class_name, window_name ); \
	if ( x ) \
	   break; \
    }

#define GET_PROCESS_PID( x, process_name ) \
	while ( !( x ) ) { \
	( x ) = utils::process_id_name( process_name ); \
	if ( x ) \
	   break; \
    }
#endif

struct cached_entity_t {
	void* entity;
	float health, max_health, shield, max_shield;
	vec3_t origin, head_position;
	bool teammate;
	float distance;
};

namespace utils {
	namespace globals {
		inline driver_manager driver;
		inline uintptr_t main_module;
		inline std::vector<cached_entity_t> entity, tmp_entity;
	}

	namespace settings {
		inline bool aimbot_enable;
		inline int aimbot_fov = 4;
		inline int aimbot_hotkey = 6;
		inline int aimbot_smoothing;
		
		inline bool visuals_shieldbar, visuals_teammates, visuals_box, visuals_healthbar, visuals_distance, visuals_name;
		inline int visuals_maxdistance = 100;
		inline float visuals_box_color[4] = { 255.0f, 255.0f, 255.0f, 255.0f };
		inline float visuals_name_color[4] = { 255.0f, 255.0f, 255.0f, 255.0f };
		inline float visuals_shieldbar_color[4] = { 100.f, 149.f, 237.f, 255.0f };

		inline bool visuals_crosshair;

		inline bool misc_nospread, misc_norecoil, misc_speedhack;
		inline int misc_speedhack_key;
		inline float misc_speedhack_value;
	}

	DWORD process_id_name( const std::string& );
	vec3_t calculate_angle( const vec3_t&, const vec3_t& );
	vec3_t world_to_screen( const vec3_t&, const vec3_t&, const vec3_t&, float );
	D3DCOLOR float_to_argb( float[] );
	std::string base64_decode( const std::string& );
	std::string base64_encode( const std::string& );
	float calculate_fov( const vec3_t&, vec3_t );
	int random_int( int, int );
	bool set_maximum_privileges( const char* );

	template <typename T>
	[[nodiscard]] vec4_t hsv_to_rgb( T h, T s, T v, const float alpha ) {
		int _r, _g, _b;

		if ( s != 0 ) {

			h == 360 ? h = 0 : h = h / 60;
			auto i = static_cast<int>( trunc( h ) );
			int f = h - i;

			const int p = v * ( 1 - s );
			const int q = v * ( 1 - s * f );
			const int t = v * ( 1 - s * ( 1 - f ) );

			switch ( i ) {
			case 0:
				_r = v;
				_g = t;
				_b = p;
				break;

			case 1:
				_r = q;
				_g = v;
				_b = p;
				break;

			case 2:
				_r = p;
				_g = v;
				_b = t;
				break;

			case 3:
				_r = p;
				_g = q;
				_b = v;
				break;

			case 4:
				_r = t;
				_g = p;
				_b = v;
				break;

			default:
				_r = v;
				_g = p;
				_b = q;
				break;
			}
		} else {
			_r = v;
			_g = v;
			_b = v;
		}

		return vec4_t( static_cast<uint8_t>( _r * 255 ), static_cast<uint8_t>( _g * 255 ), static_cast<uint8_t>( _b * 255 ), alpha );
	}
}
