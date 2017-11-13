#include"CShape.h"
CShape::CShape(DWORD numv, float averd, HWND h)
	:mNumVertices(numv), mAvergaeDegree(averd), mHwnd(h)
{
	mMaxDegree = -1;
	mMinDegree = -1;
	mTerminalCliqueSize = -1;
	mRealAverageDegree = -1;
	mMaxColorSet = -1;
	mMaxDeletedDegree = -1;
	mVerts.resize(numv);
	mMatrix.resize(numv);
	mColor.resize(numv);
	mColorTable = {
		WHITE,RED,GREEN,YELLOW,BLUE,ORANGE,PURPLE,CYAN,MANGENTA,
		LIME,PINK,TEAL,LACENDER,BROWN,BEIGE,MAROON,MINT,OLIVE,
		CORAL,NAVY,GREY
	};
	for (int i = 0; i < 500; i++) {
		mColorTable.push_back(D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256));
	}
}

void CShape::GenLinesByCells(const std::vector<int>& a, const std::vector<int>& b) {
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

void CShape::GenerateSmallestLastOrder() {
	using std::cout;
	using std::endl;
	bool terminal = false;
	std::unordered_map<int, std::size_t> degree;
	std::vector<int> marker;
	marker.resize(mMatrix.size());
	std::vector<std::unordered_set<int>> bucket(mMaxDegree+1);
	for (int i = 0; i < mMatrix.size(); i++) {
		marker[i]=mMatrix.size();
		degree[i] = mMatrix[i].size();
		bucket[degree[i]].insert(i);
	}
	int current_order = mMatrix.size()-1;
	int mindegree = 0;
	while (true) {
		while (bucket[mindegree].empty()) {
			mindegree++;
		}
		auto ptr = bucket[mindegree].begin();
		int node = *ptr;
		mSmallestLastOrder.push_front(node);
		mDeletedDegree.push_back(degree[node]);//modify
		if (current_order==0) break;
		bucket[mindegree].erase(ptr);
		marker[node] = 0;
		for (auto ptr = mMatrix[node].begin(); ptr != mMatrix[node].end(); ptr++) {
			int v = *ptr;
			if (marker[v] > current_order) {
				marker[v] = current_order;
				bucket[degree[v]].erase(v);
				degree[v] = degree[v] - 1;
				bucket[degree[v]].insert(v);
				mindegree = min(mindegree, degree[v]);
			}
		}
		current_order--;
		if (!terminal) {
			int count = 0;
			std::size_t temp_degree = 0;
			for (auto&x : bucket) {
				if (x.size() != 0) {
					count++;
					temp_degree = x.size();
				}
			}
			if (count == 1) {
				for (auto&x : bucket) {
					if (x.size() > 0) {
						if (degree[*x.begin()] == x.size() - 1) {
							mTerminalCliqueSize = temp_degree;
							terminal = true;
						}
					}
				}

			}
		}
	}
	std::reverse(mDeletedDegree.begin(), mDeletedDegree.end());
}

void CShape::GenerateVertexColor() {
	for (auto ptr = mSmallestLastOrder.begin(); ptr != mSmallestLastOrder.end(); ptr++) {
		int c = -1;
		int node = *ptr;
		bool canColor = true;
		do {
			c++;
			canColor = true;
			for (auto nearptr = mMatrix[node].begin(); nearptr != mMatrix[node].end(); nearptr++) {
				if (mColor[*nearptr] == c) {
					canColor = false;
					break;
				}
			}
		} while (!canColor);

		mColor[node] = c;

		if (mColorResult.find(c) == mColorResult.end()) {
			mColorResult[c] = 1;
		}
		else mColorResult[c]++;
	}
}

void CShape::OutputColorResult() {
	std::ofstream colorf("color_result.csv", std::ofstream::out);
	int temp_total = 0;
	
	for (auto&x : mColorResult) {
		colorf << x.second << ",";
		temp_total += x.second;
	}
	
	colorf.close();
}

void CShape::OutputDegreeResult() {
	std::ofstream of("degree_result.csv", std::ofstream::out);
	of << "original degree,degree when deleted" << std::endl;
	int i = 0;
	for (auto ite = mSmallestLastOrder.begin(); ite != mSmallestLastOrder.end(); ite++) {
		of << mMatrix[*ite].size() << "," << mDeletedDegree[i] << std::endl;
		i++;
	}
	of.close();
}

size_t CShape::GetMaxDeletedDegree() {
	if (mMaxDeletedDegree == -1) {
		std::size_t mx = 0;
		for (auto&x : mDeletedDegree) {
			mx = max(mx, x);
		}
		mMaxDeletedDegree = mx;
	}
	return mMaxDeletedDegree;
}

int CShape::GetMaxColorSet() {
	if (mMaxColorSet == -1) {
		int max_color = 0;
		for (auto&x : mColorResult) {
			max_color = max(max_color, x.second);
		}
		mMaxColorSet = max_color;
	}
	return mMaxColorSet;
}

void CShape::GenerateColoring() {
	using std::cout;
	using std::endl;
	__int64 cntsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
	float secsPerCnt = 1000.0f / (float)cntsPerSec;
	__int64 prevTimeStamp = 0;
	__int64 currTimeStamp = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&prevTimeStamp);
	GenerateSmallestLastOrder();
	GenerateVertexColor();
	QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
	float dt = (currTimeStamp - prevTimeStamp)*secsPerCnt;
	//cout << "coloring time: " << dt << std::endl;
	mColoringTime = dt;
	SendMessage(mHwnd, WM_COLORING_FINSHED, 0, 0);
	OutputColorResult();
	OutputDegreeResult();
}

void CShape::Init() {
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

std::tuple<std::size_t, std::size_t> CShape::GetMinMaxDegree() {
	float acc = 0;
	if (mMinDegree == -1) {
		std::size_t mx = 0;
		std::size_t mi = INT_MAX;
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
float CShape::GetAverageDegree() {
	if (mRealAverageDegree + 1 < 0.01f) {
		float acc = 0;
		for (auto& x : mMatrix) {
			acc += x.size();
		}
		mRealAverageDegree = acc / mNumVertices;
	}
	return mRealAverageDegree;
}