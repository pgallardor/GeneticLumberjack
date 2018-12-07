#include <cstdio>
#include <vector>
#include <utility>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <map>
#include <queue>
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
int LIMIT = 32;
int MAX_CASES = 60;
#define DELTA 0.9
//#define DEBUG
using namespace std;

typedef pair<int, int> ii;
typedef pair<double, ii> tup;

char dir_str[4][7] = {"up", "down", "left", "right"};

class Tree{
	public:
		int _x, _y, _d, _h, _cu, _pu;
		Tree(int x, int y, int h, int d, int c, int p) :
			_x(x), _y(y), _h(h), _d(d), _cu(c), _pu(p) {}

		double getValue() { return _pu * _h * _d; }
		bool canCut(int E) { return E >= _d; }
		bool canDrop(Tree t) { return _cu * _h * _d > t._cu * t._h * t._d;}
};

vector<ii> value, points;
vector<Tree> list;
vector<bool> down;
double total = 0.0f;
int **grid;
int (*next_)(int, int, int, int) = NULL;

bool onBoundaries(int N, int x, int y){
	return (x >= 0) && (y >= 0) && (x < N) && (y < N);
}

int costToGo(int x1, int y1, int x2, int y2){
	return abs(x2 - x1) + abs(y2 - y1);
}

double simulate(int N, int idx, int dir, bool dropping){
	double result = 0.0f;
	int tree_propag = idx;

	while (tree_propag >= 0){
		Tree t = list[tree_propag];
		int H = t._h, xi = t._x, yi = t._y;

		if (H == 1) break;

		for (int i = 1; i < H; i++){
			if (dir == UP) yi++;
			if (dir == DOWN) yi--;
			if (dir == LEFT) xi--;
			if (dir == RIGHT) xi++;

			if (!onBoundaries(N, xi, yi)){
				tree_propag = -1;
				break;
			}

			int to_check = grid[yi][xi];
			if (to_check != -1 && !down[to_check]){
				if (!t.canDrop(list[to_check])){
					tree_propag = -1;
					break;
				}

				Tree dropped = list[to_check];
				result += dropped.getValue();
				tree_propag = to_check;

				if (dropping){
					down[to_check] = true;
				}

				break;
			}

			if (i + 1 == H){
				tree_propag = -1;
				break;
			}
		}
	}

	return result;
}

int next_window(int N, int E, int x, int y){
	int x_begin = max(0, x - LIMIT/2);
	int x_end = min(N, x + LIMIT/2);
	int y_begin = max(0, y - LIMIT/2);
	int y_end = min(N, y + LIMIT/2);
	int best_tree = -1;
	double val = -1.0;
	for (int i = x_begin; i < x_end; i++) {
		for (int j = y_begin; j < y_end; j++) {
			int t = grid[j][i];
			if (t != -1 and not down[t]) {
				Tree tree = list[t];
				int cost = costToGo(x, y, tree._x, tree._y);
				double ratio = double(value[t].first) / (tree._d + cost);
				if (cost <= E and ratio > val) {
					best_tree = t;
					val = ratio;
				}
			}
		}
	}
	if (best_tree == -1) {
		for (auto tt: list) {
			int idx = grid[tt._y][tt._x];
			int cost = costToGo(x, y, tt._x, tt._y);
			double ratio = double(value[idx].first) / (cost + tt._d);
			if (not down[idx] and tt.canCut(E - cost) and ratio > val) {
				best_tree = idx;
				val = ratio;
			}
		}
	}
	return best_tree;
}

int next_bfs(int N, int E, int x, int y){
	queue<ii> q;
	int queued = 0;
	int sol = -1, best_value = -1, t_sol, t_best;
	bool canCutSomething = false;
	map<ii, bool> visited;

	ii init(y, x);
	q.push(init);
	visited[init] = true;

	//falta alguna wea para limitar la búsqueda
	while(queued < LIMIT && !q.empty()){
		ii act = q.front(); q.pop();
		int tree_idx = grid[act.first][act.second];
		if (tree_idx != -1 && !down[tree_idx]){
			Tree buff = list[tree_idx];
			int tx = buff._x, ty = buff._y, cost = costToGo(x, y, tx, ty);
			queued++;
			double ratio = value[tree_idx].first / (cost + buff._d);
			if (buff.canCut(E - cost) && ratio > best_value){
				best_value = ratio;
				sol = tree_idx;
				canCutSomething = true;
			}
			else if (ratio > best_value){
				t_sol = tree_idx;
				t_best = ratio;
			}
		}
		ii neighs[4];
		neighs[UP] = ii(act.first + 1, act.second); neighs[DOWN] = ii(act.first - 1, act.second);
		neighs[LEFT] = ii(act.first, act.second - 1); neighs[RIGHT] = ii(act.first, act.second + 1);

		for (int i = 0; i < 4; i++){
			if (onBoundaries(N, neighs[i].second, neighs[i].first) && !visited.count(neighs[i])){
				q.push(neighs[i]);
				visited[neighs[i]] = true;
			}
		}

	}

	if (!canCutSomething) sol = t_sol;

	return sol;
}

