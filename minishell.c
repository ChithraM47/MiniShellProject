/*Implement a minimalistic shell, mini-shell(msh) as part of the Linux Internal module
Name :- Chithra M
Regd. no:- ECEP 25009F

*/

#include<stdio.h>  // Standard I/O functions
#include<string.h> // String manipulation functions
#include<unistd.h> // POSIX API (fork, exec, etc.)
#include<fcntl.h>  // File control options (open, O_RDONLY, etc.)
#include<sys/wait.h> // For wait and related macros
#include<sys/stat.h> // File status information
#include<stdlib.h> // Standard library functions (malloc, system, etc.)
#include "main.h" // Custom header file

char *arr[200]; // Array to store external command names
char prompt[100]="minishell$"; // Default shell prompt
char input_string[100]; // Buffer for user input

int main()
{
	char input_string[100]; // Local buffer for input
	system("clear"); // Clear the terminal screen

	extract_external_commands(); // Load external commands from file

	scan_input(prompt,input_string); // Start shell input loop
}

void extract_external_commands(void)
{
	char buffer[100]; // Buffer to hold each line
	char ch; // Character read from file
	int fd=open("f1.txt",O_RDONLY); // Open file for reading
	int i=0,l=0; // Index variables

	while((read(fd,&ch,1))>0) // Read file character by character
	{
		if(ch=='\n') // End of command line
		{
			buffer[l]='\0'; // Null-terminate the string
			arr[i]=malloc(sizeof(arr)); // Allocate memory for command string
			strcpy(arr[i],buffer); // Copy command into array
			i++; // Move to next array index
			l=0; // Reset buffer index
		}
		else
		{
			buffer[l++]=ch; // Add character to buffer
		}
	}
}
