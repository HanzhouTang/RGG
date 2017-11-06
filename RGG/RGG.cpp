#include "d3dApp.h"
#include "DirectInput.h"
#include <crtdbg.h>
#include "GfxStats.h"
#include <list>
#include<string>
#include<sstream>
#include<iostream>
#include<list>
#include "Vertex.h"
#define SQUARE 1
#define DISK 2
int gType=SQUARE;
int gVertexNum=64000;
float gDegree=8.0f;

class RGG : public D3DApp
{
public:
	RGG(HINSTANCE hInstance, std::string winCaption, D3DDEVTYPE devType, DWORD requestedVP);
	~RGG();

	bool checkDeviceCaps();
	void onLostDevice();
	void onResetDevice();
	void updateScene(float dt);
	void drawScene();

	// Helper methods
	void buildGeoBuffers();
	void buildFX();
	void buildViewMtx();
	void buildProjMtx();
private:
	GfxStats* mGfxStats;
	DWORD mNumLines;
	DWORD mNumVertices;
	float mAvergaeDegree;
	ColoringParameter* mColor;
	std::vector<D3DXVECTOR3> mVerts;
	std::vector<Line> mLines;
	std::vector<std::unordered_set<int>> mMatrix;
	std::unordered_map<int, std::vector<int>> mMapOfNodes; // nodes in erver cells
	std::list<int> mSmallestLastOrder;
	IDirect3DVertexBuffer9* mVB;
	IDirect3DVertexBuffer9* mLB;
	//IDirect3DIndexBuffer9*  mIB;
	ID3DXEffect*            mFX;
	D3DXHANDLE              mhTech;
	D3DXHANDLE              mhWVP;
	D3DXHANDLE              mhTime;

	float mTime;

	float mCameraX;
	float mCameraZ;
	float mCameraHeight;

