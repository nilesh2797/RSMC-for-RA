# RSMC-for-RA

Weak memory models for programming languages formalize the set of behaviors that multithreaded programs may exhibit taking
into account the effect of both the hardware architectures and compiler optimizations. An important such model is the C11 model introduced in the 2011 revisions of the C and C++ standards. C11 model mostly operates on the Release-Acquire semantics (RA) and thus we developed a Relaxed Stateless Model Checking algorithm for the same.  

# File Description And Specification
rsmc.cpp is the main file which implements the RSMC algorithm for RA model. to compile it simply execute g++ rsmc.cpp and then run ./a.out < in, where 'in' is the input file 

main.cpp contains the implementation of our idea that we have suggested to maintain the coherence order and happens before order via vector clocks

segtree.cpp is just a helper file which contains implementation of segment tree with vector as it nodes

# Inpt/Output Format
'in' file contans the sample input rsmc.cpp expects

The input format is as follows : 

first line expects integer specifying number of program threads num_p

then next line expects array of integers (num_events[0...num_p)) which is number of events in each thread
