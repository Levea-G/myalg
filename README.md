extendint.h is for C/C++ users, compiler ver c99 and above is required, please follow the instructions in the file  
  
extendint++.h is for C++ users only, with most operators reconstructed, a^b represents pow(a,b), other operators are the same as normal datatypes  
  
the two files above are experimental high accuracy calculation functions for integers, may still have some unexpeceted bugs left  

DateType is a pre-defined datatype which presents a date in the form of yyyy-mm-dd, I set the maximum date to year 9999, it has a function to turn strings to Date type, and basic comparisons have been reconstructed

Chris.h is a header for C users, featuring rb-tree structure whose key can be any datatype the user defines, it requires a few rules to use, which will be revealed once it's finished (some day), now have \_\_init\_\_/insert/delet/clear/view/rank/pre/next/has for users

note:1. fft or ntt are NOT used in extendint algorithm, which means it might take huge amount of time calculationg big numbers, this might be optimized some day when I actually learn them  
2. due to my ignorance, bit opertations can yet be realized, this might NOT get optimized since i dont know how  
3. in my ide, errors pop up when I set the 'maxlength' variable too big (about 5e4), I guess there's some stack overflow issue, so be really carefull when modifying this  
4. here's an example code for Chris.h  
<code>//sample
&ensp;&ensp;&ensp;srand(time(NULL));
&ensp;&ensp;&ensp;Chris\*t=(Chris*)calloc(1,sizeof(Chris)); // make a base
&ensp;&ensp;&ensp;\_\_init\_\_(t,&cmp_,&pnode_); // set the compare method and default print method, here cmp_ and pnode_ are predefined
&ensp;&ensp;&ensp;for(ll i=0;i<10;i++){
&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;data\*temp=(data*)calloc(1,sizeof(data)); // some self-defined datatype 'data'
&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;temp->age=i;
&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;temp->salary=rand()%3000+3000;
&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;temp->sex=rand()%2;
&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;for(ll i=0;i<10;i++)temp->name[i]=rand()%26+97;
&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;insert(t,temp); // insert it into the structure
&ensp;&ensp;&ensp;}
&ensp;&ensp;&ensp;view(t,"\n",&p_) // checkup all the elements in the structure, the second param marks the separate string, the third param is optional, which temporarily replaces the default print method of t if given as the method to print within the function
&ensp;&ensp;&ensp;clear(t); // release all the memory, idk if gcc will do it for me, but i choose to be carefull
</code>5. remind that if you use Chris.h, you won't be able to define "struct node" or "struct Chris" in your own files, which will cause redefinitions, same to other funcs in the file, but you can consider changing their names yourself
