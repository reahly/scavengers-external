#include <array>
#include <thread>
#include <xorstr.hh>
#include <lazy_importer.hh>
#include "classes/structs.hh"
#include "config/config.hh"
#include "gui/gui.hh"
#include "gui/library/imgui_impl_win32.h"
#include "memory/driver.hh"
#include "overlay/overlay.hh"
#include "utils/utils.hh"
#include <fstream>
#include "features/aimbot.hh"
#include "features/esp.hh"
#include "features/misc.hh"

void gui_thread( ) {
	gui::init( );

	auto msg = MSG( );
	LI_FN( memset )( &msg, 0, sizeof msg );
	auto menu_movement_x = int( ), menu_movement_y = int( );

	while ( msg.message != WM_QUIT ) {
		if ( LI_FN( PeekMessageA ).cached( )( &msg, nullptr, 0U, 0U, PM_REMOVE ) ) {
			LI_FN( TranslateMessage ).cached( )( &msg );
			LI_FN( DispatchMessageA ).cached( )( &msg );

			continue;
		}

		ImGui_ImplDX9_NewFrame( );
		ImGui_ImplWin32_NewFrame( );
		ImGui::NewFrame( );

		static auto open = true;
		if ( !open ) {
			LI_FN( CloseHandle ).cached( )( utils::globals::driver.handle );
			LI_FN( exit )( -1 );
		}

		ImGui::SetNextWindowPos( ImVec2( xf( 0 ), xf( 0 ) ), ImGuiCond_Once );
		ImGui::SetNextWindowSize( ImVec2( xf( 400 ), xf( 280 ) ) );
		ImGui::Begin( _( "Vitasnella [scavengers]" ), &open, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove );
		{
			if ( ImGui::IsMouseClicked( 0 ) ) {
				POINT cursor_position;
				RECT menu_position;

				LI_FN( GetCursorPos ).cached( )( &cursor_position );
				LI_FN( GetWindowRect ).cached( )( gui::window, &menu_position );

				menu_movement_x = cursor_position.x - menu_position.left;
				menu_movement_y = cursor_position.y - menu_position.top;
			}

			if ( menu_movement_y >= 0 && menu_movement_y <= 30 && ImGui::IsMouseDragging( ImGuiMouseButton_Left ) ) {
				auto cursor_position = POINT( );

				LI_FN( GetCursorPos ).cached( )( &cursor_position );
				LI_FN( SetWindowPos ).cached( )( gui::window, nullptr, cursor_position.x - menu_movement_x, cursor_position.y - menu_movement_y, 0, 0, SWP_NOSIZE );
			}

			if ( ImGui::BeginTabBar( _( "primary_interface_tabbar" ), ImGuiTabBarFlags_NoTooltip ) ) {
				if ( ImGui::BeginTabItem( _( "COMBAT" ) ) ) {
					ImGui::Checkbox( _( "Aimbot enabled###combat.aimbot_enabled" ), &utils::settings::aimbot_enable );
					{
						ImGui::SameLine( );
						ImGui::hotkey( "###aimbind", &utils::settings::aimbot_hotkey );
					}

					ImGui::SliderInt( _( "Fov" ), &utils::settings::aimbot_fov, 4, 180 );
					ImGui::SliderInt( _( "Smooth" ), &utils::settings::aimbot_smoothing, 0, 30 );

					ImGui::EndTabItem( );
				}

				if ( ImGui::BeginTabItem( _( "VISUALS" ) ) ) {
					if ( ImGui::BeginTabBar( _( "visuals_bar" ), ImGuiTabBarFlags_NoTooltip ) ) {
						if ( ImGui::BeginTabItem( _( "PLAYERS" ) ) ) {
							ImGui::Checkbox( _( "Draw Teammates" ), &utils::settings::visuals_teammates );
							ImGui::Checkbox( _( "Box Esp" ), &utils::settings::visuals_box );
							if ( utils::settings::visuals_box ) {
								ImGui::SameLine( );
								ImGui::ColorEdit4( _( "Box Color" ), reinterpret_cast<float*>( &utils::settings::visuals_box_color ), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_DisplayHSV | ImGuiColorEditFlags_AlphaBar );
							}

							ImGui::Checkbox( _( "Name Esp" ), &utils::settings::visuals_name );
							if ( utils::settings::visuals_name ) {
								ImGui::SameLine( );
								ImGui::ColorEdit4( _( "Name Color" ), reinterpret_cast<float*>( &utils::settings::visuals_name_color ), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_DisplayHSV | ImGuiColorEditFlags_AlphaBar );
							}

							ImGui::Checkbox( _( "Shield Esp" ), &utils::settings::visuals_shieldbar );
							if ( utils::settings::visuals_shieldbar ) {
								ImGui::SameLine( );
								ImGui::ColorEdit4( _( "Shield Color" ),
									reinterpret_cast<float*>( &utils::settings::visuals_shieldbar_color ),
									ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel |
									ImGuiColorEditFlags_DisplayHSV | ImGuiColorEditFlags_AlphaBar );
							}
							
							ImGui::Checkbox( _( "Distance Esp" ), &utils::settings::visuals_distance );
							ImGui::Checkbox( _( "Health Esp" ), &utils::settings::visuals_healthbar );
							ImGui::SliderInt( _( "Max distance" ), &utils::settings::visuals_maxdistance, 50, 5000 );
							
							ImGui::EndTabItem( );
						}

						if ( ImGui::BeginTabItem( _( "OTHERS" ) ) ) {
							ImGui::Checkbox( _( "Crosshair" ), &utils::settings::visuals_crosshair );

							ImGui::EndTabItem( );
						}

						ImGui::EndTabBar( );
					}

					ImGui::EndTabItem( );
				}

				if ( ImGui::BeginTabItem( _( "MISC" ) ) ) {
					ImGui::Checkbox( _( "No Recoil" ), &utils::settings::misc_norecoil );
					ImGui::Checkbox( _( "No Spread" ), &utils::settings::misc_nospread );
					ImGui::Checkbox( _( "Speed hack" ), &utils::settings::misc_speedhack );
					{
						ImGui::SameLine( );
						ImGui::hotkey( _( "###speed_key" ), &utils::settings::misc_speedhack_key );
						if ( utils::settings::misc_speedhack )
							ImGui::SliderFloat( _( "Value" ), &utils::settings::misc_speedhack_value, 2.f, 100.f );
					}
					
					ImGui::EndTabItem( );
				}

				if ( ImGui::BeginTabItem( _( "SETTINGS" ) ) ) {
					if ( ImGui::Button( _( "Save Config" ) ) )
						config.save( );

					if ( ImGui::Button( _( "Load Config" ) ) ) {
						const std::string m_directory = _( "C:\\configs" );

						if ( auto input_file = std::ifstream( m_directory + "/" + _( "scavenger.ini" ) ); input_file.good( ) )
							config.load( std::string( std::istreambuf_iterator( input_file ), std::istreambuf_iterator<char>( ) ) );
					}

					if ( ImGui::Button( _( "Import Config" ) ) ) {
						if ( const auto* content = ImGui::GetClipboardText( ); content )
							config.load( utils::base64_decode( std::string( content ) ) );
					}

					if ( ImGui::Button( _( "Export Config" ) ) ) {
						const std::string m_directory = _( "C:\\configs" );

						if ( auto input_file = std::ifstream( m_directory + "/" + _( "scavenger.ini" ) ); input_file.good( ) )
							ImGui::SetClipboardText( utils::base64_encode( std::string( std::istreambuf_iterator( input_file ), std::istreambuf_iterator<char>( ) ) ).c_str( ) );
					}
					
					ImGui::EndTabItem( );
				}

				ImGui::EndTabBar( );
			}
		}

		auto* background_draw_list = ImGui::GetBackgroundDrawList( );
		background_draw_list->AddRectFilled( ImVec2( ImGui::GetWindowPos( ).x, ImGui::GetWindowPos( ).y ), ImVec2( ImGui::GetWindowPos( ).x + ImGui::GetWindowSize( ).x, ImGui::GetWindowPos( ).y + ImGui::GetWindowSize( ).y ), ImColor( 28, 38, 43, 255 ) );
		ImGui::End( );
		ImGui::EndFrame( );
		gui::d3ddevice->SetRenderState( D3DRS_ZENABLE, FALSE );
		gui::d3ddevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		gui::d3ddevice->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );

		if ( gui::d3ddevice->BeginScene( ) >= xf( 0 ) ) {
			ImGui::Render( );
			ImGui_ImplDX9_RenderDrawData( ImGui::GetDrawData( ) );
			gui::d3ddevice->EndScene( );
		}

		if ( const auto result = gui::d3ddevice->Present( nullptr, nullptr, nullptr, nullptr ); result == D3DERR_DEVICELOST && gui::d3ddevice->TestCooperativeLevel( ) == D3DERR_DEVICENOTRESET ) {
			ImGui_ImplDX9_InvalidateDeviceObjects( );
			gui::d3ddevice->Reset( &gui::d3dapp );
			ImGui_ImplDX9_CreateDeviceObjects( );
		}

		std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
	}

	ImGui_ImplDX9_Shutdown( );
	ImGui_ImplWin32_Shutdown( );
	ImGui::DestroyContext( );
	gui::end( );
}

