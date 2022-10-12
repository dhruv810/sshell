#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <termios.h>

#define CMDLINE_MAX 512

int standardOut = -1;
int standardIn = -1;

char copy_cmd[CMDLINE_MAX];

// dont change
void execute_code(char *args[]) {
        int exit_val = 0;
        if (strcmp(args[0], "pwd") == 0) {
                printf("%s\n", getcwd(NULL, CMDLINE_MAX));
        }
        else {
                exit_val = execvp(args[0], args);
                if (exit_val == -1) {
                        perror("execvp");
                }
                exit(1);
        }
        fprintf(stderr, "+ completed '%s' [%d]\n", copy_cmd, EXIT_SUCCESS);                    
        exit(0);
}

// dont change
void redirect_output(char *args[], int number_of_arguments) {
       for (int i = 0; i < number_of_arguments; i++) {
                if ((strcmp(args[i], ">") == 0) && (args[i+1] != NULL)) {
                        int fd = open(args[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        standardOut = dup(STDOUT_FILENO);
                        dup2(fd, STDOUT_FILENO);
			close(fd);
                        args[i] = NULL;
			break;
                }
        }
};

// dont change
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

// dont change
void get_arguments(char *cmd, char *args[], int *number_of_arguments) {
        int i = 0;
        char* s2;
        s2 = (char*)malloc(512);
        strcpy(s2, cmd);
        char *token = strtok(s2, " ");
        while( token != NULL ) {
                args[i++] = token;
                token = strtok(NULL, " ");
        }
        free(token);
        *number_of_arguments = i;
};

// modify
void pipe_handler(char * commands[], int num_pipe, int output_redirection) {
        int j = 0;
        int end = 0;
        int inpipe = -1;
        int outpipe = -1;
        int previnpipe = -1;
        pid_t *pids[512] = {};
        int tmp = 0;
        while (commands[j] != NULL && end == 0) {
                int num_args = 0;
                char *args[CMDLINE_MAX] = {};
                get_arguments(commands[j], args, &num_args);
               
                int pip[2];
                if((num_pipe > 0) & (j != num_pipe))
                {
                        pipe(pip);
                        inpipe = pip[0];
                        outpipe = pip[1];
                }

                pid_t pid = fork();
                if (pid < 0) {
                        printf("Could not create chile process");
                }
                if (pid == 0) {
                        if (j == num_pipe && output_redirection == 1) {
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
                        // redirect output to outpipe
                        else if (outpipe != -1) {
                                dup2(outpipe, STDOUT_FILENO);
                        }
                        
                        // redirect input from previnpipe
                        if(previnpipe != -1) {
                                dup2(previnpipe, STDIN_FILENO);
                        }
                        
                        execute_code(args);
                        
                        exit(0);
                }
                else {
                        pids[tmp] = &pid;
                        if(previnpipe != -1) {
                                close(previnpipe);
                        }
                        
                        //redirect output to outpipe
                        if(outpipe != -1) {
                                close(outpipe);
                        }
                        previnpipe = inpipe;
                }
                j++;
                
        }
        int p = 0;
        while (pids[p] != NULL) {
                waitpid(*pids[p], NULL, 0);
                p++;
        }
};

// dont change
int check_for_pipe(char *cmd, char *commands[]) {
        int i = 0;
        char* s2;
        s2 = (char*)malloc(CMDLINE_MAX);
        strcpy(s2, cmd);
        char *token = strtok(s2, "|");
        while(token != NULL) {
                commands[i++] = token;
                token = strtok(NULL, "|");
        }
        free(token);
        return i - 1;
};

// dont change
int check_for_output_redirection(char *cmd) {
        int i = 0;
        char* s2;
        s2 = (char*)malloc(CMDLINE_MAX);
        strcpy(s2, cmd);
        char *token = strtok(s2, ">");
        while (token != NULL ) {
                if (i >= 1) {
                        return 1; 
                }
                i++;
                token = strtok(NULL, ">");
        }
        free(token);
        return 0;
};

//dont change
int check_for_input_redirection(char *cmd) {
        int i = 0;
        char* s2;
        s2 = (char*)malloc(CMDLINE_MAX);
        strcpy(s2, cmd);
        char *token = strtok(s2, "<");
        while (token != NULL ) {
                if (i >= 1) {
                        return 1; 
                }
                i++;
                token = strtok(NULL, "<");
        }
        free(token);
        return 0;
};

// dont change
void add_spaces(char *cmd) {
    int i = 0;
    int k = 0;
    int size = strlen(cmd);
    char new_str[512];
    while (k < size) {
        if (cmd[k] == '>' || cmd[k] == '<') {
            new_str[i++] = ' ';
            new_str[i++] = cmd[k++];
            new_str[i++] = ' ';
        }
        else {
            new_str[i++] = cmd[k++];
        }
    }
    new_str[i++] = '\0';
    strcpy(cmd, new_str);
}

int main(void) {
        char cmd[CMDLINE_MAX];
        char *commands[CMDLINE_MAX] = {};
        int is_out_redirect = -1;
        int is_in_redirect = -1;
        int num_pipe = 0;
        int num_arguments = 0;
        char *args[CMDLINE_MAX] = {};

        while (1) {
                char *nl;
                int retval = 0;

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

                
                strcpy(copy_cmd, cmd);

                add_spaces(cmd);
                
                get_arguments(cmd, args, &num_arguments);
                /* Builtin command */
                if (!strcmp(cmd, "exit")) {
                        fprintf(stderr, "Bye...\n");
                        fprintf(stderr, "+ completed '%s' [%d]", copy_cmd, EXIT_SUCCESS);
                        break;
                }
                else if (strcmp(args[0], "cd") == 0 && args[1] != NULL) {
                        char *direc = args[1];
                        if (chdir(direc) == -1)
                                perror("chdir");
                        
                        printf("%s\n", getcwd(NULL, CMDLINE_MAX));
                        // fprintf(stderr, "+ completed '%s' [%d]", copy_cmd, EXIT_SUCCESS);
                }
                
                /* Regular command */
                pid_t pid = fork();
                if (pid != 0) {
                        int status;
                        waitpid(pid, &status, 0);
                        retval = WIFEXITED(status);
                }
                else {
                        num_pipe = check_for_pipe(cmd, commands);
                        is_out_redirect = check_for_output_redirection(cmd);
                        is_in_redirect = check_for_input_redirection(cmd);
                        
                        if (num_pipe == 0) {
                                if (is_out_redirect) {
                                        redirect_output(args, num_arguments);
                                }
                                if (is_in_redirect) {
                                        
                                        redirect_input(args, num_arguments);
                                }
                                
                                execute_code(args);
                                
                                if (is_out_redirect) {
                                        if (standardOut != -1) {
                                                dup2(standardOut, STDOUT_FILENO);
                                                close(standardOut);
                                                standardOut = -1;
                                        }
                                }
                                
                                if (is_in_redirect) {
                                        if (standardIn != -1) {
                                                dup2(standardIn, STDIN_FILENO);
                                                close(standardIn);
                                                standardIn = -1;
                                        }
                                }
                        }
                        else {
                                pipe_handler(commands, num_pipe, is_out_redirect);
                                exit(0);
                        }
                }
                fprintf(stdout, "Return status value for '%s': %d\n",
                        cmd, retval);

                memset(commands, 0, sizeof(commands));
                is_out_redirect = -1;
                is_in_redirect = -1;
                num_pipe = 0;
                num_arguments = 0;
                memset(args, 0, sizeof(args));
        }
        return EXIT_SUCCESS;
};