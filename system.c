#include "main.h"
#include <string.h>

extern char *arr[200]; // External array of available external commands
extern char prompt[100]; // External prompt string
char in_string[100]; // Buffer to store the user input
int flag=0; // Flag to check if custom prompt is set
char *pmt; // Custom prompt string
int pid; // Process ID for current child
int ch_pid; // Process ID for background child

struct list *head; // Head of job list for background processes

// List of shell built-in commands
char *builtins[] = {"echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
					"set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source",
					"exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help", NULL};

// Signal handler for SIGINT, SIGTSTP, and SIGCHLD
void signal_handler(int signum)
{
	if(signum==SIGINT)
	{
		if(pid==0) // If not in child process
		{
			if(flag==0)
			{
				printf("\n%s\n",prompt); // Default prompt
			}
			else
			{
				printf("\n%s\n",pmt); // Custom prompt
			}
		}
	}
    else if(signum==SIGTSTP)
	{
		if(pid==0)
		{
			if(flag==0)
			{
				printf("\n%s\n",prompt);
			}
			else
			{
				printf("\n%s\n",pmt);
			}
		}
		else
		{
		     insert_data(); // Add background job to list
		}
	}
	if(signum == SIGCHLD)
	{
		waitpid(ch_pid,NULL,WNOHANG); // Clean up background child
	}
}

// Reads and handles user input
void scan_input(char *prompt, char *input_string)
{
	signal(SIGINT,signal_handler);
	signal(SIGTSTP,signal_handler);
	
	while(1)
	{
		if(flag==0)
		    printf("%s",prompt);
		else
		    printf("%s",pmt);
		input_string[0]='\0';
		scanf("%[^\n]",input_string); // Read full line
		strcpy(in_string,input_string);
		getchar(); // Consume newline
		if(strstr(input_string,"PS1")) // Check for prompt change
		{
			int len=strlen(input_string);
			if((strchr(input_string,' ')))
			{
				printf("%s : command not found\n",input_string);
			}
			else
			{
				flag=1;
				pmt=strchr(input_string,'='); // Set custom prompt
				pmt++;
			}
		}
		else
		{
			if (strcmp(input_string, "jobs") == 0)
            {
                print(head); // Show background jobs
            }
            else if (strcmp(input_string, "fg") == 0)
            {
				if(head==NULL)
				{
					printf("bash: %s: current: no such job\n",input_string);
				}
				else
				{
                    struct list *temp = head;
                    while (temp->ptr != NULL)
                    {
                       temp = temp->ptr;
                    }
                    kill(temp->spid, SIGCONT); // Resume job
                    waitpid(temp->spid, NULL, WUNTRACED); // Wait
                    printf("%s\n", temp->str);
                    delete_data(); // Remove from job list
				}
            }
            else if (strcmp(input_string, "bg") == 0)
            {
				if(head==NULL)
				{
					printf("bash: %s: current: no such job\n",input_string);
				}
				else{
                   struct list *temp = head;
                   while (temp->ptr != NULL)
                   {
                      temp = temp->ptr;
                   }
                   kill(temp->spid, SIGCONT); // Resume job in background
                   ch_pid=temp->spid;
                   signal(SIGCHLD, signal_handler);
                   printf("%s\n", temp->str);
                   delete_data();
				}
            }
		    char *cmd=get_command(input_string); // Extract command
		    int ret=check_command_type(cmd); // Determine command type
		    if(ret==EXTERNAL)
		    {
			    pid=fork();
			    if(pid==0)
			    {
				   signal(SIGINT,SIG_DFL);
				   signal(SIGTSTP,SIG_DFL);
				   execute_external_commands(input_string);
				   exit(0);
			    }
			    else if(pid > 0)
			    {
				  int wstatus;
				  waitpid(pid,&wstatus,WUNTRACED); // Wait for foreground
				  pid=0;
			    }
		    }
		    else if(ret == BUILTIN)
		    {
			    execute_internal_commands(input_string);
		    }
		}
	}
}

// Extract the first word (command) from input
char *get_command(char *input_string)
{
	char *new_arr=malloc(20);
	int i=0;
	while(input_string[i]!=' ' && input_string[i]!='\0')
	{
		new_arr[i]=input_string[i];
		i++;
	}
	new_arr[i]='\0';
	return new_arr;
}

// Check whether command is built-in, external, or not found
int check_command_type(char *command)
{
	int i=0;
	while(builtins[i]!=NULL)
	{
		if(strcmp(builtins[i],command)==0)
		{
			return BUILTIN;
		}
		i++;
	}
	i=0;
	while(arr[i]!=NULL)
	{
		if(strcmp(arr[i],command)==0)
		{
			return EXTERNAL;
		}
		i++;
	}
	return NO_COMMAND;
}

