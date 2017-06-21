#include <bits/stdc++.h>
#include "main.cpp"
#include "trie.cpp"

struct Command
{
	int pid, eid;
	int type;
	int var, value, trueGoto, falseGoto;
	string localRegister;
	Command()
	{
		type = -1;
		var = value = 0;
		trueGoto = falseGoto = INF;
	}
};

std::vector<std::vector<Command> > program;
std::vector<int> exec, pexec;
int n = 0; //total number of events
std::map<string, int> stoi; // string to index
std::vector<string> itos; // index to string
std::stack<Command> readCommands, writeCommands, ifCommands, jumpCommands;
vnt commandExecuted; // stores the executed command
trie toBeExecuted, executed;
std::vector<map<string, int> > registers;

int traceCount = 0;

void explore()
{

	while(true)
	{
		if(!jumpCommands.empty())
		{
			Command c = jumpCommands.top();
			cout << "(" << c.pid << ", " << c.eid << ") jump " << c.trueGoto << endl;

			jumpCommands.pop();
			int pid = c.pid, eid = exec[pid]++;

			evnt e(num_p, num_var, LOCAL, eid, pid, 0);
			e.cmd = ii(c.pid, c.eid);
			trace[pid].pb(e);

			if(e.eid > 0)
				addEdge(trace[pid][e.eid-1], e, PO);

			Command c1 = program[pid][c.trueGoto];
			if(c1.type == READ)
				readCommands.push(c1);
			else
			{
				if(c1.type == WRITE)
					writeCommands.push(c1);
				else
				{
					if(c1.type == IF)
						ifCommands.push(c1);
					else
						jumpCommands.push(c1);
				}
			}

			// there might have some changes made to e, thus need to update these changes in trace also
			trace[pid][exec[pid]-1] = e;
			commandExecuted.pb(nodeType(c.pid, c.eid, e.eid, e.parameter.X, e.parameter.Y));

			continue;
		}

		if(!ifCommands.empty())
		{
			Command c = ifCommands.top();
			cout << "(" << c.pid << ", " << c.eid << ") if " << c.localRegister  << "(" << registers[c.pid][c.localRegister] << ") == " << c.value << endl;

			ifCommands.pop();
			int pid = c.pid, eid = exec[pid]++;

			evnt e(num_p, num_var, LOCAL, eid, pid, 0);
			e.cmd = ii(c.pid, c.eid);
			trace[pid].pb(e);

			if(e.eid > 0)
				addEdge(trace[pid][e.eid-1], e, PO);

			if(c.value == registers[c.pid][c.localRegister])
			{

				Command c1 = program[pid][c.trueGoto];
				if(c1.type == READ)
					readCommands.push(c1);
				else
				{
					if(c1.type == WRITE)
						writeCommands.push(c1);
					else
					{
						if(c1.type == IF)
							ifCommands.push(c1);
						else
							jumpCommands.push(c1);
					}
				}
			}
			else
			{
				Command c1 = program[pid][c.falseGoto];
				if(c1.type == READ)
					readCommands.push(c1);
				else
				{
					if(c1.type == WRITE)
						writeCommands.push(c1);
					else
					{
						if(c1.type == IF)
							ifCommands.push(c1);
						else
							jumpCommands.push(c1);
					}
				}
			}

			// there might have some changes made to e, thus need to update these changes in trace also
			trace[pid][exec[pid]-1] = e;
			commandExecuted.pb(nodeType(c.pid, c.eid, e.eid, e.parameter.X, e.parameter.Y));

			continue;
		}

		if(!writeCommands.empty())
		{
			Command c = writeCommands.top();
			cout << "(" << c.pid << ", " << c.eid << ") w " << itos[c.var] << " = " << c.value << endl;

			writeCommands.pop();
			int pid = c.pid, eid = exec[pid]++, var = c.var;

			evnt e(num_p, num_var, c.type, eid, pid, var);
			e.cmd = ii(c.pid, c.eid);
			e.value = c.value;
			trace[pid].pb(e);

			if(e.eid > 0)
				addEdge(trace[pid][e.eid-1], e, PO);
			else
			{
				e.maxw[var][e.pid] = e.eid;
			}

			update(e);
			vii conflicts = listAllReads(e);
			rep(i, 0, conflicts.size())
			{
				ii cur = conflicts[i];
				// can optimize this
				vnt possible; 
				int cutFromHere;
				rep(j, 0, commandExecuted.size())
				{
					if(cur == ii(commandExecuted[j].pid, commandExecuted[j].teid))
					{
						cutFromHere = j;
						break;
					}
					possible.pb(commandExecuted[j]);
				}
				rep(j, cutFromHere+1, commandExecuted.size())
				{
					nodeType cmd = commandExecuted[j];
					if(e.pre[0][cmd.pid] >= cmd.teid)
						possible.pb(commandExecuted[j]);
				}
				possible.pb(nodeType(c.pid, c.eid, e.eid, INF, INF));
				possible.pb(nodeType(cur.X, trace[cur.X][cur.Y].cmd.Y, cur.Y, e.pid, e.eid));
				if(!executed.find(possible))
				{
					toBeExecuted.add(possible);
					executed.add(possible);
					// rep(i, 0, possible.size())
					// {
					// 	cout << "((" << possible[i].X.X << ", " << possible[i].X.Y << ")(" << possible[i].Y.X << ", " << possible[i].Y.Y << "))\n";
					// }
					// cout << "reversed (" << e.pid << ", " << e.eid << ") and (" << cur.X << ", " << cur.Y << ")\n";
				}
 			}

 			if(program[pid].size() > c.eid+1)
 			{
 				Command c1 = program[pid][c.eid+1];
 				if(c1.type == READ)
 					readCommands.push(c1);
 				else
 				{
 					if(c1.type == WRITE)
 						writeCommands.push(c1);
 					else
 					{
 						if(c1.type == IF)
 							ifCommands.push(c1);
 						else
 							jumpCommands.push(c1);
 					}
 				}
 			}

			// there might have some changes made to e, thus need to update these changes in trace also
			trace[pid][exec[pid]-1] = e;
			commandExecuted.pb(nodeType(c.pid, c.eid, e.eid, e.parameter.X, e.parameter.Y));
			continue;
		}

		if(!readCommands.empty())
		{
			Command c = readCommands.top();
			readCommands.pop();
			int pid = c.pid, eid = exec[pid]++, var = c.var;

			evnt e(num_p, num_var, c.type, eid, pid, var);
			e.cmd = ii(c.pid, c.eid);
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
					vnt possible = commandExecuted;
					possible.pb(nodeType(c.pid, c.eid, e.eid, param[i].X, param[i].Y));
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

			cout << "(" << c.pid << ", " << c.eid << ") r " << itos[e.var] << " = " << e.value << "(" << trace[e.parameter.X][e.parameter.Y].cmd.X << ", " << trace[e.parameter.X][e.parameter.Y].cmd.Y << ")" << endl;

			registers[pid][c.localRegister] = e.value;

			if(program[pid].size() > c.eid+1)
 			{
 				Command c1 = program[pid][c.eid+1];
 				if(c1.type == READ)
 					readCommands.push(c1);
 				else
 				{
 					if(c1.type == WRITE)
 						writeCommands.push(c1);
 					else
 					{
 						if(c1.type == IF)
 							ifCommands.push(c1);
 						else
 							jumpCommands.push(c1);
 					}
 				}
 			}

			// there might have some changes made to e, thus need to update these changes in trace also
			trace[pid][exec[pid]-1] = e;
			commandExecuted.pb(nodeType(c.pid, c.eid, e.eid, e.parameter.X, e.parameter.Y));
			continue;
		}

		break;
	}

	toBeExecuted.add(commandExecuted); // I know this looks weird but this is necessary, will fix later
	toBeExecuted.remove(commandExecuted);

	cout << "\ntraceCount = " << ++traceCount << endl;
	cout << endl;
	linebreak();
	cout << endl;
}

