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
#include <sys/ioctl.h>

using namespace std;

#define escp 27
unsigned int xcor = 1, ycor = 1;
#define esc 27
#define cls printf("%c[2J", esc)
#define pos() printf("\033[%d;%df",xcor,ycor)

struct winsize terminal;
unsigned int term_row;
unsigned int term_col;
int cur_window = 0;
string cur_dir;
string app_home;
stack<string> forward_stack;
stack<string> back_stack;
vector<dirent> cur_dirs;

int list_files();
void update_list(string);
void travel(string);
