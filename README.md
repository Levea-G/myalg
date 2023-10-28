extendint.h is for C/C++ users, compiler ver c99 and above is required, please follow the instructions in the file  
  
extendint++.h is for C++ users only, with most operators reconstructed, a^b represents pow(a,b), other operators are the same as normal datatypes  
  
the two files above are experimental high accuracy calculation functions for integers, may still have some unexpeceted bugs left  

DateType is a pre-defined datatype which presents a date in the form of yyyy-mm-dd, I set the maximum date to year 9999, it has a function to turn strings to Date type, and basic comparisons have been reconstructed

Chris.h is a header for C++ users, featuring rb-tree structure which adjusts memory usage according to current node count, meanwhile being twice as fast as stl set when inserting and hundred times faster than stl set when freeing the memory

note:1. extendint:fft or ntt is NOT used in extendint algorithm, which means it might take huge amount of time calculationg big numbers, this might be optimized some day when I actually learn them  
2. extendint:due to my ignorance, bit opertations can yet be realized, this might NOT get optimized since i dont know how  
3. extendint:in my ide, errors pop up when I set the 'maxlength' variable too big (about 5e4), I guess there's some stack overflow issue, so be really carefull when modifying this  
4. Chris:in the structure, ll means int, which is not but a personal habit. I didn't name an iterator class for Chris, but you can just use [] operator in the way like tree[10], which locates the 10th key in 'tree', it'll return a pointer pointing at the key, to get the key itself, use *tree[10]. 'find' method returns the rank of the given key in the tree, and returns -1 if the key doesn't exist. 'view' method does a dfs through the tree, you can set the seperating and ending characters, but this method requires either an extra 'print' method telling it how to print the key or overloading << operator in the key class. 
