#include <bits/stdc++.h>
#include "segtree-vector.cpp"

using namespace std;

#define READ 0
#define WRITE 1

#define PO 0
#define RF 1
#define CO 2

#define NOPRINT 0

void linebreak()
{
	rep(i, 0, 100)
		cout << "-";
	cout << endl;
}

struct evnt
{
	int eid, pid;
	bool type;
	ii parameter;
	std::vector<std::vector<int> > suc, pre, maxw;
	int var, value;

	evnt(int num_p, int num_var, bool typ, int ind, int pind, int var1)
	{
		suc.resize(num_var+1, vi(num_p, INF));
		pre.resize(num_var+1, vi(num_p, -INF));
		maxw.resize(num_var+1, vi(num_p, -INF));
		parameter = ii(INF, INF);
		eid = ind;
		type = typ;
		pid = pind;
		var = var1;
	}

	void print()
	{
		cout << "event :" << " : [pid=" << pid << ", eid=" << eid << "] : " << var << "\n";
		cout << "successor vector clock :\n[\n";
		rep(i, 0, suc.size())
		{
			cout << "\tvar = " << i << " : (";
			rep(j, 0, suc[0].size())
			{
				cout << suc[i][j] << ", ";
			}
			cout << ")\n";
		}
		cout << "]\n";
		cout << "predecessor vector clock :\n[\n";
		rep(i, 0, suc.size())
		{
			cout << "\tvar = " << i << " : (";
			rep(j, 0, suc[0].size())
			{
				cout << pre[i][j] << ", ";
			}
			cout << ")\n";
		}
		cout << "]\n";
		cout << "maxw vector clock :\n[\n";
		rep(i, 0, suc.size())
		{
			cout << "\tvar = " << i << " : (";
			rep(j, 0, suc[0].size())
			{
				cout << maxw[i][j] << ", ";
			}
			cout << ")\n";
		}
		cout << "]\n";
	}
};

std::vector<std::vector<evnt> > trace;
std::vector<vector<segTreeMax> > stMax;
std::vector<vector<segTreeMin> > stMin;

int num_p, num_var = 0; // number of programs and number of variables

void succhange(evnt &e, int var, int pid, const int &value, bool PRINT = false)
{
	if(PRINT)
		cout << "updating successors of (" << e.pid << ", " << e.eid << ")\n";
	e.suc[var][pid] = value;
	stMin[e.pid][var].modify(e.eid, e.suc[var]);
}

void prechange(evnt &e, int var, int pid, const int &value, bool PRINT = false)
{
	if(PRINT)
		cout << "updating predecessors of (" << e.pid << ", " << e.eid << ")\n";
	e.pre[var][pid] = value;
	stMax[e.pid][var].modify(e.eid, e.pre[var]);
}

void succhange(evnt &e, int var, const vi &value, bool PRINT = false)
{
	if(PRINT)
		cout << "updating successors of (" << e.pid << ", " << e.eid << ")\n";
	e.suc[var] = value;
	stMin[e.pid][var].modify(e.eid, e.suc[var]);
}

void prechange(evnt &e, int var, const vi &value, bool PRINT = false)
{
	if(PRINT)
		cout << "updating predecessors of (" << e.pid << ", " << e.eid << ")\n";
	e.pre[var] = value;
	stMax[e.pid][var].modify(e.eid, e.pre[var]);
}

void update(evnt &e)
{
	rep(i, 0, e.suc.size())
	{
		prechange(e, i, stMax[e.pid][i].query(0, e.eid+1), NOPRINT);
		succhange(e, i, stMin[e.pid][i].query(e.eid, stMin[e.pid][i].n), NOPRINT);
	}
}

