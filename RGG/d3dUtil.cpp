#include "d3dUtil.h"
#include "Vertex.h"

void GenVertexSquare(int numVertices,
	std::vector<D3DXVECTOR3>& verts)
{
	int width = MAP_LENGTH;
	int depth = MAP_LENGTH;
	verts.resize(numVertices);
	// Offsets to translate grid from quadrant 4 to center of 
	// coordinate system.
	float xOffset = -width * 0.5f;
	float zOffset = depth * 0.5f;
	auto center = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	int k = 0;
	while (k < numVertices) {
		int i = rand() % depth;
		int j = rand() % width;
		verts[k].x = (j + xOffset) / 100;
		verts[k].z = (-i + zOffset) / 100;
		verts[k].y = 0.0f;
		++k; // Next vertex
	}
}


//Get the vertexs in the cell, and the length of cell is R
std::vector<int> GetByR(std::vector<std::vector<int>>& map, int i, int j, int r) {
	int iend = min(i + r, MAP_LENGTH);
	int jend = min(j + r, MAP_LENGTH);
	std::vector<int> temp;
	i = max(0, i);
	j = max(0, j);
	for (i; i < iend; i++) {
		for (int tempj = j; tempj < jend; tempj++) {
			if (map[i][tempj]!=-1) {
				temp.push_back(map[i][tempj]);
			}
		}
	}
	return temp;
}


std::vector<Line> GenLineBySets(const std::vector<int>& a, const std::vector<int>& b, float scalarR,\
	const std::vector<D3DXVECTOR3>& verts, std::vector<std::unordered_set<int>>& matrix) {
	using std::cout;
	std::vector<Line> t;
	for (std::size_t i = 0; i < a.size(); i++) {
		for (std::size_t j = 0; j < b.size(); j++) {
			if (Distance(verts[a[i]], verts[b[j]]) <= scalarR) {
				t.push_back(Line(a[i], b[j]));
				matrix[a[i]].insert(b[j]);
				matrix[b[j]].insert(a[i]);
			}
		}
	}
	return t;

}



std::tuple<int, float> GenVertexAndCellSquare(int numVertices,\
	std::vector<D3DXVECTOR3>&verts, float averageDegree,\
	std::unordered_map<int,std::vector<int>>& gmap) {
	int width = MAP_LENGTH;
	int depth = MAP_LENGTH;
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	srand(seed);
	std::vector<std::vector<int>> map(MAP_LENGTH, std::vector<int>(MAP_LENGTH, -1));
	verts.resize(numVertices);
	// Offsets to translate grid from quadrant 4 to center of 
	// coordinate system.
	float xOffset = -width * 0.5f;
	float zOffset = depth * 0.5f;
	auto center = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	int k = 0;
	while (k < numVertices) {
		int i = rand() % depth;
		int j = rand() % width;
		verts[k].x = (j + xOffset) / 100;
		verts[k].z = (-i + zOffset) / 100;
		verts[k].y = 0.0f;
		map[i][j] = k;
		++k; // Next vertex
	}
	
	int r = sqrt((averageDegree + 1) * 1000 * 1000 / (D3DX_PI*numVertices)) + 1;
	float scalarR = (averageDegree + 1) / (D3DX_PI*numVertices) * 100;
	for (int i = 0; i < MAP_LENGTH; i += r) {
		for (int j = 0; j < MAP_LENGTH; j += r) {
			auto vs = GetByR(map, i, j, r);
			gmap[i * 1000 + j] = vs;
		}
	}
	return std::make_tuple(r, scalarR);
}

