Arslan Sanabayev, CS283. 03/15/2025. Answers for 4-ShellP2

1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  Fork creates a child process, allowing the parent process (shell) to remain active while the command runs separately. If we called execvp directly, the shell itself would be replaced, preventing further command execution

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If fork() fails, it returns -1, indicating that no child process was created due to insufficient system resources. My implementation checks for this case and prints an error message ("Fork failed") while keeping the shell running.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  execvp() searches for the command in directories listed in the PATH environment variable. It tries each directory in PATH until it finds an executable file matching the command

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  wait() ensures the parent process (shell) waits for the child process (command execution) to complete before continuing

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  WEXITSTATUS(status) extracts the exit status of a terminated child process. This is important for checking whether a command executed successfully or failed.
    
6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  It treats quoted strings as single arguments, preserving spaces inside quotes while still splitting unquoted words correctly.


7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  I improved quote handling, space trimming, and memory safety. Also, no dragon (yet) TT

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  Signals are software interrupts used for interprocess communication (IPC). They differ from other IPC methods like pipes or shared memory) because they are asynchronous and can be sent to any process.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  SIGKILL () terminates a process immediately; cannot be caught or ignored; SIGTERM () - asks a process to terminate, allowing cleanup. SIGINT () - sent when pressing Ctrl+C to interrupt a running process. I use this one a lot!

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  SIGSTOP suspends a process. Unlike SIGINT, it cannot be caught or ignored because it’s handled at the kernel level, ensuring the process stops execution.
