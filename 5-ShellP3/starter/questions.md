Arslan Sanabayev, as5764, 03.16.2025, CS283

1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

The implementation uses a loop to call waitpid() on each forked process ID, ensuring all child processes complete before the shell returns to the prompt. If waitpid() calls were omitted, zombie processes would accumulate, the shell would return to the prompt prematurely, pipeline execution would be unpredictable, return codes wouldn't be captured correctly, and system resources would be wasted by uncollected process entries in the process table.

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

Closing unused pipe ends after dup2() is necessary to prevent resource leaks, broken pipe handling, and hanging processes. Most critically, processes reading from pipes won't receive EOF until all write ends are closed. In a pipeline, if unused pipe ends remain open, reading processes might wait indefinitely for EOF, causing the pipeline to hang. The shell would also gradually exhaust the system's file descriptor table if these resources aren't properly released.

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

The cd command must be a built-in because it needs to change the current working directory of the shell process itself. If implemented as an external command, the directory change would only affect the child process executing cd, not the parent shell. When the child process exits, the shell would remain in its original directory, making the cd command effectively useless. Each Unix process has its own working directory context that doesn't affect its parent when modified.

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

To support arbitrary pipe chains, I would replace the fixed arrays with dynamically allocated memory. The command_list_t structure would store a pointer to a dynamically allocated array of command buffers instead of a fixed array. The initialization would count pipe symbols to estimate the initial allocation size, and the execute_pipeline function would dynamically allocate pipe and pid arrays based on the command count. The trade-offs include increased memory management complexity with potential for leaks, slower performance due to dynamic allocation overhead, the need for graceful allocation failure handling, and increased debugging difficulty. A hybrid approach could use fixed-size arrays for common cases and dynamically grow only when necessary, balancing simplicity and flexibility.
