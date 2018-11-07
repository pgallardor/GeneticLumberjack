#pragma once
#include <vector>
#include <cmath>
#include <cstdio>
#include <algorithm>
#include "chromosome.hpp"
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
using namespace std;

struct Tree{
	int _x, _y, _h, _d, _c, _p;
	Tree(int x, int y, int h, int d, int c, int p) : 
		 _x(x), _y(y), _h(h), _d(d), _c(c), _p(p) {}
};

class Info{
	public:
		vector<Tree> tr;
		vector<vector<int> > adj[4];

		int steps;
		Info(vector<Tree> t, int s, int G){
			tr = vector<Tree>(t);
			steps = s;

			this->build(G);
		}
		double objective(Chromosome c);
		double cut(int i, int dir, vector<bool> &down);
		void print(Chromosome c);
		void build(int grid_size);
};

void Info::build(int grid_size){
	int N = this->tr.size();
	Tree buf(0,0,0,0,0,0);
	int **grid;

	for (int i = 0; i < 4; i++){
		adj[i].assign(N, vector<int>());
	}

	grid = new int* [grid_size];
	for (int i = 0; i < grid_size; i++){
		grid[i] = new int[grid_size];
		for (int j = 0; j < grid_size; j++) grid[i][j] = -1;
	}

	for (int i = 0; i < N; i++){
		buf = this->tr[i];
		grid[buf._y][buf._x] = i;
	}

	for (int i = 0; i < N; i++){
		buf = this->tr[i];
		int h = buf._h, x = buf._x, y = buf._y;
		for (int k = 1; k <= h; k++){
			if (x + k < grid_size && grid[y][x + k] != -1){
				int idx = grid[y][x + k];
				Tree nei = tr[idx];
				if (buf._c * buf._d * buf._h > nei._c * nei._d * nei._h)
					adj[RIGHT][i].push_back(idx);
			}

			if (y + k < grid_size && grid[y + k][x] != -1){
				int idx = grid[y + k][x];
				Tree nei = tr[idx];
				if (buf._c * buf._d * buf._h > nei._c * nei._d * nei._h)
					adj[UP][i].push_back(idx);
			}

			if (x - k >= 0 && grid[y][x - k] != -1){
				int idx = grid[y][x - k];
				Tree nei = tr[idx];
				if (buf._c * buf._d * buf._h > nei._c * nei._d * nei._h)
					adj[LEFT][i].push_back(idx);
			}

			if (y - k >= 0 && grid[y - k][x] != -1){
				int idx = grid[y - k][x];
				Tree nei = tr[idx];
				if (buf._c * buf._d * buf._h > nei._c * nei._d * nei._h)
					adj[DOWN][i].push_back(idx);
			}
		}
	}

	for (int i = 0; i < grid_size; i++){
		for (int j = 0; j < grid_size; j++) 
			printf("%d ", grid[i][j]);
		printf("\n");
	}

	for (int i = 0; i < grid_size; i++){
		delete [] grid[i];
	}
	delete [] grid;
}

void Info::print(Chromosome c){
	int N = c.getLength();
	vector<bool> down(N, false);
	char s[5][7] = {"up", "down", "left", "right", "lul"};

	int x = 0, y = 0, t_steps = this->steps, dx, dy;

	for (int i = 0; i < N; i++){
		ii act = c.at(i);
		if (down[act.first]) continue;

		Tree t = this->tr[act.first];
		dx = t._x - x, dy = t._y - y;

		if (dx < 0){
			while (x > t._x){
				if (!t_steps) return;
				t_steps--;
				x--;
				printf("move left\n");
			}
		}
		else{
			while(x < t._x){
				if (!t_steps) return;
				t_steps--;
				x++;
				printf("move right\n");
			}
		}

		if (dy < 0){
			while (y > t._y){
				if (!t_steps) return;
				t_steps--;
				y--;
				printf("move down\n");
			}
		}
		else{
			while(y < t._y){
				if (!t_steps) return;
				t_steps--;
				y++;
				printf("move up\n");
			}
		}

		if (t_steps < t._d) return;
		t_steps -= t._d;
		printf("cut %s\n", s[act.second]);
		cut(act.first, act.second, down);
	}
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
	Tree t = this->tr[i], buff(0,0,0,0,0,0);
	int tree_i = i;
	double val = t._h * t._p * t._d;
	down[i] = true;

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
		Info *_in;
		vector<Chromosome> _pop;
		int _pop_size, _chr_len, _best_idx;
		double _side_mutation_ratio, _order_mutation_ratio, _cross_ratio, _time_limit;
		Chromosome tournament(int t_size);
		void crossover(Chromosome &p1, Chromosome &p2);

	public:
		Genetic(Info *i, int clen, int p_size, double side_r, double order_r, double cross_r, double tlimit);
		Chromosome run(int it_limit);
};

