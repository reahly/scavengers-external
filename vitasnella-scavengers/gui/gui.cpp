#include "gui.hh"
#include <comdef.h>
#include <d3d9.h>
#include <lazy_importer.hh>
#include <string>
#include <tchar.h>
#include <Windows.h>
#include <xorstr.hh>
#include <fmt/core.h>
#include "library/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND, UINT, WPARAM, LPARAM );

LRESULT __stdcall window_process( const HWND window, const UINT message, const WPARAM wparam, const LPARAM lparam ) {
	if ( ImGui_ImplWin32_WndProcHandler( window, message, wparam, lparam ) )
		return true;

	switch ( message ) {
	case WM_SIZE: {
		if ( gui::d3ddevice != nullptr && wparam != SIZE_MINIMIZED ) {
			gui::d3dapp.BackBufferWidth = LOWORD( lparam );
			gui::d3dapp.BackBufferHeight = HIWORD( lparam );

			ImGui_ImplDX9_InvalidateDeviceObjects( );
			gui::d3ddevice->Reset( &gui::d3dapp );
			ImGui_ImplDX9_CreateDeviceObjects( );
		}
		return 0;
	}

	case WM_SYSCOMMAND: {
		if ( const auto v9 = wparam & 0xFFF0; v9 != SC_KEYMENU && ( v9 != SC_CLOSE || message == VK_F4 ) )
			return LI_FN( DefWindowProcA ).forwarded( )( window, message, wparam, lparam );

		return 0;
	}

	case WM_DESTROY: LI_FN( PostQuitMessage ).cached( )( 0 );
		return 0;

	default:
		break;
	}

	return LI_FN( DefWindowProcA ).forwarded( )( window, message, wparam, lparam );
}

void gui::init( ) {
	wc = { sizeof( WNDCLASSEXA ), CS_CLASSDC, window_process, 0L, 0L, LI_FN( GetModuleHandleA ).cached( )( nullptr ), nullptr, nullptr, nullptr, nullptr, "GUI", nullptr };
	if ( !LI_FN( RegisterClassExA ).cached( )( &wc ) )
		return;

	auto screen_rect = RECT( );
	LI_FN( GetWindowRect ).cached( )( LI_FN( GetDesktopWindow ).cached( )( ), &screen_rect );

	window = LI_FN( CreateWindowExA ).cached( )( 0, wc.lpszClassName, "", WS_POPUP, screen_rect.right / 2 - 150, screen_rect.bottom / 2 - 75, 400, 280, nullptr, nullptr, wc.hInstance, nullptr );
	if ( !window )
		return;

	d3d = LI_FN( Direct3DCreate9 ).cached( )( D3D_SDK_VERSION );
	if ( !d3d )
		return;

	LI_FN( memset ).cached( )( &d3dapp, 0, sizeof d3dapp );
	d3dapp.Windowed = TRUE;
	d3dapp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dapp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dapp.EnableAutoDepthStencil = TRUE;
	d3dapp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dapp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if ( const auto create_device = d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dapp, &d3ddevice ); create_device != D3D_OK )
		return;

	ImGui::CreateContext( );
	ImGui_ImplWin32_Init( window );
	ImGui_ImplDX9_Init( d3ddevice );

	ImGui::GetIO( ).FontDefault = ImGui::GetIO( ).Fonts->AddFontFromFileTTF( _( R"(C:\Windows\Fonts\tahoma.ttf)" ), 14.f );
	LI_FN( ShowWindow ).cached( )( window, SW_SHOWDEFAULT );
	LI_FN( UpdateWindow ).cached( )( window );
}

void gui::end( ) {
	ImGui_ImplDX9_Shutdown( );
	ImGui_ImplWin32_Shutdown( );
	if ( d3ddevice )
		d3ddevice->Release( );

	if ( d3d )
		d3d->Release( );

	LI_FN( DestroyWindow ).cached( )( window );
	LI_FN( UnregisterClassA ).cached( )( wc.lpszClassName, wc.hInstance );
}
