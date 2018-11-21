#include <iostream>
#include <vector>
#include <map>

#define UNDEF -1

using namespace std;

enum Dir {UP, DOWN, LEFT, RIGHT};

// first: value, second: energy
typedef pair<long, long> par;
typedef vector<vector<par>> matriz;

matriz DP;

map<int, pair<int, int>> ortho = {
	{UP, {LEFT, RIGHT}},
	{DOWN, {LEFT, RIGHT}},
	{LEFT, {UP, DOWN}},
	{RIGHT, {UP, DOWN}}
};

// retorna la lista de indeices de arboles que estan a distancia menor que h y pesan menos que arbol i
vector<int> may_be_dominoed(int i, int dir) {
	// not implemented
	return {};
}

long profit(int t) {
	// not implemented
	return 0;
}

long cost(int t) {
	// not implemented
	return 0;
}

par best(par p, par q) {
	// not implemented
	return {0, 0};
}

// retorna los arboles que estan entre t1 y t2, independiente de si son botables o no
vector<int> trees_between(int t1, int t2) {
	// not implemented
	return {};
}

par dp(int t, int dir) {
	if (DP[dir][t].first != UNDEF) {
		return DP[dir][t];
	}
	vector<int> trees = may_be_dominoed(t, dir);
	if (trees.empty()) {
		return DP[dir][t] = {profit(t), cost(t)};
	}
	par ans = {UNDEF, UNDEF};
	for (auto tree: trees) {
		vector<int> between = trees_between(t, tree);
		vector<par> results(between.size());
		par last = dp(tree, dir);
		long value = profit(t) + last.first;
		long energy = cost(t);
		for (int i = 0; i < between.size(); i++) {
			par o = ortho[dir];
			results[i] = best(dp(between[i], o.first), dp(between[i], o.second));
			value += results[i].first;
			energy += results[i].second;
		}
		ans = best({value, energy}, ans);

	}
	return DP[dir][t] = ans;
}

int main(int argc, char const *argv[]) {
	vector<vector<int>> M(10, vector<int>(10));
	return 0;
}
