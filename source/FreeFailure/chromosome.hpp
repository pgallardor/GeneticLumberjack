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
		Chromosome(vector<ii> v);
		ii at(int i);
		double getFitness();
		void setFitness(double f);
		int getLength();
		Chromosome getCopy();
		vector<ii> getVector();
		void swap(int i, int j);
		void mutate(int i);
};

Chromosome::Chromosome(int N){
	vector<bool> used(N, false);
	int r, s;
	for (int i = 0; i < N; i++){
		do{
			r = rand() % N;
		}while(used[r]);

		used[r] = true;
		s = rand() % 4;
		_v.push_back(ii(r, s));
	}
	
	_fit = -1.0f;
}

Chromosome::Chromosome(vector<ii> v){
	_v = vector<ii>(v);
}

ii Chromosome::at(int i){
	if (i < _v.size())
		return _v[i];
	return ii(-1, -1);
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

Chromosome Chromosome::getCopy(){
	return Chromosome(this->_v);
}

vector<ii> Chromosome::getVector(){
	return vector<ii>(this->_v);
}

void Chromosome::swap(int i, int j){
	ii aux = _v[i];
	_v[i] = _v[j];
	_v[j] = aux;
}

void Chromosome::mutate(int i){
	_v[i].second = (_v[i].second + rand() % 4) % 4;
}