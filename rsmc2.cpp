#include <bits/stdc++.h>
#include "main.cpp"
#include "trie.cpp"

std::vector<map<string, int> > registers;
bool halt = false;

// #define TRACE

struct Command
{
	int pid, eid;
	int type;
	int var, value, trueGoto, falseGoto;
	string localRegister, operand1, operand2, operation; // these are required for local arithmetic commands
	string operation_arc;
	Command()
	{
		type = -1;
		var = value = 0;
		trueGoto = falseGoto = INF;
		operation_arc = "";
	}

	int op1()
	{
		if(operand1.size() == 0)
			return 0;
		else
		{
			if(isdigit(operand1[0]))
				return atoi(operand1.c_str());
			else
				return registers[pid][operand1];
		}
	}

	int op2()
	{
		if(operand2.size() == 0)
			return 0;
		else
		{
			if(isdigit(operand2[0]))
				return atoi(operand2.c_str());
			else
				return registers[pid][operand2];
		}
	}

	int getResult()
	{
		if(operation == "+")
			return op1() + op2();
		if(operation == "*")
			return op1() * op2();
		if(operation == "-")
			return op1() - op2();
		if(operation == "/")
			return op1() / op2();
		if(operation == "==")
			return op1() == op2();
		if(operation == "<")
			return op1() < op2();
		if(operation == ">")
			return op1() > op2();
		if(operation == "xor")
			return op1() ^ op2();
		if(operation == "=")
			return op1();
	}


};

//Returns the tail of a string
std::string tail(std::string const& source, size_t const length) {
  if (length >= source.size()) { return source; }
  return source.substr(source.size() - length);
} // tail

int operand_value(Command c, int initial_value)
{
	if(c.operation_arc.empty())
		return initial_value;
	else
	{
		if(c.operation_arc[0]=='+')
			return initial_value+stoi(c.operation_arc.substr(1));
		else if(c.operation_arc[0]=='-')
			return initial_value-stoi(c.operation_arc.substr(1));
		else if(c.operation_arc[0]=='*')
			return initial_value*stoi(c.operation_arc.substr(1));
		else if(c.operation_arc[0]=='/')
			return initial_value/stoi(c.operation_arc.substr(1));
		else
			return initial_value;
	}	
}


std::vector<std::vector<Command> > program;
std::vector<int> exec, pexec;
int n = 0; //total number of events
std::map<string, int> vartoi; // string to index
std::vector<string> itos; // index to string
std::stack<Command> readCommands, writeCommands, localCommands, fenceCommands;
vnt commandExecuted; // stores the executed command
trie toBeExecuted, executed;
std::map<pair<string, int>, int> conditions;

int traceCount = 0;

