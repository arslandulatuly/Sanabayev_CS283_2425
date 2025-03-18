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

//Arslan Sanabayev, as5764, 03.16.2025, CS283

static int last_return_code = 0;

Built_In_Cmds exec_cd_command(cmd_buff_t *cmd) {
    if (cmd->argc == 1) {
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

Built_In_Cmds exec_dragon_command() {
    const char* DRAGON_ART =
        "                                                                        @%%%%                       \n"
        "                                                                     %%%%%%                         \n"
        "                                                                    %%%%%%                          \n"
        "                                                                 % %%%%%%%           @              \n"
        "                                                                %%%%%%%%%%        %%%%%%%           \n"
        "                                       %%%%%%%  %%%%@         %%%%%%%%%%%%@    %%%%%%  @%%%%        \n"
        "                                  %%%%%%%%%%%%%%%%%%%%%%      %%%%%%%%%%%%%%%%%%%%%%%%%%%%          \n"
        "                                %%%%%%%%%%%%%%%%%%%%%%%%%%   %%%%%%%%%%%% %%%%%%%%%%%%%%%           \n"
        "                               %%%%%%%%%%%%%%%%%%%%%%%%%%%%% %%%%%%%%%%%%%%%%%%%     %%%            \n"
        "                             %%%%%%%%%%%%%%%%%%%%%%%%%%%%@ @%%%%%%%%%%%%%%%%%%        %%            \n"
        "                            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% %%%%%%%%%%%%%%%%%%%%%%                \n"
        "                            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%              \n"
        "                            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%@%%%%%%@              \n"
        "      %%%%%%%%@           %%%%%%%%%%%%%%%%        %%%%%%%%%%%%%%%%%%%%%%%%%%      %%                \n"
        "    %%%%%%%%%%%%%         %%@%%%%%%%%%%%%           %%%%%%%%%%% %%%%%%%%%%%%      @%                \n"
        "  %%%%%%%%%%   %%%        %%%%%%%%%%%%%%            %%%%%%%%%%%%%%%%%%%%%%%%                        \n"
        " %%%%%%%%%       %         %%%%%%%%%%%%%             %%%%%%%%%%%%@%%%%%%%%%%%                       \n"
        "%%%%%%%%%@                % %%%%%%%%%%%%%            @%%%%%%%%%%%%%%%%%%%%%%%%%                     \n"
        "%%%%%%%%@                 %%@%%%%%%%%%%%%            @%%%%%%%%%%%%%%%%%%%%%%%%%%%%                  \n"
        "%%%%%%%@                   %%%%%%%%%%%%%%%           %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%              \n"
        "%%%%%%%%%%                  %%%%%%%%%%%%%%%          %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%      %%%%  \n"
        "%%%%%%%%%@                   @%%%%%%%%%%%%%%         %%%%%%%%%%%%@ %%%% %%%%%%%%%%%%%%%%%   %%%%%%%%\n"
        "%%%%%%%%%%                  %%%%%%%%%%%%%%%%%        %%%%%%%%%%%%%      %%%%%%%%%%%%%%%%%% %%%%%%%%%\n"
        "%%%%%%%%%@%%@                %%%%%%%%%%%%%%%%@       %%%%%%%%%%%%%%     %%%%%%%%%%%%%%%%%%%%%%%%  %%\n"
        " %%%%%%%%%%                  % %%%%%%%%%%%%%%@        %%%%%%%%%%%%%%   %%%%%%%%%%%%%%%%%%%%%%%%%% %%\n"
        "  %%%%%%%%%%%%  @           %%%%%%%%%%%%%%%%%%        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  %%% \n"
        "   %%%%%%%%%%%%% %%  %  %@ %%%%%%%%%%%%%%%%%%          %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    %%% \n"
        "    %%%%%%%%%%%%%%%%%% %%%%%%%%%%%%%%%%%%%%%%           @%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    %%%%%%% \n"
        "     %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%              %%%%%%%%%%%%%%%%%%%%%%%%%%%%        %%%   \n"
        "      @%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                  %%%%%%%%%%%%%%%%%%%%%%%%%               \n"
        "        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                      %%%%%%%%%%%%%%%%%%%  %%%%%%%          \n"
        "           %%%%%%%%%%%%%%%%%%%%%%%%%%                           %%%%%%%%%%%%%%%  @%%%%%%%%%         \n"
        "              %%%%%%%%%%%%%%%%%%%%           @%@%                  @%%%%%%%%%%%%%%%%%%   %%%        \n"
        "                  %%%%%%%%%%%%%%%        %%%%%%%%%%                    %%%%%%%%%%%%%%%    %         \n"
        "                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                      %%%%%%%%%%%%%%            \n"
        "                %%%%%%%%%%%%%%%%%%%%%%%%%%  %%%% %%%                      %%%%%%%%%%  %%%@          \n"
        "                     %%%%%%%%%%%%%%%%%%% %%%%%% %%                          %%%%%%%%%%%%%@          \n"
        "                                                                                 %%%%%%%@           \n";
    printf("%s", DRAGON_ART);
    return BI_EXECUTED;
}

Built_In_Cmds match_command(const char *input) {
    if (strcmp(input, EXIT_CMD) == 0) return BI_CMD_EXIT;
    if (strcmp(input, "cd") == 0) return BI_CMD_CD;
    if (strcmp(input, "rc") == 0) return BI_EXECUTED;
    if (strcmp(input, "dragon") == 0) return BI_CMD_DRAGON;
    return BI_NOT_BI;
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    Built_In_Cmds type = match_command(cmd->argv[0]);
    switch (type) {
        case BI_CMD_CD: return exec_cd_command(cmd);
        case BI_EXECUTED: return exec_rc_command();
        case BI_CMD_DRAGON: return exec_dragon_command();
        default: return type;
    }
}

int alloc_cmd_buff(cmd_buff_t *cmd_buff) {
    cmd_buff->_cmd_buffer = malloc(SH_CMD_MAX);
    if (!cmd_buff->_cmd_buffer) return ERR_MEMORY;
    cmd_buff->argc = 0;
    cmd_buff->input_file = NULL;
    cmd_buff->output_file = NULL;
    cmd_buff->append_output = 0;
    for (int i = 0; i < CMD_ARGV_MAX; i++) {
        cmd_buff->argv[i] = NULL;
    }
    return OK;
}

int free_cmd_buff(cmd_buff_t *cmd_buff) {
    if (!cmd_buff) return OK;

    if (cmd_buff->input_file) {
        free(cmd_buff->input_file);
        cmd_buff->input_file = NULL;
    }
    if (cmd_buff->output_file) {
        free(cmd_buff->output_file);
        cmd_buff->output_file = NULL;
    }

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
    if (cmd_buff->input_file) {
        free(cmd_buff->input_file);
        cmd_buff->input_file = NULL;
    }
    if (cmd_buff->output_file) {
        free(cmd_buff->output_file);
        cmd_buff->output_file = NULL;
    }
    cmd_buff->append_output = 0;

    free_cmd_buff(cmd_buff);
    return alloc_cmd_buff(cmd_buff);
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    bool in_quotes = false;
    int pos = 0, arg_pos = 0;

    clear_cmd_buff(cmd_buff);

    while (isspace(cmd_line[pos])) pos++;
    int arg_start = pos;

    while (cmd_line[pos] != '\0') {
        if (cmd_line[pos] == '"') {  // handle quoted arguments
            in_quotes = !in_quotes;
            if (!in_quotes) {
                int len = pos - arg_start;
                if (len > 0 && arg_pos < CMD_ARGV_MAX - 1) {
                    cmd_buff->argv[arg_pos++] = strndup(&cmd_line[arg_start], len);
                }
                arg_start = pos + 1;
            } else {
                arg_start = pos + 1;
            }
        }
        else if ((cmd_line[pos] == '<' || cmd_line[pos] == '>') && !in_quotes) {  // handle redirections
            if (pos > arg_start && arg_pos < CMD_ARGV_MAX - 1) {
                int len = pos - arg_start;
                cmd_buff->argv[arg_pos++] = strndup(&cmd_line[arg_start], len);
            }

            bool append_mode = false;
            if (cmd_line[pos] == '>' && cmd_line[pos + 1] == '>') {  // check for append `>>`
                append_mode = true;
                pos++;
            }

            char redir_type = cmd_line[pos++];
            while (isspace(cmd_line[pos])) pos++;
            arg_start = pos;

            while (cmd_line[pos] != '\0' && !isspace(cmd_line[pos]) && cmd_line[pos] != '<' &&
                   cmd_line[pos] != '>' && cmd_line[pos] != '|') {
                pos++;
            }

            if (pos > arg_start) {
                char *filename = strndup(&cmd_line[arg_start], pos - arg_start);
                if (redir_type == '<') {
                    cmd_buff->input_file = filename;
                } else {
                    cmd_buff->output_file = filename;
                    cmd_buff->append_output = append_mode;
                }
                arg_start = pos;
            }
            continue;
        }
        else if (isspace(cmd_line[pos]) && !in_quotes) {
            if (pos > arg_start && arg_pos < CMD_ARGV_MAX - 1) {
                int len = pos - arg_start;
                cmd_buff->argv[arg_pos++] = strndup(&cmd_line[arg_start], len);
            }
            while (isspace(cmd_line[pos + 1])) pos++;
            arg_start = pos + 1;
        }
        pos++;
    }

    if (pos > arg_start && arg_pos < CMD_ARGV_MAX - 1) {  // add last argument if necessary
        int len = pos - arg_start;
        cmd_buff->argv[arg_pos++] = strndup(&cmd_line[arg_start], len);
    }

    cmd_buff->argc = arg_pos;
    cmd_buff->argv[arg_pos] = NULL;  // nullterminate argv list

    return (arg_pos > 0) ? OK : WARN_NO_CMDS;
}

int build_cmd_list(char *cmd_line, command_list_t *clist) {
    char *cmd_copy = strdup(cmd_line);  // make a copy to avoid modifying original
    if (!cmd_copy) return ERR_MEMORY;

    char *token;
    char *rest = cmd_copy;
    int cmd_count = 0;

    clist->num = 0;

    while ((token = strtok_r(rest, PIPE_STRING, &rest)) != NULL) {
        if (cmd_count >= CMD_MAX) {
            free(cmd_copy);
            return ERR_TOO_MANY_COMMANDS;
        }

        int rc = build_cmd_buff(token, &clist->commands[cmd_count]);
        if (rc == OK) {
            cmd_count++;
        }
    }

    free(cmd_copy);

    if (cmd_count == 0) {
        return WARN_NO_CMDS;
    }

    clist->num = cmd_count;
    return OK;
}

int free_cmd_list(command_list_t *cmd_list) {
    if (!cmd_list) return OK;

    for (int i = 0; i < cmd_list->num; i++) {
        free_cmd_buff(&cmd_list->commands[i]);
    }

    memset(cmd_list, 0, sizeof(command_list_t));
    return OK;
}

int exec_cmd(cmd_buff_t *cmd) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        last_return_code = errno;
        return ERR_EXEC_CMD;
    } else if (pid == 0) {
        // input redirections
        if (cmd->input_file) {
            int input_fd = open(cmd->input_file, O_RDONLY);
            if (input_fd == -1) {
                fprintf(stderr, "%s: %s\n", cmd->input_file, strerror(errno));
                exit(errno);
            }
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
        }

        if (cmd->output_file) {
            int flags = O_WRONLY | O_CREAT;
            if (cmd->append_output) {
                flags |= O_APPEND;
            } else {
                flags |= O_TRUNC;
            }
            int output_fd = open(cmd->output_file, flags, 0644);
            if (output_fd == -1) {
                fprintf(stderr, "%s: %s\n", cmd->output_file, strerror(errno));
                exit(errno);
            }
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
        }

        execvp(cmd->argv[0], cmd->argv);
        int err = errno;
        fprintf(stderr, "%s: %s\n", cmd->argv[0], strerror(err));
        exit(err);
    } else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            last_return_code = WEXITSTATUS(status);
        } else {
            last_return_code = 1;
        }
    }
    return OK;
}

