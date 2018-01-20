#pragma once
#include"d3dUtil.h"
class CShape {
protected:
	DWORD mNumLines;
	DWORD mNumVertices;
	float mAverageDegree;
	float mR;
	float mRealRSquared;
	float mRealR;
	float mDistance;
	float mInitTime;
	float mColoringTime;
	float mBackboneTime;
	float mRealAverageDegree;
	std::size_t mMaxDegree;
	std::size_t mMinDegree;
	std::size_t mTerminalCliqueSize;
	int mMaxColorSet;
	std::size_t mMaxDeletedDegree;
	std::map<int, int> mColorResult;
	std::vector<int> mColor;
	std::list<int> mSmallestLastOrder;
	std::vector<std::size_t> mDeletedDegree;
	std::unordered_map<int, std::unordered_set<int>> mColorSets;
	HWND mHwnd;
	std::vector<D3DXVECTOR3> mVerts;
	std::vector<D3DCOLOR> mColorTable;
	std::vector<Line> mLines;
	std::vector<std::unordered_set<int>> mMatrix;
	std::vector<std::vector<std::unordered_set<int>>> mBipartiteGraphs;
	std::vector<int> mBipartiteGraphsSize;
	std::vector<std::vector<std::vector<int>>> mCells;
	std::vector<int> mMax4Colors;
	CShape(DWORD numv, float averd, HWND h);
	void GenLinesByCells(const std::vector<int>& a, const std::vector<int>& b);
	virtual void GenerateVertices() = 0;
	virtual void SpliteIntoCells() = 0;
	virtual void GenerateLines() = 0;
	virtual float Distance(D3DXVECTOR3 a, D3DXVECTOR3 b) = 0;
	void GenerateSmallestLastOrder();
	void GenerateVertexColor();
	void GenerateColoring();
	void OutputColorResult();
	void OutputDegreeResult();
	void GenerateBackbone();
	void GenerateBipartites();
	std::vector<std::unordered_set<int>> CombineColorSets(int c1, int c2);
	void Visiting(const std::vector<std::unordered_set<int>>& subgrah,int v, std::unordered_set<int>& visted);
	std::vector<std::unordered_set<int>>& Trim(std::vector<std::unordered_set<int>>&);
	std::vector<std::unordered_set<int>>& DeleteTail(std::vector<std::unordered_set<int>>&);
	int GetValidVerticeNumber(std::vector < std::unordered_set<int>>&);
public:
	float GetDominationPercentage(std::vector<int> verts);
	void OutputDegreeDistribution();
	void Init();
	void Color() {
		std::thread([&] {GenerateColoring(); }).detach();
	}
	std::vector<Line> GetBiggestBackboneLines();
	std::vector<Line> GetSecondBackboneLines();
	std::vector<int> GetSecondBackboneVertices();
	std::vector<int> GetBiggesttBackboneVertices();
	void Backbone() {
		std::thread([&] {GenerateBackbone(); }).detach();
	}
    
	D3DCOLOR GetColor(int vertice) {
		return mColorTable[mColor[vertice]];
	}
	const std::vector<Line>& GetLines() { return mLines; }
	const std::vector<D3DXVECTOR3>& GetVertices() { return mVerts; }
	const std::vector<std::unordered_set<int>>& GetMatrix() { return mMatrix; }
	float GetInitTime() { return mInitTime; }
	int GetVerticesNumber() { return mNumVertices; }
	std::size_t GetLinesNumber() { return mLines.size(); }
	inline std::size_t GetColorNumber() { return mColorResult.size(); }
	inline  float GetR() { return mR; }
	std::tuple<std::size_t, std::size_t> GetMinMaxDegree();
	float GetAverageDegree();
	float GetBackboneTime() { return mBackboneTime; }
	std::size_t GetMaxDeletedDegree();
	int GetMaxColorSet();
	std::size_t GetTerminalCliqueSize() { return mTerminalCliqueSize; }
	float GetColoringTime() { return mColoringTime; }
	void GetMax4Color(std::vector<int>& colorSet);
};
