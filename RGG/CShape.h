#pragma once
#include"d3dUtil.h"
class CShape {
protected:
	DWORD mNumLines;
	DWORD mNumVertices;
	float mAvergaeDegree;
	float mR;
	float mRealRSquared;
	float mRealR;
	float mDistance;
	float mInitTime;
	float mRealAverageDegree;
	int mMaxDegree;
	int mMinDegree;
	std::vector<D3DXVECTOR3> mVerts;
	std::vector<Line> mLines;
	std::vector<std::unordered_set<int>> mMatrix;
	std::vector<int> mColor;
	std::list<int> mSmallestLastOrder;
	std::vector<std::vector<std::vector<int>>> mCells;
	CShape(DWORD numv, float averd) :mNumVertices(numv), mAvergaeDegree(averd) {
		mMaxDegree = -1;
		mMinDegree = -1;
		mRealAverageDegree = -1;
		mVerts.resize(numv);
		mMatrix.resize(numv);
		mSmallestLastOrder.resize(numv);
		mColor.resize(numv);
	}
	void GenLinesByCells(const std::vector<int>& a, const std::vector<int>& b) {
		for (std::size_t i = 0; i < a.size(); i++) {
			for (std::size_t j = 0; j < b.size(); j++) {
				if (Distance(mVerts[a[i]], mVerts[b[j]]) <= mDistance) {
					mLines.push_back(Line(a[i], b[j]));
					mMatrix[a[i]].insert(b[j]);
					mMatrix[b[j]].insert(a[i]);
				}
			}
		}
	}
	virtual void GenerateVertices() = 0;
	virtual void SpliteIntoCells() = 0;
	virtual void GenerateLines() = 0;
	virtual float Distance(D3DXVECTOR3 a, D3DXVECTOR3 b) = 0;

public:
	void Init() {
		__int64 cntsPerSec = 0;
		QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
		float secsPerCnt = 1000.0f / (float)cntsPerSec;
		__int64 prevTimeStamp = 0;
		__int64 currTimeStamp = 0;
		QueryPerformanceCounter((LARGE_INTEGER*)&prevTimeStamp);
		GenerateVertices();
		SpliteIntoCells();
		GenerateLines();
		QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
		mInitTime = (currTimeStamp - prevTimeStamp)*secsPerCnt;
	}
	const std::vector<Line>& GetLines() { return mLines; }
	const std::vector<D3DXVECTOR3>& GetVertices() { return mVerts; }
	const std::vector<std::unordered_set<int>>& GetMatrix() { return mMatrix; }
	int GetInitTime() { return mInitTime; }
	int GetVerticesNumber() { return mNumVertices; }
	int GetLinesNumber() { return mLines.size(); }
	inline  float GetR() { return mR; }
	std::tuple<int, int> GetMinMaxDegree() {
		float acc = 0;
		if (mMinDegree == -1) {
			int mx = 0;
			int mi = INT_MAX;
			for (auto& x : mMatrix) {
				mx = max(mx, x.size());
				mi = min(mi, x.size());
				acc += x.size();
			}
			mMaxDegree = mx;
			mMinDegree = mi;
			mRealAverageDegree = acc / mNumVertices;
		}
		return std::make_tuple(mMinDegree, mMaxDegree);
	}
	float GetAverageDegree() {
		if (mRealAverageDegree + 1 < 0.01f) {
			float acc = 0;
			for (auto& x : mMatrix) {
				acc += x.size();
			}
			mRealAverageDegree = acc / mNumVertices;
		}
		return mRealAverageDegree;
	}
};
