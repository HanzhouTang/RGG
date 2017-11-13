#define _CRT_SECURE_NO_WARNINGS
#ifndef D3DUTIL_H
#define D3DUTIL_H
#include<iostream>
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr.h>
#include <string>
#include <sstream>
#include <vector>
#include<tuple>
#include<unordered_set>
#include<unordered_map>
#include<chrono>
#include<random>
#include<fstream>
#include<thread>
#define WM_COLORING_FINSHED WM_USER+1
#define MAP_LENGTH  1000
//=============================================================
//color table
const D3DCOLOR WHITE = D3DCOLOR_XRGB(255, 255, 255); 
const D3DCOLOR RED = D3DCOLOR_XRGB(255, 0, 0);     
const D3DCOLOR GREEN = D3DCOLOR_XRGB(0, 255, 0);    
const D3DCOLOR YELLOW = D3DCOLOR_XRGB(255, 255, 0);   
const D3DCOLOR BLUE = D3DCOLOR_XRGB(0, 0, 255);    
const D3DCOLOR ORANGE = D3DCOLOR_XRGB(245, 130, 48);
const D3DCOLOR PURPLE = D3DCOLOR_XRGB(145, 30, 180);
const D3DCOLOR CYAN = D3DCOLOR_XRGB(70, 240, 240);
const D3DCOLOR MANGENTA = D3DCOLOR_XRGB(240, 50, 230);
const D3DCOLOR LIME = D3DCOLOR_XRGB(210, 245, 60);
const D3DCOLOR PINK = D3DCOLOR_XRGB(250, 190, 190);
const D3DCOLOR TEAL = D3DCOLOR_XRGB(0, 128, 128);
const D3DCOLOR LACENDER = D3DCOLOR_XRGB(230, 190, 255);
const D3DCOLOR BROWN = D3DCOLOR_XRGB(170,110,40);
const D3DCOLOR BEIGE = D3DCOLOR_XRGB(255, 250, 200);
const D3DCOLOR MAROON = D3DCOLOR_XRGB(128, 0, 0);
const D3DCOLOR MINT = D3DCOLOR_XRGB(170, 255, 195);
const D3DCOLOR OLIVE = D3DCOLOR_XRGB(128, 128, 0);
const D3DCOLOR CORAL = D3DCOLOR_XRGB(255, 215, 180);
const D3DCOLOR NAVY = D3DCOLOR_XRGB(0, 0, 128);
const D3DCOLOR GREY = D3DCOLOR_XRGB(128, 128, 128);

//==============================================================
//struct for line 
struct Line {
	int begin;
	int end;
	Line(int a, int b) {
			begin = a;
			end = b;
	}
	Line(const Line& line) {
		begin = line.begin;
		end = line.end;
	}
};

//===============================================================
// Globals for convenient access.
class D3DApp;
extern D3DApp* gd3dApp;
extern IDirect3DDevice9* gd3dDevice;
//===============================================================
// Clean up

#define ReleaseCOM(x) { if(x){ x->Release();x = 0; } }

//===============================================================

// Debug

#if defined(DEBUG) | defined(_DEBUG)
	#ifndef HR
	#define HR(x)                                      \
	{                                                  \
		HRESULT hr = x;                                \
		if(FAILED(hr))                                 \
		{                                              \
			DXTrace(__FILE__, __LINE__, hr, #x, TRUE); \
		}                                              \
	}
	#endif

#else
	#ifndef HR
	#define HR(x) x;
	#endif
#endif 

#endif // D3DUTIL_H