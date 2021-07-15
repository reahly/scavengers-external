#include "config.hh"
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <xorstr.hh>
#include "../utils/utils.hh"

nlohmann::json json;
c_config config;

void assign_item( item& item ) {
	if ( json[item.name.c_str( )].empty( ) )
		return;

	if ( item.type == _( "float" ) )
		*static_cast<float*>( item.value ) = json[item.name.c_str( )].get<float>( );
	if ( item.type == _( "int" ) )
		*static_cast<int*>( item.value ) = json[item.name.c_str( )].get<int>( );
	if ( item.type == _( "bool" ) )
		*static_cast<bool*>( item.value ) = json[item.name.c_str( )].get<bool>( );
	if ( item.type == _( "float [4]" ) )
		for ( auto i = 0; i < 4; i++ )
			static_cast<float*>( item.value )[i] = json[item.name.c_str( )][std::to_string( i ).c_str( )].get<float>( );
	if ( item.type == _( "int [80]" ) )
		for ( auto i = 0; i < 80; i++ )
			static_cast<int*>( item.value )[i] = json[item.name.c_str( )][std::to_string( i ).c_str( )].get<int>( );
	if ( item.type == _( "char [100]" ) )
		for ( auto i = 0; i < 100; i++ )
			static_cast<char*>( item.value )[i] = json[item.name.c_str( )][std::to_string( i ).c_str( )].get<char>( );
	if ( item.type == _( "bool [2]" ) )
		for ( auto i = 0; i < 2; i++ )
			static_cast<bool*>( item.value )[i] = json[item.name.c_str( )][std::to_string( i ).c_str( )].get<bool>( );

}

void save_item( item& item ) {
	if ( item.type == _( "float" ) )
		json[item.name.c_str( )] = *static_cast<float*>( item.value );
	if ( item.type == _( "int" ) )
		json[item.name.c_str( )] = *static_cast<int*>( item.value );
	if ( item.type == _( "bool" ) )
		json[item.name.c_str( )] = *static_cast<bool*>( item.value );
	if ( item.type == _( "float [4]" ) )
		for ( auto i = 0; i < 4; i++ )
			json[item.name.c_str( )][std::to_string( i ).c_str( )] = static_cast<float*>( item.value )[i];
	if ( item.type == _( "int [80]" ) )
		for ( auto i = 0; i < 80; i++ )
			json[item.name.c_str( )][std::to_string( i ).c_str( )] = static_cast<int*>( item.value )[i];
	if ( item.type == _( "char [100]" ) ) {
		for ( auto i = 0; i < 100; i++ )
			json[item.name.c_str( )][std::to_string( i ).c_str( )] = static_cast<char*>( item.value )[i];
	}
	if ( item.type == _( "bool [2]" ) )
		for ( auto i = 0; i < 2; i++ )
			json[item.name.c_str( )][std::to_string( i ).c_str( )] = static_cast<bool*>( item.value )[i];
}

bool c_config::init( ) {
	if ( !std::filesystem::exists( _( "C:\\configs" ) ) ) 
		if ( !std::filesystem::create_directory( _( "C:\\configs" ) ) ) 
			return false;

	item( utils::settings::aimbot_enable, _( "aimbot_enable" ), m_items );
	item( utils::settings::aimbot_fov, _( "aimbot_fov" ), m_items );
	item( utils::settings::aimbot_hotkey, _( "aimbot_hotkey" ), m_items );
	item( utils::settings::aimbot_smoothing, _( "aimbot_smoothing" ), m_items );
	
	item( utils::settings::visuals_teammates, _( "visuals_teammates" ), m_items );
	item( utils::settings::visuals_box, _( "visuals_box" ), m_items );
	item( utils::settings::visuals_healthbar, _( "visuals_healthbar" ), m_items );
	item( utils::settings::visuals_distance, _( "visuals_distance" ), m_items );
	item( utils::settings::visuals_name, _( "visuals_name" ), m_items );
	item( utils::settings::visuals_maxdistance, _( "visuals_maxdistance" ), m_items );
	item( utils::settings::visuals_box_color, _( "visuals_box_color" ), m_items );
	item( utils::settings::visuals_name_color, _( "visuals_name_color" ), m_items );

	item( utils::settings::visuals_crosshair, _( "visuals_crosshair" ), m_items );

	item( utils::settings::misc_norecoil, _( "misc_norecoil" ), m_items );
	item( utils::settings::misc_nospread, _( "misc_nospread" ), m_items );

	return true;
}

bool c_config::save( ) {
	const std::string m_directory = _( "C:\\configs" );

	auto output_file = std::ofstream( m_directory + "/" + _( "scavenger.ini" ) );

	if ( !output_file.good( ) )
		return false;

	for ( auto& item : m_items )
		save_item( item );

	output_file << std::setw( 4 ) << json << std::endl;
	output_file.close( );

	return true;
}

bool c_config::load( const std::string& content ) {
	if ( !nlohmann::json::accept( content ) )
		return false;

	json = nlohmann::json::parse( content );
	for ( auto& item : m_items )
		assign_item( item );

	return true;
}