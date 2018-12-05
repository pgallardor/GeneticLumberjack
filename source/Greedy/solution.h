#pragma once 
#include <utility>
#include <algorithm>
#include <vector>
#include <utility>
#define DELTA 0.8

using namespace std;

typedef pair<int, int> ii;
//tuple contains (ratio, (tree_index, dir))
typedef pair<double, ii> tup;


void print_moves(int &E, int x1, int y1, int x2, int y2){
	if (x1 < x2){
		while (E && x1++ < x2){
			printf("move right\n");
			E--;
		}
	}

	else if (x1 > x2){
		while (E && x1-- > x2){
			printf("move left\n");
			E--;
		}
	}

	if (!E) return;

	if (y1 < y2){
		while (E && y1++ < y2){
			printf("move up\n");
			E--;
		}
	}

	else if (y1 > y2){
		while(E && y1-- > y2){
			printf("move down\n");
			E--;
		}
	}
}


class Tree{
	public:
		int _x, _y, _d, _h, _cu, _pu;
		Tree(int x, int y, int h, int d, int c, int p) : 
			_x(x), _y(y), _h(h), _d(d), _cu(c), _pu(p) {}
		
		double getValue() { return _pu * _h * _d; }
		bool canCut(int E) { return E >= _d; }
		bool canDrop(Tree t) { return _cu * _h * _d > t._cu * t._h * t._d;}
};

vector<ii> value;
vector<Tree> list;
vector<bool> down;
double total = 0.0f;
int **grid;


bool onBoundaries(int N, int x, int y){
	return (x >= 0) && (y >= 0) && (x < N) && (y < N);
}

int costToGo(int x1, int y1, int x2, int y2){
	return abs(x2 - x1) + abs(y2 - y1);
}

class Solution {
	private:
		vector<tup> _sol_trees, _worst;
		ii _start_point;
		double _worst_ratio;
		tup explore_vert(int N, map<int, bool> &p, int V, int x, int y);
		tup explore_hor(int N, map<int, bool> &p, int V, int x, int y);
	public:
		Solution(vector<tup> st, int x, int y);
		void restart();
		void improve(int N, int E, int V);
		void simulate(int E);
		void print();
};

Solution::Solution(vector<tup> st, int x, int y){
	this->_sol_trees = vector<tup>(st);
	this->_start_point = ii(x, y);
	double _mean = 0.0f;
	for(auto t: _sol_trees){
		_mean += t.first;
	}

	_mean /= double(_sol_trees.size());

	for(auto t: _sol_trees){
		if (double(t.first) < _mean){
			_worst.push_back(t);
		}
	}

	sort(_worst.begin(), _worst.end());
	_worst_ratio = _worst[0].first;
}

void Solution::print(){
	for (auto t : _sol_trees){
		ii m = t.second;

		printf("tree #%d (dropped %d): %.2f points per energy unit.\n", m.first, m.second, t.first);
	}

	printf("Worst trees:\n");
	for (auto wt: _worst){
		ii m = wt.second;

		printf("tree #%d (dropped %d): %.2f points per energy unit.\n", m.first, m.second, wt.first);
	}
}

void Solution::restart(){
	this->_sol_trees.clear();
	this->_worst.clear();
}


tup Solution::explore_hor(int N, map<int, bool> &p, int V, int x, int y){
	
	tup best = tup(-1.0f, ii(-1, -1));
	double ratio;
	for (int i = 0; i < V; i++){
		if (onBoundaries(N, x + i, y)){
			int ti = grid[y][x + i];
			if (ti != -1 && !p.count(ti)){
				Tree t = list[ti]; 
				ratio = double(value[ti].first) / (i + t._d);

				if (ratio > best.first){
					best = tup(ratio, ii(ti, value[ti].second));
				}
			}
		}
		if (i == 0) continue;

		if (onBoundaries(N, x - i, y)){
			int ti = grid[y][x - i];
			if (ti != -1 && !p.count(ti)){
				Tree t = list[ti]; 
				ratio = double(value[ti].first) / (i + t._d);

				if (ratio > best.first){
					best = tup(ratio, ii(ti, value[ti].second));
				}
			}
		}
	}

	return best;
}

tup Solution::explore_vert(int N, map<int, bool> &p, int V, int x, int y){
	
	tup best = tup(-1.0f, ii(-1, -1));
	double ratio;
	for (int i = 0; i < V; i++){
		if (onBoundaries(N, x, y + i)){
			int ti = grid[y + i][x];
			if (ti != -1 && !p.count(ti)){
				Tree t = list[ti]; 
				ratio = double(value[ti].first) / (i + t._d);

				if (ratio > best.first){
					best = tup(ratio, ii(ti, value[ti].second));
				}
			}
		}
		if (i == 0) continue;

		if (onBoundaries(N, x, y - i)){
			int ti = grid[y - i][x];
			if (ti != -1 && !p.count(ti)){
				Tree t = list[ti]; 
				ratio = double(value[ti].first) / (i + t._d);

				if (ratio > best.first){
					best = tup(ratio, ii(ti, value[ti].second));
				}
			}
		}
	}
	return best;
}


void Solution::improve(int N, int E, int V){
	int sx = _start_point.first, sy = _start_point.second;
	int x = 0, y = 0;
	vector<tup> to_replace, new_sol;
	map<int, bool> picked, replaced;

	while(x < sx && y < sy){
		x++;
		auto nt = this->explore_vert(N, picked, V, x, y);
		if (nt.first > _worst_ratio){
			to_replace.push_back(nt);
			picked[nt.second.first] = true;
		}
		y++;
		nt = this->explore_hor(N, picked, V, x, y);

		if (nt.first > _worst_ratio){
			to_replace.push_back(nt);
			picked[nt.second.first] = true;
		}
	}

	while(x < sx){
		x++;
		auto nt = this->explore_vert(N, picked, V, x, y);
		if (nt.first > _worst_ratio){
			to_replace.push_back(nt);
			picked[nt.second.first] = true;
		}
	}

	while(y < sy){
		y++;
		auto nt = this->explore_vert(N, picked, V, x, y);
		if (nt.first > _worst_ratio){
			to_replace.push_back(nt);
			picked[nt.second.first] = true;
		}
	}

	sort(to_replace.begin(), to_replace.end());

	printf("To replace:\n");
	for (auto tr: to_replace){
		ii m = tr.second;
		printf("tree #%d (dropped %d): %.2f points per energy unit.\n", m.first, m.second, tr.first);
	}

	//pick first from to replace that is better than the best worst
	
	int i = 0, j = to_replace.size() - 1;
	
	while(_worst[i] < to_replace[j] && i < _worst.size() && j < to_replace.size()){
		new_sol.push_back(to_replace[j]);
		replaced[_worst[i].second.first] = true;

		i++; j--;
	}

	for (auto t: _sol_trees){
		if (replaced.count(t.second.first)) continue;
		new_sol.push_back(t);
	}

	_sol_trees.clear();
	_sol_trees = vector<tup>(new_sol);
	//emplace them at the beginning of the sol array
	printf("new sol\n");
	for (auto t : _sol_trees){
		ii m = t.second;

		printf("tree #%d (dropped %d): %.2f points per energy unit.\n", m.first, m.second, t.first);
	}

}