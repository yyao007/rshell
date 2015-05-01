# rshell(hw1)
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

#### Added features for ls
I implemented my own `ls` command. You can run this by following instructions in section How to install rshell. The `ls` (to run my `ls`, you should type `bin/ls`) can:

1. Run with three flags `-a` `-l` `-R` and act just like the GNU `ls`.
2. Run with multiple flags together such as `ls -alR` which is the same as `ls -a -l -R`.
3. Support three corresponding flags `--all` = `-a`, `--long` = `-l`, `--recursive` = `-R`.
4. Handle optional file parameter. You can pass in zero, one, or many files. For example, if you pass in `ls ..`, then ls will list the files in the previous folder. If the parameter is a file rather than a directory, then ls will just print that file.
5. Display different types of files in different colors. It will print directories in blue; executables in green; and hidden files (with the `-a` flag only) with a gray background. These effects can be combinable.
6. Automatic indent. Act just like GNU ls's formatting. It can display files in standard order based on the current terminal size without splitting onto two lines.
7. The ls will give an error message of "Invalid flag" for flags beyond the listed three; "Cannot recognize option" for flags with the `--` prefix not in the list; and "ls: Cannot access ..." for invalid file parameters.

**All source codes can be found at [rshell/src](https://github.com/yyao007/rshell/tree/master/src)**

## How to install rshell
To run rshell on your Linux system, you need to run the following commands.
```
 $ git clone https://github.com/yyao007/rshell.git
 $ cd rshell
 $ git checkout hw0
 $ make
 $ bin/rshell
```
To run ls, you need to run the following commands:
```
 $ git clone https://github.com/yyao007/rshell.git
 $ cd rshell
 $ git checkout hw1
 $ make
 $ bin/ls # you can add parameters after bin/ls
```
Make sure you have installed the termcap library to run the ls. The termcap library and headers are part of the libncurses5-dev package, which can be installed using (following commands are for ubuntu users):
```
 sudo apt-get install libncurses5-dev
```

## Test cases
The test cases are in [rshell/tests](https://github.com/yyao007/rshell/tree/master/tests).

## Limitations
1. This first version of rshell can't run `cd` command, this feature will be added in future implementation.
2. This rshell now will regard the appearance of single `|` or `&` as syntax error.
3. rshell can only report two error messages and that won't be sufficient. Further error messages will be added in the future.

#### ls
1. The ls can only handle the three flags specified before. Any other flags will lead to an error message.
2. The order of the files may act different from the GNU ls. Cases like `ls ./ ..` will display the `..` directory first which actually should display the current folder first.

## Known bugs
* If you type any connector in the beginning of the command and then append another connector, such as `|| |` `&& &|`  `;;` `||||`, then rshell will report an error saying cannot start with connectors which actually should be reported as a syntax error. **(FIXED)**
* When you run the ls command with `-l` flag many times on the hammer server, an unexpected error of "do_ypcall: clnt_call: RPC: Unable to send; errno = Operation not permitted" will appear.

* Upon this time, rshell will not crash during runtime. If anyone can find a bug that would crash my rshell, please let me know and I will be appreciated.
