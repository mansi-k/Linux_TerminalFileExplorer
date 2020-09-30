#include <stdio.h>
#include <iostream>
#include <bits/stdc++.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>

using namespace std;

#define escp 27
#define clsr printf("%c[2J", escp)

extern stack<string> forward_stack;
extern stack<string> back_stack;
//extern vector<string> cur_files;
extern vector<dirent> cur_dirs;

int list_files();
