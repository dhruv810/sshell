#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

#define CMDLINE_MAX 512

// going to save stdout and stdin before changing it
int standardOut = -1;
int standardIn = -1;

// saving the copy of cmd
// going to change cmd to add space before and after "<" and ">" so redirection works
// even if it doesnt have space around it
char copy_cmd[CMDLINE_MAX];

// small function that execute code
// using this to avoid retyping same code
void execute_code(char *args[]) {             
        // handling pwd 
        if (strcmp(args[0], "pwd") == 0) {
                printf("%s\n", getcwd(NULL, CMDLINE_MAX));   
        }
        else {        
                // executing code      
                // managing errors    
                if (execvp(args[0], args) == -1) {
                        perror("execvp");
                }
                exit(1);
        }                  
        exit(0);
}

// changing the output file
void redirect_output(char *args[], int number_of_arguments) {
       for (int i = 0; i < number_of_arguments; i++) {
                if ((strcmp(args[i], ">") == 0) && (args[i+1] != NULL)) {
                        // using this from lecture slides, syscalls.pdf - pg.31
                        int fd = open(args[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        // if fails to open, display error
                        if (fd == -1) {
                                printf("Error: cannot open output file\n");
                        }
                        // storing STDOUT_FILENO so later reset it to whatever it was
                        dup2(fd, STDOUT_FILENO);
                        standardOut = dup(STDOUT_FILENO);       
                        // setting ">" to NULL, so while executing everything after and including ">" will be ignored
			close(fd);
                        args[i] = NULL;
			break;
                }
                else if ((strcmp(args[i], ">") == 0) && (args[i+1] == NULL)) {
                        printf("Error: no output file\n");
                        exit(1);
                }
        }
};

// this is same as redirect_output
void redirect_input(char *args[], int number_of_arguments) {
       for (int i = 0; i < number_of_arguments; i++) {
                if ((strcmp(args[i], "<") == 0) && (args[i+1] != NULL)) {
                        int fd = open(args[i+1], O_RDONLY);
                        standardIn = dup(STDIN_FILENO);
                        dup2(fd, STDIN_FILENO);
			close(fd);
                        args[i] = NULL;
			break;
                }
        }
};

// getting arguments of code
// passing refrence of number_of_arguments to change value in the function
// this function will be called from more than one functiomn
// thats why assigning the value of number_of_arguments in this function
// to avoid keeping track of return values and the variables
void get_arguments(char *cmd, char *args[], int *number_of_arguments) {
        int i = 0;
        // copying cmd so the original cmd says unchanged
        char* copied_cmd;
        copied_cmd = (char*)malloc(512);
        strcpy(copied_cmd, cmd);
        // dividing copied cmd at " " and storing each arguments into args
        // this is where I learned to use strtok
        // https://www.educative.io/answers/splitting-a-string-using-strtok-in-c
        char *token = strtok(copied_cmd, " ");
        while( token != NULL ) {
                args[i++] = token;
                token = strtok(NULL, " ");
        }
        // freeing the memory and seting number of arguments
        free(token);
        *number_of_arguments = i;
};

// cmd with pipes will be executed here
void pipe_handler(char * commands[], int num_pipe, int output_redirection) {
        int cmd_num = 0;        // index of current command being run
        int read_pipe = -1;      // read and write pip
        int write_pipe = -1;
        int previous_read_pipe = -1;            // readpipe for next command
        pid_t pid;

        // used this video to understand the following process
        // https://www.youtube.com/watch?v=Mqb2dVRe0uo
        while (commands[cmd_num] != NULL) {
                // getting args from command.
                int num_args = 0;
                char *args[CMDLINE_MAX] = {};
                get_arguments(commands[cmd_num], args, &num_args);

                // if error then going to next iteration
                if (strcmp(args[0],"|") || strcmp(args[0],">")) {
                        printf("Error: missing command\n");
                        continue;
                }
                
                int pip[2];
                if((num_pipe > 0) & (cmd_num != num_pipe)) {
                        // not going create pipe for the last command
                        pipe(pip);
                        read_pipe = pip[0];
                        write_pipe = pip[1];
                }

                pid = fork();
                if (pid == 0) {
                        if (cmd_num == num_pipe && output_redirection == 1) {
                                // read_pipe for current commnad is write_pipe for previous cmd
                                // but for the last command out_pipe is stdout
                                // but if output_redirection is true then changing stdout to given file
                                for (int i = 0; i < num_args; i++) {
                                        if ((strcmp(args[i], ">") == 0) && (args[i+1] != NULL)) {
                                                int fd = open(args[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                                                standardOut = dup(STDOUT_FILENO);
                                                dup2(fd, STDOUT_FILENO);
                                                close(fd);
                                                args[i] = NULL;
                                                break;
                                        }
                                }
                        }
                        // changing stdout to write_pip only if it is it exist
                        else if (write_pipe != -1) {
                                dup2(write_pipe, STDOUT_FILENO);
                        }
                        
                        // changing stdin to read_pipe only if it is it exist
                        // for the first command there is no pipe to read from
                        // for next commands, chile process will set read_pipe to previous_read_pipe
                        // so when child process is parent process, it can read from the read_pipe
                        if(previous_read_pipe != -1) {
                                dup2(previous_read_pipe, STDIN_FILENO);
                        }
                        // executing the comamand
                        execute_code(args);
                        // exiting the parent process
                        exit(0);
                }
                else {
                        // closing pipes if it is open
                        if(previous_read_pipe != -1) {
                                close(previous_read_pipe);
                        }
                        if(write_pipe != -1) {
                                close(write_pipe);
                        }
                        // setting previous_read_pipe to read_pipe
                        // so during next iteration, function will read read_pipe
                        previous_read_pipe = read_pipe;
                }
                cmd_num++;
        } 
        // waiting for child processs to end
        waitpid(pid, NULL, 0);
};

// using strtok get all commands divided by pipe
void check_for_pipe(char *cmd, char *commands[], int *num_pipe) {
        int i = 0;
        char* copied_cmd;
        copied_cmd = (char*)malloc(CMDLINE_MAX);
        strcpy(copied_cmd, cmd);
        // diving copied_cmd from | and counting number of string it returns
        char *token = strtok(copied_cmd, "|");
        while(token != NULL) {
                commands[i++] = token;
                token = strtok(NULL, "|");
        }
        free(token);
        // if cmd has 0 pipe we will get 1 string
        // if cmd has 1 pipe we will get 2 string
        // so returning i-1 
        *num_pipe = i - 1;
};

// // using strtok to check for if cmd required to redirect output
// void check_for_output_redirection(char *cmd, int *is_out_redirect) {
//         int i = 0;
//         char* copied_cmd;
//         copied_cmd = (char*)malloc(CMDLINE_MAX);
//         strcpy(copied_cmd, cmd);
//         // diving function at ">"
//         char *token = strtok(copied_cmd, ">");
//         while (token != NULL ) {
//                 // if there is a more than one string that means
//                 // output needs to be redirected
//                 if (i >= 1) {
//                         *is_out_redirect = 1; 
//                         return;
//                 }
//                 i++;
//                 token = strtok(NULL, ">");
//         }
//         free(token);
//         *is_out_redirect = 0;
// };

// // using strtok to check for if cmd required to redirect input
// void check_for_input_redirection(char *cmd, int *is_in_redirect) {
//         int i = 0;
//         char* copied_cmd;
//         copied_cmd = (char*)malloc(CMDLINE_MAX);
//         strcpy(copied_cmd, cmd);
//         // diving cmd from  "<" and if recived more than one string that means
//         // the input for code needs to be redirected
//         char *token = strtok(copied_cmd, "<");
//         while (token != NULL ) {
//                 if (i >= 1) {
//                         *is_in_redirect = 1;
//                         return;
//                 }
//                 i++;
//                 token = strtok(NULL, "<");
//         }
//         free(token);
//         *is_in_redirect = 0;
// };

// using this function to add space before and after "<" and ">"
// so it will be easy to execute code
void add_spaces(char *cmd, int* is_in_redirect, int* is_out_redirect) {
        int i = 0;
        int k = 0;
        int size = strlen(cmd);
        // after adding spaces the new cmd may get longer than CMDLINE_MAX
        // but for the simplicity of the project using CMDLINE_MAX
        char new_str[CMDLINE_MAX];
        while (k < size) {
                if (cmd[k] == '>' || cmd[k] == '<') {
                        if (cmd[k] == '>'){
                                *is_out_redirect = 1;
                        
                        }
                        printf("is_out_redirect %d\n", *is_out_redirect);   
                        if (cmd[k] == '<') {
                                *is_in_redirect = 1;
                        }
                        new_str[i++] = ' ';
                        new_str[i++] = cmd[k++];
                        new_str[i++] = ' ';
                }
                else {
                new_str[i++] = cmd[k++];
                }
        }
        new_str[i++] = '\0';
        // coping new_str over cmd
        // now cmd has ">" and "<" saparated by spaces
        strcpy(cmd, new_str);
}

//  this function executes functions of extra feature called "Directory stack"
// if error returning 1 else 0
int directory_stack(char *dirs[], char *cmd, char *location, int *num_dirs) {      
        if (strcmp(cmd, "pushd") == 0) {
                if (chdir(location) != -1) {
                        dirs[*num_dirs] = getcwd(dirs[*num_dirs], CMDLINE_MAX);
                        *num_dirs += 1;
                        return 1;
                }
                else {  // error managment
                        perror("chdir");
                }
        }
        else if (strcmp(cmd, "dirs") == 0) {    // printing dirs
                int i = *num_dirs - 1;
                while (i >= 0) {
                        printf("%s\n", dirs[i]);
                        i--;
                }
        }
        else if (strcmp(cmd, "popd") == 0) {
                if (*num_dirs == 1) {   // if there's only one directory, then it is current working directory
                                        // so not removing it
                        printf("%s\n", getcwd(NULL, CMDLINE_MAX));
                        // returning 1 because removing current working directory is error
                        // not so sure aboht this 
                        // maybe it should be 0 because executing as it supposed to do so
                        return 1;
                }
                *num_dirs -= 1;
                dirs[*num_dirs] = NULL;
                // changing current directory to previous one in stack
                if (chdir(dirs[*num_dirs - 1]) == -1) {
                        // if chdir returns error, retuning 1
                        return 1;
                }
                else {
                        perror("chdir");
                }
        }
        return 0;
}

int main(void) {
        // variables
        char cmd[CMDLINE_MAX];
        char *commands[CMDLINE_MAX] = {};       // stores all commands sapated by pipe
        int num_pipe = 0;                       // ex. cat file.txt | head -3
                                                // commands[0] = cat file.txt and commands[1] = head -3
        // keeping track of need for input and output redirection
        int is_out_redirect = 0;
        int is_in_redirect = 0;
        // storing arguments and number of arguments in cmd with added space around "<" and  ">"
        char *args[CMDLINE_MAX] = {};
        int num_arguments = 0;
        // storing directories and its size
        char *dirs[CMDLINE_MAX] = {};
        int num_dirs = 0;

        while (1) {
                // provide code
                char *nl;
                int retval = 0;

                if (num_dirs == 0) {
                        dirs[num_dirs] = getcwd(dirs[num_dirs], CMDLINE_MAX);
                        num_dirs++;
                }

                /* Print prompt */
                printf("sshell$ ");
                fflush(stdout);

                /* Get command line */
                fgets(cmd, CMDLINE_MAX, stdin);

                /* Print command line if stdin is not provided by terminal */
                if (!isatty(STDIN_FILENO)) {
                        printf("%s", cmd);
                        fflush(stdout);
                }

                /* Remove trailing newline from command line */
                nl = strchr(cmd, '\n');
                if (nl)
                        *nl = '\0';

                // making copy of cmd becuase going to edit cmd
                strcpy(copy_cmd, cmd);
                // adding spaced so get_arguments can easily saperate arguments
                add_spaces(cmd, &is_in_redirect, &is_out_redirect);
                get_arguments(cmd, args, &num_arguments);

                /* Builtin command */
                if (!strcmp(cmd, "exit")) {
                        fprintf(stderr, "Bye...\n");
                        fprintf(stderr, "+ completed '%s' [%d]", copy_cmd, EXIT_SUCCESS);
                        break;
                }
                else if (strcmp(args[0], "cd") == 0 && args[1] != NULL) {
                        // args[1] is new location also making sure that it is not NULL to avoid error
                        if (chdir(args[1]) == -1) {       // error managment
                                if(errno == EACCES) {
                                        printf("Permission denied.\n");
                                }
                        }
                        printf("%s\n", getcwd(NULL, CMDLINE_MAX));
                }
                if ((strcmp(args[0], "pushd") == 0 && args[1] != NULL) || strcmp(args[0], "popd") == 0 || strcmp(args[0], "dirs") == 0) {
                        // if args[0] is function of directory stack then executing it in other function
                        // avoidig pushd without filename or path
                        retval = directory_stack(dirs, args[0], args[1], &num_dirs);
                }
                
                /* Regular command */
                pid_t pid = fork();
                if (pid != 0) {
                        // chile process
                        // waiting for parent process to execute
                        int status;
                        waitpid(pid, &status, 0);
                        // we need to print 0 for succesfull execution and 1 for error
                        // but WIFEXITED return 1 for succesfull execution anf 0 for error
                        // so using !
                        // this where I learned about the return value of WIFEXITED
                        // https://www.ibm.com/docs/en/ztpf/1.1.0.14?topic=zf-wifexitedquery-status-see-if-child-process-ended-normally
                        retval = ! WIFEXITED(status);
                        fprintf(stderr, "+ completed '%s' [%d]\n", cmd, retval);
                }
                else {
                        // chile process
                        check_for_pipe(cmd, commands, &num_pipe);               // checking for pipes
                        // check_for_output_redirection(cmd, &is_out_redirect);    // checking for output redirection
                        // check_for_input_redirection(cmd, &is_in_redirect);      // checking for input redirection
                        
                        // executing cmd with and without pipe differently
                        if (num_pipe == 0) {
                                printf("is_out_redirect %d\n", is_out_redirect);
                                if (is_out_redirect) {
                                       
                                        // changing output fd if cmd has ">"
                                        redirect_output(args, num_arguments);
                                }
                                if (is_in_redirect) {
                                        // changing input fd if cmd has "<"
                                        redirect_input(args, num_arguments);
                                }
                                // ececuting code
                                execute_code(args);
                                // reseting stdout if changed before
                                if (is_out_redirect) {
                                        if (standardOut != -1) {
                                                dup2(standardOut, STDOUT_FILENO);
                                                close(standardOut);
                                                standardOut = -1;
                                        }
                                }
                                // reseting stdin if changed before
                                if (is_in_redirect) {
                                        if (standardIn != -1) {
                                                dup2(standardIn, STDIN_FILENO);
                                                close(standardIn);
                                                standardIn = -1;
                                        }
                                }
                        }
                        else {
                                // this function handles cmd with pipes
                                pipe_handler(commands, num_pipe, is_out_redirect);
                                exit(0); // dont know with what value process should exit
                        }
                }
                // resetting all variables to defalut value
                memset(commands, 0, sizeof(commands));
                is_out_redirect = 0;
                is_in_redirect = 0;
                num_pipe = 0;
                num_arguments = 0;
                memset(args, 0, sizeof(args));
        }
        return EXIT_SUCCESS;    // from source code
};