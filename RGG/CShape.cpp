#include"CShape.h"
CShape::CShape(DWORD numv, float averd, HWND h)
	:mNumVertices(numv), mAverageDegree(averd), mHwnd(h)
{
	mMaxDegree = -1;
	mMinDegree = -1;
	mTerminalCliqueSize = -1;
	mRealAverageDegree = -1;
	mMaxColorSet = -1;
	mMaxDeletedDegree = -1;
	mVerts.resize(numv);
	mMatrix.resize(numv);
	mColor.resize(numv, -1);
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
	std::unordered_set<int> marker;
	auto t = GetMinMaxDegree();
	std::size_t maxdegree = std::get<1>(t);
	std::size_t mindegree = std::get<0>(t);
	std::vector<std::unordered_set<int>> bucket(maxdegree + 1);
	for (int i = 0; i < mMatrix.size(); i++) {
		marker.insert(i);
		degree[i] = mMatrix[i].size();
		bucket[degree[i]].insert(i);
	}
	while (true) {

		while (bucket[mindegree].empty()) {
			mindegree++;
		}
		auto ptr = bucket[mindegree].begin();
		int node = *ptr;
		mSmallestLastOrder.push_front(node);
		mDeletedDegree.push_back(mindegree);
		if (mSmallestLastOrder.size() == mMatrix.size()) break;
		bucket[mindegree].erase(ptr);
		marker.erase(node);
		for (auto ptr = mMatrix[node].begin(); ptr != mMatrix[node].end(); ptr++) {
			int v = *ptr;
			if (marker.find(v) != marker.end()) {
				bucket[degree[v]].erase(v);
				degree[v] = degree[v] - 1;
				bucket[degree[v]].insert(v);
				mindegree = min(mindegree, degree[v]);
			}
		}

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
		if (mColorSets.find(c) == mColorSets.end()) {
			mColorSets[c] = std::unordered_set<int>();
			mColorSets[c].insert(node);
		}
		else {
			mColorSets[c].insert(node);
		}
		if (mColorResult.find(c) == mColorResult.end()) {
			mColorResult[c] = 1;
		}
		else mColorResult[c]++;
	}
}

void CShape::OutputColorResult() {
	std::ofstream colorf("color_result.csv", std::ofstream::out);
	for (auto&x : mColorResult) {
		colorf << x.second << ",";
	}
	colorf.close();
}

float CShape::GetDominationPercentage(std::vector<int> verts)
{
	std::unordered_set<int> covering;
	for (int x : verts) {
		covering.insert(x);
		for (int y : mMatrix[x]) {
			covering.insert(y);
		}
	}
	std::cout << "covering size: " << covering.size() << std::endl;
	return (static_cast<float>(covering.size()) / static_cast<float>(mMatrix.size())) * 100;
}

