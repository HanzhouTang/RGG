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
	void setMaxDegreeAndMinDegree(std::size_t max, std::size_t min);
	void setCameraHeight(float n);
	void setAverageDegree(float n);
	void setVertexCount(DWORD n);
	void setR(float r);
	void setInitTime(float t);
	void update(float dt);
	void display();
	inline std::size_t GetMaxDegree() { return mMaxDegree; }
	inline std::size_t GetMinDegree() { return mMinDegree; }
	inline void SetEdgeNumber(std::size_t e) { mEdge = e; }
	inline void SetMaxDeletedDegree(std::size_t d) { mMaxDeletedDegree=d; }
	inline void SetColorNeeded(std::size_t c) { mColorNeeded = c; }
	inline void SetMaxColorSet(int t) { mMaxColorSet = t; }
	inline void SetTerminalClique(std::size_t t) { mTerminalCliqueSize = t; }
	inline void SetColoringTime(float t) { mColoringTime = t; }
	inline void SetColoringFinished() { mColorFinished = true; }
	inline void SetBiggestBipartiteEdgeNumber(std::size_t n) { mBiggestBipartiteEdgeNumber = n; }
	inline void SetBiggestBipartiteVerticesNumber(std::size_t n) { mBiggestBipartiteVerticesNumber = n; }
	inline void SetBiggestBipartiteDominationPercentage(float n) { mBiggestBipartiteDominationPercentage = n; }
	inline void SetSecondBipartiteEdgeNumber(std::size_t n) { mSecondBipartiteEdgeNumber = n; }
	inline void SetSecondBipartiteVerticesNumber(std::size_t n) { mSecondBipartiteVerticesNumber = n; }
	inline void SetSecondBipartiteDominationPercentage(float n) { mSecondBipartiteDominationPercentage = n; }
	inline void SetBackbonePhase(int i) { mBackbonePhase = i; }
	inline void SetBackboneTime(float t) { mBackboneTime = t; }
	inline void SetSphereFacesNumber(std::size_t n) { mSphereFacesNumber = n; }
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
	bool mColorFinished;
	int mBackbonePhase;
	int mMaxColorSet;
	float mColoringTime;
	float mBackboneTime;
	std::size_t mSphereFacesNumber;
	std::size_t mColorNeeded;
	std::size_t mMaxDegree;
	std::size_t mMinDegree;
	std::size_t mEdge;
	std::size_t mMaxDeletedDegree;
	std::size_t mTerminalCliqueSize;
	std::size_t mBiggestBipartiteEdgeNumber;
	std::size_t mBiggestBipartiteVerticesNumber;
	float       mBiggestBipartiteDominationPercentage;
	std::size_t mSecondBipartiteEdgeNumber;
	std::size_t mSecondBipartiteVerticesNumber;
	float       mSecondBipartiteDominationPercentage;

}; 
#endif // GFX_STATS_H