void addEdge(evnt &preve, evnt &e, int edgeType)
{
	if(edgeType == PO)
	{
		// update vector clocks of preve
		// cout << "adding PO edge between (" << preve.pid << ", " << preve.eid << ") and (" << e.pid << ", " << e.eid << ")\n";
		update(preve);
		// copy vector clocks of preve into e
		rep(i, 0, num_var+1)
		{
			e.pre[i] = preve.pre[i];
		}
		e.maxw = preve.maxw;
		// change somethings
		prechange(e, 0, e.pid, preve.eid);// updating hb order of the same thread
		if(e.type == WRITE)
		{
			// if its a write event then its going to hide all events occured previously in the hbUcox order
			prechange(e, e.var, max(e.pre[0], e.pre[e.var]));
			// change maxw
			rep(i, 0, num_p)
			{
				e.maxw[e.var][i] = -INF;
			}
			e.maxw[e.var][e.pid] = e.eid;
		}
		// now updating successor of e

		succhange(preve, 0, preve.pid, e.eid);
		succhange(preve, e.var, preve.pid, e.eid);

	}
	else
	{
		if(edgeType == RF)
		{
			// cout << "adding RF edge between (" << preve.pid << ", " << preve.eid << ") and (" << e.pid << ", " << e.eid << ")\n";

			update(preve);
			int var = preve.var;

			rep(i, 0, num_var+1)
			{
				if(i != e.var)	
					e.maxw[i] = max(e.maxw[i], preve.maxw[i]);
			}

			rep(i, 0, num_p)
			{
				e.maxw[e.var][i] = -INF;
			}
			e.maxw[e.var][preve.pid] = preve.eid;
		
			// we can even skip this step since we add rf edge just after adding po edge so no need to update e
			// update(e);
			
			// update predecessor of e wrt preve
			prechange(e, 0, preve.pid, preve.eid);
			rep(i, 0, num_var+1)
			{
				prechange(e, i, max(e.pre[i], preve.pre[i]));
			}
			//update successor of preve wrt e
			succhange(preve, 0, e.pid, min(preve.suc[0][e.pid], e.eid));
			succhange(preve, var, e.pid, min(preve.suc[var][e.pid], e.eid));

			// now update predecessors of preve about e
			rep(i, 0, num_p)
			{
				if(preve.pre[0][i] != INF and preve.pre[0][i] != -INF)
				{
					succhange(trace[i][preve.pre[0][i]], 0, e.pid, min(trace[i][preve.pre[0][i]].suc[0][e.pid], e.eid));
				}
				if(preve.pre[var][i] != INF and preve.pre[var][i] != -INF)
				{
					succhange(trace[i][preve.pre[var][i]], var, e.pid, min(trace[i][preve.pre[var][i]].suc[var][e.pid], e.eid));
				}
			}
		}
		else
		{
			// cout << "adding CO edge between (" << preve.pid << ", " << preve.eid << ") and (" << e.pid << ", " << e.eid << ")\n";

			update(preve);
			update(e);

			int var = e.var;
			//updating predecessor of e wrt preve, note here we change only the var vector clock
			prechange(e, var, preve.pid, max(preve.eid, e.pre[var][preve.pid]));
			prechange(e, var, max(e.pre[var], preve.pre[var]));
			//updating successor of preve wrt e, note here we change only the var vector clock
			succhange(preve, var, e.pid, min(e.eid, preve.suc[var][e.pid]));
			succhange(preve, var, min(preve.suc[var], e.suc[var]));

			// now update successors of e about preve
			rep(i, 0, num_p)
			{
				if(min(e.suc[var][i], e.suc[0][i]) != INF and min(e.suc[var][i], e.suc[0][i]) != -INF)
				{
					prechange(trace[i][min(e.suc[var][i], e.suc[0][i])], var, max(trace[i][min(e.suc[var][i], e.suc[0][i])].pre[var], e.pre[var]));
				}
			}
			// now update predecessors of preve about e
			rep(i, 0, num_p)
			{
				if(max(preve.pre[var][i], preve.pre[0][i]) != INF and max(preve.pre[var][i], preve.pre[0][i]) != -INF)
				{
					succhange(trace[i][max(preve.pre[var][i], preve.pre[0][i])], var, min(trace[i][max(preve.pre[var][i], preve.pre[0][i])].suc[var], preve.suc[var]));
				}
			}
		}
	}
}

std::vector<ii> listAllRf(const evnt &e)
{
	std::vector<ii> toReturn;
	int var = e.var;
	// cout << "these are possible parameters for the event : \n";
	int count = 1;
	rep(i, 0, num_p)
	{
		rep(j, max(-1, e.pre[var][i])+1, trace[i].size())
		{
			if(trace[i][j].type == WRITE and trace[i][j].var == var)
			{
				// cout << count++ << ". (" << i << ", " << j << ")\n";
				toReturn.pb(ii(i, j));
			}	
		}
	}
	// cout << "\n";
	return toReturn;
}

// int main()
// {
// 	int n = 0; //total number of events

// 	// cout << "number of programs and number of variables\n";
// 	cin >> num_p >> num_var;

// 	trace.resize(num_p);
// 	std::vector<int> exec(num_p, 0);
// 	std::vector<int> num_events(num_p, 0);

// 	for(int i = 0; i < num_p; ++i)
// 	{
// 		cin >> num_events[i];
// 		n += num_events[i];
// 	}

// 	stMax.resize(num_p, std::vector<segTreeMax>(num_var+1));
// 	stMin.resize(num_p, std::vector<segTreeMin>(num_var+1));

// 	rep(i, 0, num_p)
// 	{
// 		rep(j, 0, num_var+1)
// 		{
// 			stMax[i][j].resize(num_p, num_events[i]);
// 			stMin[i][j].resize(num_p, num_events[i]);
// 		}
// 	}
// 	while(n--)
// 	{
// 		bool type;
// 		int pid;
// 		int var;

// 		cin >> type >> pid >> var;

// 		int eid = exec[pid]++;

// 		evnt e(num_p, num_var, type, eid, pid, var);
// 		trace[pid].pb(e);

// 		if(e.eid > 0)
// 			addEdge(trace[pid][e.eid-1], e, PO);
// 		else
// 		{
// 			if(e.type == WRITE)
// 				e.maxw[var][e.pid] = e.eid;
// 		}

// 		if(type == READ)
// 		{
// 			listAllRf(e);
// 			int x, y;
// 			cin >> x >> y;
// 			vi maxw = e.maxw[var];
// 			addEdge(trace[x][y], e, RF);
// 			rep(i, 0, num_p)
// 			{
// 				if(maxw[i] != -INF and i != x)
// 				{
// 					addEdge(trace[i][maxw[i]], trace[x][y], CO);
// 				}
// 			}
// 		}

// 		// there might have some changes made to e, thus need to update these changes in trace also
// 		trace[pid][exec[pid]-1] = e;
// 	}
// 	rep(i, 0, trace.size())
// 	{
// 		rep(j, 0, trace[i].size())
// 		{
// 			update(trace[i][j]);
// 			trace[i][j].print();
// 			linebreak();
// 		}
// 	}
// }