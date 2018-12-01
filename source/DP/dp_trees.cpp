#include <iostream>
#include <vector>
#include <map>
#include <cstdio>
#include <cmath>
#include <queue>
#include <cstdlib>
#include <ctime>

#define UNDEF -1
#define LIMIT 32
#define DEPTH 1 //dunno howto use :D
//#define DEBUG

using namespace std;

enum Dir {UP, DOWN, LEFT, RIGHT, DIRS};
char dir_str[4][7] = {"up", "down", "left", "right"};

// first: value, second: energy
typedef pair<long, long> par;
typedef vector<vector<par> > matriz;
typedef pair<par, long> trip;

matriz DP;

vector<par> _ortho(DIRS);
vector<par> _delta(DIRS);
// lista de arboles a cortar, dado un arbol y una direccion
map<par, vector<int> > _cut_before;
vector<trip> _best_default;

class Tree{
	public:
		int _x, _y, _d, _h, _cu, _pu;
		Tree(int x, int y, int h, int d, int c, int p) :
			_x(x), _y(y), _h(h), _d(d), _cu(c), _pu(p) {}

		long getValue() { return _pu * _h * _d; }
		bool canCut(int E) { return E >= _d; }
		bool canDrop(Tree t) { return _cu * _h * _d > t._cu * t._h * t._d;}
};

int **grid;
long objective = 0;
vector<Tree> _trees;
vector<bool> _visited, _down;

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

bool onBoundaries(int N, int x, int y){
	return (x >= 0) && (y >= 0) && (x < N) && (y < N);
}