	D3DXMATRIX mView;
	D3DXMATRIX mProj;
};


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{

	auto cmd= GetCommandLine();
	std::istringstream iss(cmd);

	do
	{
		std::string subs;
		iss >> subs;
		if (subs == std::string("-v") || subs == std::string("-V")) {
			iss >> subs;
			gVertexNum = atof(subs.c_str());
		}
		else if (subs == "-d" || subs == "-D") {
			iss >> subs;
			gDegree = atof(subs.c_str());
		}
		else if (subs == "-square") {
			gType = SQUARE;
		}
		else if (subs == "-disk") {
			gType = DISK;
		}
	} while (iss);
	
	RGG app(hInstance, "RGG", D3DDEVTYPE_HAL, D3DCREATE_HARDWARE_VERTEXPROCESSING);
	gd3dApp = &app;

	DirectInput di(DISCL_NONEXCLUSIVE | DISCL_FOREGROUND, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	gDInput = &di;

	return gd3dApp->run();
}

int main() {
	using std::cout;
	using std::endl;
	// Enable run-time memory check for debug builds.

	
	
	auto cmd = GetCommandLine();
	std::istringstream iss(cmd);
	do
	{
		std::string subs;
		iss >> subs;
		if (subs == std::string("-v") || subs == std::string("-V")) {
			iss >> subs;
			gVertexNum = atof(subs.c_str());
		}
		else if (subs == "-d" || subs == "-D") {
			iss >> subs;
			gDegree = atof(subs.c_str());
		}
		else if (subs == "-square") {
			gType = SQUARE;
		}
		else if (subs == "-disk") {
			gType = DISK;
		}
	} while (iss);

	RGG app(GetModuleHandle(NULL), "RGG", D3DDEVTYPE_HAL, D3DCREATE_HARDWARE_VERTEXPROCESSING);
	gd3dApp = &app;
	
	DirectInput di(DISCL_NONEXCLUSIVE | DISCL_FOREGROUND, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	gDInput = &di;
	
	return gd3dApp->run();

	
}

RGG::RGG(HINSTANCE hInstance, std::string winCaption, D3DDEVTYPE devType, DWORD requestedVP)
	: D3DApp(hInstance, winCaption, devType, requestedVP)
{
	if (!checkDeviceCaps())
	{
		MessageBox(0, "checkDeviceCaps() Failed", 0, 0);
		PostQuitMessage(0);
	}

	mGfxStats = new GfxStats();
	mCameraX = 0.0f;
	mCameraZ = 0.0f;
	mCameraHeight = 15.0f;
	mTime = 0.0f;
	mNumVertices = gVertexNum;
	mAvergaeDegree = gDegree;
	
	buildGeoBuffers();

	buildFX();
	
	onResetDevice();
	InitAllVertexDeclarations();
	mColor =new ColoringParameter(mMatrix, mSmallestLastOrder,\
		mGfxStats->GetMinDegree(), mGfxStats->GetMaxDegree());
	CreateThread(0, 0, Coloring, mColor, 0, NULL);
}

RGG::~RGG()
{ 
	delete mColor;
	delete mGfxStats;
	ReleaseCOM(mVB);
	ReleaseCOM(mLB);
	ReleaseCOM(mFX);
	DestroyAllVertexDeclarations();
}

bool RGG::checkDeviceCaps()
{
	D3DCAPS9 caps;
	HR(gd3dDevice->GetDeviceCaps(&caps));

	// Check for vertex shader version 2.0 support.
	if (caps.VertexShaderVersion < D3DVS_VERSION(2, 0))
		return false;

	// Check for pixel shader version 2.0 support.
	if (caps.PixelShaderVersion < D3DPS_VERSION(2, 0))
		return false;

	return true;
}

void RGG::onLostDevice()
{
	mGfxStats->onLostDevice();
	HR(mFX->OnLostDevice());
}

void RGG::onResetDevice()
{
	mGfxStats->onResetDevice();
	HR(mFX->OnResetDevice());


	// The aspect ratio depends on the backbuffer dimensions, which can 
	// possibly change after a reset.  So rebuild the projection matrix.
	buildProjMtx();
}

void RGG::updateScene(float dt)
{
	mGfxStats->setVertexCount(mNumVertices);
	//	mGfxStats->setTriCount(mNumTriangles);
	mGfxStats->update(dt);

	// Get snapshot of input devices.
	gDInput->poll();

	// Check input.
	if (gDInput->keyDown(DIK_S))
		mCameraHeight += 2.0f * dt;
	if (gDInput->keyDown(DIK_W))
		mCameraHeight -= 2.0f * dt;
	// Divide to make mouse less sensitive. 
	mCameraZ += gDInput->mouseDX() / 100.0f;
	mCameraX += gDInput->mouseDY() / 100.0f;
	mCameraHeight = max(mCameraHeight, 1.05f);
	// Accumulate time for simulation.  
	mTime += dt;

	// The camera position/orientation relative to world space can 
	// change every frame based on input, so we need to rebuild the
	// view matrix every frame with the latest changes.
	buildViewMtx();
}


void RGG::drawScene()
{
	// Clear the backbuffer and depth buffer.
	HR(gd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0));
	D3DXMATRIX matRotateX;
	D3DXMATRIX matRotateZ;
	D3DXMatrixRotationX(&matRotateX, mCameraX);
	D3DXMatrixRotationZ(&matRotateZ, mCameraZ);
	HR(gd3dDevice->BeginScene());
	// Let Direct3D know the vertex buffer, index buffer and vertex 
	// declaration we are using.
	HR(gd3dDevice->SetStreamSource(0, mVB, 0, sizeof(VertexPos)));
	HR(gd3dDevice->SetVertexDeclaration(VertexPos::Decl));
	// Setup the rendering FX
	HR(mFX->SetTechnique(mhTech));
	HR(mFX->SetMatrix(mhWVP, &(matRotateX*matRotateZ*mView*mProj)));
	HR(mFX->SetFloat(mhTime, mTime));
	// Begin passes.
	UINT numPasses = 0;
	HR(mFX->Begin(&numPasses, 0));
	for (UINT i = 0; i < numPasses; ++i)
	{
		HR(mFX->BeginPass(i));
		HR(gd3dDevice->DrawPrimitive(D3DPT_POINTLIST, 0, mNumVertices));
			HR(gd3dDevice->SetStreamSource(0, mLB, 0, sizeof(VertexPos)));
			HR(gd3dDevice->DrawPrimitive(D3DPT_LINELIST, 0, mLines.size()));
		
		HR(mFX->EndPass());
	}
	HR(mFX->End());
	mGfxStats->setCameraHeight(mCameraHeight);
	mGfxStats->display();
	HR(gd3dDevice->EndScene());
	// Present the backbuffer.
	HR(gd3dDevice->Present(0, 0, 0, 0));
}

void RGG::buildGeoBuffers()
{

	using std::cout;
	__int64 cntsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
	float secsPerCnt = 1000.0f / (float)cntsPerSec;
	__int64 prevTimeStamp = 0;
	__int64 currTimeStamp = 0;
	mMatrix.resize(mNumVertices);
	QueryPerformanceCounter((LARGE_INTEGER*)&prevTimeStamp);
	if (gType == SQUARE) {
		auto t = GenVertexAndCellSquare(mNumVertices, mVerts, mAvergaeDegree,mMapOfNodes);
		
		GenLinkingByCell(std::get<0>(t), std::get<1>(t), mLines,mMatrix,mMapOfNodes,mVerts);
		mGfxStats->setR(sqrtf(std::get<1>(t)) / 10);
	}
	else if (gType == DISK) {
		auto t = GenVertexAndCellDisk(mNumVertices, mVerts, mAvergaeDegree,mMapOfNodes);
		GenLinkingByCell(std::get<0>(t), std::get<1>(t), mLines,mMatrix,mMapOfNodes,mVerts);
		mGfxStats->setR(sqrtf(std::get<1>(t)) / 5);
	}

	QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
	float dt = (currTimeStamp- prevTimeStamp)*secsPerCnt;
	mGfxStats->setInitTime(dt);
	int mx = 0;
	int mn = INT_MAX;
	for (auto& x : mMatrix) {
		mx = max(x.size(), mx);
		mn = min(x.size(), mn);
	}
	mGfxStats->setAverageDegree(static_cast<float>(mLines.size() * 2) /static_cast<float>(mNumVertices));
	mGfxStats->setMaxDegreeAndMinDegree(mx, mn);
	HR(gd3dDevice->CreateVertexBuffer(mLines.size()* 2 * sizeof(VertexPos),
		D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &mLB, 0));
	HR(gd3dDevice->CreateVertexBuffer(mNumVertices * sizeof(VertexPos),
		D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &mVB, 0));	// Now lock it to obtain a pointer to its internal data, and write the
	// grid's vertex data.
	VertexPos* v = 0;
	HR(mVB->Lock(0, 0, (void**)&v, 0));
	for (DWORD i = 0; i < mNumVertices; ++i)
		v[i] = mVerts[i];
	HR(mVB->Unlock());

	HR(mLB->Lock(0, 0, (void**)&v, 0));

	for (DWORD i = 0; i < mLines.size(); i++) {
		v[i * 2] = mLines[i].begin;
		v[i * 2 + 1] = mLines[i].end;
	}
	HR(mLB->Unlock());
	
}

void RGG::buildFX()
{
	// Create the FX from a .fx file.
	ID3DXBuffer* errors = 0;
	HR(D3DXCreateEffectFromFile(gd3dDevice, "heightcolor.fx",
		0, 0, D3DXSHADER_DEBUG, 0, &mFX, &errors));
	if (errors)
		MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);

	// Obtain handles.
	mhTech = mFX->GetTechniqueByName("HeightColorTech");
	mhWVP = mFX->GetParameterByName(0, "gWVP");
	mhTime = mFX->GetParameterByName(0, "gTime");
}

void RGG::buildViewMtx()
{
	//float x = mCameraRadius * cosf(mCameraRotationY);
	//float z = mCameraRadius * sinf(mCameraRotationY);
	D3DXVECTOR3 pos(0, mCameraHeight, 0);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 0.0f, 0.1f);
	D3DXMatrixLookAtLH(&mView, &pos, &target, &up);
}

void RGG::buildProjMtx()
{
	float w = (float)md3dPP.BackBufferWidth;
	float h = (float)md3dPP.BackBufferHeight;
	D3DXMatrixPerspectiveFovLH(&mProj, D3DX_PI * 0.25f, w / h, 1.0f, 5000.0f);
}


//mesh 
//ball
