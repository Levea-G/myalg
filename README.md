extendint.h is for C/C++ users, compiler ver c99 and above is required, please follow the instructions in the file  
  
extendint++.h is for C++ users only, with most operators reconstructed, a^b represents pow(a,b), other operators are the same as normal datatypes  
  
the two files above are experimental high accuracy calculation functions for integers, may still have some unexpeceted bugs left  
  
note:1. fft or ntt are NOT used in this algorithm, which means it might take huge amount of time calculationg big numbers, this might be optimized some day when I actually learn them  
2. due to my ignorance, bit opertations can yet be realized, this might NOT get optimized since i dont know how  
3. in my ide, errors pop up when I set the 'maxlength' variable too big (about 5e4), I guess there's some stack overflow issue, so be really carefull when modifying this
