# rshell(hw0)
This rshell is a basic command shell which can implement several commands in Linux terminal. For example, if you type `ls` it will list the files in current directory and `pwd` to print current directory.

## Features
1. Print a command prompt containing login user name and host machine name.
2. Read in a single command on one line or several commands on one line by using connectors `||` or `&&` or `;` and execute.
3. You can type `exit` to exit rshell.
4. Anything that appears after a `#` character is considered a comment.
5. If the command line is ended with any connectors, rshell will prompt the user to input extra commands.
6. Rshell will give an error message if the input command line is invalid.

**All source codes can be found at /src directory**

## How to install rshell
To run rshell on your Linux system, you need to type the following commands.
```
 $ git clone https://github.com/yyao007/rshell.git
 $ cd rshell
 $ git checkout hw0
 $ make
 $ bin/rshell
```

## Limitations
1. This first version of rshell can't run `cd` command, this feature will be added in future implementation.
2. This rshell now will regard the appearance of single `|` or `&` as syntax error.

