#ifndef GFX_STATS_H
#define GFX_STATS_H

#include <d3dx9.h>

class GfxStats
{
public:
	GfxStats();
	~GfxStats();

	void onLostDevice();
	void onResetDevice();

	void addVertices(DWORD n);
	void subVertices(DWORD n);
	void setMaxDegreeAndMinDegree(int max, int min);
	void setCameraHeight(float n);
	void setAverageDegree(float n);
	void setVertexCount(DWORD n);
	void setR(float r);
	void setInitTime(float t);
	void update(float dt);
	void display();
	inline int GetMaxDegree() { return mMaxDegree; }
	inline int GetMinDegree() { return mMinDegree; }

private:
	// Prevent copying
	GfxStats(const GfxStats& rhs);
	GfxStats& operator=(const GfxStats& rhs);
	
private:
	ID3DXFont* mFont;
	float mFPS;
	float mInitTime;
	float mMilliSecPerFrame;
	float mCameraHeight;
	DWORD mNumVertices;
	float mAverageDegree;
	float mR;
	int mMaxDegree;
	int mMinDegree;
};
#endif // GFX_STATS_H