std::tuple<int, float> GenVertexAndCellDisk(int numVertices,\
	std::vector<D3DXVECTOR3>&verts, float averageDegree,\
	std::unordered_map<int, std::vector<int>>& gmap) {
	using std::cout;
	using std::endl;
	int width = MAP_LENGTH;
	int depth = MAP_LENGTH;
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	//srand(seed); 
	std::vector<std::vector<int>> map(MAP_LENGTH, std::vector<int>(MAP_LENGTH, -1));
	verts.resize(numVertices);
	// Offsets to translate grid from quadrant 4 to center of 
	// coordinate system.
	float xOffset = -width * 0.5f;
	float zOffset = depth * 0.5f;
	auto center = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	int k = 0;
	while (k < numVertices) {
		int i = rand() % depth;
		int j = rand() % width;
		if ((i - 500)*(i - 500) + (j - 500)*(j - 500) <= 250000) {
			verts[k].x = (j + xOffset) / 100;
			verts[k].z = (-i + zOffset) / 100;
			verts[k].y = 0.0f;
			map[i][j] = k;
			++k; // Next vertex
		}
	}
	
	int r = sqrt((averageDegree + 1) * 500 * 500 / (numVertices)) + 1;
	
	float scalarR = (averageDegree + 1) / (numVertices) * 25;


	for (int i = 0; i < MAP_LENGTH; i += r) {
		for (int j = 0; j < MAP_LENGTH; j += r) {
			auto vs = GetByR(map, i, j, r);
			gmap[i * 1000 + j] = vs;
		}
	}
	return std::make_tuple(r, scalarR);
}

//在cell的时候还是有bug，好好找一找

void GenLinkingByCell(int r, float scalarR, std::vector<Line>&lines,\
	std::vector<std::unordered_set<int>>& matrix,const std::unordered_map<int, std::vector<int>>& gmap,\
	const std::vector<D3DXVECTOR3>&verts) {
	using std::cout;
	using std::endl;
	matrix.resize(verts.size());
	for (int i = 0; i < MAP_LENGTH; i += r) {
		for (int j = 0; j < MAP_LENGTH; j += r) {
			//cout << " i= " << i << " j= " << j << endl;
			std::unordered_map<int, std::vector<int>>::const_iterator ptr;
			ptr = gmap.find(i * 1000 + j);
			auto vs = ptr->second;
			for (std::size_t x = 0; x < vs.size(); x++) {
				for (std::size_t y = x + 1; y < vs.size(); y++) {
					auto a = verts[vs[x]], b = verts[vs[y]];
					if (Distance(a, b) <= scalarR) {
						lines.push_back(Line(vs[x], vs[y]));
						matrix[vs[x]].insert(vs[y]);
						matrix[vs[y]].insert(vs[x]);
					}
				}
			}
		
			std::vector<Line> tempLines;
			ptr=gmap.find((i - r) * 1000 + j + r);
			if (ptr != gmap.cend()) {
				tempLines = GenLineBySets(vs, ptr->second, scalarR, verts, matrix);
				lines.insert(lines.end(), tempLines.begin(), tempLines.end());
			}
		   
			ptr = gmap.find(i * 1000 + j + r);
			if (ptr != gmap.cend()) {
				tempLines = GenLineBySets(vs, ptr->second, scalarR, verts, matrix);
				lines.insert(lines.end(), tempLines.begin(), tempLines.end());
			}

			ptr = gmap.find((i + r) * 1000 + (j + r));
			if (ptr != gmap.cend()) {
				tempLines = GenLineBySets(vs, ptr->second, scalarR, verts, matrix);
				lines.insert(lines.end(), tempLines.begin(), tempLines.end());
			}

			ptr= gmap.find((i + r) * 1000 + j);
			if (ptr != gmap.cend()) {
				tempLines = GenLineBySets(vs, ptr->second, scalarR, verts, matrix);
				lines.insert(lines.end(), tempLines.begin(), tempLines.end());
			}

		}
	}

}


void GenVertexDisk(int numVertices,
	std::vector<D3DXVECTOR3>& verts)
{
	int width = 1000;
	int depth = 1000;
	verts.resize(numVertices);
	// Offsets to translate grid from quadrant 4 to center of 
	// coordinate system.
	float xOffset = -width * 0.5f;
	float zOffset = depth * 0.5f;
	auto center = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	int k = 0;
	while (k < numVertices) {
		int i = rand() % depth;
		int j = rand() % width;
		if ((i - 500)*(i - 500) + (j - 500)*(j - 500) <= 250000) {
			// Negate the depth coordinate to put in quadrant four.  
			// Then offset to center about coordinate system.
			verts[k].x = (j + xOffset) / 100;
			verts[k].z = (-i + zOffset) / 100;
			verts[k].y = 0.0f;
			++k; // Next vertex
		}
	}
}

