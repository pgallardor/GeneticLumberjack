#include <iostream>
#include <vector>
#include <string>
#include <map>

#define UNDEF -1

using namespace std;

typedef pair<long, long> par;
typedef vector<vector<par> > matrix;
typedef pair<par, long> trip;

enum Action {
	MOVE_UP, MOVE_LEFT, MOVE_DOWN, MOVE_RIGHT,
	CUT_UP, CUT_LEFT, CUT_DOWN, CUT_RIGHT
};

// Direcciones. UP=0, ... RIGHT=3
enum Dir {UP, DOWN, LEFT, RIGHT};

// variaciones en las coordenadas actuales cuando se quiere mover a en alguna dirección
vector<par> delta = {
	{0, 1}, // UP
	{0, -1}, // DOWN
	{-1, 0}, // LEFT
	{1, 0}, // RIGHT
};

// entrega par con direcciones ortogonales a la direccion recibida.
// ej: orthogonal(UP) -> {LEFT, RIGHT}
par orthogonal(int dir) {
	return {(dir + 1)%4, (dir + 3)%4};
}

// entrega el mejor de los pares recibidos, bajo algun criterio
par best(par p, par q) {
	return (p.first/p.second > q.first/q.second) ? p : q;
}

class Tree{
public:
	int x, y, d, h, c, p;
	Tree(int x, int y, int h, int d, int c, int p) :
		x(x), y(y), h(h), d(d), c(c), p(p) {}
	long getValue() {
		return p * h * d;
	}
	bool canCut(int E) {
		return E >= d;
	}
	long getWeight() {
		return c * h * d;
	}
	long getCost() {
		return d;
	}
	bool canDrop(Tree t) {
		return this->getWeight() > t.getWeight();
	}
};

class Grid {
private:
	vector<vector<int>> grid;
public:
	Grid (size_t n) {
		grid = vector<vector<int>>(n, vector<int>(n, UNDEF));
	}
	bool onBoundaries(int x, int y) {
		return (x >= 0) && (y >= 0) && (x < grid.size()) && (y < grid.size());
	}
	int at(int x, int y) {
		return grid[x][y];
	}
	void reset() {
		for (size_t i = 0; i < grid.size(); i++) {
			for (size_t j = 0; j < grid.size(); j++) {
				set(i, j, UNDEF);
			}
		}
	}
	size_t size() {
		return grid.size();
	}
	void set(int x, int y, int index) {
		grid[x][y] = index;
	}

	void print() {
		for (size_t i = 0; i < grid.size(); i++) {
			for (size_t j = 0; j < grid.size(); j++) {
				cerr << grid[i][j] << " ";
			}
			cerr << endl;
		}
	}
};

class Solution {
private:
	vector<int> solution;
	static const vector<string> str;
public:
	Solution () {
	}
	void clear() {
		solution.clear();
	}
	void add(int action) {
		solution.push_back(action);
	}
	void print() {
		for (auto action: solution) {
			cerr << Solution::str[action] << endl;
		}
	}
};

// tiene que tener el mismo orden que el Enum
const vector<string> Solution::str = {
	"move up", "move left", "move down", "move right",
	"cut up", "cut left", "cut down", "cut right"
};

class Forest {
private:
	vector<Tree> trees;
	Grid* grid;
public:
	Forest (Grid* grid) {
		this->grid = grid;
		trees = vector<Tree>();
	}
	void add(Tree t) {
		grid->set(t.x, t.y, trees.size());
		trees.push_back(t);
	}
	Tree getTree(int i) {
		return trees[i];
	}
	size_t size() {
		return trees.size();
	}
	vector<int> dominoables(int dir, int tree) {
		Tree t = trees[tree];
		int H = t.h;
		vector<int> result;
		int dx = delta[dir].first;
		int dy = delta[dir].second;
		int x = t.x, y = t.y;
		for (int h = 1; h < H; h++){
			x += dx; y += dy;
			if (not grid->onBoundaries(x, y)) {
				break;
			}
			int tree_idx = grid->at(x, y);
			if (tree_idx != UNDEF and t.canDrop(trees[tree_idx])) {
				result.push_back(tree_idx);
			}
		}
		return result;
	}
	vector<int> between(int tree1, int tree2, int dir) {
		par dxy = delta[dir];
		int dx = dxy.first;
		int dy = dxy.second;
		Tree buff1 = trees[tree1];
		Tree buff2 = trees[tree2];
		int x_i = buff1.x + dx;
		int y_i = buff1.y + dy;
		int x_f = buff2.x;
		int y_f = buff2.y;
		vector<int> result;
		for ( ; x_i != x_f or y_i != y_f; x_i += dx, y_i += dy){
			if (not grid->onBoundaries(x_i, y_i)) {
				 break;
			}
			int tree_idx = grid->at(x_i, y_i);
			if (tree_idx != UNDEF) {
				result.push_back(tree_idx);
			}
		}

		return result;
	}
};

class Estimate {
private:
	matrix DP;
	int grid_size;
	vector<bool> visited;
	Forest* forest;
	map<par, vector<int>> cut_before;
public:
	Estimate (Forest* forest, int grid_size) {
		this->grid_size = grid_size;
		this->visited.assign(forest->size(), false);
		this->forest = forest;
		DP = matrix(4, vector<par>(forest->size(), {UNDEF, UNDEF}));

		for (int dir = 0; dir < 4; dir++) {
			for (int t = 0; t < forest->size(); t++) {
				DP[dir][t] = dp(0, dir, t);
			}
		}
	}

	par dp(int depth, int dir, int t) {
		Tree tree = this->forest->getTree(t);
		if (DP[dir][t].first != UNDEF) {
			return DP[dir][t];
		}
		vector<int> dominoable = forest->dominoables(dir, t);
		if (dominoable.empty() or this->visited[t]) {
			DP[dir][t] = {tree.getValue(), tree.getCost()};
			return DP[dir][t];
		}
		visited[t] = true;
		par ans = {0, UNDEF};
		for (auto tt: dominoable) {
			vector<int> between = forest->between(t, tt, dir);
			vector<par> results(between.size());
			par last = dp(depth + 1, dir, tt);
			long value = forest->getTree(t).getValue() + last.first;
			long energy = forest->getTree(t).getCost();
			for (int i = 0; i < between.size(); i++) {
				par o = orthogonal(dir);
				results[i] = best(dp(depth + 1, o.first, between[i]), dp(depth + 1, o.second, between[i]));
				value += results[i].first;
				energy += results[i].second;
			}
			par new_ans = {value, energy};
			ans = best(new_ans, ans);
			if (ans == new_ans and not between.empty()) {
				cut_before[{t, dir}] = between;
			}
		}
		this->visited[t] = false;
		return DP[dir][t] = ans;
	}
	void print_all() {
		for (int dir = 0; dir < 4; dir++) {
			for (int t = 0; t < forest->size(); t++) {
				print(dir, t);
			}
		}
	}
	void print(int dir, int tree) {
		par p = DP[dir][tree];
		cerr << "dir: " << dir << ", tree: " << tree;
		cerr << ", profit: " << p.first << ", energy: " << p.second << endl;
	}
};


int main(int argc, char const *argv[]) {
	int E, N, T;
	cin >> E >> N >> T;
	Grid grid(N);
	Forest forest(&grid);
	for (size_t i = 0; i < T; i++) {
		int x, y, h, d, c, p;
		cin >> x >> y >> h >> d >> c >> p;
		Tree t(x, y, h, d, c, p);
		forest.add(t);
	}
	Estimate estimate(&forest, N);
	estimate.print_all();
	return 0;
}