// Execute external commands (with or without pipes)
void execute_external_commands(char *input_string)
{
    char *args[100];
    int i = 0;
	if(strstr(input_string,"|"))
	{
		execute_pipe_command(input_string); // Handle piping
	}
	else 
	{
        args[i] = strtok(input_string, " ");
        while (args[i] != NULL && i < 99)
        {
              i++;
              args[i] = strtok(NULL, " ");
        }
        args[i] = NULL;

       if (execvp(args[0], args) == -1)
       {
          perror("execvp failed");
          exit(0);
       }
	}
}

// Handles piped commands
void execute_pipe_command(char *input_string)
{
	char *word[20];
	char buf[100];
	int len=strlen(input_string);
	input_string[len]='\0';
	int w=0,l=0,k=0;
	while(k<=len)
	{
		if(input_string[k]==' ' || input_string[k]=='\0')
		{
			buf[w]='\0';
			word[l]=malloc(strlen(buf)+1);
			strcpy(word[l],buf);
			l++;
			w=0;
		}
		else
		{
			buf[w]=input_string[k];
			w++;
		}
		k++;
	}
	int pipe_c=0;
	int arr[pipe_c+1];
	arr[0]=0;
	for(int i=0;i<l;i++)
	{
		if(strcmp(word[i],"|")==0)
		{
			pipe_c++;
			arr[pipe_c]=i+1;
			word[i]=NULL; // Null terminate previous command
		}
	}
	for(int i=0;i<pipe_c+1;i++)
	{
		int fd[2];
		if(i<pipe_c)
		{
			int p=pipe(fd);
			if(p==-1)
			{
				perror("p\n");
				return;
			}
		}
		int c=fork();
		if(c==-1)
		{ 
			perror("c\n");
			return;
		}
		if(c==0)
		{
			if(i<pipe_c)
			{
				close(fd[0]);
				dup2(fd[1],1); // Redirect stdout
			}
			execvp(word[arr[i]],word+arr[i]); // Execute command
		}
		else if(c>0)
		{
			if(i<pipe_c)
			{
				close(fd[1]);
				dup2(fd[0],0); // Redirect stdin
			}
		}
	}
	for(int i=0;i<pipe_c+1;i++)
	{
		wait(NULL); // Wait for all children
	}
}

// Executes built-in commands like cd, echo, etc.
void execute_internal_commands(char *input_string)
{
	char *args[100];
    int i = 0;

    args[i] = strtok(input_string, " ");
    while (args[i] != NULL && i < 99) {
        i++;
        args[i] = strtok(NULL, " ");
    }
    args[i] = NULL;

	if(strcmp(input_string,"exit")==0)
	{
		exit(0);
		printf("\n");
	}
	else if(strcmp(input_string,"pwd")==0)
	{
		char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL)
		{
            printf("%s\n", cwd);
        } else 
		{
            perror("pwd");
        }
	}
	else if(strcmp(input_string,"cd")==0)
	{
		char *ptr1=strchr(input_string,' ');
		if(ptr1!=NULL)
		{
		  printf("\x1b[34m%s\n",ptr1+1); // Show directory name
		  chdir(ptr1+1);
		}
	}
	else if(strcmp(args[0],"echo")==0)
	{
		if(strcmp(args[1],"$?")==0)
		{
			int wstatus;
			wait(&wstatus);
			printf("%d",WEXITSTATUS(wstatus)); // Print exit code
		}
		else if(strcmp(args[1],"$$")==0)
		{
			printf("%d\n ",getpid()); // Print current shell PID
		}
		else if(strcmp(args[1],"SHELL")==0 || strcmp(args[1],"shell")==0)
		{
			printf("%s\n", getenv("SHELL")); // Print shell path
		}
		else
		{
			printf("%s : command is not found\n",input_string);
		}
	}
	else 
	{
		printf("%s : command is not found\n",input_string);
	}
}

// Insert background job into list
int insert_data(void)
{
    struct list *new = malloc(sizeof(struct list));
    new->spid = pid;
    strcpy(new->str, in_string);
	printf("%s\n",new->str);
    new->ptr = NULL;
    if (head == NULL)
    {
        head = new;
        return 0;
    }
    struct list *temp = head;
    while (temp->ptr != NULL)
    {
        temp = temp->ptr;
    }
    temp->ptr = new;
    return 0;
}

// Print all jobs in the background list
int print(struct list *head)
{
    if (head == NULL)
        return 0;
    else
    {
        struct list *temp = head;
        while (temp != NULL)
        {
            printf("%d   ", temp->spid);
            printf("%s\n", temp->str);
            temp = temp->ptr;
        }
    }
}

// Delete the most recent job from background list
int delete_data(void)
{
    if (head == NULL)
    {
        return 0;
    }
    struct list *temp = head;
    struct list *temp1 = NULL;
    if (temp->ptr == NULL)
    {
        head = NULL;
        free(temp);
        return 0;
    }
    while (temp->ptr != NULL)
    {
        temp1 = temp;
        temp = temp->ptr;
    }
    free(temp);
    temp1->ptr = NULL;
    return 0;
}