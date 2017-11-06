#define _CRT_SECURE_NO_WARNINGS
#ifndef D3DUTIL_H
#define D3DUTIL_H
#define MAP_LENGTH  1000
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
//==============================================================
//struct for line 
struct Line {
	D3DXVECTOR3 begin;
	D3DXVECTOR3 end;
	Line(D3DXVECTOR3 a, D3DXVECTOR3 b) {
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
// Geometry generation.

void GenVertexSquare(int numVertices,
	std::vector<D3DXVECTOR3>& verts);

void GenVertexDisk(int numVertices,
	std::vector<D3DXVECTOR3>& verts);

void GenLinkingLines(std::vector<D3DXVECTOR3>& verts,
	int averageDegree, std::vector<Line>&lines);

void GenSmallestLastOrder(const std::vector<std::unordered_set<int>>& matrix,std::list<int>& order,\
	int mindegree,int maxdegree);

float Distance(D3DXVECTOR3 a, D3DXVECTOR3 b);

std::tuple<int,float> GenVertexAndCellSquare(int numVertices,\
	std::vector<D3DXVECTOR3>&verts, float average, std::unordered_map<int, std::vector<int>>& gmap);

std::tuple<int,float> GenVertexAndCellDisk(int numVertices,\
	std::vector<D3DXVECTOR3>&verts, float average, std::unordered_map<int, std::vector<int>>& gmap);
void GenLinkingByCell(int r,float sclarR, std::vector<Line>&lines,\
	std::vector<std::unordered_set<int>>& matrix, const std::unordered_map<int, std::vector<int>>& gmap,\
	const std::vector<D3DXVECTOR3>&verts);

std::vector<Line> GenLineBySets(const std::vector<int>& a, const std::vector<int>& b,\
	float scalarR, const std::vector<D3DXVECTOR3>& verts, std::vector<std::unordered_set<int>>& matrix);

std::vector<int> GetByR(std::vector<std::vector<int>>& map, int i, int j, int r);

//===============================================================
// MultiThreading
struct ColoringParameter {
	std::list<int>& mOrder;
	const std::vector<std::unordered_set<int>>& mMatrix;
	int mMaxDegree, mMinDegree;
	ColoringParameter(const std::vector<std::unordered_set<int>>& matrix,std::list<int>& list,\
		int mindegree, int maxdegree) :mOrder(list),mMatrix(matrix){
		mMaxDegree = maxdegree;
		mMinDegree = mindegree;
	}
};
DWORD WINAPI Coloring(LPVOID colorPara);

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