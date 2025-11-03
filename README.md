# minishell

## FUNCTIONS

### Type of functions

*readline*
char *readline(char *str)
This is basically the terminal waiting for a prompt.
readline() is running in an infinite loop waiting for the user's
line. This is the starting point of the shell.
Once the user has typed his line, it now becomes the line to be parsed.
Each element is being parsed and tokenized.

I've heard a lot of *LEXER* *TOKEN* *PARSER*.
Let's see what this is suppose to mean and how much sens it makes for us.

COMMAND/WORD echo hi
PIPE |
REDIRECTION < > << >> 
LOGICAL && ||
PARENTHESIS ()
ASSIGNEMENT/VARIABLE FOO=bar, $USER

So once we have the line. We need to scan through it to identify
the different TOKENS. we say that the `choper` is spliting the 
line-element into different categories...
Then it is the parser's job to figure out what is what.
left cmd, right cmd, redirect to outfile

built-in cmd execute differently
- echo
- cd
- pwd
- export
- unset
- env
- exit
*simple strcmp* + function that modify the env.

prompt the user and
returns the user-provided-input

#### readline functions (signal handler)
*The trio*
Those three functions are being called when the signal SIGINT is sent=
(CTRL C). those 3 functions always get call together.
- `rl_replace_line()` first we replace the line with an empty buffer
- `rl_on_new_line()` 2nd we set the cursor at the begining
- `rl_redisplay()` 3rd we redraw the original prompt.

#### History function
*we go through the history-list of command using up arrow.*
- `add_history()`: adds the cmd-line to a list. (history)
- `rl_clear_history()`: clear that list.

#### Fd functions
- fork(): initiate a new child process
- wait(): parent wait for its child to finish his process to be sure that parent does not run his process before child is done.
- access(): check file existence, type, permission

#### File functions
- stat(): gets info about the file
- lstat(): like stat but with symbolic file.
- fstat(): like fstat but we already have the fd.
- unlink(): delete a file

#### SIGNALS functions
- signal(): listen to signal and run the callback function
- sigaction(): determine and change a signal action.
- sigemptyset(): ?

#### Process functions
- kill(): kill a process: send a signal
- exit(): terminate current process
- execve(): execute a binary

#### ENVIRONMENT functions
- getcwd(): get the current directory path
- chdir(): change directory
- getenv(): get the shell variable list

- opendir() open a directory
- readdir() 
- closedir() close a directory

#### REDIRECTION functions
- dup2() redirection the input/output of a command. 
- pipe() create a two ends-fds array. one end for read, the other to write. 
so you can use the output of a command as the input for another one and so on.

#### ERROR
- strerror() print a specific error msg base on its error code
- perror() print on the stderror channel (fd = 2)

#### TERMINAL
- ttyname()
- ttyslot()
- ioctl()
- tcsetattr()

#### BONUS
- tc*
- tg*
- tputs()

- build in cmds
- other cmds to run based on the different context
- the AST: the binary tree that handle the execution part.

There are 3 main layers that each have a specific responsability
and is reprensented in a specific data structure

## INTERPRETING THE USER PROMPT

### THE LEXER 
Input: cli
Its responsability: check each character signification.
Form KEYWORDS.
< infile | cat -e
{LESSER, WORD, PIPE, WORD, WORD}
output
Data structure: a linked list 
the lexer goes through the `user input` and make out a linked list out of it.
a node is either a word, a pipe, a lesser, a double lesser, a greater, a double greater, an and, an or

### THE PARSER
Responsability: Assemble a structure of KEYWORDS that can be treated properly:
split the elements in the AST ready for execution
Data structure: link list

### EXECUTOR
Responsability: Execute each cmd in this specific order: fork, exec, pipe, redirect 
by walking the cmd tree and run it.
Data structure: AST (binary tree)
Based on the number of commands, the process will be repeted *number of cmd* time
Built-in commands are being processedd by the parent because they will change the environment (cd)
Each command is represented as a new child.

### THE EXECUTION
once the input is parsed, ready to be walked by the AST, we walk the AST recursively. as there is an operator/cmd
#### FOR BUILT IN CMDS

Oct 27, 20:03
For the command that modify the env list, do we have to user our own *envlist* ?
Or the shell has its own envlist (but where does it come from...?)
in the case that: we are using the actual shell env list, using expand (that will append a variable to our env list) do we have to malloc the whole list and add the variable ? or is there another solution ?

### THE ENV LIST 
As we run our minishell. we need to make a local copy of our env list.
So that if assign a variable and export it in the current process and we run a new minishell process, we can access the variable. so each time we run a minishell, it makes a copy of the previous/actual shell env list. 

So how to update the env list ?

- Make a copy of the env list
- store it in our own struct.
- all of our built-in build on that.

2. How to update

Let’s say you’ve got env as a char ** where each string looks like "KEY=value".

To update, find the variable with strncmp(env[i], key, keylen) and check that the next char is =.

Free that string.

Allocate a new "KEY=value" and replace it at the same index.

If the key doesn’t exist, you append it (realloc your array).

Nov 4, 02:59
So the plan is to merge the current codebase with Mohid's new lexer.
The lexer store the content in a linked list. with a value a type and the next token.
