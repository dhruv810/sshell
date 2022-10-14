# SSHELL

### Dhruv Fadadu

### Ali Osmani

#

We started the project by following the suggested work phase. We used to provide links to understand the starter code and start the project. We started the project from the first day it became available to finish it on time. From phase 0 to 3 did not have any trouble given that provided links had all the information we needed. But after that we had to change the design many times.

We started by writing “Makefile”, after understanding the source code. We used the following YouTube video to understand how “Makefile” works.

- https://www.youtube.com/watch?v=aw9wHbFTnAQ

We have watched many videos to finish the project, but we are going to link only videos that were useful.

We tested the Makefile on the source code, so it was easy to implement since there were not any errors in code. We tested our code on our laptops only. We wanted to test it on CSIF before submitting but since we had unlimited submissions we just check if our code is compatible by submitting it on gradescope.

---

The sshell.c was implemented by following the steps listed below.

1. Get the command and edit it
2. Get the arguments
3. Manages built-in commands
4. Check for pipe, input redirection and output redirection
   1. if input redirection and/or output redirection, changes the input and/or output directory
   2. if pipe, if executes using pipe handler
   3. else executes it normally

---

For the first part, we needed to extract arguments from the string command. For which we used the website below to understand how to parse the string.

Writing the `get_arguments` was complicated but once we found the following way to split the command, we finished it in a couple of minutes. It is an amazing website that allows us to write and test the function on their website. This function splits the function from “ ”, and saves arguments into the array.

- <https://www.educative.io/answers/splitting-a-string-using-strtok-in-c>

Later we used the same methods to implement `check_for_pipe`, `check_for_output_redirection`, and `check_for_input_redirection.`

---

We used online C compiler to test that part individually before implementing it on the project to avoid errors.

- <https://www.onlinegdb.com/online_c_compiler>

We wanted to make sure that our code is working before implementing it on the project. So, we used an online compiler to test many functions before writing on the project. Some of which are following

- get_arguments
- check_for_pipe
- check_for_output_redirection
- check_for_input_redirection
- add_spaces
- directory_stack

---

In phase 4 description, it has mentioned that the output redirection sign '>' might not be surrounded by " ". So, we decided to add spaces around the '>', using the `add_spaces` function, so we can use the `get_arguments` function without changing it.

Later in extra features, the same thing was said about the input redirection. So, we figured adding spaces is important and decided to call the `add_spaces` function right after getting the command.

Currently, We are using the `check_for_output_redirection`, `check_for_input_redirection` function to check for input and output redirection. They were implemented before the `add_space` function.

In these functions, we divide the command by the '<' and '>', and if we get more than one string that means the command has an output redirection and/or input redirection.

Because output redirection and input redirection are similar processes we implemented both functions at once. You can notice that both functions are identical.

**We realized that `get_arguments()` could be used to check commands for input and/or output redirection but because of limited time we could not implement it.**

---

Pipe was the hardest part in this project. We had to use lecture slides and more than 10 YouTube videos. But the video that was helpful is below.

- <https://www.youtube.com/watch?v=Mqb2dVRe0uo>

We started pipe by splitting the command from '|' and saving sub or small commands in an array. Then we wrote a code that is similar to the one in the video to execute commands using pipe. Because we had more than one command we decided to go with the while command, and use variable `previous_read_pipe` to store the read file directory. This is a little complicated process and it is hard to explain with the code. So, the code has plenty of comments to help readers understand it in more detail.

We started testing the pipe before doing other parts of the project. It was a frustrating process. We had crashed my laptop at least 5 times. But in the end it turned out well.

---

The directory stack was the last function we implemented. It was simple and easy but confusing at the same time. We were getting an error but did not understand its meaning. Because it was the last day to submit an assignment and we had yet to write a report we decided to skip that part.

---

At the end of the project, while writing the report we went through the project information file and realized that we didn’t do the part of the error management. And due to limited time we couldn’t implement it.
