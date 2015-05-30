# rshell(hw2)
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
    * Any command has any combinations of `| & ;` (single character included), despite the correct connectors and pipe `|`.
    * Any command has spaces between connectors such as `& &`, `|| ||`, `; &&`.

#### Added features of ls
I implemented my own `ls` command. You can run this by following instructions in section How to install rshell. The `ls` (to run my `ls`, you should type `bin/ls`) can:

1. Run with three flags `-a` `-l` `-R` and act just like the GNU `ls`.

2. Run with multiple flags together such as `ls -alR` which is the same as `ls -a -l -R`.

3. Support three corresponding flags `--all` = `-a`, `--long` = `-l`, `--recursive` = `-R`.

4. Handle optional file parameter. You can pass in zero, one, or many files. For example, if you pass in `ls ..`, then ls will list the files in the previous folder. If the parameter is a file rather than a directory, then ls will just print that file.

5. Display different types of files in different colors. It will print directories in blue; executables in green; and hidden files (with the `-a` flag only) with a gray background. These effects can be combinable.

6. Automatic indent. Act just like GNU ls's formatting. It can display files in standard order based on the current terminal size without splitting onto two lines.

7. The ls will give an error message of "Invalid flag" for flags beyond the listed three; "Cannot recognize option" for flags with the `--` prefix not in the list; and "ls: Cannot access ..." for invalid file parameters.

#### Added features of IO redirection and piping
I implemented IO redirection and piping in my rshell. Now the rshell can:

1. Handle input redirection `<`, output redirection`>` and `>>`, and piping`|`.

2. Handle `<<<` input redirection that would redirect from a string instead of a file. For example, the two commands below will give the same output:
   ```
    $ echo hello world! | cat
    $ cat <<< "hello world!"
   ```
3. Perform output redirection on whatever file descriptors you want by placing a number before the `>` command. For example,
   ```
    $ g++ main.cpp 2> errorfile
   ```
   will direct stderr(which is where g++ prints error messages) to the errorfile.

4. Work all of the features together. For example,
   ```
    $ ls -l > file && cat < file | tr A-Z a-z | tee lowercase | tr a-z A-Z > uppercase
   ```
   will store all the informations of files in the current directory in lower cases in the file lowercase, and store them in upper cases in the file uppercase at the same time.

5. There are additional error messages in rshell:
    * Any command started with a `|`.
    * Any command ended with a `|`.
    * Any command has IO redirection operators other than the five operators given before.
    * Any command using `<<<` input redirection and has double quotes which are not in pairs.
    * Any command has nothing between redirect or piping operators.

#### Added features of cd and signals
Now the rshell can handle cd command to change directory and signals like ctrl C and ctrl Z without quitting rshell. Added features are:

1. `cd <PATH>` will change the current working directory to <PATH>. If the <PATH> doesn't exist, then cd will report an error message. If the <PATH> begins with a `/`, then it's an absolute path indicates that you can change to that directory wherever you are.

2. `cd` will change the current working directory to the user's home directory.

3. `cd -` will change the current working directory to the previous working directory.

4. The prompt now will display the current working directory. If that directory contains the user's home folder, then rshell will replace the home folder with a `~`. For example, my home folder is `/home/csmajs/yyao007`. So if I'm in the directory `/home/csmajs/yyao007/cs100/rshell`, then the prompt would display `~/cs100/rshell`.

5. If the user types ctrl+C which is `^C`, rshell will not exit. Instead, the current foreground job would exit itself. If the job is killed, then control will return to the rshell.

6. If the user types ctrl+Z which is `^Z`, rhsell will stop the current foreground job and put it in background. Then, if the user types `fg`, rshell will bring the newest background job to the foreground. The user can also type `bg` to let the newest background job continue running parallel with the rshell. If there is no background job, the `fg` and `bg` commands will report an error message.

7. rshell now will run the `ls` command of my own version.

**All source codes can be found at [rshell/src](https://github.com/yyao007/rshell/tree/master/src)**

## How to install rshell
To run rshell on your Linux system, you need to run the following commands.
```
 $ git clone https://github.com/yyao007/rshell.git
 $ cd rshell
 $ git checkout hw3
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
1. This first version of rshell can't run `cd` command, this feature will be added in future implementation. **(ADDED CD COMMAND)**

2. This rshell now will regard the appearance of single `|` or `&` as syntax error. **(ADDED PIPING `|` COMMAND)**

3. rshell can only report two error messages and that won't be sufficient. Further error messages will be added in the future.

4. rshell would only ignore double quotes when in `<<<` input string redirection mode.

#### ls
1. The ls can only handle the three flags specified before. Any other flags will lead to an error message.

2. The order of the files may act different from the GNU ls. Cases like `ls ./ ..` will display the `..` directory first which actually should display the current folder first.

3. The `fg` and `bg` commands cannot handle any flags so that they can only bring back the newest background job.

## Known bugs
* If you type any connector in the beginning of the command and then append another connector, such as `|| |` `&& &|`  `;;` `||||`, then rshell will report an error saying cannot start with connectors which actually should be reported as a syntax error. **(FIXED)**

* When you run the ls command with `-l` flag many times on the hammer server, an unexpected error of "do_ypcall: clnt_call: RPC: Unable to send; errno = Operation not permitted" will appear.

* When using the `<<<` operator, if there's a connector inside of a pair of double quotes, rshell will detect that connector rather than regard it as a part of the string.

* When the command is `./do | ls` while `./do` is a while loop which will output a string every second, then the pipe would last here for a second then continue to the next command prompt. If the user types ctrl+Z when the pipe pauses there, that signal would cause rshell to exit.

* Upon this time, rshell will not crash during runtime. If anyone can find a bug that would crash my rshell, please let me know and I will be appreciated.
