
#include "d3dUtil.h"
#include "d3dApp.h"
#include "GfxStats.h"
#include <tchar.h>

GfxStats::GfxStats()
: mFont(0), mFPS(0.0f), mMilliSecPerFrame(0.0f), mCameraHeight(0), mNumVertices(0),mAverageDegree(0),mR(0),mInitTime(0)
{
	D3DXFONT_DESC fontDesc;
	fontDesc.Height          = 18;
    fontDesc.Width           = 0;
    fontDesc.Weight          = 0;
    fontDesc.MipLevels       = 1;
    fontDesc.Italic          = false;
    fontDesc.CharSet         = DEFAULT_CHARSET;
    fontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
    fontDesc.Quality         = DEFAULT_QUALITY;
    fontDesc.PitchAndFamily  = DEFAULT_PITCH | FF_DONTCARE;
    _tcscpy(fontDesc.FaceName, _T("Times New Roman"));

	HR(D3DXCreateFontIndirect(gd3dDevice, &fontDesc, &mFont));
}

GfxStats::~GfxStats()
{
	ReleaseCOM(mFont);
}

void GfxStats::onLostDevice()
{
	HR(mFont->OnLostDevice());
}

void GfxStats::onResetDevice()
{
	HR(mFont->OnResetDevice());
}

void GfxStats::addVertices(DWORD n)
{
	mNumVertices += n;
}

void GfxStats::subVertices(DWORD n)
{
	mNumVertices -= n;
}

void GfxStats::setCameraHeight(float n)
{
	mCameraHeight = n;
}

void GfxStats::setVertexCount(DWORD n)
{
	mNumVertices = n;
}
void GfxStats::setMaxDegreeAndMinDegree(int max, int min) {
	mMaxDegree = max;
	mMinDegree = min;
}

void GfxStats::update(float dt)
{
	// Make static so that their values persist accross function calls.
	static float numFrames   = 0.0f;
	static float timeElapsed = 0.0f;

	// Increment the frame count.
	numFrames += 1.0f;

	// Accumulate how much time has passed.
	timeElapsed += dt;

	// Has one second passed?--we compute the frame statistics once 
	// per second.  Note that the time between frames can vary so 
	// these stats are averages over a second.
	if( timeElapsed >= 1.0f )
	{
		// Frames Per Second = numFrames / timeElapsed,
		// but timeElapsed approx. equals 1.0, so 
		// frames per second = numFrames.

		mFPS = numFrames;

		// Average time, in miliseconds, it took to render a single frame.
		mMilliSecPerFrame = 1000.0f / mFPS;

		// Reset time counter and frame count to prepare for computing
		// the average stats over the next second.
		timeElapsed = 0.0f;
		numFrames   = 0.0f;
	}
}

void GfxStats::setAverageDegree(float n){
	mAverageDegree = n;
}
void GfxStats::setR(float r) {
	mR = r;
}
void GfxStats::setInitTime(float t) {
	mInitTime = t;
}
void GfxStats::display()
{
	// Make static so memory is not allocated every frame.
	static char buffer[512];

	sprintf(buffer, "Frames Per Second = %.2f\n"
		"Milliseconds Per Frame = %.4f\n"
		"Camera Height = %f\n"
		"Vertex Count = %d\n"
		"Average Degree = %f\n"
		"Distance bound = %f\n"
		"Initing time = %fms\n"
		"Max degree = %d\n"
		"Min degree = %d\n"
		"Press key w or s to zoom", mFPS, mMilliSecPerFrame, mCameraHeight, mNumVertices,mAverageDegree,mR,mInitTime,mMaxDegree,mMinDegree);

	RECT R = {5, 5, 0, 0};
	HR(mFont->DrawText(0, buffer, -1, &R, DT_NOCLIP, D3DCOLOR_XRGB(255,255,255)));
}