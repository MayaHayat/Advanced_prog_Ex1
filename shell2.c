// #include <sys/stat.h>
// #include <sys/wait.h>
// #include <fcntl.h>
// #include "stdio.h"
// #include "errno.h"
// #include "stdlib.h"
// #include "unistd.h"
// #include <string.h>

// int main() {
// char command[1024];
// char *token;
// char *outfile;
// int i, fd, amper, redirect, retid, status;
// char *argv[10];

// while (1)
// {
//     printf("hello: ");
//     fgets(command, 1024, stdin);
//     command[strlen(command) - 1] = '\0';

//     /* parse command line */
//     i = 0;
//     token = strtok (command," ");
//     while (token != NULL)
//     {
//         argv[i] = token;
//         token = strtok (NULL, " ");
//         i++;
//     }
//     argv[i] = NULL;

//     /* Is command empty */
//     if (argv[0] == NULL)
//         continue;

//     /* Does command line end with & */ 
//     if (! strcmp(argv[i - 1], "&")) {
//         amper = 1;
//         argv[i - 1] = NULL;
//     }
//     else 
//         amper = 0; 

//     if (! strcmp(argv[i - 2], ">")) {
//         redirect = 1;
//         argv[i - 2] = NULL;
//         outfile = argv[i - 1];
//         }
//     else 
//         redirect = 0; 

//     // // Writing into stderr - DOESN'T WORK
//     // if (! strcmp(argv[i - 2], "2>")) {
//     //     redirect = 2;
//     //     argv[i - 2] = NULL;
//     //     outfile = argv[i - 1];
//     //     }
//     // else 
//     //     redirect = 0; 


//     // Adding to a file
//     if (! strcmp(argv[i - 2], ">>")) {
//         redirect = 3;
//         argv[i - 2] = NULL;
//         outfile = argv[i - 1];
//         }
//     else 
//         redirect = 0; 

//     // Adding echo
//     if (! strcmp(argv[0], "echo")) {
//         if (! strcmp(argv[1], "$?")){
//             printf("HI");
//         }
//         else{
//             for (int j = 1 ; argv[j] != NULL ; j++){
//                 printf("%s", argv[j]);
//                 if (argv[j+1] != NULL){
//                     printf(" ");
//                 }
//             }
//             printf("\n");
//         }
//     }
   

//     /* for commands not part of the shell command language */ 

//     if (fork() == 0) { 
//         /* redirection of IO ? */
//         if (redirect == 0) {
//             fd = creat(outfile, 0660); 
//             close (STDOUT_FILENO) ; 
//             dup(fd); 
//             close(fd); 
//             /* stdout is now redirected */
//         } 
//         else if (redirect == 2) {
//             fd = creat(outfile, 0660); 
//             close (STDERR_FILENO) ; 
//             dup(fd); 
//             close(fd); 
//             /* stdout is now redirected */
//         } 
//         else if (redirect == 3) {
//             fd = open(outfile, O_RDWR | O_APPEND | O_CREAT, 0660); 
//             close (STDOUT_FILENO) ; 
//             dup(fd); 
//             close(fd); 
//             /* stdout is now redirected */
//         } 
//         execvp(argv[0], argv);
//     }
//     /* parent continues here */
//     if (amper == 0)
//         retid = wait(&status);
// }
// }

#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>
#include <signal.h> 

int last_status = 0;

void handle_cntl_c(int sig){
    printf("You typed control + c\n");
}

int main() {
char command[1024];
char *token;
char *outfile;
int i, fd, amper, redirect, retid, status;
char *argv[10];

// Handle cntl c
signal(SIGINT, handle_cntl_c);

while (1)
{
    printf("hello: ");
    fgets(command, 1024, stdin);
    command[strlen(command) - 1] = '\0';

    /* parse command line */
    i = 0;
    token = strtok (command," ");
    while (token != NULL)
    {
        argv[i] = token;
        token = strtok (NULL, " ");
        i++;
    }
    argv[i] = NULL;

    /* Is command empty */
    if (argv[0] == NULL)
        continue;

    /* Does command line end with & */ 
    if (! strcmp(argv[i - 1], "&")) {
        amper = 1;
        argv[i - 1] = NULL;
    }
    else 
        amper = 0; 

    if (! strcmp(argv[i - 2], ">")) {
        redirect = 1;
        argv[i - 2] = NULL;
        outfile = argv[i - 1];
        }
    else 
        redirect = 0; 

    // Adding echo
    if (! strcmp(argv[0], "echo")) {
        if (! strcmp(argv[1], "$?")){
            printf("%d \n", last_status);
        }
        else{
            for (int j = 1 ; argv[j] != NULL ; j++){
                printf("%s", argv[j]);
                if (argv[j+1] != NULL){
                    printf(" ");
                }
            }
            printf("\n");
        }
        continue;
    }

    // change prompt
    if (i > 2 && !strcmp(argv[0], "prompt" )&& !strcmp(argv[1], "=")){
        printf("Handle prompt");
        continue;
    }

    // Handle CD
    if (i > 1 && !strcmp(argv[0], "cd")){
        if (chdir(argv[1]) != 0){
            perror("cd");
            last_status = 1;
        }
        else{
            last_status = 0;
        }
        continue;  // MAKE SURE NEEDED
    }

    // Quit the prgram when user asks
    if (!strcmp(argv[0], "quit")){
        break;
    }

    /* for commands not part of the shell command language */ 

    if (fork() == 0) { 
        /* redirection of IO ? */
        if (redirect) {
            fd = creat(outfile, 0660); 
            close (STDOUT_FILENO) ; 
            dup(fd); 
            close(fd); 
            /* stdout is now redirected */
        } 
        execvp(argv[0], argv);
    }
    /* parent continues here */
    if (amper == 0)
        retid = wait(&status);
}
}