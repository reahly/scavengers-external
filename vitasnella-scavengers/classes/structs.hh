#pragma once
#include "../utils/utils.hh"

#define OFFSET(func, type, offset) type func { return utils::globals::driver.read<type>( reinterpret_cast<uintptr_t>( this ) + offset ); }  // NOLINT

template <class T>
struct tarray {
	friend struct fstring;

	tarray( ) {
		data = 0;
		size = max_size = 0;
	}

	[[nodiscard]] size_t num( ) const {
		return size;
	}

	T& operator[]( size_t i ) {
		return data[i];
	}

	const T& operator[]( size_t i ) const {
		return data[i];
	}

	[[nodiscard]] bool valid_index( const size_t i ) const {
		return i < num( );
	}

	T get( const int idx ) {
		const auto list = this->data;
		return utils::globals::driver.read<T>( list + idx * sizeof( T ) );
	}

	T data;
	int32_t size;
	int32_t max_size;
};

struct fstring : tarray<uintptr_t> {
	[[nodiscard]] std::wstring get( ) const {
		auto str = utils::globals::driver.read_unicode( data, 128 );
		if ( str.empty( ) )
			str += _( L"NPC" );
		
		return str;
	}
};

class transform {
public:
	vec4_t rot{ };
	vec3_t translation{ };
	vec3_t scale{ };

	transform( ) = default;
	transform( const vec4_t a, const vec3_t b, const vec3_t c ) {
		rot = a;
		translation = b;
		scale = c;
	}

	[[nodiscard]] matrix_t matrix_scale( ) const {
		matrix_t m;
		m._41 = translation.x;
		m._42 = translation.y;
		m._43 = translation.z;

		const auto x2 = rot.x + rot.x;
		const auto y2 = rot.y + rot.y;
		const auto z2 = rot.z + rot.z;

		const auto xx2 = rot.x * x2;
		const auto yy2 = rot.y * y2;
		const auto zz2 = rot.z * z2;
		m._11 = ( 1.0f - ( yy2 + zz2 ) ) * scale.x;
		m._22 = ( 1.0f - ( xx2 + zz2 ) ) * scale.y;
		m._33 = ( 1.0f - ( xx2 + yy2 ) ) * scale.z;

		const auto yz2 = rot.y * z2;
		const auto wx2 = rot.w * x2;
		m._32 = ( yz2 - wx2 ) * scale.z;
		m._23 = ( yz2 + wx2 ) * scale.y;

		const auto xy2 = rot.x * y2;
		const auto wz2 = rot.w * z2;
		m._21 = ( xy2 - wz2 ) * scale.y;
		m._12 = ( xy2 + wz2 ) * scale.x;

		const auto xz2 = rot.x * z2;
		const auto wy2 = rot.w * y2;
		m._31 = ( xz2 + wy2 ) * scale.z;
		m._13 = ( xz2 - wy2 ) * scale.x;

		m._14 = 0.0f;
		m._24 = 0.0f;
		m._34 = 0.0f;
		m._44 = 1.0f;

		return m;
	}
};

struct ulevel_t {
	OFFSET( actors( ), tarray<uintptr_t>, 0x98 )
};

struct uworld_t {
	OFFSET( game_instance( ), uintptr_t, 0x180 )
	OFFSET( persistent_level( ), ulevel_t*, 0x30 )

	uintptr_t get_local_players( ) {
		return utils::globals::driver.read<uintptr_t>( game_instance( ) + 0x38 );
	}
};

struct minimal_view_info_t {
	vec3_t location;
	vec3_t rotation;
	float fov;
};

struct camera_t {
	float unk;
	char pad_0xc[0xc];
	minimal_view_info_t pov;
};

struct character_t {
	struct attritube_set_character_t {
		void set_damage_multiplier( ) {
			for ( auto i = 0x10a0; i < 0x1520; i += 0x90 ) {
				utils::globals::driver.write( reinterpret_cast<uintptr_t>( this ) + i + 0x8, FLT_MAX );
				utils::globals::driver.write( reinterpret_cast<uintptr_t>( this ) + i + 0xc, FLT_MAX );
			}
		}
	};
	
	OFFSET( attritube_set_character( ), attritube_set_character_t*, 0x1298 )
};

struct player_camera_manager_t {
	OFFSET( camera( ), camera_t, 0x1b10 )
};

