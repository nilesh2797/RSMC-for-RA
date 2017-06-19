#ifndef TRIE_CPP
#define TRIE_CPP

#include "trie.h"

// #define DEBUG

node::node()
{
	leaf = false;
	num = 0;
}

trie::trie()
{
	head = new node;
	cnt = 0;
}

bool trie::empty()
{
	return cnt == 0 or head->num == 0;
}

void trie::add(const viiii &s, node* parent, int i)
{
	if(i == s.size())
	{
		parent->leaf = true;
		return;
	}
	else
	{
		#ifdef DEBUG
			cout << "adding s[" << i << "] = ((" << s[i].X.X << ", " << s[i].X.Y << ")(" << s[i].Y.X << ", " << s[i].Y.Y << "))\n";
		#endif
		if(parent->child[s[i]] == NULL)
		{
			parent->child[s[i]] = new node;
			parent->num++;
		}
		add(s, parent->child[s[i]], i+1);
		return;
	}
}

void trie::add(const viiii &s)
{
	add(s, head, 0);
	cnt++;
	return;
}

bool trie::find(const viiii &s, node* parent, int i)
{
	if(i == s.size() or parent == NULL)
	{
		if(parent == NULL)
			return false;
		else
			return parent->leaf;
	}
	else
	{
		return find(s, parent->child[s[i]], i+1);
	}
}

bool trie::find(const viiii &s)
{
	return find(s, head, 0);
}

bool trie::remove(const viiii &s, node* parent, int i)
{
	if(i == s.size())
	{
		return true;
	}
	else
	{
		#ifdef DEBUG
		cout << "removing s[" << i << "] = ((" << s[i].X.X << ", " << s[i].X.Y << ")(" << s[i].Y.X << ", " << s[i].Y.Y << "))\n";
		#endif

		bool b = remove(s, parent->child[s[i]], i+1);
		if(b)
		{
			delete parent->child[s[i]];
			parent->child[s[i]] = NULL;

			#ifdef DEBUG
			if(parent->child[s[i]] == NULL)
				cout << "removed s[" << i << "] = ((" << s[i].X.X << ", " << s[i].X.Y << ")(" << s[i].Y.X << ", " << s[i].Y.Y << "))\n";
			#endif

			if(--parent->num)
				return false;
			else
			{
				return true;
			}
		}
		return false;
	}
}

void trie::remove(const viiii &s)
{
	remove(s, head, 0);
	cnt--;
}

void trie::getRun(viiii &s, node* parent)
{
	// cout << "num = " << parent->num << endl;
	if(parent != NULL and parent->num > 0)
	{
		node* next = NULL;
		for(map<iiii, node*>::iterator it = parent->child.begin(); it != parent->child.end(); ++it)
		{
			if(it->second != NULL)
			{
				#ifdef DEBUG
				cout << "found ((" << it->first.X.X << ", " << it->first.X.Y << ")(" << it->first.Y.X << ", " << it->first.Y.Y << "))\n";
				#endif

				s.push_back(it->first);
				next = it->second;
				break;
			}
		}
		getRun(s, next);
		// cout << "returned\n";
	}
	return;
}

viiii trie::getRun()
{
	viiii s;
	getRun(s, head);
	return s;
}

#endif