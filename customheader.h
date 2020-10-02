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
#include <sys/wait.h>

using namespace std;

unsigned int xcor = 1, ycor = 1;
#define esc 27
#define cls printf("%c[2J", esc)
#define cursor printf("\033[%d;%df",xcor,ycor)
#define setcout(cx,cy) printf("\033[%d;%df",cx,cy)
#define clr_line printf("%c[2K", 27)

bool cmd_mode = false;
struct winsize terminal;
unsigned int term_row;
unsigned int term_col;
int cur_window = 0;
string cur_dir;
string app_home;
stack<string> forward_stack;
stack<string> back_stack;
vector<dirent> cur_dirs;
int scr_rows;
int scr_cols;

void list_files();
void update_list();
void travel();
void set_termios();
void reset_termios();
void command_mode();
void execute_cmd(string);
