# Minishell

A minimal Unix shell written in C.

This project re-implements core shell behavior: command parsing, pipes, redirections, environment handling, signals, and built-in commands.

The objective is to understand how a real shell interprets user input and interacts with the operating system.

---

# Overview

Minishell is structured into three main layers:

1. **Lexer** – Tokenizes the user input  
2. **Parser** – Builds an execution structure  
3. **Executor** – Walks the structure and runs commands  

Each layer has a clear responsibility and its own data structure.

---

# Entry Point

## `readline()`

```c
char *readline(char *prompt);
```
readline() waits for user input in an infinite loop.

Once the user submits a line:

It becomes the raw input string

It is added to history

It is tokenized by the lexer

It is parsed into an AST

It is executed

Main loop:
```c
while (1)
{
    input = readline(prompt);
    add_history(input);
    tokens = lexer(input);
    ast = parser(tokens);
    execute(ast);
}
```
## Lexer
### Responsibility

The lexer scans the raw input string character by character and classifies tokens.

Supported Tokens

WORD / COMMAND

PIPE |

REDIR_IN <

REDIR_OUT >

HEREDOC <<

APPEND >>

AND &&

OR ||

PARENTHESIS ( )

ASSIGNMENT FOO=bar

VARIABLE $USER

## Parser
### Responsibility

The parser transforms the flat token list into a structured representation suitable for execution.

It determines:

Command boundaries

Pipe structure

Redirections

Logical grouping

Output

## Executor
### Responsibility

The executor walks the AST recursively and executes commands.

It handles:

fork()

execve()

pipe()

dup2()

wait()

Each external command runs in a child process.

## Built-in Commands

echo

cd

pwd

export

unset

env

exit

Most built-ins use:

strcmp()

custom environment list manipulation

## Signals

signal()

sigaction()

kill()