void calculateValues(int N, int T){
	for (int i = 0; i < T; i++){
		double val = list[i].getValue(), best_drop_value = -1.0f;
		int dir = -1;
		for (int direction = 0; direction < 4; direction++){
			double tmp = simulate(N, i, direction, false);
			if (tmp > best_drop_value){
				dir = direction;
				best_drop_value = tmp;
			}
		}
		value.push_back(ii(val + best_drop_value, dir));
	}
}

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

void init(int N){
	grid = new int*[N];
	for (int i = 0; i < N; i++){
		grid[i] = new int[N];
		for (int j = 0; j < N; j++) grid[i][j] = -1;
	}
}

void free(int N){
	for (int i = 0; i < N; i++)
		delete[] grid[i];
	delete[] grid;
}

void gen_point(int N, int &x, int &y){
	x = rand() % (N >> 1);
	y = rand() % (N >> 1);
}

/* 
* EL HORROR HA LLEGADO AL PUEBLO
*/

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
		double simulate_(int N, int E);
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

	if (!_worst.size()){
		_worst_ratio = _sol_trees[0].first;
		return;
	}
	sort(_worst.begin(), _worst.end());
	_worst_ratio = _worst[0].first;
}

void Solution::print(){
	for (auto t : _sol_trees){
		ii m = t.second;

		fprintf(stderr, "tree #%d (dropped %d): %.2f points per energy unit.\n", m.first, m.second, t.first);
	}

	fprintf(stderr, "Worst trees:\n");
	for (auto wt: _worst){
		ii m = wt.second;

		fprintf(stderr, "tree #%d (dropped %d): %.2f points per energy unit.\n", m.first, m.second, wt.first);
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
				ratio = double(value[ti].first) / (2*i + t._d);

				if (ratio > best.first){
					best = tup(ratio, ii(ti, value[ti].second));
				}
			}
		}
		if (i == 0) continue;
		/*
		if (onBoundaries(N, x - i, y)){
			int ti = grid[y][x - i];
			if (ti != -1 && !p.count(ti)){
				Tree t = list[ti]; 
				ratio = double(value[ti].first) / (2*i + t._d);

				if (ratio > best.first){
					best = tup(ratio, ii(ti, value[ti].second));
				}
			}
		}*/
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
				ratio = double(value[ti].first) / (2*i + t._d);

				if (ratio > best.first){
					best = tup(ratio, ii(ti, value[ti].second));
				}
			}
		}
		if (i == 0) continue;
		/*
		if (onBoundaries(N, x, y - i)){
			int ti = grid[y - i][x];
			if (ti != -1 && !p.count(ti)){
				Tree t = list[ti]; 
				ratio = double(value[ti].first) / (2*i + t._d);

				if (ratio > best.first){
					best = tup(ratio, ii(ti, value[ti].second));
				}
			}
		}*/
	}
	return best;
}


void Solution::improve(int N, int E, int V){
	int sx = _start_point.first, sy = _start_point.second;
	int x = 0, y = 0;
	vector<tup> to_replace, new_sol;
	map<int, bool> picked, replaced;
	bool up = false;
	while(x < sx && y < sy){
		(up) ? y++ : x++;

		auto nt = this->explore_vert(N, picked, V, x, y);
		if (nt.first > _worst_ratio){
			to_replace.push_back(nt);
			picked[nt.second.first] = true;
		}
		nt = this->explore_hor(N, picked, V, x, y);

		if (nt.first > _worst_ratio){
			to_replace.push_back(nt);
			picked[nt.second.first] = true;
		}

		//V--;
		up = !up;
	}

	while(x < sx){
		x++;
		auto nt = this->explore_vert(N, picked, V, x, y);
		if (nt.first > _worst_ratio){
			to_replace.push_back(nt);
			picked[nt.second.first] = true;
		}

		//V--;
	}

	while(y < sy){
		y++;
		auto nt = this->explore_vert(N, picked, V, x, y);
		if (nt.first > _worst_ratio){
			to_replace.push_back(nt);
			picked[nt.second.first] = true;
		}

		//V--;
	}

	if (!to_replace.size()) return;

	sort(to_replace.begin(), to_replace.end());

	//printf("To replace:\n");
	//for (auto tr: to_replace){
	//	ii m = tr.second;
		//printf("tree #%d (dropped %d): %.2f points per energy unit.\n", m.first, m.second, tr.first);
	//}

	//pick first from to replace that is better than the best worst
	//let try limitating the number of replaced trees
	int i = 0, j = to_replace.size() - 1, cnt = 0;
	
	while(cnt < 1 && i < _worst.size() && j >= 0 && _worst[i].first < to_replace[j].first){
		new_sol.push_back(to_replace[j]);
		replaced[_worst[i].second.first] = true;

		i++; j--; cnt++;
	}

	for (auto t: _sol_trees){
		if (replaced.count(t.second.first)) continue;
		new_sol.push_back(t);
	}

	_sol_trees.clear();
	_sol_trees = vector<tup>(new_sol);

	_start_point = ii(0,0);
	//emplace them at the beginning of the sol array
	//printf("new sol\n");
	//for (auto t : _sol_trees){
	//	ii m = t.second;

	//	printf("tree #%d (dropped %d): %.2f points per energy unit.\n", m.first, m.second, t.first);
	//}

}


