#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <autocorrect.h>

#define SIZE 10

char** get_input()
{
    int s, i, j;
    char x;
    /*
        Need some better idea!
    */
    char **input = malloc(SIZE*sizeof(char*));
    if(input == NULL) {
        perror("Not enough memory");
        exit(1);
    }
    for(i=0; i<SIZE; ++i) {
        input[i] = malloc(SIZE*sizeof(char));
    }
    s = i = j = 0;
    /*
        s = 0 => Outside the word
        s = 1 => Inside the word
    */
    while((x = getchar()) != '\n') {
        if(x == ' ') {
            if(s) {
                input[i++][j] = 0;
                j = s = 0;
            }
        }
        else {
            s = 1;
            input[i][j++] = x;
        }
    }
    input[i++][j]=0;
    input[i] = NULL;
    return input;
}

int main(void)
{
    char **input;
    pid_t child_pid;
    int child_exit_code;

    while(1) {
        putchar('>');
        input = get_input();
        /*
            change directory has to be done
            by parent process only
        */
        if(!strcmp(input[0], "cd")) {
            if(chdir(input[1]) < 0) {
                perror(input[1]);
            }
        }
        /* To get rid of this shell */
        else if(!strcmp(input[0], "exit")) {
            free(input);
            exit(0); 
        }
        else {
            child_pid = fork();
            if(child_pid < 0) {
                perror("Unable to Fork");
                exit(1);
            }
            else if(child_pid == 0) {
                /*
                    execute each command as a child process
                */
                if(execvp(input[0], input) < 0) {
                    perror("Unable to execute the command");
                    char* match = best_match(input[0]);
                    if(match == NULL) {
                        exit(1);
                    }
                    strcpy(input[0], match);
                    free(match);
                    /*
                        Choice to run the best match
                    */
                    printf("Did you mean:%s?(y/n)", match);
                    if(getchar() == 'y' || getchar() == 'Y') {
                        /*
                            Replace child with matched process
                            TODO: Put match in a shared memory and execute from parent so that cd and exit can work
                        */
                        if(execvp(input[0], input) < 0) {
                            perror("Unable to execute the new command");
                            exit(1);
                        }
                    }
                    exit(1);
                }
            }
            else {
                /*
                    wait for the child to die
                */
                waitpid(child_pid, &child_exit_code, 0);
            }
        }
        /* It is always to good to free the allocated memory */
        free(input);
    }

    return 0;
}
