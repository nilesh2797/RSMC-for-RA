#include <bits/stdc++.h>
#include "main.cpp"
#include "trie.cpp"

struct Command
{
	int pid, eid;
	bool type;
	int var, value;
	Command()
	{
		type = -1;
		var = value = 0;
	}
};

std::vector<std::vector<Command> > program;
std::vector<int> exec;
int n = 0; //total number of events
std::map<string, int> stoi; // string to index
std::vector<string> itos;
std::stack<Command> readCommands, writeCommands;
viiii commandExecuted;
trie toBeExecuted;

int traceCount = 0;

void explore(int iter)
{
	#ifdef DEBUG
	cout << "iter = " << iter << endl;
	#endif

	while(iter--)
	{
		if(!writeCommands.empty())
		{
			Command c = writeCommands.top();
			cout << "(" << c.pid << ", " << c.eid << ") w " << itos[c.var] << " = " << c.value << endl;

			writeCommands.pop();
			int pid = c.pid, eid = c.eid, var = c.var;
			exec[pid]++;

			evnt e(num_p, num_var, c.type, eid, pid, var);
			e.value = c.value;
			trace[pid].pb(e);

			if(e.eid > 0)
				addEdge(trace[pid][e.eid-1], e, PO);
			else
			{
				e.maxw[var][e.pid] = e.eid;
			}

			if(program[pid].size() > eid+1)
			{
				Command c1 = program[pid][eid+1];
				if(c1.type == WRITE)
					writeCommands.push(c1);
				else
					readCommands.push(c1);
			}

			// there might have some changes made to e, thus need to update these changes in trace also
			trace[pid][exec[pid]-1] = e;
			commandExecuted.pb(iiii(ii(e.pid, e.eid), ii(e.parameter.X, e.parameter.Y)));
		}
		else
		{
			Command c = readCommands.top();
			readCommands.pop();
			int pid = c.pid, eid = c.eid, var = c.var;
			exec[pid]++;

			evnt e(num_p, num_var, c.type, eid, pid, var);
			trace[pid].pb(e);

			if(e.eid > 0)
				addEdge(trace[pid][e.eid-1], e, PO);

			std::vector<ii> param = listAllRf(e);

			if(param.size())
			{
				int x = param[0].X, y = param[0].Y;
				e.parameter.X = x;
				e.parameter.Y = y;
				e.value = trace[x][y].value;

				vi maxw = e.maxw[var];
				addEdge(trace[x][y], e, RF);

				rep(i, 0, param.size())
				{
					viiii possible = commandExecuted;
					possible.pb(iiii(ii(e.pid, e.eid), ii(param[i].X, param[i].Y)));
					toBeExecuted.add(possible);
				}

				rep(i, 0, num_p)
				{
					if(maxw[i] != -INF and i != x)
					{
						addEdge(trace[i][maxw[i]], trace[x][y], CO);
					}
				}
			}
			else
			{
				e.parameter.X = -1;
				e.parameter.Y = -1;
				e.value = 0;
			}

			cout << "(" << c.pid << ", " << c.eid << ") r " << itos[e.var] << " = " << e.value << "(" << e.parameter.X << ", " << e.parameter.Y << ")" << endl;

			if(program[pid].size() > eid+1)
			{
				Command c1 = program[pid][eid+1];
				if(c1.type == WRITE)
					writeCommands.push(c1);
				else
					readCommands.push(c1);
			}

			// there might have some changes made to e, thus need to update these changes in trace also
			trace[pid][exec[pid]-1] = e;
			commandExecuted.pb(iiii(ii(e.pid, e.eid), ii(e.parameter.X, e.parameter.Y)));
		}
	}
	toBeExecuted.remove(commandExecuted);

	cout << "traceCount = " << ++traceCount << endl;
	cout << endl;
	linebreak();
	cout << endl;
}