int execute_pipeline(command_list_t *clist) {
    int num_cmds = clist->num;

    if (num_cmds == 0) {
        return WARN_NO_CMDS;
    }

    if (num_cmds == 1) {
        Built_In_Cmds cmd_type = exec_built_in_cmd(&clist->commands[0]);
        if (cmd_type == BI_CMD_EXIT) return OK_EXIT;
        if (cmd_type == BI_EXECUTED) return OK;
        return exec_cmd(&clist->commands[0]);
    }
    for (int i = 0; i < num_cmds; i++) {
        Built_In_Cmds cmd_type = match_command(clist->commands[i].argv[0]);
        if (cmd_type != BI_NOT_BI) {

            fprintf(stderr, "Error: Built-in commands cannot be used in pipelines\n");
            return ERR_EXEC_CMD;
        }
    }

    // to handle multiple commands with pipes
    int pipes[CMD_MAX - 1][2];
    pid_t pids[CMD_MAX];

    // create needed pipes
    for (int i = 0; i < num_cmds - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return ERR_EXEC_CMD;
        }
    }

    for (int i = 0; i < num_cmds; i++) {
        pids[i] = fork();

        if (pids[i] < 0) {
            perror("fork");
            return ERR_EXEC_CMD;
        } else if (pids[i] == 0) {  // child process
            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            } else if (clist->commands[i].input_file) {
                int input_fd = open(clist->commands[i].input_file, O_RDONLY);
                if (input_fd == -1) {
                    perror(clist->commands[i].input_file);
                    exit(errno);
                }
                dup2(input_fd, STDIN_FILENO);
                close(input_fd);
            }

            if (i < num_cmds - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            } else if (clist->commands[i].output_file) {
                int flags = O_WRONLY | O_CREAT | (clist->commands[i].append_output ? O_APPEND : O_TRUNC);
                int output_fd = open(clist->commands[i].output_file, flags, 0644);
                if (output_fd == -1) {
                    perror(clist->commands[i].output_file);
                    exit(errno);
                }
                dup2(output_fd, STDOUT_FILENO);
                close(output_fd);
            }

            // close pipe file descriptors in the child
            for (int j = 0; j < num_cmds - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            execvp(clist->commands[i].argv[0], clist->commands[i].argv);
            perror(clist->commands[i].argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    // close pipe file descriptors in the parent
    for (int i = 0; i < num_cmds - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // wait for child processes to complete
    int status;
    for (int i = 0; i < num_cmds; i++) {
        waitpid(pids[i], &status, 0);
        if (i == num_cmds - 1) { // only use the last command's return code
            if (WIFEXITED(status)) {
                last_return_code = WEXITSTATUS(status);
            } else {
                last_return_code = 1;
            }
        }
    }

    return OK;
}

int exec_local_cmd_loop() {
    char cmd_line[SH_CMD_MAX];
    command_list_t cmd_list;
    int rc;

    for (int i = 0; i < CMD_MAX; i++) {
        alloc_cmd_buff(&cmd_list.commands[i]);
    }
    cmd_list.num = 0;

    while (1) {
        printf("%s", SH_PROMPT);  
        if (fgets(cmd_line, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }

        cmd_line[strcspn(cmd_line, "\n")] = '\0';

        rc = build_cmd_list(cmd_line, &cmd_list);

        if (rc == WARN_NO_CMDS) {
            continue;
        } else if (rc == ERR_TOO_MANY_COMMANDS) {
            printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
            continue;
        }

        rc = execute_pipeline(&cmd_list);
        if (rc == OK_EXIT) {
            printf("exiting...\n");
            break;
        }

        for (int i = 0; i < cmd_list.num; i++) {
            clear_cmd_buff(&cmd_list.commands[i]);
        }
        cmd_list.num = 0;
    }

    free_cmd_list(&cmd_list);
    return OK;
}