float Distance(D3DXVECTOR3 a, D3DXVECTOR3 b) {
	return (a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y) + (a.z - b.z)*(a.z - b.z);
}


void GenSquareLinkingLines(std::vector<D3DXVECTOR3>& verts,
	int averageDegree, std::vector<Line>&lines, std::vector<std::unordered_set<int>>& matrix) {
	matrix.resize(verts.size());
	float r = (averageDegree + 1) / (D3DX_PI*verts.size()) * 100;
	for (int i = 0; i < verts.size(); i++) {
		for (int j = i + 1; j < verts.size(); j++) {
			if (Distance(verts[i], verts[j]) <= r) {
				lines.push_back(Line(i, j));
				matrix[i].insert(j);
				matrix[j].insert(i);
			}
		}
	}
}


//when R is 5.0
float SphereDistance(D3DXVECTOR3 a, D3DXVECTOR3 b) {
	double theta = acos(D3DXVec3Dot(&a,&b)/25);
	return 5*theta;
}


//when R is 5.0
void GenSphereLinkingLines(const std::vector<D3DXVECTOR3>& verts,\
	float d, std::vector<Line>&lines, std::vector<std::unordered_set<int>>& matrix) {
	matrix.resize(verts.size());
	float n = verts.size();
	//需要计算
	float r = 5* acos((n-2*d-2)/n);
	for (int i = 0; i < verts.size(); i++) {
		for (int j = i + 1; j < verts.size(); j++) {
			if (SphereDistance(verts[i], verts[j]) <= r) {
				lines.push_back(Line(i, j));
				matrix[i].insert(j);
				matrix[j].insert(i);
			}
		}
	}
}




void GenSmallestLastOrder(const std::vector<std::unordered_set<int>>& matrix, std::list<int>& order,\
	int mindegree,int maxdegree,std::vector<int>& degree_list) {
	using std::cout;
	using std::endl;
	bool terminal = false;
	//cout << "mindegree= " << mindegree << "maxdegree= " << maxdegree << endl;
	std::unordered_map<int, int> degree;
	std::unordered_set<int> marker;
	std::vector<std::unordered_set<int>> bucket(maxdegree+1);
	for (int i = 0; i < matrix.size(); i++) {
		marker.insert(i);
		degree[i] = matrix[i].size();
		bucket[degree[i]].insert(i);
	}
	while (true) {
		//cout << "=============================================================================" << endl << endl;
		//cout << "mindegree= " << mindegree << endl;
		while (bucket[mindegree].empty()) {
			mindegree++;
		}
		auto ptr = bucket[mindegree].begin();
		int node = *ptr;
		//cout << "node= " << node << endl;
		order.push_front(node);
		degree_list.push_back(mindegree);
		//cout << "mindegree= "<<mindegree << endl;
		//system("pause");
		if (order.size() == matrix.size()) break;
		bucket[mindegree].erase(ptr);
		marker.erase(node);
		for (auto ptr = matrix[node].begin(); ptr != matrix[node].end(); ptr++) {
			int v = *ptr;
			if (marker.find(v)!=marker.end()) {
				//cout << "v= " << v <<" degree of (v)= "<<degree[v]<<endl;
				bucket[degree[v]].erase(v);
				//cout << "here" << endl;
				degree[v] = degree[v] - 1;
				bucket[degree[v]].insert(v);
				//cout << "there" << endl;
				mindegree = min(mindegree, degree[v]);
			}
		}

		if (!terminal) {
			int count = 0, temp_degree = 0;
			for (auto&x : bucket) {
				if (x.size() != 0) {
					count++;
					temp_degree = x.size();
				}
			}
			if (count == 1 ) {
				for (auto&x : bucket) {
					if (x.size() > 0) {
						if (degree[*x.begin()] == x.size() - 1) {
							cout << "terminal cluque size: " << temp_degree << endl;
							terminal = true;
						}
					}
				}
				
			}
		}
	}
}

