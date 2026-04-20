# MiniShellProject
Custom Linux MiniShell developed in C to execute system commands using fork(), exec(), and wait() system calls.
#  MiniShell — Linux Command Line Shell

##  Description

MiniShell is a custom Linux command-line shell developed using **C programming**.  
This project demonstrates the use of **Linux system calls** such as `fork()`, `exec()`, and `wait()` to create and manage processes.

The shell accepts user commands, executes them, and displays the output similar to a standard Linux terminal.

---

##  Features

- Execute Linux commands
- Process creation using `fork()`
- Command execution using `exec()`
- Process synchronization using `wait()`
- Continuous command execution loop
- Basic input parsing
- Error handling for invalid commands

---

##  Technologies Used

- **Programming Language:** C
- **Operating System:** Linux (Ubuntu)
- **Compiler:** GCC
- **Concepts:**  
  - System Calls  
  - Process Management  
  - Linux Internals  

---

##  Project Files

This project contains:

- `minishell.c` — Main shell implementation  
- `.h files` — Header files (if used)  
- `Makefile` — Compilation automation (if used)

---

##  How to Compile

Run the following command in Linux:

```bash
gcc minishell.c -o minishell
How to Run
./minishell
Sample Output
myshell> ls
file1.txt
file2.c

myshell> pwd
/home/user

myshell> exit
