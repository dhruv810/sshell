# SSHELL
### Dhruv Fadadu
### Ali Osmani
#
['lint-maximum-line-length', 80]

We started the project by following the suggested work phase. We used to provide links to understand the starter code and start the project. We started the project since the first day it became available to finished it on time.

After understanding the source code, we started by writing “Makefile”. We used the following YouTube video to understand how “Makefile” works.

- https://www.youtube.com/watch?v=aw9wHbFTnAQ

We have watched many videos to finish the project, but we are going to link only videos that were useful.

We tested the Makefile on the source code, so it was easy to implemnt since there were not any errors in code.


For the nect part, we needed to extract arguments from string command. For this part we used on line c compiler to test that part individually and later implemented on the project.

- https://www.onlinegdb.com/online_c_compiler

We wanted to make sure that our code is working before implementing on the project. So, we used online compiler to test many functions of the project. Many of which are the following,

- get_arguments
- check_for_pipe
- check_for_output_redirection
- check_for_input_redirection
- add_spaces
- directory_stack

Pase 0 to 3, were easy implement given that provided link has all the information we needed. But after that we had to change design many time.

At low-level we were able to implement all functions easily but at high-level there was some issues.

Writing the `get_arguments()` was complicated but once we found the following way to split the command, we finished it in couple minutes. It is an amazing website that allowed us to write and test the function on their website.

- https://www.educative.io/answers/splitting-a-string-using-strtok-in-c

Later we used the same methods to implement check_for_pipe check_for_output_redirection, and check_for_input_redirection functions. 

At phase 4 we decided to add spaces arround the '>' so we can use the `get_arguments()` function without changing it.

we need to reconfigure the code to make sure that it recognices the output redirection even if it is not surrounded by spaces. That's when we thought about `add_spaces()` function. Because of this function we could use `get_arguments()` without changing it.

We are using the check_for_output_redirection, check_for_input_redirection function to check for input and output redirection.

At the end of the project, we realized that `get_arguments()` could be used to check command for input and/or output redirection but because of time constraints we could not implement it.

- PIPE

Pipe was the hadest part in this project. We had to use lecture slides and more than 10 youtube videos. But the one that was helpful is listed below.

- https://www.youtube.com/watch?v=Mqb2dVRe0uo

We started pipe by splitting the command from '|' and saving it in an array. Then we wrote a code that is similar to the one in video to execute each command in a loop.

At the end of the project, while writing the report we went of the assignment and realized that we didnt manage the error as we supposed to, so we did error managment at the end of assignment.