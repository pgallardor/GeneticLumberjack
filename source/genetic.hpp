#pragma once
#include <vector>
#include "chromosome.hpp"
using namespace std;

struct Tree{
	int _x, _y, _h, _d, _c, _p;
	Tree(x, y, h, d, c, p) { _x(x); _y(y); _h(h); _d(d); _c(c); _p(p); }
}

class Info{
	public:
		vector<Tree> tr;
		vector<vector<int> > adj;

		int steps;
		Info(vector<Tree> t, int s){
			tr = vector<Tree>(t);
			steps = s;
			this->build();
		}
		double objective(Chromosome c);
		void print(Chromosome c);
}

double Info::objective(Chromosome c){
	int N = c.getLength();
	vector<bool> down(N, false);


}

class Genetic{
	private:
		vector<Chromosome> _pop;
		int _pop_size;
		double _side_mutation_ratio, _order_mutation_ratio, _cross_ratio;

	public:
		Genetic();
}