#pragma once
#include <vector>
#include <cstdlib>
#include <utility>
using namespace std;

typedef pair<int, int> ii;

class Chromosome{
	private:
		vector<ii> _v;
		double _fit;

	public:
		Chromosome(int N);
		Chromosome(vector<int> v);
		ii at(int i);
		double getFitness();
		void setFitness(double f);
		int getLength();
}

Chromosome::Chromosome(int N){
	vector<bool> used(N, false);
	int r;
	for (int i = 0; i < N; i++){
		do{
			r = rand() % N;
		}while(used[r]);

		used[r] = true;
		_v.push_back(r);
	}

	_fit = -1.0f;
}

Chromosome::Chromosome(vector<int> v){
	_v = vector<int>(v);
}

ii Chromosome::at(int i){
	if (i < _v.size())
		return _v[i];
	return -1;
}

int Chromosome::getLength(){
	return _v.size();
}

double Chromosome::getFitness(){
	return _fit;
}

void Chromosome::setFitness(double f){
	_fit = f;
}