struct player_state_t {
	OFFSET( team( ), int, 0x38C )
	OFFSET( name( ), fstring, 0x368 )
};

struct weapon_t {
	void set_spread( const float val ) {
		for ( auto i = 0x598; i < 0x598 + 0x14; i += sizeof( float ) ) {
			utils::globals::driver.write<float>( reinterpret_cast<uintptr_t>( this ) + i, val );
		}
	}

	void set_no_recoil( ) {
		utils::globals::driver.write<float>( reinterpret_cast<uintptr_t>( this ) + 0x950, 1000.f );
		utils::globals::driver.write<float>( reinterpret_cast<uintptr_t>( this ) + 0x950 + 0x04, 0.f );
		utils::globals::driver.write<float>( reinterpret_cast<uintptr_t>( this ) + 0x950 + 0x08, 0.f );
		utils::globals::driver.write<float>( reinterpret_cast<uintptr_t>( this ) + 0x950 + 0x0c, 0.f );
		utils::globals::driver.write<float>( reinterpret_cast<uintptr_t>( this ) + 0x950 + 0x10, 0.f );
	}
};

struct mesh_t {
	struct anim_script_instance_t {
		OFFSET( weapon( ), weapon_t*, 0x5a0 )
	};

	OFFSET( anim_script_instance( ), anim_script_instance_t*, 0x6b0 )
};

struct pawn_t {
	OFFSET( player_state( ), player_state_t*, 0x2a8 )
	OFFSET( mesh( ), mesh_t*, 0x2e8 )
	OFFSET( speed( ), float, 0xA4 )
	
	void set_speed( const float val ) {
		utils::globals::driver.write<float>( reinterpret_cast<uintptr_t>( this ) + 0xA4, val );
	}
};

struct root_component_t {
	OFFSET( relative_location( ), vec3_t, 0x11c )
};

struct controller_t {
	OFFSET( pawn( ), pawn_t*, 0x2b8 )
	OFFSET( control_rotation( ), vec3_t, 0x2f0 )
	OFFSET( player_camera_manager( ), player_camera_manager_t*, 0x328 )
	OFFSET( root_component( ), root_component_t*, 0x2d0 )
	OFFSET( character( ), character_t*, 0x2c8 )

	void set_control_rotation( const vec3_t& value ) {
		utils::globals::driver.write( reinterpret_cast<uintptr_t>( this ) + 0x2f0, value );
	}
};

struct stat_component_t {
	OFFSET( health( ), float, 0x2e4 + 0x4 )
	OFFSET( max_health( ), float, 0x2e4 + 0x08 )
	OFFSET( shield( ), float, 0x2d8 + 0x4 )
	OFFSET( max_shield( ), float, 0x2d8 + 0x08 )
};

struct actor_t {
	OFFSET( root_component( ), root_component_t*, 0x148 )
	OFFSET( stat_component( ), stat_component_t*, 0x1348 )
	OFFSET( base_eye_height( ), float, 0x294 )
	OFFSET( mesh( ), mesh_t*, 0x2e8 )
	OFFSET( player_state( ), player_state_t*, 0x2a8 )
	
	vec3_t head_position( ) {
		const auto actor_position = this->root_component( )->relative_location( );
		auto headpos = actor_position;
		headpos.z += this->base_eye_height( ) + 2;

		return headpos;
	}

	vec3_t get_bone_position( const int id ) {
		const auto get_bone_index = [ & ]( const int index ) -> transform {
			const auto bone_array = utils::globals::driver.read<uintptr_t>( reinterpret_cast<uintptr_t>( this->mesh( ) ) + 0x740 );
			if ( !bone_array )
				return { };

			return utils::globals::driver.read<transform>( bone_array + index * 0x30 );
		};

		const auto bone = get_bone_index( id );
		const auto component_world = utils::globals::driver.read<transform>( reinterpret_cast<uintptr_t>( this->mesh( ) ) + 0x1C0 );

		const auto matrix = matrix_multiplication( bone.matrix_scale( ), component_world.matrix_scale( ) );
		return vec3_t( matrix._41, matrix._42, matrix._43 );
	}
};

struct entity_t {
	OFFSET( player_controller( ), controller_t*, 0x30 )
	
	static uworld_t* get_world( );
	static entity_t* get_local_player( );
};