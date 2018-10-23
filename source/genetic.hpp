#pragma once
#include <vector>
#include <cmath>
#include <map>
#include "chromosome.hpp"
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
using namespace std;

struct Tree{
	int _x, _y, _h, _d, _c, _p;
	Tree(x, y, h, d, c, p) { _x(x); _y(y); _h(h); _d(d); _c(c); _p(p); }
}

class Info{
	public:
		vector<Tree> tr;
		vector<vector<int> > adj[4];

		int steps;
		Info(vector<Tree> t, int s){
			tr = vector<Tree>(t);
			steps = s;
			this->build();
		}
		double objective(Chromosome c);
		double cut(int i, int dir, vector<bool> &down);
		void print(Chromosome c);
		void build();
}

void build(){
	int N = this->tr.size();

	//decidí usar map pa ke kede T log T
}

double Info::objective(Chromosome c){
	int N = c.getLength();
	double fx = 0.0f;
	vector<bool> down(N, false);

	int x = 0, y = 0, t_steps = this->steps;

	for (int i = 0; i < N && t_steps > 0; i++){
		ii act = c.at(i);
		if (down[act.first]) continue;

		Tree t = this->tr[act.first];
		t_steps -= (abs(x - t._x) + abs(y - t._y));

		if (t_steps < t._d) break;

		x = t._x; y = t._y;
		fx += this->cut(act.first, act.second, down);
		t_steps -= t._d;
	}

	return fx;
}

double Info::cut(int i, int dir, vector<bool> &down){
	Tree t = this->tr[i], buff;
	int tree_i = i;
	double val = t._h * t._p * t._d;

	while(adj[dir][tree_i].size()){
		bool found = false;
		for (int k = 0; k < adj[dir][i].size(); k++){
			int tree_k = adj[dir][i][k]; 
			if (down[tree_k]) continue;
				
			found = true;
			buff = this->tr[tree_k];
			val += buff._h * buff._p * buff._d;
			down[tree_k] = true;
			tree_i = tree_k;
			break;
		}

		if (!found) break;
	}

	return val;
}

class Genetic{
	private:
		vector<Chromosome> _pop;
		int _pop_size;
		double _side_mutation_ratio, _order_mutation_ratio, _cross_ratio;

	public:
		Genetic();
}