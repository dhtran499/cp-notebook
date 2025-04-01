#include <bits/stdc++.h>
using namespace std;

#define  int  long long
#define  ALL(x)  (x).begin(), (x).end()
#define  FOR(i, a, b)  for(int i = (a), _b = (b); i <= _b; ++ i)
#define  FORD(i, b, a)  for(int i = (b), _a = (a); i >= _a; -- i)
#define  FORE(i, v)  for(__typeof((v).begin()) i = (v).begin(); i != (v).end(); ++ i)

const string NAME = "";

const int NTEST = 1000;

// Don't forget this!!!
mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

////////////// DISTRIBUTIONS

// return int in [L,R] inclusive
int randInt(int L, int R){
  assert(L <= R);
	return uniform_int_distribution<int>(L,R)(rng);
}

// return double in [L,R] inclusive
double randDouble(double L, double R){
  assert(L <= R);
	return uniform_real_distribution<double>(L,R)(rng);
}

// http://cplusplus.com/reference/random/geometric_distribution/geometric_distribution/
// flip a coin which is heads with probability p until you flip heads
// mean value of c is 1/p-1
int randGeo(double p) {
  assert(0 < p && p <= 1); // p large -> closer to 0
	return geometric_distribution<int>(p)(rng);
}

////////////// VECTORS + PERMS

// shuffle a vector
template<class T>
  void shuf(vector<T>& v){
  shuffle(ALL(v),rng);
}

// generate random permutation of [0,N-1]
vector<int> randPerm(int N){
  vector<int> v(N);
  iota(ALL(v),0);
  shuf(v); return v;
}

// random permutation of [0,N-1] with first element 0
vector<int> randPermZero(int N){
  vector<int> v(N - 1);
  iota(ALL(v),1);
	shuf(v);
  v.insert((v).begin(),0);
  return v;
}

// shuffle permutation of [0,N-1]
vector<int> shufPerm(vector<int> v){
	int N = (v).size();
  vector<int> key = randPerm(N);
	vector<int> res(N);
  FOR(i, 0, N - 1) res[key[i]] = key[v[i]];
	return res;
}

// vector with ALL entries in [L,R]
vector<int> randVector(int N, int L, int R){
	vector<int> res;
  FOR(i, 0, N - 1) res.push_back(randInt(L, R));
	return res;
}

// vector with ALL entries in [L,R], unique
vector<int> randVectorUnique(int N, int L, int R){
	set<int> so_far;
  vector<int> res;
	FOR(i, 0, N - 1){
		int x;
    do{
      x = randInt(L, R);
    }while(so_far.count(x) != 0);
		so_far.insert(x);
    res.push_back(x);
	}
	return res;
}

////////////// GRAPHS

// relabel edges ed according to perm, shuffle
vector<pair<int, int>> relabelAndShuffle(vector<pair<int, int>> ed, vector<int> perm){
	FORE(t, ed) {
		t->first = perm[t->first];
    t->second = perm[t->second];
		if (rng() & 1) swap(t->first, t->second);
	}
	shuf(ed); return ed;
}

// shuffle graph with vertices [0,N-1]
vector<pair<int, int>> shufGraph(int N, vector<pair<int, int>> ed){ // randomly swap endpoints, rearrange labels
	return relabelAndShuffle(ed,randPerm(N));
}

vector<pair<int, int>> shufGraphZero(int N, vector<pair<int, int>> ed){
	return relabelAndShuffle(ed,randPermZero(N));
}

// shuffle tree given N-1 edges
vector<pair<int, int>> shufTree(vector<pair<int, int>> ed){
  return shufGraph((int)ed.size() + 1, ed);
}
// randomly swap endpoints, rearrange labels
vector<pair<int, int>> shufRootedTree(vector<pair<int, int>> ed){
	return relabelAndShuffle(ed,randPermZero((int)ed.size() + 1));
}

////////////// GENERATING TREES

// for generating tall tree
pair<int, int> geoEdge(int i, double p){
  assert(i > 0); 
	return {i, max(0LL, i - 1 - randGeo(p))};
}

// generate edges of tree with verts [0,N-1]
// smaller back -> taller tree
vector<pair<int, int>> treeRand(int N, int back){ 
	assert(N >= 1 && back >= 0);
  vector<pair<int, int>> ed; 
	FOR(i, 1, N - 1) ed.emplace_back(i, i - 1 - randInt(0,min(back,i - 1)));
	return ed;
}

// generate path
vector<pair<int, int>> path(int N){
  return treeRand(N,0);
}

// generate tall tree (large diameter)
// the higher the p the taller the tree
vector<pair<int, int>> treeTall(int N, double p){
  assert(N >= 1); 
	vector<pair<int, int>> ed;
  FOR(i, 1, N - 1) ed.push_back(geoEdge(i, p));
	return ed;
}

// generate tall tree, then add rand at end
vector<pair<int, int>> treeTallShort(int N, double p) { 
	assert(N >= 1);
  int mid = (N + 1) / 2;
	vector<pair<int, int>> ed = treeTall(mid, p); 
	FOR(i, mid, N - 1) ed.emplace_back(i,randInt(0,i - 1));
	return ed;
}

// lots of stuff connected to either heavy1 or heavy2
vector<pair<int, int>> treeTallHeavy(int N, double p) { 
	assert(N >= 1); // + bunch of rand
	vector<pair<int, int>> ed;
  int heavy1 = 0, heavy2 = N / 2;
	FOR(i, 1, N - 1) {
		if(i < N/4) ed.emplace_back(i,heavy1);
		else if (i > heavy2 && i < 3 * N / 4) ed.emplace_back(i, heavy2);
		else ed.push_back(geoEdge(i, p));
	}
	return ed;
}

// heavy tall tree + random
// lots of verts connected to heavy1 or heavy2
vector<pair<int, int>> treeTallHeavyShort(int N, double p) { 
	assert(N >= 1); // + almost-path + rand
	vector<pair<int, int>> ed;
  int heavy1 = 0, heavy2 = N / 2;
	FOR(i, 1, N - 1) {
		if(i < N / 4) ed.emplace_back(i, heavy1);
		else if (i <= heavy2) ed.push_back(geoEdge(i, p)); // tall -> heavy1
		else if (i > heavy2 && i < 3 * N / 4) ed.emplace_back(i, heavy2);
		else ed.emplace_back(i,randInt(0,i - 1));
	}
	return ed;
}

int randPrime(int l, int r) {
	while(1) {
		int x = randInt(l,r);
		bool bad = 0;
		for (int i = 2; i * i <= x; ++ i) if (x % i == 0) bad = 1;
		if (!bad) return x;
	}
}

void gentest(void){
  ofstream out((NAME + ".inp").c_str());



  out.close();
}

signed main(void){
  srand(time(nullptr));

  FOR(i, 1, NTEST){
    gentest();
    system((NAME + "_ac.exe").c_str());
    system((NAME + "_slow.exe").c_str());
    if(system(("fc " + NAME + ".out " + NAME + ".ans").c_str()) != 0){
      cout << "Testcase " << i << ": WA!!!" << endl;
      return 0;
    } else cout << "Testcase " << i << ": AC!!!" << endl;
  }
}
