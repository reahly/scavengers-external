#pragma once
#include <cstdint>
#include <lazy_importer.hh>
#include <string>
#include <vector>
#include <Windows.h>

class driver_manager {
public:
	HANDLE handle{ };
	int pid;

	explicit driver_manager( ): pid( 0 ) {
		update_handle( );
	}

	void update_handle( ) {

	}

	void set_pid( const int pid_ ) {
		pid = pid_;
	}

	void raw_read( const uintptr_t src, const LPVOID dst, const size_t size ) const {
		
	}

	template <typename T>
	T read( const uintptr_t src ) {
		return T{};
	}

	[[nodiscard]] std::wstring read_unicode( const std::uintptr_t address, const std::size_t size ) const {
		return std::wstring( L"" );
	}

	[[nodiscard]] std::string read_widechar( const std::uintptr_t address, const std::size_t size ) const {
		return std::string( "" );
	}

	template <typename type>
	type read_chain( const std::initializer_list<uintptr_t> _offsets, const bool read_first_offset ) {
		std::vector offsets = _offsets;
		const auto offsets_size = offsets.size( );
		auto last_pointer = read<uintptr_t>( ( read_first_offset ? read<uintptr_t>( offsets[0] ) : offsets[0] ) + offsets[1] );
		for ( size_t i = 2; i < offsets_size - 1; i++ )
			if ( !( last_pointer = read<uintptr_t>( last_pointer + offsets[i] ) ) )
				return type( );

		return read<type>( last_pointer + offsets[offsets_size - 1] );
	}

	template <typename T>
	void write( const uintptr_t src, T value ) {
		
	}

	[[nodiscard]] void* get_module_base( const std::string& module_name ) const {
		return nullptr;
	}
};