void GenVertexColor(const std::vector<std::unordered_set<int>>& matrix, \
	const std::list<int>& order, std::vector<int>& color, std::unordered_map<int, int>& color_result){

	using std::cout;
    using std::endl;
	using std::cout;
	using std::endl;
	color.resize(matrix.size(),-1);
	for (auto ptr = order.begin(); ptr != order.end(); ptr++) {
		int c = -1;
		int node = *ptr;
		bool canColor = true;
		do {
			c++;
			canColor = true;
			for (auto nearptr = matrix[node].begin(); nearptr != matrix[node].end(); nearptr++) {
				if (color[*nearptr] == c) {
					canColor = false;
					break;
				}
			}
		} while (!canColor);
		
		color[node] = c;

		if (color_result.find(c) == color_result.end()) {
			color_result[c] = 1;
		}
		else color_result[c]++;
	}
	
	
	//system("pause");
	
}


DWORD WINAPI Coloring(LPVOID colorPara){
	using std::cout;
	using std::endl;
	std::vector<int> degree_list;
	std::unordered_map<int, int> color_result;
	ColoringParameter* ptr = static_cast<ColoringParameter*>(colorPara);
	cout << ptr->mMatrix.size()<<endl;
	__int64 cntsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
	float secsPerCnt = 1000.0f / (float)cntsPerSec;
	__int64 prevTimeStamp = 0;
	__int64 currTimeStamp = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&prevTimeStamp);
	GenSmallestLastOrder(ptr->mMatrix, ptr->mOrder, ptr->mMinDegree, ptr->mMaxDegree, degree_list);
	GenVertexColor(ptr->mMatrix, ptr->mOrder,ptr->mColor,color_result);
	QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
	float dt = (currTimeStamp - prevTimeStamp)*secsPerCnt;
	SendMessage(ptr->mHwnd, WM_COLORING_FINSHED, 0, 0);
	cout << "part II using time: " << dt << endl;
	cout << "number of color: " << color_result.size() << endl;
	int max_color=0;
	std::ofstream colorf("color_result.csv", std::ofstream::out);
	int temp_total = 0;
	for (auto&x : color_result) {
		colorf << x.second <<",";
		max_color = max(max_color, x.second);
		temp_total += x.second;
	}
	colorf.close();
	//cout << "total colored: " << temp_total<<endl;
	cout << "max color class size: " << max_color << endl;
	
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!翻转 degree_list

	std::reverse(degree_list.begin(), degree_list.end());
	int mx = 0;
	int i = 0;
	std::ofstream of("degree_result.csv",std::ofstream::out);
	of << "original degree,degree when deleted" << endl;
	for (auto ite = ptr->mOrder.begin(); ite != ptr->mOrder.end(); ite++) {
		of << ptr->mMatrix[*ite].size() << "," << degree_list[i]<<endl;
		mx = max(mx, degree_list[i]);
		i++;
	}
	of.close();
	cout << "max degree when deleted " << mx << endl;
	return 0;
}

void GenVertexSphere(int numVertices, std::vector<D3DXVECTOR3>& verts) {

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::mt19937 generator(seed);
	std::uniform_real_distribution<double> uniform01(0.0, 1.0);
	verts.resize(numVertices);
	for (int k = 0; k < numVertices; k++) {
		double theta = 2 * D3DX_PI * uniform01(generator);
		double phi = acos(1 - 2 * uniform01(generator));
		double x = sin(phi) * cos(theta);
		double y = sin(phi) * sin(theta);
		double z = cos(phi);
		verts[k].x = x*5;
		verts[k].z = z*5;
		verts[k].y = y*5;
	}

}


void GenThetaTable(std::vector<float>& theta_phi_table,float r) {//先快点做，等会再改
	using std::cout;
	using std::endl;
	float  theta = 01;
	float phi = 0.001;
	//cout << "r= " << r << endl;
	while (phi < D3DX_PI) {
		theta = r / (sin(phi) * 5);
		if(theta>2*D3DX_PI)
		theta = 2*D3DX_PI;
		theta_phi_table.push_back(theta);
		//std::cout << " theta= " << theta << " phi =" << phi << std::endl;
		phi += r / 5;
	}

}



