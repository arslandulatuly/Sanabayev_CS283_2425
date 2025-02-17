#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    memset(clist, 0, sizeof(command_list_t));

    if (strlen(cmd_line) == 0) {
        return WARN_NO_CMDS;
    }
		
	 char *saveptr;
	 char *token = strtok_r(cmd_line, PIPE_STRING, &saveptr);
	 while (token != NULL) {
        // Check if we've exceeded maximum commands
        if (clist->num >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }
        
        // Skip leading spaces
        while (*token == SPACE_CHAR) token++;
        
        // Skip if empty after trimming
        if (strlen(token) == 0) {
            token = strtok_r(NULL, PIPE_STRING, &saveptr);
            continue;
        }
        
        command_t *curr_cmd = &clist->commands[clist->num];
        
        // Get the executable name (first word)
        char *space = strchr(token, SPACE_CHAR);
        if (space) {
            // We have arguments
            size_t exe_len = space - token;
            if (exe_len >= EXE_MAX) {
                return ERR_CMD_OR_ARGS_TOO_BIG;
            }
            strncpy(curr_cmd->exe, token, exe_len);
            curr_cmd->exe[exe_len] = '\0';
            
            // Skip spaces between exe and args
            while (*space == SPACE_CHAR) space++;
            
            // Copy args if any exist
            if (*space != '\0') {
                // Remove trailing spaces from args
                char *end = space + strlen(space) - 1;
                while (end > space && *end == SPACE_CHAR) end--;
                *(end + 1) = '\0';
                
                if (strlen(space) >= ARG_MAX) {
                    return ERR_CMD_OR_ARGS_TOO_BIG;
                }
                strcpy(curr_cmd->args, space);
            }
        } else {
            // No arguments, just executable
            char *end = token + strlen(token) - 1;
            while (end > token && *end == SPACE_CHAR) end--;
            *(end + 1) = '\0';
            
            if (strlen(token) >= EXE_MAX) {
                return ERR_CMD_OR_ARGS_TOO_BIG;
            }
            strcpy(curr_cmd->exe, token);
        }
        
        clist->num++;
        token = strtok_r(NULL, PIPE_STRING, &saveptr);
    }
    
    return OK;
}