Genetic::Genetic(Info *i, int clen, int p_size, double side_r, double order_r, double cross_r, double tlimit){
	_chr_len = clen; _pop_size = p_size;
	_side_mutation_ratio = side_r;
	_order_mutation_ratio = order_r;
	_cross_ratio = cross_r;
	_time_limit = tlimit;
	_in = i;
	_best_idx = -1;

	for (int i = 0; i < _pop_size; i++){
		Chromosome c(_chr_len);
		c.setFitness(_in->objective(c));

		if (_best_idx == -1 || _pop[_best_idx].getFitness() < c.getFitness()) _best_idx = i;
		_pop.push_back(c);
	}
}

void Genetic::crossover(Chromosome &p1, Chromosome &p2){
	int size = _chr_len;

	vector<ii> parent1 = p1.getVector(), parent2 = p2.getVector();

    int number1 = rand() % size;
    int number2 = rand() % size;

    int start = min(number1, number2);
    int end = max(number1, number2);

    vector<ii> child1;
    vector<ii> child2;

    for(int i = start; i<end; i++)
    {
        child1.push_back(parent1[i]);
        child2.push_back(parent2[i]);
    }

    int geneIndex = 0;
    ii geneInparent1 = ii(-1, -1);
    ii geneInparent2 = ii(-1, -1);

    for (int i = 0; i<size; i++)
    {
        geneIndex = (end + i) % size;
        geneInparent1 = parent1[geneIndex];
        geneInparent2 = parent2[geneIndex];

        bool is_there = false;
        for(int i1 = 0; i1<child1.size(); i1++)
        {
            if(child1[i1] == geneInparent2)
            {
                is_there = true;
            }
        }
        if(!is_there)
        {
            child1.push_back(geneInparent2);
        }

        bool is_there1 = false;
        for(int i1 = 0; i1<child2.size(); i1++)
        {
            if(child2[i1] == geneInparent1)
            {
                is_there1 = true;
            }
        }
        if(!is_there1)
        {
            child2.push_back(geneInparent1);
        }
    }

    std::rotate(child1.begin(), child1.begin()+start, child1.end());
    std::rotate(child2.begin(), child2.begin()+start, child2.end());

    p1 = Chromosome(child1);
    p2 = Chromosome(child2);

}

Chromosome Genetic::tournament(int t_size){
	vector<bool> chosen(_pop_size, false);
	Chromosome buf(1);
	int cnt = 0, i;
	while(cnt < t_size){
		do{
			i = rand() % _pop_size;
		}while(chosen[i]);
		chosen[i] = true;
		if (cnt == 0 || buf.getFitness() < _pop[i].getFitness())
			buf = _pop[i];
		cnt++;
	}

	return buf;
}

Chromosome Genetic::run(int it_limit){
	//
	for (int i = 0; i < it_limit; i++){
		vector<Chromosome> new_pop;
		double probs;
		new_pop.push_back(_pop[_best_idx]); //ensure the best
		_best_idx = 0;
		for (int k = 0; k < _pop_size / 2; k++){
			Chromosome p1 = this->tournament(4);
			Chromosome p2 = this->tournament(4);
			
			probs = (rand() % 101) / 100;
			if (probs < _cross_ratio)
				this->crossover(p1, p2);

			new_pop.push_back(p1);
			new_pop.push_back(p2);
		}

		_pop_size = new_pop.size();

		for (int k = 0; k < _pop_size; k++){
			probs = (rand() % 101) / 100;
			if (probs < _order_mutation_ratio){
				int a = rand() % _chr_len, b = rand() % _chr_len;
				new_pop[k].swap(a, b);
			}

			probs = (rand() % 101) / 100;
			if (probs < _side_mutation_ratio){
				int p = rand() % 10 + 1, sm;
				if (p < 7)
					sm = rand() % (_chr_len / 3);
				else
					sm = rand() % 2*_chr_len/3 + (_chr_len / 3);
				new_pop[k].mutate(sm);
			}
			new_pop[k].setFitness(_in->objective(new_pop[k]));
			if (new_pop[_best_idx].getFitness() < new_pop[k].getFitness()) _best_idx = k;

		}

		_pop = new_pop;
	}

	return _pop[_best_idx];
}