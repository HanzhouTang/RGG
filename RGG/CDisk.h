#pragma once
#include"CShape.h"
class CDisk :public CShape {
private:
	const int Length = 10;
	const int SquareSize = 10000;
	const int Radius = (SquareSize/2)*(SquareSize/2);

public:
	CDisk(DWORD numv, float avgd,HWND h) :CShape(numv, avgd,h) {
		mR = sqrt((mAverageDegree + 1) / (mNumVertices));
		mRealRSquared = ((Length*Length)*(mAverageDegree + 1)) / (mNumVertices * 4);
		mRealR = sqrt(mRealRSquared);
		mDistance = mRealRSquared;
	}
protected:
	virtual float Distance(D3DXVECTOR3 a, D3DXVECTOR3 b) {
		return (a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y) + (a.z - b.z)*(a.z - b.z);
	}
	virtual void GenerateVertices() {
		using std::vector;
		unsigned seed = static_cast<unsigned>( std::chrono::system_clock::now().time_since_epoch().count());
		srand(seed);
		float tempR = sqrt((SquareSize*SquareSize)*(mAverageDegree + 1) / (mNumVertices * 4));
		std::size_t CellNum = static_cast<std::size_t>(SquareSize / tempR) + 1;
		mCells.resize(CellNum, vector<vector<int>>(CellNum));
		int scalar = SquareSize / Length;
		float xOffset = -SquareSize * 0.5f;
		float zOffset = SquareSize * 0.5f;
		int center = SquareSize / 2;
		std::size_t i = 0;
		while (i < mNumVertices) {
			int k = rand() % SquareSize;
			int j = rand() % SquareSize;
			if ((k - center)*(k - center) + (j - center)*(j - center) <= Radius) {
				mVerts[i].x = (j + xOffset) / scalar;
				mVerts[i].z = (-k + zOffset) / scalar;
				mVerts[i].y = 0.0f;
				mCells[static_cast<std::size_t>(j / tempR)][static_cast<std::size_t>(k / tempR)].push_back(i);
				i++;
			}
		}
	}

	virtual void SpliteIntoCells() {}

	virtual void GenerateLines() {
		for (int i = 0; i < mCells.size(); i++) {
			for (int j = 0; j < mCells[i].size(); j++) {
				const auto& vs = mCells[i][j];
				for (std::size_t x = 0; x < vs.size(); x++) {
					for (std::size_t y = x + 1; y < vs.size(); y++) {
						auto a = mVerts[vs[x]], b = mVerts[vs[y]];
						if (Distance(a, b) <= mDistance) {
							mLines.push_back(Line(vs[x], vs[y]));
							mMatrix[vs[x]].insert(vs[y]);
							mMatrix[vs[y]].insert(vs[x]);
						}
					}
				}
				if (i + 1 < mCells.size())
					GenLinesByCells(mCells[i][j], mCells[i + 1][j]);
				if (j + 1 < mCells[i].size())
					GenLinesByCells(mCells[i][j], mCells[i][j + 1]);
				if (i + 1 < mCells.size() && j + 1 < mCells[i].size())
					GenLinesByCells(mCells[i][j], mCells[i + 1][j + 1]);
				if (i - 1 >= 0 && j + 1 < mCells[i].size())
					GenLinesByCells(mCells[i][j], mCells[i - 1][j + 1]);
			}
		}
	}

};