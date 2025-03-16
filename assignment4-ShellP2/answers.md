Arslan Sanabayev, CS283. 03/15/2025. Answers for 4-ShellP2

1. Fork creates a child process, allowing the parent process (shell) to remain active while the command runs separately.
   If we called execvp directly, the shell itself would be replaced, preventing further command execution

2. If fork() fails, it returns -1, indicating that no child process was created due to insufficient system resources.
   My implementation checks for this case and prints an error message ("Fork failed") while keeping the shell running.

3. execvp() searches for the command in directories listed in the PATH environment variable.
   It tries each directory in PATH until it finds an executable file matching the command

4. wait() ensures the parent process (shell) waits for the child process (command execution) to complete before continuing

5. WEXITSTATUS(status) extracts the exit status of a terminated child process. This is important for checking whether
   a command executed successfully or failed.

6.  It treats quoted strings as single arguments, preserving spaces inside quotes while still splitting unquoted words correctly.

7.  I improved quote handling, space trimming, and memory safety. Also, no dragon (yet) TT

8.  a. Signals are software interrupts used for interprocess communication (IPC). They differ from other IPC methods
       like pipes or shared memory) because they are asynchronous and can be sent to any process.

    b. SIGKILL () terminates a process immediately; cannot be caught or ignored; SIGTERM () - asks a process to terminate, allowing cleanup.
       SIGINT () - sent when pressing Ctrl+C to interrupt a running process. I use this one a lot!

    c. SIGSTOP suspends a process. Unlike SIGINT, it cannot be caught or ignored because
    it’s handled at the kernel level, ensuring the process stops execution.


    
