# ls
A mimic of the `ls` command in Linux, incorporating functionalities similar to the `-a` and `-l` flags.

# How to Use
1. Compile the program using `gcc -o ls ls.c`
2. To execute, enter `./ls /directory_name`

## Using the Flags
- To use the `-a` flag, enter either
  - `./ls -a /directory_name`
  - `./ls /directory_name -a`

- To use the `-l` flag, enter either
  - `./ls -l /directory_name`
  - `./ls /directory_name -l`
 
# Example Usage
```bash
user@server:~$ ./ls lsTestFolder/
someCodingFile.c
timeMachinePlans.txt
user@server:~$ ./ls -a lsTestFolder/
.
..
someCodingFile.c
.superSecrectFile.txt
timeMachinePlans.txt
user@server:~$ ./ls -l lsTestFolder/
total 8
-rw-r--r-- 1 spyork users 463 Jul  1  17:27 someCodingFile.c
-rw-r--r-- 1 spyork users 2530 Jul  1  17:32 timeMachinePlans.txt
spyork@spirit:~$```