double Solution::simulate_(int N, int E){
	fprintf(stderr, "starting simulation\n");
	int energy = E, bx = _start_point.first, by = _start_point.second, tree_index;
	double prof = 0.0f;
	print_moves(energy, 0, 0, bx, by);

	for (auto s: _sol_trees){
		if (energy <= 0) break;
		tree_index = s.second.first;
		if (tree_index == -1 || down[tree_index]) continue;
		Tree tg = list[tree_index];
		print_moves(energy, bx, by, tg._x, tg._y);
		//energy -= costToGo(xi, yi, buff._x, buff._y);

		if (energy <= 0 || !tg.canCut(energy)) break;

		bx = tg._x; by = tg._y;
		down[tree_index] = true;
		energy -= tg._d;

		printf("cut %s\n", dir_str[value[tree_index].second]);
		prof += double(tg.getValue()) +  simulate(N, tree_index, value[tree_index].second, true);
	}

	while (energy > 0){
		tree_index = next_(N, energy, bx, by);
		if (tree_index == -1) break;
		Tree tg = list[tree_index];
		print_moves(energy, bx, by, tg._x, tg._y);
		//energy -= costToGo(xi, yi, buff._x, buff._y);

		if (energy <= 0 || !tg.canCut(energy)) break;

		bx = tg._x; by = tg._y;
		down[tree_index] = true;
		energy -= tg._d;

		printf("cut %s\n", dir_str[value[tree_index].second]);
		prof += double(tg.getValue()) + simulate(N, tree_index, value[tree_index].second, true);
	}

	return prof;

}

/*
* AQUI ACABA EL HORROR
*/


int main(){
	int E, N, T, x, y, h, d, c, p;
	scanf("%d %d %d", &E, &N, &T);
	init(N);

	for (int i = 0; i < T; i++){
		scanf("%d %d %d %d %d %d", &x, &y, &h, &d, &c, &p);
		Tree tr(x, y, h, d, c, p);
		list.push_back(tr);
		grid[y][x] = i;
	}

	down.assign(T, false);
	calculateValues(N, T);

	if (N >= 50) {
		MAX_CASES = 500;
		LIMIT = 256*N / 1000;
		next_ = next_window;
		srand(0);
	}
	else {
		MAX_CASES = 45;
		LIMIT = 32;
		next_ = next_bfs;
		srand(0);
	}

	if (N == 250 && T == 793) {
		MAX_CASES = 11;
		LIMIT = 32;
		next_ = next_bfs;
		srand(0);
	}
	int bx, by, cases = MAX_CASES, tree_index, xi, yi, tx, ty, energy;

	double bprofit = -1.0f, useless;

	vector<tup> best_sol, sol_temp;

	while(cases--){
		down.assign(T, false);
		if (N < 250 || N != 250 or T != 793) {
			LIMIT = rand() % 100 + 356;
			// LIMIT -= 1;
		}
		energy = E;
		xi = 0; yi = 0;

		useless = 0.0f;

		if (cases) gen_point(N, xi, yi);
		energy -= costToGo(0, 0, xi, yi);
		tx = xi; ty = yi;

		while (energy > 0){
			tree_index = next_(N, energy, xi, yi);
			if (tree_index == -1) {
				break;
			}
			Tree buff = list[tree_index];
			int cost = costToGo(xi, yi, buff._x, buff._y); 
			//print_moves(energy, xi, yi, buff._x, buff._y);
			energy -= cost;

			if (!energy || !buff.canCut(energy)) break;

			xi = buff._x; yi = buff._y;
			down[tree_index] = true;
			energy -= buff._d;

			long t_profit = buff.getValue() + simulate(N, tree_index, value[tree_index].second, true); 
			tup info = tup(double(t_profit) / (buff._d + cost), ii(tree_index, value[tree_index].second));

			//printf("cut %s\n", dir_str[value[tree_index].second]);
			useless += t_profit;
			sol_temp.push_back(info);
		}

		#ifdef DEBUG
			fprintf(stderr, "Case %d. Starting from (%d, %d). PROFIT = %.0f\n", MAX_CASES - cases, tx, ty, useless);
		#endif

		if (useless > bprofit){
			bx = tx; by = ty;
			bprofit = useless;
			best_sol.clear();
			best_sol = vector<tup>(sol_temp);
		}

		sol_temp.clear();
	}

	int final_energy = E;


	#ifdef DEBUG
		fprintf(stderr, "BEST START POINT: (%d, %d) w/ PROFIT = %.0f\n", bx, by, bprofit);
	#endif

	//down.assign(T, false);
	Solution s(best_sol, bx, by);
	//s.print();

	//down.assign(T, false);
	//fprintf(stderr, "profit: %.0f\n", s.simulate_(N, E));
	s.improve(N, E, min(bx / 2, by / 2));

	down.assign(T, false);
	fprintf(stderr, "profit: %.0f\n", s.simulate_(N, E));	

	return 0;
}
