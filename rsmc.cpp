#include <bits/stdc++.h>
#include "main.cpp"

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

int main()
{
	int n = 0; //total number of events
	std::map<string, int> stoi; // string to index
	std::vector<string> itos;
	itos.pb("dummy");
	cin >> num_p;

	trace.resize(num_p);
	std::vector<int> exec(num_p, 0);
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
			//Take input the type of the event, w/r/i 
			cin >> ch;
			//Take input the variable name
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
			else if(ch == 'r' or ch == 'R')
			{
				c.type = READ;
			}
			else if(ch == 'i' or ch == 'I')
			{
				
			}

			program[i].pb(c);
		}
	}

	stMax.resize(num_p, std::vector<segTreeMax>(num_var+1));
	stMin.resize(num_p, std::vector<segTreeMin>(num_var+1));

	rep(i, 0, num_p)
	{
		rep(j, 0, num_var+1)
		{
			stMax[i][j].resize(num_p, num_events[i]);
			stMin[i][j].resize(num_p, num_events[i]);
		}
	}

	std::stack<Command> readCommands, writeCommands;

	rep(i, 0, num_p)
	{
		if(program[i][0].type == READ)
			readCommands.push(program[i][0]);
		else
			writeCommands.push(program[i][0]);
	}

	// genearting a random run
	while(n--)
	{
		if(!writeCommands.empty())
		{
			Command c = writeCommands.top();
			cout << "(" << c.pid << ", " << c.eid << ") ";
			cout << "w " << itos[c.var] << " = " << c.value << endl;

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

			int x = param[0].X, y = param[0].Y;
			e.parameter.X = x;
			e.parameter.Y = y;
			e.value = trace[x][y].value;
			cout << "(" << c.pid << ", " << c.eid << ") ";
			cout << "r " << itos[e.var] << " = " << e.value << "(" << x << ", " << y << ")" << endl;
			

			vi maxw = e.maxw[var];
			addEdge(trace[x][y], e, RF);

			rep(i, 0, num_p)
			{
				if(maxw[i] != -INF and i != x)
				{
					addEdge(trace[i][maxw[i]], trace[x][y], CO);
				}
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
		}
	}

	rep(i, 0, trace.size())
	{
		rep(j, 0, trace[i].size())
		{
			update(trace[i][j]);
			// trace[i][j].print();
			// linebreak();
		}
	}
}