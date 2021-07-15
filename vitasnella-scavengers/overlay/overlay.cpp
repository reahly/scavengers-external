#include "overlay.hh"
#include <dwmapi.h>
#include <thread>
#include "render.hh"
#include "../utils/utils.hh"
#include "../gui/gui.hh"
#include "../classes/structs.hh"

bool overlay::init_d3d( ) {

	return true;
}

bool overlay::init_overlay( ) {
	
	return true;
}

bool overlay::start( ) {
	if ( !init_overlay( ) || !init_d3d( ) )
		return false;

	LI_FN( SetThreadPriority ).cached( )( LI_FN( GetCurrentThread ).cached( )( ), THREAD_PRIORITY_HIGHEST );
	while ( true ) {
		
		if ( LI_FN( GetForegroundWindow ).cached( )( ) == hwnd ) {
			if ( utils::settings::visuals_crosshair ) {
				drawing::draw_crosshair( screen_center.x, screen_center.y, 1.f, D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
			}

			const auto local_player = entity_t::get_local_player( );
			if ( !local_player )
				continue;

			auto camera = local_player->player_controller( )->player_camera_manager( )->camera( );
			for ( auto i = 0; i < utils::globals::entity.size( ); i++ ) { // NOLINT(modernize-loop-convert)
				const auto entity = utils::globals::entity[i];
				if ( !entity.health || entity.max_health <= 1 || entity.teammate && !utils::settings::visuals_teammates )
					continue;

				if ( entity.distance > utils::settings::visuals_maxdistance )
					continue;

				auto origin = entity.origin, head = entity.head_position;
				if ( origin.empty( ) || head.empty( ) )
					continue;

				auto screen_origin = utils::world_to_screen( origin, camera.pov.rotation, camera.pov.location, camera.pov.fov );
				auto screen_head = utils::world_to_screen( head, camera.pov.rotation, camera.pov.location, camera.pov.fov );
				if ( !screen_origin.empty( ) && !screen_head.empty( ) ) {
					const auto h = screen_origin.y - screen_head.y;
					const auto w = h / 2;

					const auto x = screen_origin.x - w / 2;
					const auto y = screen_head.y;

					if ( utils::settings::visuals_box )
						drawing::draw_outlined_rectangle( x, y, w, h, utils::float_to_argb( utils::settings::visuals_box_color ), D3DCOLOR_ARGB( 255, 0, 0, 0 ) );

					if ( utils::settings::visuals_healthbar ) {
						const uint8_t green = static_cast<uint8_t>( std::min( 1.0f, entity.health / entity.max_health ) * 255.0f );
						const uint8_t red = 255 - green;

						const auto health_calc = std::clamp( static_cast<int>( entity.health ) * static_cast<int>( h ) / static_cast<int>( entity.max_health ), 0, static_cast<int>( h ) );
						drawing::draw_rectangle( x - 5, y - 1, 3, h + 2, D3DCOLOR_ARGB( 255, 10, 10, 10 ) );
						drawing::draw_rectangle( x - 4, y + h - health_calc, 1, health_calc, D3DCOLOR_ARGB( 255, red, green, 0 ) );
					}

					if ( utils::settings::visuals_name || utils::settings::visuals_distance ) {
						std::wstring name = static_cast<actor_t*>( entity.entity )->player_state( )->name( ).get( );

						std::wstring string_to_draw_name;
						if ( utils::settings::visuals_name && utils::settings::visuals_distance )
							string_to_draw_name = name.append( _( L" [" ) ).append( std::to_wstring( static_cast<int>( entity.distance ) ).append( _( L"m" ) ).append( _( L"]" ) ) );
						else if ( utils::settings::visuals_name && !utils::settings::visuals_distance )
							string_to_draw_name = name;
						else
							string_to_draw_name = std::to_wstring( static_cast<int>( entity.distance ) ).append( _( L"m" ) );

						//draw_string(string_to_draw_name)
					}

					if ( utils::settings::visuals_shieldbar && entity.shield != 0 && entity.max_health != 0 ) {
						const auto width = entity.shield * w / entity.max_shield;

						drawing::draw_outlined_rectangle( x - 1, y + h + 3, w + 2, 4, D3DCOLOR_ARGB( 255, 0, 0, 0 ), D3DCOLOR_ARGB( 0, 0, 0, 0 ) );
						drawing::draw_rectangle( x, y + h + 4, w, 2, D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
						drawing::draw_rectangle( x, y + h + 4, width, 2, utils::float_to_argb( utils::settings::visuals_shieldbar_color ) );
					}
				}
			}
		}

		std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
	}
}
