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
- rl_replace_line() first we replace the line with an empty buffer
- rl_on_new_line() 2nd we set the cursor at the begining
- rl_redisplay() 3rd we redraw the original prompt.

#### History function
*we go through the history-list of command using up arrow.*
- add_history(): adds the cmd-line to a list. (history)
- rl_clear_history(): clear that list.

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
- pipe() create a two ends-fds array. one end for read, the other to write. so you can use the output of a command as the input for another one and so on.

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

good seing Mohid 
