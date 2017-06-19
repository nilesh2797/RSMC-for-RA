#ifndef TRIE_H
#define TRIE_H

#include "bits/stdc++.h"
using namespace std;

typedef pair<int, int> ii;
typedef pair<ii, ii> iiii;
typedef std::vector<iiii> viiii;

struct node
{
	bool leaf;
	int num;
	map<iiii, node*> child;

	node();
};


struct trie
{
	node* head;
	int cnt;

	trie();

	bool empty();

	bool remove(const viiii &s, node* parent, int i);

	void remove(const viiii &s); // need to implement this

	void add(const viiii &s, node* parent, int i);

	void add(const viiii &s);

	bool find(const viiii &s, node* parent, int i);

	bool find(const viiii &s);

	void getRun(viiii &s, node* parent);

	viiii getRun(); // need to implement this
};

#endif