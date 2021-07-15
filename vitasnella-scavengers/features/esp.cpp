#include "esp.hh"
#include <thread>
#include "../classes/structs.hh"

void esp::init_players( ) {
	LI_FN( SetThreadPriority ).cached( )( LI_FN( GetCurrentThread ).cached( )( ), THREAD_PRIORITY_HIGHEST );
	while ( true ) {
		std::this_thread::sleep_for( std::chrono::milliseconds( 5 ) );

		utils::globals::tmp_entity.clear( );
		auto world = entity_t::get_world( );
		if ( !world )
			continue;

		const auto local_player = entity_t::get_local_player( );
		if ( !local_player || !local_player->player_controller( ) || !local_player->player_controller( )->pawn( ) || !local_player->player_controller( )->player_camera_manager( ) || local_player->player_controller( )->pawn( )->player_state( )->team( ) == -1 )
			continue;

		for ( auto i = 0; i < world->persistent_level( )->actors( ).size; i++ ) {
			auto actor = reinterpret_cast<actor_t*>( world->persistent_level( )->actors( ).get( i ) );
			if ( !actor || !actor->stat_component( ) || reinterpret_cast<uintptr_t>( actor ) == reinterpret_cast<uintptr_t>( local_player->player_controller( )->pawn( ) ) )
				continue;
			
			const auto health = actor->stat_component( )->health( );
			if ( static_cast<int>( health ) <= 0 || static_cast<int>( health ) > 200 )
				continue;

			auto ent = cached_entity_t( );
			ent.entity = actor;
			ent.health = health;
			ent.max_health = actor->stat_component( )->max_health( );
			ent.shield = actor->stat_component( )->shield( );
			ent.max_shield = actor->stat_component( )->max_shield( );
			ent.origin = actor->get_bone_position( 0 );
			ent.head_position = actor->head_position( );
			ent.teammate = local_player->player_controller( )->pawn( )->player_state( )->team( ) == actor->player_state( )->team( );
			ent.distance = local_player->player_controller( )->player_camera_manager( )->camera( ).pov.location.dist_to( actor->root_component( )->relative_location( ) ) / 100.f;
			
			utils::globals::tmp_entity.emplace_back( ent );
		}

		utils::globals::entity = utils::globals::tmp_entity;
	}
}
