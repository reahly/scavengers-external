#include "aimbot.hh"
#include <lazy_importer.hh>
#include <thread>

#include "../classes/structs.hh"

actor_t* closest_enemy( controller_t* player_controller, const camera_t camera, pawn_t* local_pawn ) {
	const auto get_fov = []( const vec3_t& source, vec3_t aim_angle ) {
		aim_angle -= source;

		if ( aim_angle.x > 180.f )
			aim_angle.x -= 360.f;
		else if ( aim_angle.x < -180.f )
			aim_angle.x += 360.f;

		if ( aim_angle.y > 180.f )
			aim_angle.y -= 360.f;
		else if ( aim_angle.y < -180.f )
			aim_angle.y += 360.f;

		aim_angle.make_absolute( );
		return aim_angle.x + aim_angle.y;
	};

	if ( !local_pawn || !local_pawn->player_state( ) )
		return nullptr;

	actor_t* best_actor = nullptr;
	auto fov = utils::settings::aimbot_fov;

	auto world = entity_t::get_world( );
	if ( !world )
		return nullptr;

	for ( auto i = 0; i < world->persistent_level( )->actors( ).size; i++ ) {
		auto actor = reinterpret_cast<actor_t*>( world->persistent_level( )->actors( ).get( i ) );
		if ( !actor || reinterpret_cast<uintptr_t>( actor ) == reinterpret_cast<uintptr_t>( local_pawn ) || local_pawn->player_state( )->team( ) == actor->player_state( )->team( ) )
			continue;

		const auto health = static_cast<int>( actor->stat_component( )->health( ) );
		if ( health <= 0 || health > 200 )
			continue;

		const auto relative_pos = actor->head_position( ) - camera.pov.location;
		const auto yaw = atan2( relative_pos.y, relative_pos.x ) * 180.0f / PI;
		const auto pitch = -( acos( relative_pos.z / camera.pov.location.distance_3d( actor->head_position( ) ) ) * 180.0f / PI - 90.0f );

		const auto calculated_fov = get_fov( player_controller->control_rotation( ), vec3_t( pitch, yaw, 0.f ) );
		if ( calculated_fov > fov )
			continue;

		fov = calculated_fov;
		best_actor = actor;
	}

	return best_actor;
}

void aimbot::init( ) {
	while ( true ) {
		std::this_thread::sleep_for( std::chrono::milliseconds( 5 ) );

		if ( !utils::settings::aimbot_enable )
			continue;

		if ( !LI_FN( GetAsyncKeyState )( utils::settings::aimbot_hotkey ) )
			continue;

		auto* local_player = entity_t::get_local_player( );
		if ( !local_player )
			continue;

		auto player_controller = local_player->player_controller( );
		if ( !player_controller || !player_controller->pawn( ) )
			continue;

		auto camera_manager = player_controller->player_camera_manager( );
		if ( !camera_manager )
			continue;

		auto* actor = closest_enemy( player_controller, camera_manager->camera( ), player_controller->pawn( ) );
		if ( !actor )
			continue;

		const auto distance = local_player->player_controller( )->player_camera_manager( )->camera( ).pov.location.dist_to( actor->root_component( )->relative_location( ) ) / 100.f;
		if ( distance > utils::settings::visuals_maxdistance )
			continue;
		
		const auto relative_pos = actor->head_position( ) - camera_manager->camera( ).pov.location;
		const auto pitch = -( acos( relative_pos.z / camera_manager->camera( ).pov.location.distance_3d( actor->head_position( ) ) ) * 180.0f / PI - 90.0f );
		const auto yaw = atan2( relative_pos.y, relative_pos.x ) * 180.0f / PI;

		const auto aimangles = utils::settings::aimbot_smoothing > 0 ? vec3_t( ( pitch - camera_manager->camera( ).pov.rotation.x ) / utils::settings::aimbot_smoothing + camera_manager->camera( ).pov.rotation.x, ( yaw - camera_manager->camera( ).pov.rotation.y ) / utils::settings::aimbot_smoothing + camera_manager->camera( ).pov.rotation.y, 0.f ) : vec3_t( pitch, yaw, 0.f );
		player_controller->set_control_rotation( aimangles );
	}
}