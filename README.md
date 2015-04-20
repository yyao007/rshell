# rshell(hw0)
This rshell is a basic command shell which can implement several commands in Linux terminal. For example, if you type `ls` it will list the files in current directory and `pwd` to print current directory.

## Features
1. Print a command prompt containing login user name and host machine name.
2. Read in a single command on one line or several commands on one line by using connectors `||` or `&&` or `;` and execute.
3. You can type `exit` to exit rshell.
4. Anything that appears after a `#` character is considered a comment which will be ignored.
5. If the command line is ended with any connector, rshell will prompt the user to input extra commands until none of the input lines is ended with a connector or the former input commands are invalid. You can still type `#` on the extra input line and anything after that will be ignored.
6. Connectors are detected from left to right, which means `ls && pwd || ls -a` will only execute `ls` and `pwd`.
7. rshell will give an error message if the input command line is invalid. Invalid line includes but not limited to:
    * Any command starts with connectors `|| && ;`.
    * Any command has any combinations of `| & ;` (single character included), despite the correct connectors.
    * Any command has spaces between connectors such as `& &`, `|| ||`, `; &&`.

**All source codes can be found at rshell/src/rshell.cpp**

## How to install rshell
To run rshell on your Linux system, you need to type the following commands.
```
 $ git clone https://github.com/yyao007/rshell.git
 $ cd rshell
 $ git checkout hw0
 $ make
 $ bin/rshell
```

## Test cases
The test cases are in the rshell/tests/exec.script.

## Limitations
1. This first version of rshell can't run `cd` command, this feature will be added in future implementation.
2. This rshell now will regard the appearance of single `|` or `&` as syntax error.
3. rshell can only report two error messages and that won't be sufficient. Further error messages will be added in the future.

## Known bugs
* If you type any connector in the beginning of the command and then append another connector, such as `|| |` `&& &|`  `;;` `||||`, then rshell will report an error saying cannot start with connectors which actually should be reported as a syntax error.

* Upon this time, rshell will not crash during runtime. If anyone can find a bug that would crash my rshell, please let me know and I will be appreciated.
