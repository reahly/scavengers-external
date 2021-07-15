#pragma once
#include <Windows.h>
#include <d3d9.h>
#include "library/imgui_impl_dx9.h"
#include "library/imgui_internal.h"

#define F2B( f ) ((1.0f/255) * ( f ) )

namespace gui {
	inline HWND window;
	inline WNDCLASSEXA wc;
	inline LPDIRECT3DDEVICE9 d3ddevice;
	inline LPDIRECT3D9 d3d;
	inline D3DPRESENT_PARAMETERS d3dapp;

	void init( );
	void end( );
}