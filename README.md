extendint.h is for C/C++ users, compiler ver c99 and above is required, please follow the instructions in the file  
  
extendint++.h is for C++ users only, with most operators reconstructed, a^b represents pow(a,b), other operators are the same as normal datatypes  
  
the two files above are experimental high accuracy calculation functions for integers, may still have some unexpeceted bugs left  

DateType is a pre-defined datatype which presents a date in the form of yyyy-mm-dd, I set the maximum date to year 9999, it has a function to turn strings to Date type, and basic comparisons have been reconstructed

Chris.h is a header for C users, featuring rb-tree structure whose key can be any datatype the user defines, it might requires a few rules to use, which will be revealed once it's finished (some day), now have \_init\_/insert/f_all/dfs for users

note:1. fft or ntt are NOT used in this algorithm, which means it might take huge amount of time calculationg big numbers, this might be optimized some day when I actually learn them  
2. due to my ignorance, bit opertations can yet be realized, this might NOT get optimized since i dont know how  
3. in my ide, errors pop up when I set the 'maxlength' variable too big (about 5e4), I guess there's some stack overflow issue, so be really carefull when modifying this 
4. here's an example code for Chris.h 
<code>Chris\*tree=(Chris\*)calloc(1,sizeof(Chris);
  \_init\_(tree,cmp,pnode);//cmp and pnodes are two functions defined by user
  //your operations
  f_all(tree->rt);//free all the nodes in the tree
  free(tree);//free the pointer aiming the tree itself
</code> 
5. remind that if you use Chris.h, you won't be able to define "struct node" or "struct Chris" in your own files, which will cause redefinitions, same to other funcs in the file, but you can consider changing their names yourself