void init_cheat( ) {
	config.init( );
	
	utils::globals::driver = driver_manager( );

	auto pid = DWORD( );
	GET_PROCESS_PID( pid, _( "Scavenger-Win64-Shipping.exe" ) )
	utils::globals::driver.set_pid( pid );
	
	GET_MODULE( utils::globals::main_module, _( "Scavenger-Win64-Shipping.exe" ) )

	std::thread( &gui_thread ).detach( );
	std::thread( &overlay::start ).detach( );
	std::thread( &aimbot::init ).detach( );
	std::thread( &esp::init_players ).detach( );
	std::thread( &misc::init ).detach( );
}

bool __stdcall DllMain( const HMODULE, const DWORD reason, LPVOID ) {
	if ( reason != DLL_PROCESS_ATTACH )
		return false;

#ifdef _DBG
	LI_FN( AllocConsole ).cached( )( );
	FILE* fp;
	freopen_s( &fp, _( "CONOUT$" ), _( "w" ), stdout );
#endif

	for ( std::array<std::string, 6> modules = { _( "user32.dll" ), _( "advapi32.dll" ), _( "wininet.dll" ), _( "d3d9.dll" ), _( "shell32.dll" ), _( "Wevtapi.dll" ) }; const auto& module : modules ) {
		if ( !LI_FN( LoadLibraryA ).cached( )( module.c_str( ) ) ) {
			printf( _( "failed loading %s \n" ), module.c_str( ) );
			LI_FN( system ).cached( )( _( "pause" ) );
			return false;
		}
	}

	std::thread( &init_cheat ).detach( );

	return true;
}