void CShape::OutputDegreeDistribution() {
	std::vector<int> distribution(mMaxDegree + 1, 0);
	for (auto& x : mMatrix) {
		distribution[x.size()]++;
	}
	std::ofstream f("degree_distribution.csv", std::ofstream::out);
	for (auto x : distribution) {
		f << x << ",";
	}
	f.close();
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


std::vector<Line> CShape::GetSecondBackboneLines()
{
	std::vector<Line> ret;
	for (int i = 0; i < mBipartiteGraphs[1].size(); i++) {
		if (mBipartiteGraphs[1][i].size() > 0) {
			for (int x : mBipartiteGraphs[1][i]) {
				if (i < x) {
					ret.push_back(Line(i, x));
				}
			}
		}
	}
	return ret;
}

std::vector<int> CShape::GetSecondBackboneVertices()
{
	std::vector<int> ret;
	const auto& subgraph = mBipartiteGraphs[1];
	for (int i = 0; i < subgraph.size(); i++) {
		if (subgraph[i].size() > 0)
			ret.push_back(i);
	}
	return ret;
}

std::vector<Line> CShape::GetBiggestBackboneLines()
{
	std::vector<Line> ret;
	for (int i = 0; i < mBipartiteGraphs[0].size(); i++) {
		if (mBipartiteGraphs[0][i].size() > 0) {
			for (int x : mBipartiteGraphs[0][i]) {
				if (i < x) {
					ret.push_back(Line(i, x));
				}
			}
		}
	}
	return ret;
}

std::vector<int> CShape::GetBiggesttBackboneVertices()
{
	std::vector<int> ret;
	const auto& subgraph = mBipartiteGraphs[0];
	for (int i = 0; i < subgraph.size(); i++) {
		if (subgraph[i].size() > 0)
			ret.push_back(i);
	}
	return ret;
}


int CShape::GetValidVerticeNumber(std::vector<std::unordered_set<int>>&subgraph)
{
	int ret = 0;
	for (int i = 0; i < subgraph.size(); i++) {
		if (subgraph[i].size() > 0)
			ret++;
	}
	return ret;
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


void CShape::GenerateBipartites() {
	using std::cout;
	using std::endl;
	for (int i = 0; i < 3; i++) {
		for (int j = i + 1; j < 4; j++) {
			if (mBipartiteGraphs.size() == 0) {
				mBipartiteGraphs.push_back(CombineColorSets(mMax4Colors[i], mMax4Colors[j]));
				mBipartiteGraphsSize.push_back(GetValidVerticeNumber(mBipartiteGraphs.back()));
			}
			else if (mBipartiteGraphs.size() == 1) {
				mBipartiteGraphs.push_back(CombineColorSets(mMax4Colors[i], mMax4Colors[j]));
				mBipartiteGraphsSize.push_back(GetValidVerticeNumber(mBipartiteGraphs.back()));
				if (mBipartiteGraphsSize[0] < mBipartiteGraphsSize[1]) {
					std::swap(mBipartiteGraphsSize[0], mBipartiteGraphsSize[1]);
					std::swap(mBipartiteGraphs[0], mBipartiteGraphs[1]);
				}
				
			}
			else {
				auto temp = CombineColorSets(mMax4Colors[i], mMax4Colors[j]);
				int size = GetValidVerticeNumber(temp);
				if (size > mBipartiteGraphsSize[0]) {
					std::swap(size, mBipartiteGraphsSize[0]);
					std::swap(temp, mBipartiteGraphs[0]);
				}
				else if (size > mBipartiteGraphsSize[1]) {
					std::swap(size, mBipartiteGraphsSize[1]);
					std::swap(temp, mBipartiteGraphs[1]);
				}
			}
			for (int x : mBipartiteGraphsSize) {
				cout << "! " << x << " ";
			}
			cout << endl;
		}

	}
}

void CShape::GenerateBackbone() {
	using std::cout;
	using std::endl;
	__int64 cntsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
	float secsPerCnt = 1000.0f / (float)cntsPerSec;
	__int64 prevTimeStamp = 0;
	__int64 currTimeStamp = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&prevTimeStamp);
	GetMax4Color(mMax4Colors);
	GenerateBipartites();
	QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
	float dt = (currTimeStamp - prevTimeStamp)*secsPerCnt;
	mBackboneTime = dt;
	SendMessage(mHwnd, WM_BACKBONE_FINSHED, 0, 0);
}

void CShape::GetMax4Color(std::vector<int>& colorSet)
{
	using std::tuple;
	using std::vector;
	using std::endl;
	using std::cout;
	vector<tuple<int, int>> temp;
	for (auto x : mColorResult) {
		temp.push_back(std::make_tuple(x.second, x.first));
	}
	std::sort(temp.begin(), temp.end(), [](auto const &t1, auto const &t2) {
		return std::get<0>(t1) > std::get<0>(t2);
	});
	int i = 0;
	for (const auto& x : temp) {
		if (i < 4) {
			colorSet.push_back(std::get<1>(x));
			cout << "color: " << std::get<1>(x) << " size: " << std::get<0>(x) << endl;
		}
		else break;
		i++;

	}
}

std::vector<std::unordered_set<int>> CShape::CombineColorSets(int c1, int c2) {
	using std::cout;
	using std::endl;
	std::vector<std::unordered_set<int>> subgraph = mMatrix;
	std::unordered_set<int> set1 = mColorSets[c1], set2 = mColorSets[c2];
	for (int i = 0; i < subgraph.size(); i++) {
		if (mColor[i] != c1&&mColor[i] != c2) {
			subgraph[i].clear();
		}
		else {
			if (mColor[i] == c1) {

				for (auto ptr = subgraph[i].begin(); ptr != subgraph[i].end();) {
					if (mColor[*ptr] != c2) {
						ptr = subgraph[i].erase(ptr);
					}
					else {
						ptr++;
					}
				}
			}
			if (mColor[i] == c2) {
				for (auto ptr = subgraph[i].begin(); ptr != subgraph[i].end();) {
					if (mColor[*ptr] != c1) {
						ptr = subgraph[i].erase(ptr);
					}
					else ptr++;
				}
			}
		}
	}
	return Trim(DeleteTail(subgraph));
	//return Trim(DeleteTail(subgraph));
}


void CShape::Visiting(const std::vector<std::unordered_set<int>>& subgrah, int v, std::unordered_set<int>& visted) {
	using std::cout;
	using std::endl;
	visted.insert(v);
	for (auto x : subgrah[v]) {
		if (visted.find(x) == visted.end())
			Visiting(subgrah, x, visted);
	}
}

std::vector<std::unordered_set<int>>&  CShape::Trim(std::vector<std::unordered_set<int>>& subgraph) {
	using std::cout;
	using std::endl;
	std::unordered_set<int> visted;
	std::unordered_set<int> max_visted;
	for (int i = 0; i < subgraph.size(); i++) {
		if (subgraph[i].size() > 0) {
			Visiting(subgraph, i, visted);
			if (max_visted.size() < visted.size()) {
				std::swap(max_visted, visted);
			}
			visted.clear();
		}
	}

	for (int i = 0; i < subgraph.size(); i++) {
		if (max_visted.find(i) == max_visted.end()) {
			subgraph[i].clear();
		}
	}

	/*int total = 0;
	for (int i = 0; i < subgraph.size(); i++) {
		if (subgraph[i].size() > 0)
			total++;
	}
	cout << "total size: " << total << endl;
	*/

	return subgraph;
}

std::vector<std::unordered_set<int>>& CShape::DeleteTail(std::vector<std::unordered_set<int>>& subgraph)
{
	std::vector<int> tails;
	for (int i = 0; i < subgraph.size(); i++) {
		if (subgraph[i].size() == 1) {
			tails.push_back(*(subgraph[i].begin()));
			subgraph[tails.back()].erase(i);
			subgraph[i].clear();
		}
		while (!tails.empty()) {
			int v = tails.back();
			tails.pop_back();
			if (subgraph[v].size() == 1) {
				tails.push_back(*(subgraph[v].begin()));
				subgraph[v].clear();
				subgraph[tails.back()].erase(v);
			}
		}
	}
	return subgraph;
}


