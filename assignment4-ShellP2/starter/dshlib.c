#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */

 static int last_return_code = 0;

 Built_In_Cmds exec_cd_command(cmd_buff_t *cmd) {
     if (cmd->argc == 1) {
         // if no argument is given change to $HOME
         char *home = getenv("HOME");
         if (home) {
             if (chdir(home) != 0) {
                 perror("cd");
                 last_return_code = ENOENT;
                 return BI_EXECUTED;
             }
         }
         return BI_EXECUTED;
     }
     
     if (chdir(cmd->argv[1]) != 0) {
         printf("cd: %s: No such file or directory\n", cmd->argv[1]);
         last_return_code = ENOENT;
         return BI_EXECUTED;
     }
 
     last_return_code = 0;
     return BI_EXECUTED;
 }
 
 Built_In_Cmds exec_rc_command() {
     printf("%d\n", last_return_code);
     return BI_EXECUTED;
 }
 
 Built_In_Cmds match_command(const char *input) {
     if (strcmp(input, EXIT_CMD) == 0) return BI_CMD_EXIT;
     if (strcmp(input, "cd") == 0) return BI_CMD_CD;
     if (strcmp(input, "rc") == 0) return BI_RC;
     return BI_NOT_BI;
 }
 
 Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
     Built_In_Cmds type = match_command(cmd->argv[0]);
     switch (type) {
         case BI_CMD_CD:
             return exec_cd_command(cmd);
         case BI_RC:
             return exec_rc_command();
         default:
             return type;
     }
 }
 
 int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
     bool in_quotes = false;
     int pos = 0, arg_pos = 0;
     clear_cmd_buff(cmd_buff);
 
     while (isspace(cmd_line[pos])) pos++;
     int arg_start = pos;
 
     while (cmd_line[pos] != '\0') {
         if (cmd_line[pos] == '"') {
             in_quotes = !in_quotes;
             if (!in_quotes) {
                 int len = pos - arg_start;
                 if (len > 0) {
                     cmd_buff->argv[arg_pos] = strndup(&cmd_line[arg_start], len);
                     arg_pos++;
                 }
                 arg_start = pos + 1;
             }
         } else if (isspace(cmd_line[pos]) && !in_quotes) {
             if (pos > arg_start) {
                 int len = pos - arg_start;
                 cmd_buff->argv[arg_pos] = strndup(&cmd_line[arg_start], len);
                 arg_pos++;
             }
             while (isspace(cmd_line[pos + 1])) pos++;
             arg_start = pos + 1;
         }
         pos++;
     }
  
 int alloc_cmd_buff(cmd_buff_t *cmd_buff) {
    cmd_buff->argc = 0;
    cmd_buff->_cmd_buffer = NULL;
    for (int i = 0; i < CMD_ARGV_MAX; i++) {
        cmd_buff->argv[i] = NULL;
    }
    return OK;
}

int free_cmd_buff(cmd_buff_t *cmd_buff) {
    if (cmd_buff->_cmd_buffer) {
        free(cmd_buff->_cmd_buffer);
        cmd_buff->_cmd_buffer = NULL;
    }
    for (int i = 0; i < cmd_buff->argc; i++) {
        if (cmd_buff->argv[i]) {
            free(cmd_buff->argv[i]);
            cmd_buff->argv[i] = NULL;
        }
    }
    cmd_buff->argc = 0;
    return OK;
}

int clear_cmd_buff(cmd_buff_t *cmd_buff) {
    free_cmd_buff(cmd_buff);
    return alloc_cmd_buff(cmd_buff);
}
     if (pos > arg_start) {
         int len = pos - arg_start;
         cmd_buff->argv[arg_pos] = strndup(&cmd_line[arg_start], len);
         arg_pos++;
     }
 
     cmd_buff->argc = arg_pos;
     cmd_buff->argv[arg_pos] = NULL;
     return (arg_pos > 0) ? OK : WARN_NO_CMDS;
 }
 
 int exec_cmd(cmd_buff_t *cmd) {
     pid_t pid = fork();
     if (pid < 0) {
         printf("Fork failed\n");
         last_return_code = 1;
         return ERR_EXEC_CMD;
     } else if (pid == 0) {
         execvp(cmd->argv[0], cmd->argv);
         if (errno == ENOENT) {
             printf("Command not found in PATH\n");
         } else if (errno == EACCES) {
             printf("Permission denied\n");
         } else {
             printf("Failed to execute command\n");
         }
         exit(errno);
     } else {
         int status;
         waitpid(pid, &status, 0);
         if (WIFEXITED(status)) {
             last_return_code = WEXITSTATUS(status);
             if (last_return_code == EACCES) {
                 printf("Permission denied\n");
             } else if (last_return_code == ENOENT) {
                 printf("Command not found in PATH\n");
             }
         }
     }
     return OK;
 }
 
 int exec_local_cmd_loop() {
     char cmd_line[SH_CMD_MAX];
     cmd_buff_t cmd;
     int rc;
     alloc_cmd_buff(&cmd);
 
     while (1) {
         printf("%s", SH_PROMPT);
         if (fgets(cmd_line, SH_CMD_MAX, stdin) == NULL) {
             printf("\n");
             break;
         }
         cmd_line[strcspn(cmd_line, "\n")] = '\0';
         rc = build_cmd_buff(cmd_line, &cmd);
         if (rc == WARN_NO_CMDS) continue;
         Built_In_Cmds cmd_type = exec_built_in_cmd(&cmd);
         if (cmd_type == BI_CMD_EXIT) break;
         else if (cmd_type == BI_EXECUTED) continue;
         exec_cmd(&cmd);
         free_cmd_buff(&cmd);  // Prevent memory leaks
     }
     free_cmd_buff(&cmd);
     return OK;
 }
 
