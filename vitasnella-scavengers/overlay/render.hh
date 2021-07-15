#pragma once
#include <d3dx9.h>

namespace drawing {
	void draw_rectangle( int x, int y, int w, int h, DWORD color );
	void draw_outlined_rectangle( int x, int y, int w, int h, DWORD color, DWORD outline );
	void draw_line( int x1, int y1, int x2, int y2, DWORD color );

	inline void draw_crosshair( int x, int y, int len, DWORD color ) {

	}

	inline void draw_outlined_crosshair( int x, int y, int len, DWORD color, DWORD outline ) {

	}
}