void explore()
{

	while(true)
	{
		if(!localCommands.empty())
		{
			Command c = localCommands.top();
			localCommands.pop();

			if(c.operation == "jump")
			{
				#ifdef TRACE
				cout << "(" << c.pid << ", " << c.eid << ") jump " << c.trueGoto << endl;
				#endif

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
						if(c1.type == LOCAL)
							localCommands.push(c1);
						else
						{
							if(c1.type == FENCE)
								fenceCommands.push(c1);
						}
					}
				}

				// there might have some changes made to e, thus need to update these changes in trace also
				trace[pid][exec[pid]-1] = e;
				commandExecuted.pb(nodeType(c.pid, c.eid, e.eid, e.parameter.X, e.parameter.Y));
				continue;
			}
			if(c.operation == "if")
			{
				#ifdef TRACE
				cout << "(" << c.pid << ", " << c.eid << ") if " << c.localRegister  << "(" << registers[c.pid][c.localRegister] << ") == " << c.value << endl;
				#endif

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
							if(c1.type == LOCAL)
								localCommands.push(c1);
							else
							{
								if(c1.type == FENCE)
									fenceCommands.push(c1);
							}
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
							if(c1.type == LOCAL)
								localCommands.push(c1);
							else
							{
								if(c1.type == FENCE)
									fenceCommands.push(c1);
							}
						}
					}
				}

				// there might have some changes made to e, thus need to update these changes in trace also
				trace[pid][exec[pid]-1] = e;
				commandExecuted.pb(nodeType(c.pid, c.eid, e.eid, e.parameter.X, e.parameter.Y));

				continue;
			}

			#ifdef TRACE
			cout << "(" << c.pid << ", " << c.eid << ") " << c.localRegister << " = " << c.operand1  << " " << c.operation << " " << c.operand2 << endl;
			#endif

			int pid = c.pid, eid = exec[pid]++;

			evnt e(num_p, num_var, LOCAL, eid, pid, 0);
			e.cmd = ii(c.pid, c.eid);
			trace[pid].pb(e);

			registers[pid][c.localRegister] = c.getResult();

			if(e.eid > 0)
				addEdge(trace[pid][e.eid-1], e, PO);

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
 						if(c1.type == LOCAL)
 							localCommands.push(c1);
 						else
 						{
 							if(c1.type == FENCE)
 								fenceCommands.push(c1);
 						}
 					}
 				}
 			}

			// there might have some changes made to e, thus need to update these changes in trace also
			trace[pid][exec[pid]-1] = e;
			commandExecuted.pb(nodeType(c.pid, c.eid, e.eid, e.parameter.X, e.parameter.Y));
			continue;

		}

		if(!fenceCommands.empty())
		{
			Command c = fenceCommands.top();
			fenceCommands.pop();
			#ifdef TRACE
			cout << "(" << c.pid << ", " << c.eid << ")fence()\n";
			#endif

			int pid = c.pid, eid = exec[pid]++, var = num_var;

			evnt e(num_p, num_var, FENCE, eid, pid, var);
			e.cmd = ii(c.pid, c.eid);
			trace[pid].pb(e);


			if(e.eid > 0)
				addEdge(trace[pid][e.eid-1], e, PO);

			if(fenceRf != INVALID_PAIR)
			{
				int x = fenceRf.X, y = fenceRf.Y;
				addEdge(trace[x][y], e, RF);
			}

			fenceRf = ii(e.pid, e.eid);

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
 						if(c1.type == LOCAL)
 							localCommands.push(c1);
 						else
 						{
 							if(c1.type == FENCE)
 								fenceCommands.push(c1);
 						}
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
			//Write event is being committed in these lines
			#ifdef TRACE
			cout << "(" << c.pid << ", " << c.eid << ") w " << itos[c.var] << " = " << c.operand1 << endl;
			#endif

			writeCommands.pop();
			int pid = c.pid, eid = exec[pid]++, var = c.var;

			evnt e(num_p, num_var, c.type, eid, pid, var);
			e.cmd = ii(c.pid, c.eid);
			e.value = c.op1();
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
 						if(c1.type == LOCAL)
 							localCommands.push(c1);
 						else
 						{
 							if(c1.type == FENCE)
 								fenceCommands.push(c1);
 						}
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

				//This implies that initial value can be read and there are other choices also
				if(y==-1)
				{
					e.value=operand_value(c,0);
					rep(i, 0, param.size())
					{
						vnt possible = commandExecuted;
						possible.pb(nodeType(c.pid, c.eid, e.eid, param[i].X, param[i].Y));
						toBeExecuted.add(possible);
						// cout << "possible options = (" << param[i].X << ", " << param[i].Y << ")\n";
					}	
					#ifdef TRACE
					cout << "(" << c.pid << ", " << c.eid << ") r " << itos[e.var] << " = " << e.value << "(-1, -1)" << endl;
					#endif
				}
				else
				{
					e.value = operand_value(c,trace[x][y].value);

					vi maxw = e.maxw[var];
					addEdge(trace[x][y], e, RF);

					rep(i, 0, param.size())
					{
						vnt possible = commandExecuted;
						possible.pb(nodeType(c.pid, c.eid, e.eid, param[i].X, param[i].Y));
						toBeExecuted.add(possible);
						// cout << "possible options = (" << param[i].X << ", " << param[i].Y << ")\n";
					}

					rep(i, 0, num_p)
					{
						if(maxw[i] != -INF and i != x)
						{
							addEdge(trace[i][maxw[i]], trace[x][y], CO);
						}
					}
					#ifdef TRACE
					cout << "(" << c.pid << ", " << c.eid << ") r " << itos[e.var] << " = " << e.value << "(" << trace[e.parameter.X][e.parameter.Y].cmd.X << ", " << trace[e.parameter.X][e.parameter.Y].cmd.Y << ")" << endl;
					#endif
				}	

			}
			else
			{
				e.parameter.X = -1;
				e.parameter.Y = -1;
				e.value=operand_value(c,0);
				#ifdef TRACE
				cout << "(" << c.pid << ", " << c.eid << ") r " << itos[e.var] << " = " << e.value << "(-1, -1)" << endl;
				#endif
			}

			

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
 						if(c1.type == LOCAL)
 							localCommands.push(c1);
 						else
 						{
 							if(c1.type == FENCE)
 								fenceCommands.push(c1);
 						}
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

	bool flag = true;
	for(map<pair<string, int>, int >::iterator it = conditions.begin(); it != conditions.end(); ++it)
	{
		// cout << (it->X).X << " = " << registers[(it->X).Y][(it->X).X] << " cmp " << it->Y;
		if(registers[(it->X).Y][(it->X).X] != it->Y)
		{
			flag = false;
			break;
		}
	}

	++traceCount;

	if(flag)
	{
		cout << "\nCONDITION SATISFIED\n";
		halt = true;
	}

	#ifdef TRACE
	cout << "\ntraceCount = " << traceCount << endl;
	linebreak();
	cout << endl;
	// rep(i, 0, trace.size())
	// {
	// 	rep(j, 0, trace[i].size())
	// 	{
	// 		update(trace[i][j]);
	// 		trace[i][j].print();
	// 		linebreak();
	// 	}
	// }
	cout << endl;
	#endif
}

