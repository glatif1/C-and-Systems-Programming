Writer: Ghufran Latif University of San Francisco 

Summary

Upon startup, your shell will print its prompt and wait for user input. 

Prompt
	The shell prompt displays some helpful information:

	Command number (starting from 0)
	User name and host name: (username)@(hostname) followed by :
	The current working directory
	Process exit status

Scripting
	Scripting mode reads commands from standard input and executes them without showing the prompt.

History
	Typing "history" shows the last 100 commands 
	EX: if 142 commands have been typed then commands from 42-142 will be shown.
	
Redirection
	Piping and redirection is also supported for this shell.
	EX: 
		"ls | wc -l" 
			will give you the total number of files 	within the directory and nothing else.
		"ls | wc -l > output.txt"
			will write the output to the file called output.txt


Note: ^C doesn’t terminate your shell. Type "exit".

