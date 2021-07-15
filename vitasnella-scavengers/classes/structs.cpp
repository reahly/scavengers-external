#include "structs.hh"

uworld_t* entity_t::get_world( ) {
	return reinterpret_cast<uworld_t*>( utils::globals::driver.read<uintptr_t>( utils::globals::main_module + 0x608E3E0 ) );
}

entity_t* entity_t::get_local_player( ) {
	auto* world_instance = get_world( );
	if ( !world_instance )
		return nullptr;

	const auto local_player = utils::globals::driver.read<uintptr_t>( world_instance->get_local_players( ) );
	if ( !local_player )
		return nullptr;

	return reinterpret_cast<entity_t*>( local_player );
}