void traverse(vnt toBeTraversed)
{
	rep(i, 0, toBeTraversed.size())
	{
		nodeType cur = toBeTraversed[i];
		Command c = program[cur.pid][cur.ceid];
		pexec[cur.pid] = cur.ceid;

		if(c.type == LOCAL)
		{
			if(c.operation == "jump")
			{
				#ifdef TRACE
				cout << "(" << c.pid << ", " << c.eid << ") jump " << c.trueGoto << endl;
				#endif

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
			if(c.operation == "if")
			{
				#ifdef TRACE
				cout << "(" << c.pid << ", " << c.eid << ") if " << c.localRegister  << "(" << registers[c.pid][c.localRegister] << ") == " << c.value << endl;
				#endif

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

			#ifdef TRACE
			cout << "(" << c.pid << ", " << c.eid << ") " << c.localRegister << " = " << c.operand1  << " " << c.operation << " " << c.operand2 << endl;
			#endif

			int pid = c.pid, eid = exec[pid]++;

			evnt e(num_p, num_var, LOCAL, eid, pid, 0);
			e.cmd = ii(c.pid, c.eid);
			trace[pid].pb(e);

			registers[pid][c.localRegister] = c.getResult();

			if(e.eid > 0)
				addEdge(trace[pid][e.eid-1], e, PO);

			// there might have some changes made to e, thus need to update these changes in trace also
			trace[pid][exec[pid]-1] = e;
			commandExecuted.pb(nodeType(c.pid, c.eid, e.eid, e.parameter.X, e.parameter.Y));
			continue;
		}

		if(c.type == FENCE)
		{
			#ifdef TRACE
			cout << "(" << c.pid << ", " << c.eid << ")fence()\n";
			#endif

			int pid = c.pid, eid = exec[pid]++, var = num_var;

			evnt e(num_p, num_var, FENCE, eid, pid, var);
			e.cmd = ii(c.pid, c.eid);
			trace[pid].pb(e);


			if(e.eid > 0)
				addEdge(trace[pid][e.eid-1], e, PO);

			if(fenceRf != INVALID_PAIR)
			{
				int x = fenceRf.X, y = fenceRf.Y;
				addEdge(trace[x][y], e, RF);
			}

			fenceRf = ii(e.pid, e.eid);

			// there might have some changes made to e, thus need to update these changes in trace also
			trace[pid][exec[pid]-1] = e;
			commandExecuted.pb(nodeType(c.pid, c.eid, e.eid, e.parameter.X, e.parameter.Y));
			continue;
		}

		if(c.type == WRITE)
		{
			#ifdef TRACE
			cout << "(" << c.pid << ", " << c.eid << ") w " << itos[c.var] << " = " << c.operand1 << endl;
			#endif

			int pid = c.pid, eid = exec[pid]++, var = c.var;

			evnt e(num_p, num_var, c.type, eid, pid, var);
			e.cmd = ii(c.pid, c.eid);
			e.value = c.op1();
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
				e.value=operand_value(c,trace[x][y].value);

				vi maxw = e.maxw[var];
				addEdge(trace[x][y], e, RF);

				rep(i, 0, num_p)
				{
					if(maxw[i] != -INF and i != x)
					{
						addEdge(trace[i][maxw[i]], trace[x][y], CO);
					}
				}
				#ifdef TRACE
				cout << "(" << c.pid << ", " << c.eid << ") r " << itos[e.var] << " = " << e.value << "(" << trace[x][y].cmd.X << ", " << trace[x][y].cmd.Y << ")" << endl;
				#endif
			}
			else
			{
				e.value=operand_value(c,0);
				#ifdef TRACE
				cout << "(" << c.pid << ", " << c.eid << ") r " << itos[e.var] << " = " << e.value << "(-1, -1)" <<endl;
				#endif
			}

			

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

				if(!vartoi[s])
				{
					vartoi[s] = ++num_var;
					itos.pb(s);
				}

				c.var = vartoi[s];

				cin >> c.operand1;
			}
			else
			{
				if(ch == 'r' or ch == 'R')
				{
					c.type = READ;
					//Take input the variable name/operation_arc
					cin >> s;
					bool operator_found=true;
					std::size_t found = s.find('+');
  					if (found==std::string::npos)
  					{
  						found = s.find('-');
  						if(found==std::string::npos)
  						{
  							found = s.find('*');
  							if(found==std::string::npos)
  							{
  								found = s.find('/');
  								if(found==std::string::npos)
  									operator_found=false;
  							}
  						}
  					}
  					if(operator_found==true)
  					{////Execute only if there is an operator in the expression
  						c.operation_arc=s;
  						s=s.substr(0,found);
  						c.operation_arc=c.operation_arc.substr(found);
  					}
					if(!vartoi[s])
					{
						vartoi[s] = ++num_var;
						itos.pb(s);
					}

					c.var = vartoi[s];

					cin >> c.localRegister;
				}
				else
				{
					if(ch == 'l' or ch == 'L')
					{
						c.type = LOCAL;
						cin >> c.operation;
						if(c.operation == "jump")
							cin >> c.trueGoto;
						else
						{
							if(c.operation == "if")
							{
								cin >> c.localRegister >> c.value >> c.trueGoto >> c.falseGoto;
							}
							else
							{
								if(c.operation == "=")
									cin >> c.localRegister >> c.operand1;
								else
									cin >> c.localRegister >> c.operand1 >> c.operand2;
							}
						}
					}
					else
					{
						if(ch == 'f' or ch == 'F')
						{
							c.type = FENCE;
						}
					}
				}
			}

			program[i].pb(c);
		}
	}
	string fence_var = "fence";
	vartoi[fence_var] = ++num_var;
	itos.pb(fence_var);

	pair<string, int> s;
	int value;
	while(cin >> s.first >> s.second >> value)
	{
		conditions[s] = value;
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
	localCommands = stack<Command>();
	fenceCommands = stack<Command>();
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
				if(program[i][0].type == LOCAL)
					localCommands.push(program[i][0]);
				else
				{
					if(program[i][0].type == FENCE)
						fenceCommands.push(program[i][0]);
				}
			}
		}
	}

	// genearting a random run
	explore();

	while(!toBeExecuted.empty())
	{
		if(halt)
		{
			break;
		}
		// clear out things
		fenceRf = INVALID_PAIR;
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
		localCommands = stack<Command>();
		fenceCommands = stack<Command>();
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
						if(program[i][pexec[i]+1].type == LOCAL)
							localCommands.push(program[i][pexec[i]+1]);
						else
						{
							if(program[i][pexec[i]+1].type == FENCE)
								fenceCommands.push(program[i][pexec[i]+1]);
						}
					}
				}
			}
		}

		explore();
		// if(traceCount > 500)
		// 	break;
	}
	cout << "traceCount = " << traceCount << endl;
}