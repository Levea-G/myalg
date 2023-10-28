extendint.h is for C/C++ users, compiler ver c99 and above is required, please follow the instructions in the file  
  
extendint++.h is for C++ users only, with most operators reconstructed, a^b represents pow(a,b), other operators are the same as normal datatypes  
  
the two files above are experimental high accuracy calculation functions for integers, may still have some unexpeceted bugs left  

DateType is a pre-defined datatype which presents a date in the form of yyyy-mm-dd, I set the maximum date to year 9999, it has a function to turn strings to Date type, and basic comparisons have been reconstructed

Chris.h is a header for C++ users, featuring rb-tree structure which adjusts memory usage according to current node count, meanwhile being twice as fast as stl set when inserting and hundred times faster than stl set when freeing the memory

note:1. extendint:fft or ntt is NOT used in extendint algorithm, which means it might take huge amount of time calculationg big numbers, this might be optimized some day when I actually learn them  
2. extendint:due to my ignorance, bit opertations can yet be realized, this might NOT get optimized since i dont know how  
3. extendint:in my ide, errors pop up when I set the 'maxlength' variable too big (about 5e4), I guess there's some stack overflow issue, so be really carefull when modifying this  
4. Chris:in the structure, ll means int, which is not but a personal habit.  
&ensp;&ensp;&ensp;I didn't name an iterator class for Chris, but you can just use [] operator in the way like tree[10], which locates the 10th key in 'tree', it'll return a pointer pointing at the key, to get the key itself, use *tree[10].  
&ensp;&ensp;&ensp;'find' method returns the rank of the given key in the tree, and returns -1 if the key doesn't exist.  
&ensp;&ensp;&ensp;'view' method does a dfs through the tree, you can set the seperating and ending characters, but this method requires either an extra 'print' method telling it how to print the key or overloading << operator in the key class.  
   When initializing the tree, there are 3 selective parameters:display/shrink_threshold/unique, setting 'display' to 0 bans all the console output that might occur during certain operations, these outputs are meant to be used whilst debugging; setting 'unique' to 0 gives you a unique set, which declines repeat insert, setting to 1 allows it, but it won't tell the difference between two 'equal' key if there's any not included in the comparing method; adjusting 'shrink_threshold' will change the frequency of memory resizing, the higher the threshold is, the less memory operation there will be, thus making the program runs a bit faster but taking up a bit more memory, this parameter only accept a value in (0.6,1) and is default set to 0.67.  
   Output operation in the tree uses std::cout, so it's recommended to overload the key class if you want to use 'view' method, and feel free to use ios::sync_with_stdio(0) to speed up.  
   To use this structure, there are several requirements that have to be met:  
      1: the class of the key of the tree must overload '>' and '==' operator and should not make a change to them at any time in the middle of the program.  
      2: the class of the key of the tree must have a default generation method, for example if the class name is key and has a member k, then you got to have a function like key():k(0){} or at least key(){} in the class declaration, otherwise a compiler error will pop up
