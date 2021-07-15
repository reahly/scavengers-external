#include "utils.hh"
#include <Windows.h>
#include <TlHelp32.h>
#include <cmath>
#include <random>
#include "../overlay/overlay.hh"
#include "../gui/library/imgui.h"

DWORD utils::process_id_name( const std::string& process_name ) {
	auto* const process_snapshot = LI_FN( CreateToolhelp32Snapshot ).cached( )( TH32CS_SNAPPROCESS, 0 );
	if ( process_snapshot == INVALID_HANDLE_VALUE )
		return 0;

	auto process_info = PROCESSENTRY32W( );
	process_info.dwSize = sizeof( PROCESSENTRY32W );

	static auto pid = DWORD( );
	while ( LI_FN( Process32Next ).cached( )( process_snapshot, &process_info ) ) {
		if ( process_name.find( reinterpret_cast<LPCSTR>( process_info.szExeFile ) ) != std::string::npos ) {
			pid = process_info.th32ProcessID;
			break;
		}
	}

	return pid;
}

vec3_t utils::calculate_angle( const vec3_t& src, const vec3_t& dst ) {
	const auto delta = src - dst;
	if ( delta.length( ) <= 0.0f )
		return vec3_t( 0.0f, 0.0f, 0.0f );

	if ( delta.z == 0.0f && delta.length( ) == 0.0f )
		return vec3_t( 0.0f, 0.0f, 0.0f );

	if ( delta.y == 0.0f && delta.x == 0.0f )
		return vec3_t( 0.0f, 0.0f, 0.0f );

	vec3_t angles{ };
	angles.x = asinf( delta.z / delta.length( ) ) * 57.295779513082f;
	angles.y = atanf( delta.y / delta.x ) * 57.295779513082f;
	angles.z = 0.0f;

	if ( delta.x >= 0.0f )
		angles.y += 180.0f;

	angles.normalize( );
	return angles;
}

vec3_t utils::world_to_screen( const vec3_t& world_loc, const vec3_t& rotation, const vec3_t& position, const float fov ) {
	const auto to_matrix = [ ]( const vec3_t& rotation, const vec3_t& origin ) {
		const auto pitch = rotation.x * 3.14159265358979323846f / 180.f;
		const auto yaw = rotation.y * 3.14159265358979323846f / 180.f;
		const auto roll = rotation.z * 3.14159265358979323846f / 180.f;

		const auto SP = LI_FN( sinf ).cached( )( pitch );
		const auto CP = LI_FN( cosf ).cached( )( pitch );
		const auto SY = LI_FN( sinf ).cached( )( yaw );
		const auto CY = LI_FN( cosf ).cached( )( yaw );
		const auto SR = LI_FN( sinf ).cached( )( roll );
		const auto CR = LI_FN( cosf ).cached( )( roll );

		D3DMATRIX matrix;
		matrix._11 = CP * CY;
		matrix._12 = CP * SY;
		matrix._13 = SP;
		matrix._14 = 0.f;

		matrix._21 = SR * SP * CY - CR * SY;
		matrix._22 = SR * SP * SY + CR * CY;
		matrix._23 = -SR * CP;
		matrix._24 = 0.f;

		matrix._31 = -( CR * SP * CY + SR * SY );
		matrix._32 = CY * SR - CR * SP * SY;
		matrix._33 = CR * CP;
		matrix._34 = 0.f;

		matrix._41 = origin.x;
		matrix._42 = origin.y;
		matrix._43 = origin.z;
		matrix._44 = 1.f;

		return matrix;
	};

	const auto matrix = to_matrix( rotation, vec3_t( ) );
	const auto axisx = vec3_t( matrix.m[0][0], matrix.m[0][1], matrix.m[0][2] );
	const auto axisy = vec3_t( matrix.m[1][0], matrix.m[1][1], matrix.m[1][2] );
	const auto axisz = vec3_t( matrix.m[2][0], matrix.m[2][1], matrix.m[2][2] );

	const auto delta = world_loc - position;
	auto transformed = vec3_t( delta.dot( axisy ), delta.dot( axisz ), delta.dot( axisx ) );

	if ( transformed.z < 0.001f )
		transformed.z = 0.001f;

	const auto fov_angle = fov;
	const float center = overlay::width / 2;
	const float centery = overlay::height / 2;

	return vec3_t( center + transformed.x * ( center / tan( fov_angle * PI / 360 ) ) / transformed.z, centery - transformed.y * ( center / tan( fov_angle * PI / 360 ) ) / transformed.z, 0 );
}

