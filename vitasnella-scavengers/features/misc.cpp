#include "misc.hh"
#include <thread>
#include "../classes/structs.hh"

void nospread( ) {
	if ( !utils::settings::misc_nospread )
		return;
	
	auto local_player = entity_t::get_local_player( );
	if ( !local_player )
		return;

	auto player_controller = local_player->player_controller( );
	if ( !player_controller )
		return;

	auto pawn = player_controller->pawn( );
	if ( !pawn )
		return;

	auto anim_script_instance = pawn->mesh( )->anim_script_instance( );
	if ( !anim_script_instance )
		return;

	anim_script_instance->weapon( )->set_spread( 0.f );
}

void norecoil( ) {
	if ( !utils::settings::misc_norecoil )
		return;
	
	auto local_player = entity_t::get_local_player( );
	if ( !local_player )
		return;

	auto player_controller = local_player->player_controller( );
	if ( !player_controller )
		return;

	auto pawn = player_controller->pawn( );
	if ( !pawn )
		return;

	auto anim_script_instance = pawn->mesh( )->anim_script_instance( );
	if ( !anim_script_instance )
		return;

	anim_script_instance->weapon( )->set_no_recoil( );
}

void speed( ) {
	auto local_player = entity_t::get_local_player( );
	if ( !local_player || !local_player->player_controller( ) || !local_player->player_controller( )->pawn( ) )
		return;

	static auto old_speed = local_player->player_controller( )->pawn( )->speed( );
	if ( !utils::settings::misc_speedhack || utils::settings::misc_speedhack_key != 0 && !LI_FN( GetAsyncKeyState ).cached( )( utils::settings::misc_speedhack_key ) ) {
		local_player->player_controller( )->pawn( )->set_speed( old_speed );

		return;
	}
	
	local_player->player_controller( )->pawn( )->set_speed( utils::settings::misc_speedhack_value );
}

void misc::init( ) {
	LI_FN( SetThreadPriority ).cached( )( LI_FN( GetCurrentThread ).cached( )( ), THREAD_PRIORITY_LOWEST );
	while ( true ) {
		nospread( );
		norecoil( );
		speed( );
		
		std::this_thread::sleep_for( std::chrono::milliseconds( 200 ) );
	}
}