void traverse(viiii toBeTraversed)
{
	rep(i, 0, toBeTraversed.size())
	{
		iiii cur = toBeTraversed[i];
		Command c = program[cur.X.X][cur.X.Y];
		if(c.type == WRITE)
		{
			cout << "(" << c.pid << ", " << c.eid << ") w " << itos[c.var] << " = " << c.value << endl;

			int pid = c.pid, eid = c.eid, var = c.var;
			exec[pid]++;

			evnt e(num_p, num_var, c.type, eid, pid, var);
			e.value = c.value;
			trace[pid].pb(e);

			if(e.eid > 0)
				addEdge(trace[pid][e.eid-1], e, PO);
			else
			{
				e.maxw[var][e.pid] = e.eid;
			}

			// there might have some changes made to e, thus need to update these changes in trace also
			trace[pid][exec[pid]-1] = e;
			commandExecuted.pb(iiii(ii(e.pid, e.eid), ii(e.parameter.X, e.parameter.Y)));
		}
		else
		{
			int pid = c.pid, eid = c.eid, var = c.var;
			exec[pid]++;

			evnt e(num_p, num_var, c.type, eid, pid, var);
			trace[pid].pb(e);

			if(e.eid > 0)
				addEdge(trace[pid][e.eid-1], e, PO);

			int x = cur.Y.X, y = cur.Y.Y;
			e.parameter.X = x;
			e.parameter.Y = y;

			if(x > -1 and y > -1)
			{
				e.value = trace[x][y].value;

				vi maxw = e.maxw[var];
				addEdge(trace[x][y], e, RF);

				rep(i, 0, num_p)
				{
					if(maxw[i] != -INF and i != x)
					{
						addEdge(trace[i][maxw[i]], trace[x][y], CO);
					}
				}
			}
			else
			{
				e.value = 0;
			}

			cout << "(" << c.pid << ", " << c.eid << ") r " << itos[e.var] << " = " << e.value << "(" << x << ", " << y << ")" << endl;

			// there might have some changes made to e, thus need to update these changes in trace also
			trace[pid][exec[pid]-1] = e;
			commandExecuted.pb(iiii(ii(e.pid, e.eid), ii(e.parameter.X, e.parameter.Y)));
		}
	}
}

int main()
{
	itos.pb("dummy");
	cin >> num_p;

	program.resize(num_p);

	vi num_events(num_p, 0);
	rep(i, 0, num_p)
	{
		cin >> num_events[i];
		n += num_events[i];
	}

	rep(i, 0, num_p)
	{
		rep(j, 0, num_events[i])
		{
			char ch;
			Command c;
			string s;

			c.pid = i;
			c.eid = j;
			cin >> ch;
			cin >> s;

			if(!stoi[s])
			{
				stoi[s] = ++num_var;
				itos.pb(s);
			}

			c.var = stoi[s];

			if(ch == 'w' or ch == 'W')
			{
				c.type = WRITE;
				int value;
				cin >> value;
				c.value = value;
			}
			else
			{
				c.type = READ;
			}

			program[i].pb(c);
		}
	}

	trace.clear();
	trace.resize(num_p);
	exec.clear();
	exec.resize(num_p, 0);

	stMax.clear();
	stMax.resize(num_p, std::vector<segTreeMax>(num_var+1));
	stMin.clear();
	stMin.resize(num_p, std::vector<segTreeMin>(num_var+1));

	rep(i, 0, num_p)
	{
		rep(j, 0, num_var+1)
		{
			stMax[i][j].resize(num_p, num_events[i]);
			stMin[i][j].resize(num_p, num_events[i]);
		}
	}

	readCommands = stack<Command>();
	writeCommands = stack<Command>();
	commandExecuted.clear();

	rep(i, 0, num_p)
	{
		if(program[i][0].type == READ)
			readCommands.push(program[i][0]);
		else
			writeCommands.push(program[i][0]);
	}

	// genearting a random run
	explore(n);

	while(!toBeExecuted.empty())
	{
		// clear out things
		trace.clear();
		trace.resize(num_p);
		exec.clear();
		exec.resize(num_p, 0);

		stMax.clear();
		stMax.resize(num_p, std::vector<segTreeMax>(num_var+1));
		stMin.clear();
		stMin.resize(num_p, std::vector<segTreeMin>(num_var+1));

		rep(i, 0, num_p)
		{
			rep(j, 0, num_var+1)
			{
				stMax[i][j].resize(num_p, num_events[i]);
				stMin[i][j].resize(num_p, num_events[i]);
			}
		}

		readCommands = stack<Command>();
		writeCommands = stack<Command>();
		commandExecuted.clear();

		viiii toBeTraversed = toBeExecuted.getRun();
		// rep(i, 0, toBeTraversed.size())
		// {
		// 	cout << "((" << toBeTraversed[i].X.X << ", " << toBeTraversed[i].X.Y << ")(" << toBeTraversed[i].Y.X << ", " << toBeTraversed[i].Y.Y << "))\n";
		// }
		traverse(toBeTraversed);

		rep(i, 0, num_p)
		{
			// cout << "exec[" << i << "] = " << exec[i] << endl;
			if(exec[i] < num_events[i])
			{
				if(program[i][exec[i]].type == READ)
					readCommands.push(program[i][exec[i]]);
				else
					writeCommands.push(program[i][exec[i]]);
			}
		}

		explore(n - toBeTraversed.size());
		if(traceCount > 500)
			break;
	}
	

	// rep(i, 0, trace.size())
	// {
	// 	rep(j, 0, trace[i].size())
	// 	{
	// 		update(trace[i][j]);
	// 		trace[i][j].print();
	// 		linebreak();
	// 	}
	// }
}