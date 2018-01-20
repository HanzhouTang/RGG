#pragma once
#include"CShape.h"
class CSphere :public CShape {
private:
	float Radius = 5.0f;
public:
	CSphere(DWORD numv, float avgd,HWND h) :CShape(numv, avgd,h) {
		mR = acos((mNumVertices - 2 * mAverageDegree - 2) / mNumVertices);
		mRealR = Radius*mR;
		mRealRSquared = mRealR*mRealR;
		mDistance = mRealR;
	}
protected:
	virtual float Distance(D3DXVECTOR3 a, D3DXVECTOR3 b) {
		float theta = acos(D3DXVec3Dot(&a, &b) / (Radius*Radius));
		return Radius * theta;
	}
	virtual void GenerateVertices() {
		unsigned seed = static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count());
		std::mt19937 generator(seed);
		std::uniform_real_distribution<double> uniform01(0.0, 1.0);
		mVerts.resize(mNumVertices);
		for (std::size_t k = 0; k < mNumVertices; k++) {
			float theta = static_cast<float>(2 * D3DX_PI * uniform01(generator));
			float phi = static_cast<float>(acos(1 - 2 * uniform01(generator)));
			float x = sin(phi) * cos(theta);
			float y = sin(phi) * sin(theta);
			float z = cos(phi);
			mVerts[k].x = x * Radius;
			mVerts[k].z = z * Radius;
			mVerts[k].y = y * Radius;
		}
	}
	virtual void SpliteIntoCells() {
		GenThetaTable();
		DivideSphere();
	}
	virtual void GenerateLines() {
		for (int i = 0; i < mCells.size(); i++) {
			for (int j = 0; j < mCells[i].size(); j++) {
				for (int x = 0; x < mCells[i][j].size(); x++) {
					for (int y = x + 1; y < mCells[i][j].size(); y++) {
						const auto& vs = mCells[i][j];
						auto a = mVerts[vs[x]], b = mVerts[vs[y]];
						if (Distance(a, b) < mDistance) {
							mLines.push_back(Line(vs[x], vs[y]));
							mMatrix[vs[x]].insert(vs[y]);
							mMatrix[vs[y]].insert(vs[x]);
						}
					}
				}
				if (j + 1 < mCells[i].size()) {
					GenLinesByCells(mCells[i][j], mCells[i][j + 1]);
				}

				if (i + 1 < mCells.size()) {
					for (int k = 0; k < mCells[i + 1].size(); k++) {
						GenLinesByCells(mCells[i][j], mCells[i + 1][k]);
					}
				}
			}
		}
	}

private:
	std::vector<float> mThetaPhiTable;
	void GenThetaTable() {
		float  theta = 0;
		float phi = 0.001f;
		while (phi < D3DX_PI) {
			theta = mRealR / (sin(phi) * Radius);
			if (theta > 2 * D3DX_PI)
				theta = 2 * D3DX_PI;
			mThetaPhiTable.push_back(theta);
			phi += mRealR / Radius;
		}
	}


	void DivideSphere() {
		mCells.resize(mThetaPhiTable.size());
		for (int i = 0; i < mCells.size(); i++) {
			float theta_scalar = mThetaPhiTable[i];
			mCells[i].resize((int)(2 * D3DX_PI / theta_scalar) + 1);
		}
		for (int i = 0; i < mVerts.size(); i++) {
			auto node = mVerts[i];
			std::unordered_map<int, float> temp;
			float theta = atan(node.y / node.x);
			if (theta < 0) theta = theta + 2 * D3DX_PI;
			float phi = acos(node.z / Radius);
			int phi_number = static_cast<int>( phi / (mRealR / Radius));
			float theta_scalar = mThetaPhiTable[phi_number];
			int theta_number = static_cast<int>(theta / theta_scalar);
			mCells[phi_number][theta_number].push_back(i);
		}
	}

};