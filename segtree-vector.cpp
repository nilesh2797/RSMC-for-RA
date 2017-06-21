#include <bits/stdc++.h>

using namespace std;

typedef std::vector<int> vi;
typedef long long ll;
typedef std::vector<ll> vl;
typedef std::vector<std::vector<int> > vvi;
typedef std::pair<int, int> ii;
typedef std::vector<ii> vii;
typedef std::pair<ii,ii> iiii;
typedef std::vector<iiii> viiii;

#define pb push_back
#define mset(a, b) memset(a, b, sizeof a)
#define all(x) (x).begin(), (x).end()
#define bit(x) (1 << (x))
#define bitl(x) (1LL << (x))
#define sqr(x) ((x) * (x))
#define sz(x) ((int)(x.size()))
#define cnti(x) (__builtin_popcount(x)) //number of set bits in x
#define cntl(x) (__builtin_popcountll(x))
#define clzi(x) (__builtin_clz(x)) //position of leftmost bit from left 0 based indexing
#define clzl(x) (__builtin_clzll(x))
#define ctzi(x) (__builtin_ctz(x)) //position of rightmost bit from right 0 based indexing
#define ctzl(x) (__builtin_ctzll(x))
#define rep(z,a,b) for(int z=a;z<b;z++)
#define iter(it,a) for(auto it=a.begin();it!=a.end();it++)
 
#define X first
#define Y second
#define INF 1E9
 
#define output(x) cout << #x << " = " << x << endl
#define printv(x) rep(z, 0, sz(x)) cout << #x << "[" << z << "] = " << x[z] << endl
#define mod 1000000007

std::vector<int> max(std::vector<int> l, std::vector<int> r)
{
	vi res(l.size(), 0);
	rep(i, 0, l.size())
	{
		res[i] = max(l[i], r[i]);
	}
	return res;
}

struct segTreeMax
{
	int n;  // array size
	int m;  // number of processes
	std::vector<std::vector<int> > t;

	segTreeMax()
	{
		m = n = 0;
	}

	segTreeMax(const std::vector<std::vector<int> > &v, int m1)
	{
		m = m1;
		n = v.size();
		t.resize(4*n, std::vector<int>(m, -INF));
		for(int i = 0; i < n; ++i)
		{
			for(int j = 0; j < m; ++j)
				t[n+i][j] = v[i][j];
		}
		build();
	}

	segTreeMax(int n1, int m1)
	{
		m = m1;
		n = n1;
		t.resize(4*n, std::vector<int>(m, -INF));
	}

	void build() {  // build the tree
	  for (int i = n - 1; i > 0; --i) t[i] = max(t[i<<1], t[i<<1|1]);
	}

	void modify(int p, std::vector<int> value) {  // set value at position p
	  for (t[p += n] = value; p > 1; p >>= 1) t[p>>1] = max(t[p], t[p^1]);
	}

	void resize(int m1, int n1)
	{
		t.clear();
		m = m1;
		n = n1;
		t.resize(4*n, std::vector<int>(m, -INF));
	}

	std::vector<int> query(int l, int r) {  // max on interval [l, r)
	  std::vector<int> res(m, -INF);
	  for (l += n, r += n; l < r; l >>= 1, r >>= 1) {
	    if (l&1) res = max(res, t[l++]);
	    if (r&1) res = max(res, t[--r]);
	  }
	  return res;
	}
};

std::vector<int> min(std::vector<int> l, std::vector<int> r)
{
	vi res(l.size(), 0);
	rep(i, 0, l.size())
	{
		res[i] = min(l[i], r[i]);
	}
	return res;
}

struct segTreeMin
{
	int n;  // array size
	int m;  // number of processes
	std::vector<std::vector<int> > t;

	segTreeMin()
	{
		m = n = 0;
	}

	segTreeMin(const std::vector<std::vector<int> > &v, int m1)
	{
		m = m1;
		n = v.size();
		t.resize(4*n, std::vector<int>(m, INF));
		for(int i = 0; i < n; ++i)
		{
			for(int j = 0; j < m; ++j)
				t[n+i][j] = v[i][j];
		}
		build();
	}

	segTreeMin(int m1, int n1)
	{
		m = m1;
		n = n1;
		t.resize(4*n, std::vector<int>(m, INF));
	}

	void build() {  // build the tree
	  for (int i = n - 1; i > 0; --i) t[i] = min(t[i<<1], t[i<<1|1]);
	}

	void modify(int p, std::vector<int> value) {  // set value at position p
	  for (t[p += n] = value; p > 1; p >>= 1) t[p>>1] = min(t[p], t[p^1]);
	}

	void resize(int m1, int n1)
	{
		t.clear();
		m = m1;
		n = n1;
		t.resize(4*n, std::vector<int>(m, INF));
	}

	std::vector<int> query(int l, int r) {  // min on interval [l, r)
	  std::vector<int> res(m, INF);
	  for (l += n, r += n; l < r; l >>= 1, r >>= 1) {
	    if (l&1) res = min(res, t[l++]);
	    if (r&1) res = min(res, t[--r]);
	  }
	  return res;
	}
};

//sample implementation

// int main() {
// 	int n, m;
// 	cin >> n >> m;
// 	std::vector<std::vector<int> > v(n, std::vector<int>(m, 0));
// 	for (int i = 0; i < n; ++i)
// 	{
// 		for(int j = 0; j < m; ++j)
// 			cin >> v[i][j];
// 	}
// 	segTreeMax st(v, m);
// 	vi vec(m, 41);
// 	st.modify(3, vec);
// 	printv(st.query(1, 3));
// }