void traverse(vnt toBeTraversed)
{
	rep(i, 0, toBeTraversed.size())
	{
		nodeType cur = toBeTraversed[i];
		Command c = program[cur.pid][cur.ceid];
		pexec[cur.pid] = cur.ceid;

		if(c.type == JUMP)
		{
			cout << "(" << c.pid << ", " << c.eid << ") jump " << c.trueGoto << endl;

			int pid = c.pid, eid = exec[pid]++;

			evnt e(num_p, num_var, LOCAL, eid, pid, 0);
			e.cmd = ii(c.pid, c.eid);
			trace[pid].pb(e);

			if(e.eid > 0)
				addEdge(trace[pid][e.eid-1], e, PO);

			// there might have some changes made to e, thus need to update these changes in trace also
			trace[pid][exec[pid]-1] = e;
			commandExecuted.pb(nodeType(c.pid, c.eid, e.eid, e.parameter.X, e.parameter.Y));

			continue;
		}

		if(c.type == IF)
		{
			cout << "(" << c.pid << ", " << c.eid << ") if " << c.localRegister  << "(" << registers[c.pid][c.localRegister] << ") == " << c.value << endl;

			int pid = c.pid, eid = exec[pid]++;

			evnt e(num_p, num_var, LOCAL, eid, pid, 0);
			e.cmd = ii(c.pid, c.eid);
			trace[pid].pb(e);

			if(e.eid > 0)
				addEdge(trace[pid][e.eid-1], e, PO);

			// there might have some changes made to e, thus need to update these changes in trace also
			trace[pid][exec[pid]-1] = e;
			commandExecuted.pb(nodeType(c.pid, c.eid, e.eid, e.parameter.X, e.parameter.Y));

			continue;
		}

		if(c.type == WRITE)
		{
			cout << "(" << c.pid << ", " << c.eid << ") w " << itos[c.var] << " = " << c.value << endl;

			int pid = c.pid, eid = exec[pid]++, var = c.var;

			evnt e(num_p, num_var, c.type, eid, pid, var);
			e.cmd = ii(c.pid, c.eid);
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
			commandExecuted.pb(nodeType(c.pid, c.eid, e.eid, e.parameter.X, e.parameter.Y));

			continue;
		}
		
		if(c.type == READ)
		{
			int pid = c.pid, eid = exec[pid]++, var = c.var;

			evnt e(num_p, num_var, c.type, eid, pid, var);
			e.cmd = ii(c.pid, c.eid);
			trace[pid].pb(e);

			if(e.eid > 0)
				addEdge(trace[pid][e.eid-1], e, PO);

			int x = cur.ppid, y = cur.peid;
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

			cout << "(" << c.pid << ", " << c.eid << ") r " << itos[e.var] << " = " << e.value << "(" << trace[x][y].cmd.X << ", " << trace[x][y].cmd.Y << ")" << endl;

			registers[pid][c.localRegister] = e.value;

			// there might have some changes made to e, thus need to update these changes in trace also
			trace[pid][exec[pid]-1] = e;
			commandExecuted.pb(nodeType(c.pid, c.eid, e.eid, e.parameter.X, e.parameter.Y));

			continue;
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

			if(ch == 'w' or ch == 'W')
			{
				c.type = WRITE;
				cin >> s;

				if(!stoi[s])
				{
					stoi[s] = ++num_var;
					itos.pb(s);
				}

				c.var = stoi[s];

				int value;
				cin >> value;
				c.value = value;
			}
			else
			{
				if(ch == 'r' or ch == 'R')
				{
					c.type = READ;
					cin >> s;

					if(!stoi[s])
					{
						stoi[s] = ++num_var;
						itos.pb(s);
					}

					c.var = stoi[s];

					cin >> c.localRegister;
				}
				else
				{
					if(ch == 'j' or ch == 'J')
					{
						c.type = JUMP;
						cin >> c.trueGoto;
					}
					else
					{
						c.type = IF;
						cin >> c.localRegister;
						cin >> c.value >> c.trueGoto >> c.falseGoto;
					}
				}
			}

			program[i].pb(c);
		}
	}

	trace.clear();
	trace.resize(num_p);
	exec.clear();
	exec.resize(num_p, 0);
	registers.clear();
	registers.resize(num_p);

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
	ifCommands = stack<Command>();
	jumpCommands = stack<Command>();
	commandExecuted.clear();

	rep(i, 0, num_p)
	{
		if(program[i][0].type == READ)
			readCommands.push(program[i][0]);
		else
		{
			if(program[i][0].type == WRITE)
				writeCommands.push(program[i][0]);
			else
			{
				if(program[i][0].type == IF)
					ifCommands.push(program[i][0]);
				else
				{
					jumpCommands.push(program[i][0]);
				}
			}
		}
	}

	// genearting a random run
	explore();

	while(!toBeExecuted.empty())
	{
		// clear out things
		trace.clear();
		trace.resize(num_p);
		pexec.clear();
		pexec.resize(num_p, -1);
		exec.clear();
		exec.resize(num_p, 0);
		registers.clear();
		registers.resize(num_p);

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
		ifCommands = stack<Command>();
		commandExecuted.clear();

		vnt toBeTraversed = toBeExecuted.getRun();
		// cout << "toBeTraversed = \n"; 
		// rep(i, 0, toBeTraversed.size())
		// {
		// 	cout << "((" << toBeTraversed[i].X.X << ", " << toBeTraversed[i].X.Y << ")(" << toBeTraversed[i].Y.X << ", " << toBeTraversed[i].Y.Y << "))\n";
		// }
		traverse(toBeTraversed);

		rep(i, 0, num_p)
		{
			// cout << "exec[" << i << "] = " << exec[i] << endl;
			if(pexec[i]+1 < num_events[i])
			{
				if(program[i][pexec[i]+1].type == READ)
					readCommands.push(program[i][pexec[i]+1]);
				else
				{
					if(program[i][pexec[i]+1].type == WRITE)
						writeCommands.push(program[i][pexec[i]+1]);
					else
					{
						if(program[i][pexec[i]+1].type == JUMP)
							jumpCommands.push(program[i][pexec[i]+1]);
						else
							ifCommands.push(program[i][pexec[i]+1]);
					}
				}
			}
		}

		explore();
		// if(traceCount > 500)
		// 	break;
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