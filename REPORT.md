# SSHELL

### Dhruv Fadadu

### Ali Osmani

#

We started the project by following the suggested work phase. We used to provide links to understand the starter code and start the project. We started the project since the first day it became available to finished it on time.

After understanding the source code, we started by writing “Makefile”. We used the following YouTube video to understand how “Makefile” works.

- https://www.youtube.com/watch?v=aw9wHbFTnAQ

We have watched many videos to finish the project, but we are going to link only videos that were useful.

We tested the Makefile on the source code, so it was easy to implement since there were not any errors in code.

---

For the next part, we needed to extract arguments from string command. For which we used online C compiler to test that part individually and later implemented on the project.

- <https://www.onlinegdb.com/online_c_compiler>

We wanted to make sure that our code is working before implementing it on the project. So, we used online compiler to test many functions before writing on the project. Some of which are following

- get_arguments
- check_for_pipe
- check_for_output_redirection
- check_for_input_redirection
- add_spaces
- directory_stack

---

Phase 0 to 3 were easy implement given that provided links has all the information we needed. But after that we had to change design many times.

At low-level we were able to implement all functions easily but at high-level there was some issues.

Writing the `get_arguments()` was complicated but once we found the following way to split the command, we finished it in couple of minutes. It is an amazing website that allowed us to write and test the function on their website. This function splits the function from “ ”, and saves arguments in to the array.

- <https://www.educative.io/answers/splitting-a-string-using-strtok-in-c>

Later we used the same methods to implement `check_for_pipe`, `check_for_output_redirection`, and `check_for_input_redirection.`

At phase 4 we decided to add spaces around the '>' so we can use the `get_arguments()` function without changing it.

we need to change the code to make sure that it recognices the output redirection even if it is not surrounded by spaces. That's when we thought about `add_spaces()` function. Because of this function we are able use `get_arguments()` without changing it.

---

We are using the `check_for_output_redirection`, `check_for_input_redirection` function to check for input and output redirection.

In these functions, we divide the command by the '<' and '>', and if we get more than one string that mean command has an output redirection and/or input redirection.

**At the end of the project, we realized that `get_arguments()` could be used to check command for input and/or output redirection but because of limited time we could not implement it.**

---

Pipe was the hardest part in this project. We had to use lecture slides and more than 10 YouTube videos. But the one that was helpful is listed below.

- <https://www.youtube.com/watch?v=Mqb2dVRe0uo>

We started pipe by splitting the command from '|' and saving sub commands it in an array. Then we wrote a code that is like the one in the video to execute each command in a loop. Code has comments that explains it in more details.

---

At the end of the project, while writing the report we went through the project information file and realized that we didn’t do the part of the error management. And due to limited time we couldn’t implement it.