D3DCOLOR utils::float_to_argb( float col[] ) {
	const ImU32 col32_no_alpha = ImGui::ColorConvertFloat4ToU32( ImVec4( col[0], col[1], col[2], col[3] ) );
	const float a = col32_no_alpha >> 24 & 255;
	const float r = col32_no_alpha >> 16 & 255;
	const float g = col32_no_alpha >> 8 & 255;
	const float b = col32_no_alpha & 255;
	return D3DCOLOR_ARGB( static_cast<int>( a ), static_cast<int>( b ), static_cast<int>( g ), static_cast<int>( r ) );
}

std::string utils::base64_decode( const std::string& encoded_string ) {
	const std::string base64_chars = _( "ABCDEFGHIJKLMNOPQRSTUVWXYZ" "abcdefghijklmnopqrstuvwxyz" "0123456789+/" );

	std::string out;
	std::vector T( 256, -1 );
	for ( auto i = 0u; i < 64; i++ )
		T[base64_chars[i]] = i;

	auto val = 0;
	for ( auto valb = -8; unsigned char c : encoded_string ) {
		if ( T[c] == -1 )
			break;
		val = ( val << 6 ) + T[c];
		valb += 6;
		if ( valb >= 0 ) {
			out.push_back( static_cast<char>( val >> valb & 0xFF ) );
			valb -= 8;
		}
	}

	return out;
}

std::string utils::base64_encode( const std::string& decoded_string ) {
	std::string out;

	auto val = 0, valb = -6;
	for ( auto c : decoded_string ) {
		val = ( val << 8 ) + c;
		valb += 8;
		while ( valb >= 0 ) {
			out.push_back( _( "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/" )[val >> valb & 0x3F] );
			valb -= 6;
		}
	}
	if ( valb > -6 )
		out.push_back( _( "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/" )[val << 8 >> valb + 8 & 0x3F] );
	while ( out.size( ) % 4 )
		out.push_back( '=' );

	return out;
}

float utils::calculate_fov( const vec3_t& source, const vec3_t aim_angle ) {
	auto ang = aim_angle - source;
	while ( ang.y <= -180.0f ) ang.y += 360.0f;
	while ( ang.y > 180.0f ) ang.y -= 360.0f;

	return sqrtf( ang.x * ang.x + ang.y * ang.y );
}

int utils::random_int( const int min, const int max ) {
	std::random_device rd;
	std::mt19937 rng( rd( ) );
	const std::uniform_int_distribution<int> uni( min, max );

	return uni( rng );
}

bool utils::set_maximum_privileges( const char* privilege_name ) {
	HANDLE token_handle = nullptr;

	if ( !LI_FN( OpenProcessToken ).in( LI_MODULE( "advapi32.dll" ).cached( ) )( LI_FN( GetCurrentProcess )( ), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token_handle ) )
		return false;

	LUID luid{ };
	if ( !LI_FN( LookupPrivilegeValueA ).cached( )( nullptr, privilege_name, &luid ) )
		return false;

	TOKEN_PRIVILEGES token_state{ };
	token_state.PrivilegeCount = 1;
	token_state.Privileges[0].Luid = luid;
	token_state.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if ( !LI_FN( AdjustTokenPrivileges ).cached( )( token_handle, FALSE, &token_state, sizeof( TOKEN_PRIVILEGES ), nullptr, nullptr ) )
		return false;

	LI_FN( CloseHandle ).cached( )( token_handle );

	return true;
}
