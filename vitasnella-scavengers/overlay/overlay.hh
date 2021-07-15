#pragma once
#include <d3d9.h>
#include "../classes/vector.hh"

namespace overlay {
	inline IDirect3D9* d3d = nullptr;
	inline IDirect3DDevice9* device = nullptr;
	inline HWND hwnd, overlay_hwnd = nullptr;
	inline vec3_t screen_center;
	inline int width, height;
	
	bool init_d3d( );
	bool init_overlay( );
	bool start( );
};
