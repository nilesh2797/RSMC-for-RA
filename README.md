# RSMC-for-RA

Weak memory models for programming languages formalize the set of behaviors that multithreaded programs may exhibit taking
into account the effect of both the hardware architectures and compiler optimizations. An important such model is the C11 model introduced in the 2011 revisions of the C and C++ standards. C11 model mostly operates on the Release-Acquire semantics (RA) and thus we developed a Relaxed Stateless Model Checking algorithm for the same.  

# File Description And Specification
rsmc.cpp is the main file which implements the RSMC algorithm for RA model. to compile it simply execute g++ rsmc.cpp and then run ./a.out < in, where 'in' is the input file 

main.cpp contains the implementation of our idea that we have suggested to maintain the coherence order and happens before order via vector clocks

segtree.cpp is just a helper file which contains implementation of segment tree with vector as it nodes

# Inpt/Output Format
'in' file contans the sample input rsmc.cpp expects

the first line expects number of program threads(num_p)

the next line expects an array of integers num_events[0...num_p) with num_events[i] denoting number of commands in thread i

now for each thread i you enter num_events[i] commands

Format of a command :

first you enter type of command('w', 'r' and 'l')

'w' denotes a write command, if the type is 'w' you then enter <variable_name>(string) followed by <value>(int)
for e.g. "w x 1"

'r' denotes a read command, if the type is 'r' you then enter <variable_name>(string) followed by the <local_register_name>(string) in which you are going to store the read value.
for e.g. "r x r1"

'l' denotes a local command such as "if", "jump", "+", "<" and other arithmetic/logical operations, if the type is 'l' then you enter <local_command_type>(string)[possible values : "if", "jump", "+", "-", etc].

if the local command is "if" then you specify <local_register_name>(string) then <value>(int) then <if_true_then_goto>(int) then <if_false_then_goto>(int)
for e.g. "l if r1 1 4 8" this command means if(r1 == 1) then goto 4 else goto 8

if the local command is "jump", then you only need to specify the <line_number_to_jump>(int)
for e.g. "l jump 6" means goto line 6

if the local command is a arithmetic or logical operation, then the format is as following :
l <operation _symbol>(string) <local_register_name_to_store_result>(string) <operand1>(string) <operand2>(string)
for e.g. "l + r1 r1 r2" this means r1 = r1 + r2
