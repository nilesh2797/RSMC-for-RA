#ifndef TRIE_H
#define TRIE_H

#include "bits/stdc++.h"
using namespace std;

struct nodeType
{
	int pid;
	int ceid, teid; // command eid and trace eid
	int ppid, peid; // parameter pid and eid

	nodeType()
	{
		pid = ceid = teid = ppid = peid = 0;
	}

	nodeType(int p, int c, int t, int pp, int pe)
	{
		pid = p;
		ceid = c;
		teid = t;
		ppid = pp;
		peid = pe;
	}

	const void print() const
	{
		cout << "\t((" << pid << ", " << ceid << ")(" << ppid << ", " << peid << ")(" << pid << ", " << teid << ")) \n";
	}	

	bool operator<(const nodeType &rhs) const
	{
		if(pid == rhs.pid)
		{
			if(ceid == rhs.ceid)
			{
				if(teid == rhs.teid)
				{
					if(ppid == rhs.ppid)
					{
						return peid < rhs.peid;
					}
					else
						return ppid < rhs.ppid;
				}
				else
					return teid < rhs.teid;
			}
			else
				return ceid < rhs.ceid;
		}
		else
			return pid < rhs.pid;
	}
};
typedef std::vector<nodeType> vnt;

struct node
{
	bool leaf;
	int num;
	map<nodeType, node*> child;

	node();
};


struct trie
{
	node* head;
	int cnt;

	trie();

	bool empty();

	bool remove(const vnt &s, node* parent, int i);

	void remove(const vnt &s); // need to implement this

	void add(const vnt &s, node* parent, int i);

	void add(const vnt &s);

	bool find(const vnt &s, node* parent, int i);

	bool find(const vnt &s);

	void getRun(vnt &s, node* parent);

	vnt getRun(); // need to implement this
};

#endif