#include <iostream>
#include <vector>
#include <map>
#include <cstdio>
#include <cmath>

#define UNDEF -1

using namespace std;

enum Dir {UP, DOWN, LEFT, RIGHT, DIRS};
char dir_str[4][7] = {"up", "down", "left", "right"};

// first: value, second: energy
typedef pair<long, long> par;
typedef vector<vector<par>> matriz;

matriz DP;

vector<par> _ortho(DIRS);
vector<par> _delta(DIRS);
// lista de arboles a cortar, dado un arbol y una direccion
map<par, vector<int>> _cut_before;

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
vector<Tree> _trees;
vector<bool> _visited;

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
	cerr << depth << " " << t << " " << dir << endl;
	if (DP[dir][t].first != UNDEF) {
		return DP[dir][t];
	}
	vector<int> trees = may_be_dominoed(N, t, dir);
	cerr << "dominoables " << t << ":";
	for (auto _: trees) {
		cerr << _ << " ";
	}
	cerr << endl;

	if (trees.empty() or _visited[t]) {
		DP[dir][t] = {profit(t), cost(t)};
		return DP[dir][t];
	}
	_visited[t] = true;
	par ans = {UNDEF, UNDEF};
	for (auto tree: trees) {
		vector<int> between = trees_between(N, t, tree, dir);
		cerr << "between " << t << " and " << tree << ":";
		for (auto _: between) {
			cerr << _ << " ";
		}
		cerr << endl;
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
			cerr << "mejora con " << tree << endl;
			_cut_before[{t, dir}] = between;
			if (_cut_before.find({tree, dir}) != _cut_before.end()) {
				for (auto tt: _cut_before[{tree, dir}]) {
					_cut_before[{t, dir}].push_back(tt);
				}
			}
		}
	}
	_visited[t] = false;
	return DP[dir][t] = ans;
}



int main(int argc, char const *argv[]) {
	int E, N, T, x, y, h, d, c, p;
	scanf("%d %d %d", &E, &N, &T);

	init(N);
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
			par useless = dp(0, N - t - 1, t, i);
	}

	for (int i = 0; i < 4; i++){
		for (int t = 0; t < T; t++) {
			par p = DP[i][t];
			printf("%s %d: %ld %ld\n", dir_str[i], t, p.first, p.second);
		}
	}
	for (auto p: _cut_before) {
		int t = p.first.first;
		int dir = p.first.second;
		fprintf(stderr, "t = %d, dir = %s:", t, dir_str[dir]);
		for (auto q: p.second) {
			fprintf(stderr, " %d", q);
		}
		fprintf(stderr, "\n");
	}
	return 0;
}
