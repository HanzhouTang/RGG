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
#include"CSquare.h"
#include"CDisk.h"
#include"CSphere.h"
//想想办法
#define SQUARE    1
#define DISK      2
#define SPHERE    3

int gType = SPHERE;
int gVertexNum = 4000;
float gDegree = 32;


DWORD FtoDw(float f) {
	return *((DWORD*)&f);
}

class RGG : public D3DApp
{
public:
	RGG(HINSTANCE hInstance, std::string winCaption, D3DDEVTYPE devType, DWORD requestedVP);
	~RGG();

	bool checkDeviceCaps();
	void onLostDevice();
	void onResetDevice();
	void updateScene(float dt);
	void onColoringFinshed();
	void drawScene();

	// Helper methods
	void buildGeoBuffers();
	void buildFX();
	void buildViewMtx();
	void buildProjMtx();
private:
	GfxStats* mGfxStats;
	CShape* mShape;
	IDirect3DVertexBuffer9* mLB;
	IDirect3DVertexBuffer9*   mVB;
	ID3DXEffect*            mFX;
	D3DXHANDLE              mhTech;
	D3DXHANDLE              mhWVP;
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

	auto cmd = GetCommandLine();
	std::istringstream iss(cmd);

	do
	{
		std::string subs;
		iss >> subs;
		if (subs == std::string("-v") || subs == std::string("-V")) {
			iss >> subs;
			gVertexNum = atoi(subs.c_str());
		}
		else if (subs == "-d" || subs == "-D") {
			iss >> subs;
			gDegree = static_cast<float>(atof(subs.c_str()));
		}
		else if (subs == "-square") {
			gType = SQUARE;
		}
		else if (subs == "-disk") {
			gType = DISK;
		}
		else if (subs == "-sphere") {
			gType = SPHERE;
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
	auto cmd = GetCommandLine();
	std::istringstream iss(cmd);
	do
	{
		std::string subs;
		iss >> subs;
		if (subs == std::string("-v") || subs == std::string("-V")) {
			iss >> subs;
			gVertexNum = atoi(subs.c_str());
		}
		else if (subs == "-d" || subs == "-D") {
			iss >> subs;
			gDegree = static_cast<float>(atof(subs.c_str()));
		}
		else if (subs == "-square") {
			gType = SQUARE;
		}
		else if (subs == "-disk") {
			gType = DISK;
		}
		else if (subs == "-sphere") {
			gType = SPHERE;
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
	buildGeoBuffers();
	buildFX();
	onResetDevice();
	InitAllVertexDeclarations();
	mShape->Color();
}

RGG::~RGG()
{
	delete mGfxStats;
	ReleaseCOM(mLB);
	ReleaseCOM(mVB);
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

void RGG::onColoringFinshed()
{  
	
	VertexCol* v = 0;
	const auto& lines = mShape->GetLines();
	const auto& verts = mShape->GetVertices();
	HR(mLB->Lock(0, 0, (void**)&v, 0));
	for (DWORD i = 0; i < lines.size(); i++) {
		v[i * 2] = VertexCol(verts[lines[i].begin], mShape->GetColor(lines[i].begin));
		v[i * 2 + 1] = VertexCol(verts[lines[i].end], mShape->GetColor(lines[i].end));
	}
	HR(mLB->Unlock());
	HR(mVB->Lock(0, 0, (void**)&v, 0));
	for (DWORD i = 0; i < verts.size(); i++) {
		v[i] = VertexCol(verts[i], mShape->GetColor(i));
	}
	HR(mVB->Unlock());
	mGfxStats->SetColoringFinished();
	mGfxStats->SetMaxDeletedDegree(mShape->GetMaxDeletedDegree());
	mGfxStats->SetColorNeeded(mShape->GetColorNumber());
	mGfxStats->SetMaxColorSet(mShape->GetMaxColorSet());
	mGfxStats->SetTerminalClique(mShape->GetTerminalCliqueSize());
//	std::cout << "coloring time " << mShape->GetColoringTime();
	mGfxStats->SetColoringTime(mShape->GetColoringTime());

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

	HR(gd3dDevice->SetVertexDeclaration(VertexCol::Decl));
	gd3dDevice->SetRenderState(D3DRS_POINTSIZE, FtoDw(7.0f));

	// Setup the rendering FX
	HR(mFX->SetTechnique(mhTech));
	HR(mFX->SetMatrix(mhWVP, &(matRotateX*matRotateZ*mView*mProj)));
	// Begin passes.
	UINT numPasses = 0;
	HR(mFX->Begin(&numPasses, 0));
	for (UINT i = 0; i < numPasses; ++i)
	{
		HR(mFX->BeginPass(i));
		HR(gd3dDevice->SetStreamSource(0, mLB, 0, sizeof(VertexCol)));
		HR(gd3dDevice->DrawPrimitive(D3DPT_LINELIST, 0, mShape->GetLinesNumber()));
		HR(gd3dDevice->SetStreamSource(0, mVB, 0, sizeof(VertexCol)));
		HR(gd3dDevice->DrawPrimitive(D3DPT_POINTLIST, 0, mShape->GetVerticesNumber()));
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
	if (gType == SQUARE) {
		mShape = new CSquare(gVertexNum, gDegree, mhMainWnd);
	}
	else if (gType == DISK) {
		mShape = new CDisk(gVertexNum, gDegree, mhMainWnd);
	}
	else if (gType == SPHERE) {
		mShape = new CSphere(gVertexNum, gDegree, mhMainWnd);
	}
	mShape->Init();
	mGfxStats->setVertexCount(gVertexNum);
	mGfxStats->setR(mShape->GetR());
	mGfxStats->setInitTime(mShape->GetInitTime());
	mGfxStats->SetEdgeNumber(mShape->GetLines().size());
	auto temp = mShape->GetMinMaxDegree();
	mGfxStats->setMaxDegreeAndMinDegree(std::get<1>(temp), std::get<0>(temp));
	mGfxStats->setAverageDegree(mShape->GetAverageDegree());
	const auto& vertices = mShape->GetVertices();
	const auto& lines = mShape->GetLines();
	VertexCol* v = 0;
	HR(gd3dDevice->CreateVertexBuffer(vertices.size() * sizeof(VertexCol),
		D3DUSAGE_WRITEONLY | D3DUSAGE_POINTS, 0, D3DPOOL_MANAGED, &mVB, 0));

	HR(mVB->Lock(0, 0, (void**)&v, 0));

	for (DWORD i = 0; i < vertices.size(); i++) {
		v[i] = VertexCol(vertices[i], WHITE);
	}
	HR(mVB->Unlock());

	HR(gd3dDevice->CreateVertexBuffer(lines.size() * 2 * sizeof(VertexCol),
		D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &mLB, 0));
	HR(mLB->Lock(0, 0, (void**)&v, 0));

	for (DWORD i = 0; i < lines.size(); i++) {
		v[i * 2] = VertexCol(vertices[lines[i].begin], WHITE);
		v[i * 2 + 1] = VertexCol(vertices[lines[i].end], WHITE);
	}
	HR(mLB->Unlock());

}

void RGG::buildFX()
{
	// Create the FX from a .fx file.
	ID3DXBuffer* errors = 0;
	HR(D3DXCreateEffectFromFile(gd3dDevice, "color.fx",
		0, 0, D3DXSHADER_DEBUG, 0, &mFX, &errors));
	if (errors)
		MessageBox(0, (char*)errors->GetBufferPointer(), 0, 0);

	// Obtain handles.
	mhTech = mFX->GetTechniqueByName("ColorTech");
	mhWVP = mFX->GetParameterByName(0, "gWVP");
}

void RGG::buildViewMtx()
{
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
