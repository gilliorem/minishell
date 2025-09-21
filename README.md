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
Those three functions are being called when the signal SIGINT is sent.
CTRL C. most of the time, those 3 come together.
- rl_replace_line()
- rl_on_new_line()
- rl_redisplay()

#### History function
- add_history(): adds the cmd-line to a list. (history)
- rl_clear_history(): clear that list.

#### Fd functions
- fork(): initiate a new child process
- wait(): parent wait for its child to finish his process to be sure that parent does not run his process before child is done.
- access(): check file existence, type, permission
#### File functions
- stat():
- lstat():
- fstat():
- unlink():

#### SIGNALS functions
- signal(): listen to signal and run the callback function
- sigaction(): ?
- sigemptyset(): ?

#### Process functions
- kill(): kill a process: send a signal
- exit(): terminate current process
- execve(): execute a binary

#### ENVIRONMENT functions
- getcwd(): 
- chdir():
- getenv():

- opendir()
- readdir()
- closedir()

#### REDIRECTION functions
- dup2()
- pipe()

#### ERROR
- strerror()
- perror()

#### TERMINAL
- ttyname()
- ttyslot()
- ioctl()
- tcsetattr()


#### BONUS
- tc*
- tg*
- tputs()