void GenSphereLinkingLinesByCell(const std::vector<D3DXVECTOR3>& verts, \
	float d, std::vector<Line>&lines, std::vector<std::unordered_set<int>>& matrix) {
	matrix.resize(verts.size());
	float n = verts.size();
	float r = 5 * acos((n - 2 * d - 2) / n);
	std::vector<float> theta_phi_table;
	GenThetaTable(theta_phi_table, r);
	std::vector<std::vector<std::vector<int>>> cells;
	DivideSphere(cells, verts, theta_phi_table, r);
	LinkSphereLinesByCell(cells, verts, r, lines, matrix);
}

void DivideSphere(std::vector<std::vector<std::vector<int>>>& cells, const std::vector<D3DXVECTOR3>& verts,\
	const std::vector<float>& theta_phi_tables,float r) {
	using std::cout;
	using std::endl;
	cells.resize(theta_phi_tables.size());
	for (int i = 0; i < cells.size(); i++) {
		float theta_scalar = theta_phi_tables[i];
		cells[i].resize((int)(2 * D3DX_PI / theta_scalar) + 1);
	}

	for (int i = 0; i < verts.size(); i++) {
		auto node = verts[i];
		std::unordered_map<int, float> temp;
		float theta = atan(node.y / node.x);
		if (theta < 0) theta = theta + 2 * D3DX_PI;
		float phi = acos(node.z / 5);
		int phi_number = phi / (r / 5);
		//cout <<endl<< " phi_number = " << phi_number << endl;
		float theta_scalar = theta_phi_tables[phi_number];
		int theta_number = theta / theta_scalar;
		cells[phi_number][theta_number].push_back(i);
	}
}

std::vector<Line> GenSphereLineBySets(const std::vector<int>& a, const std::vector<int>& b, float scalarR, \
	const std::vector<D3DXVECTOR3>& verts, std::vector<std::unordered_set<int>>& matrix) {
	using std::cout;
	std::vector<Line> t;
	for (std::size_t i = 0; i < a.size(); i++) {
		for (std::size_t j = 0; j < b.size(); j++) {
			if (SphereDistance(verts[a[i]], verts[b[j]]) <= scalarR) {
				t.push_back(Line(a[i], b[j]));
				matrix[a[i]].insert(b[j]);
				matrix[b[j]].insert(a[i]);
			}
		}
	}
	return t;
}

void LinkSphereLinesByCell(std::vector<std::vector<std::vector<int>>> cells, const std::vector<D3DXVECTOR3>& verts, \
	float r, std::vector<Line>&lines, std::vector<std::unordered_set<int>>& matrix) {
	using std::cout;
	using std::endl;
	int acc = 0;
	//先写的简单点，然后在改

	for (int i = 0; i < cells.size(); i++) {
		for (int j = 0; j < cells[i].size(); j++) {
			//cout << " i= " << i << " j= " << j << " size= " << cells[i][j].size() << endl;
		//	acc += cells[i][j].size();
			for (int x = 0; x < cells[i][j].size(); x++) {
				for (int y = x + 1; y < cells[i][j].size(); y++) {
					auto a = verts[cells[i][j][x]], b = verts[cells[i][j][y]];
					if (SphereDistance(a, b)< r) {
						lines.push_back(Line(cells[i][j][x], cells[i][j][y]));
						matrix[cells[i][j][x]].insert(cells[i][j][y]);
						matrix[cells[i][j][y]].insert(cells[i][j][x]);
					}
				}
			}
			std::vector<Line> tempLines;
			if (j + 1 < cells[i].size()) {
				//cells[i][j+1];
				tempLines = GenSphereLineBySets(cells[i][j], cells[i][j + 1], r, verts, matrix);
				lines.insert(lines.end(), tempLines.begin(), tempLines.end());
			}
			
			if (i + 1 < cells.size()) {
				for (int k = 0; k < cells[i + 1].size(); k++) {
					tempLines = GenSphereLineBySets(cells[i][j], cells[i+1][k], r, verts, matrix);
					lines.insert(lines.end(), tempLines.begin(), tempLines.end());
				}
			}
		}
	}
	//cout << " totoal= " << acc << endl;
}



DWORD FtoDw(float f){
	return *((DWORD*)&f);
}


//考虑一波球

//上色 可以检查一波