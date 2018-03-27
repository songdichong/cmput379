Team members:
Dichong Song
Joey Wong

Instruction:
In console, input make to compile the code and make clean to delete the output file.
To run the program, input ./diffdir379 period path. The period should be a number in seconds and path could either be relative or absolute path.

Work distribution:
We designed the algorithm and testing together, Joey did the implementation of the linked list (cmput379.c ,cmput379.h),signal handler. Dichong handle the inotify, timer,reading user input,(readdir.c), debugging. 

Use open source code:
The inotify function used in readdir.c was modified from an inotify example given at http://man7.org/linux/man-pages/man7/inotify.7.html. We used it to learn about its initialization and implementation.


Testing:
We tested our program on the physical hosts in lab CSC 219. Here are our tests:
Test 1:
Adding file
Output:
file
(good)

Test 2:
Removing file
O:
file
(good)


Test 3:
Modifying file
O:
 *file
(Shows *file if file is modified in Vi. Shows -file then +file if file is modified in gedit or geany.)

Test 4
Add, then remove
Output:
Nothing 
(good)


Test5
Remove, then add
Output:
file
file
(good)

Test 6
Modify, remove, then add
Output:
file
File
(good)

Test 7
Add,modify,remove
Output:
Nothing
(good)

Test8:
Modify,Remove
Output:
file
(good)

Test 9:
Remove,add,modify
Output:
File
File
(good)


Test 10:
Add file in a directory within the testing directory
Output:
	*  directory
(good)

Test 11:
Move file in a directory within the testing directory
Output:
	-file
	*directory
(good)


Test 12:
Rename a file
O:
	-file
	+file
(good)

Test 13:
Check if it works on different directories
(good)

Test 14:
Adding file in a subdirectory
(good)

Test 15:
Removing file in subdirectory
(good)

Test 16:
After test 14, go remove the subdirectory
(good)