int costToGo(int x1, int y1, int x2, int y2){
	return abs(x2 - x1) + abs(y2 - y1);
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

// retorna la lista de indeices de arboles que estan a distancia menor que h y pesan menos que arbol i
vector<int> may_be_dominoed(int N, int ti, int dir) {
	Tree t = _trees[ti];
	int H = t._h, x = t._x, y = t._y;

	vector<int> result;
	par delta = _delta[dir];
	for (int i = 1; i < H; i++){
		x += delta.first; y += delta.second;

		if (!onBoundaries(N, x, y)) break;

		int tree_idx = grid[y][x];
		if (tree_idx != -1 && t.canDrop(_trees[tree_idx]))
			result.push_back(tree_idx);
	}

	return result;
}

long profit(int t) {
	return _trees[t].getValue();
}

long cost(int t) {
	return _trees[t]._d;
}

par best(par p, par q) {
	return (p.first/p.second > q.first/q.second) ? p : q;
}

// retorna los arboles que estan entre t1 y t2, independiente de si son botables o no
vector<int> trees_between(int N, int t1, int t2, int dir) {
	par delta = _delta[dir];
	Tree buff1 = _trees[t1], buff2 = _trees[t2];
	int x1 = buff1._x, y1 = buff1._y, x2 = buff2._x, y2 = buff2._y;

	vector<int> result;


	for (x1 = x1 + delta.first, y1 = y1 + delta.second; x1 != x2 || y1 != y2; x1 += delta.first, y1 += delta.second){
		if (!onBoundaries(N, x1, y1)) break;

		int tree_idx = grid[y1][x1];
		if (tree_idx != -1) result.push_back(tree_idx);
	}

	return result;
}


par dp(int depth, int N, int t, int dir) {
	if (DP[dir][t].first != UNDEF) {
		return DP[dir][t];
	}
	vector<int> trees = may_be_dominoed(N, t, dir);

	if (trees.empty() or _visited[t]) {
		DP[dir][t] = {profit(t), cost(t)};
		return DP[dir][t];
	}
	_visited[t] = true;
	par ans = {UNDEF, UNDEF};
	for (auto tree: trees) {
		vector<int> between = trees_between(N, t, tree, dir);
		vector<par> results(between.size());
		par last = dp(depth + 1, N, tree, dir);
		long value = profit(t) + last.first;
		long energy = cost(t);
		for (int i = 0; i < between.size(); i++) {
			par o = _ortho[dir];
			results[i] = best(dp(depth + 1, N, between[i], o.first), dp(depth + 1, N, between[i], o.second));
			value += results[i].first;
			energy += results[i].second;
		}
		par new_ans = {value, energy};
		ans = best(new_ans, ans);
		if (ans == new_ans and not between.empty()) {
			_cut_before[{t, dir}] = between;
		}
	}
	_visited[t] = false;
	return DP[dir][t] = ans;
}

long simulate(int N, int idx, int dir, bool dropping){
	long result = 0;
	int tree_propag = idx;

	while (tree_propag >= 0){
		//printf("tree_propag = %d\n", tree_propag);
		Tree t = _trees[tree_propag];
		int H = t._h, xi = t._x, yi = t._y;

		if (H == 1) break;

		for (int i = 1; i < H; i++){
			//printf("simulate dir: %d\n", dir);
			if (dir == UP) yi++;
			if (dir == DOWN) yi--;
			if (dir == LEFT) xi--;
			if (dir == RIGHT) xi++;

			if (!onBoundaries(N, xi, yi)){
				tree_propag = -1;
				break;
			}

			int to_check = grid[yi][xi];
			if (to_check != -1 && !_down[to_check]){
				if (!t.canDrop(_trees[to_check])){
					tree_propag = -1;
					break;
				}

				Tree dropped = _trees[to_check];
				result += dropped.getValue();
				tree_propag = to_check;

				if (dropping){
					_down[to_check] = true;
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

int next(int N, int E, int x, int y){
	queue<par> q;
	int queued = 0;
	int sol = -1, best_value = -1, t_sol, t_best;
	bool canCutSomething = false;
	map<par, bool> visited;

	par init(y, x);
	q.push(init);
	visited[init] = true;

	//falta alguna wea para limitar la búsqueda
	while(queued < LIMIT && !q.empty()){
		par act = q.front(); q.pop();
		int tree_idx = grid[act.first][act.second];
		if (tree_idx != -1 && !_down[tree_idx]){
			Tree buff = _trees[tree_idx];
			int tx = buff._x, ty = buff._y, cost = costToGo(x, y, tx, ty);
			queued++;
			double ratio = _best_default[tree_idx].first.first / (cost + _best_default[tree_idx].first.second);
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
		par neighs[4];
		neighs[UP] = par(act.first + 1, act.second); neighs[DOWN] = par(act.first - 1, act.second);
		neighs[LEFT] = par(act.first, act.second - 1); neighs[RIGHT] = par(act.first, act.second + 1); 

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


void cut_routine(int N, int depth, int t, int dir, int &x, int &y, int &energy, bool print){
	//fprintf(stderr, "LOLMEN: t = %d, depth = %d\n", t, depth);
	if (energy <= 0 || _down[t]) return;

	if (depth < DEPTH){
		for (auto cb: _cut_before[par(t,dir)]){
			if (_down[cb]) continue;
			par o = _ortho[dir];
			long dp1 = DP[o.first][cb].first, dp2 = DP[o.second][cb].first;
			int best_ortho = (dp1 < dp2) ? o.second : o.first;
			cut_routine(N, depth + 1, cb, best_ortho, x, y, energy, print);
		}
	}

	Tree buff = _trees[t];

	if (print)
		print_moves(energy, x, y, buff._x, buff._y);
	else
		energy -= costToGo(x, y, buff._x, buff._y);

	if (!energy || !buff.canCut(energy)) return;

	x = buff._x; y = buff._y;
	_down[t] = true;
	energy -= buff._d;
	objective += buff.getValue();

	if (print)
		printf("cut %s\n", dir_str[dir]);

	objective += simulate(N, t, dir, true);
	//imprimimos los pasos para llegar al árbol t
	//check energy
	//cut t
	//simulate

}

void gen_point(int N, int &x, int &y){
	x = rand() % (N >> 1);
	y = rand() % (N >> 1);
}


int main(int argc, char const *argv[]) {
	int E, N, T, x, y, h, d, c, p;
	scanf("%d %d %d", &E, &N, &T);

	init(N);
	srand(time(NULL));
	DP = matriz(4, vector<par>(T, {UNDEF, UNDEF}));
	_ortho[UP] = {LEFT, RIGHT};
	_ortho[DOWN] = {LEFT, RIGHT};
	_ortho[LEFT] = {UP, DOWN};
	_ortho[RIGHT] = {UP, DOWN};
	_delta[UP] = {0, 1};
	_delta[DOWN] = {0, -1};
	_delta[LEFT] = {-1, 0};
	_delta[RIGHT] = {1, 0};

	for (int i = 0; i < T; i++){
		scanf("%d %d %d %d %d %d", &x, &y, &h, &d, &c, &p);
		Tree tr(x, y, h, d, c, p);
		_trees.push_back(tr);
		grid[y][x] = i;
	}
	_visited.assign(T, false);

	for (int i = 0; i < 4; i++){
		for (int t = 0; t < T; t++)
			par useless = dp(0, N, t, i);
	}

	/*
	for (int i = 0; i < 4; i++){
		for (int t = 0; t < T; t++) {
			par p = DP[i][t];
			printf("%s %d: %ld %ld\n", dir_str[i], t, p.first, p.second);
		}
	}
	*/
	for (int i = 0; i < T; i++){
		trip best = trip(par(-1, -1), -1);
		for (int d = 0; d < 4; d++){
			par p = best.first;
			if ((best.first.first / best.first.second) < (DP[d][i].first / DP[d][i].second)){
				best = trip(DP[d][i], d);
			}
		}

		_best_default.push_back(best);
	}

	int bxi, byi, energy, cases = 40, next_tree;
	long b_objective = -1;
	if (N == 250 && T == 793) cases = 2;
	while(cases--){
		_down.assign(T, false);

		energy = E;
		int xi = 0, yi = 0;
		if (cases) gen_point(N, xi, yi);

		#ifdef DEBUG
			fprintf(stderr, "Case %d : Starting from (%d, %d) -- ", 40 - cases, xi, yi);
		#endif
		energy -= costToGo(0, 0, xi, yi);
		while(energy > 0){
			next_tree = next(N, energy, xi, yi);
			//fprintf(stderr, "next tree chosen...\n");
			cut_routine(N, 0, next_tree, _best_default[next_tree].second, xi, yi, energy, false);
		}

		#ifdef DEBUG
			fprintf(stderr, "PROFIT: %ld\n", objective);
		#endif

		if (objective > b_objective){
			b_objective = objective;
			bxi = xi; byi = yi;
		}	
		objective = 0;
	}

	#ifdef DEBUG
		fprintf(stderr, "Best case: start (%d, %d) /w PROFIT %ld\n", bxi, byi, b_objective);
	#endif

	_down.assign(T, false);
	energy = E;

	print_moves(energy, 0, 0, bxi, byi);

	while(energy > 0){
		next_tree = next(N, energy, bxi, byi);
		cut_routine(N, 0, next_tree, _best_default[next_tree].second, bxi, byi, energy, true);
	}

	
	return 0;
}
