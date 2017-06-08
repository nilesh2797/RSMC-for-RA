# RSMC-for-RA
main.cpp contains the implementation of our idea that we have suggested

segtree.cpp is just a helper file which contains implementation of segment tree with vector as it nodes

'in' file contans the sample input main.cpp expects

The input format is as follows : 

// first line expects two integers num_p, num_var which represents number of programs and number of shared variables

3//num_p 1//num_var

// next line contains num_p intergers, each specifying number of events in the program 

2 4 3

// next follows events, events are defined as tuple of 3 integers, with first integer representing whether its a write event(1) or read event(0), second integer represent the program id i.e. to ehich program it belongs and the third integer represents on which variable it is acting.

1(write event) 1(program id) 1(variable index)

1 1 1

1 2 1

1 2 1

// if its a read event you are prompted with the possible parameters for that read and you enter the choice of parameter

0 1 1

2 0

0 1 1

2 1

1 0 1

0 0 1

1 0

0 2 1

2 1
