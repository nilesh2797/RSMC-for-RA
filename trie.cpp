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

void trie::add(const vnt &s, node* parent, int i)
{
	if(i == s.size())
	{
		parent->leaf = true;
		return;
	}
	else
	{
		#ifdef DEBUG
			s[i].print();
		#endif
		if(parent->child[s[i]] == NULL)
		{
			parent->child[s[i]] = new node;
			parent->num++;
		}
		// cout << "parent->num = " << parent->num << endl;
		add(s, parent->child[s[i]], i+1);
		return;
	}
}

void trie::add(const vnt &s)
{
	#ifdef DEBUG
	cout << "adding : \n";
	#endif
	add(s, head, 0);
	#ifdef DEBUG
	cout << endl;
	#endif
	cnt++;
	return;
}

bool trie::find(const vnt &s, node* parent, int i)
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

bool trie::find(const vnt &s)
{
	return find(s, head, 0);
}

bool trie::remove(const vnt &s, node* parent, int i)
{
	if(i == s.size())
	{
		return true;
	}
	else
	{
		#ifdef DEBUG
		s[i].print();
		#endif

		bool b = remove(s, parent->child[s[i]], i+1);
		if(b)
		{
			delete parent->child[s[i]];
			parent->child[s[i]] = NULL;

			#ifdef DEBUG
			if(parent->child[s[i]] == NULL)
				cout << "remove : \n";
				s[i].print();
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

void trie::remove(const vnt &s)
{
	#ifdef DEBUG
	cout << "removing : \n";
	#endif
	remove(s, head, 0);
	#ifdef DEBUG
	cout << endl;
	#endif
	cnt--;
}

void trie::getRun(vnt &s, node* parent)
{
	// cout << "num = " << parent->num << endl;
	if(parent != NULL and parent->num > 0)
	{
		node* next = NULL;
		for(map<nodeType, node*>::iterator it = parent->child.begin(); it != parent->child.end(); ++it)
		{
			if(it->second != NULL)
			{
				#ifdef DEBUG
					(it->first).print();
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

vnt trie::getRun()
{
	vnt s;
	#ifdef DEBUG
	cout << "found : \n";
	#endif
	getRun(s, head);
	#ifdef DEBUG
	cout << endl;
	#endif
	return s;
}

#endif