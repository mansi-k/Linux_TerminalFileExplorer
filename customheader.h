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
#include <fcntl.h>
#include <signal.h>

using namespace std;

unsigned int xcor = 1, ycor = 1;
#define esc 27
#define cls printf("%c[2J", esc)
#define cursor printf("\033[%d;%df",xcor,ycor)
#define setcout(cx,cy) printf("\033[%d;%df",cx,cy)
#define clr_line printf("%c[2K", esc)

vector<string> cur_files;
struct termios initsetg, newsetg;
struct stat f_stat;
int to, from;
bool cmd_mode = false;
struct winsize terminal;
unsigned int term_row, scr_rows;
unsigned int term_col, scr_cols;
int cur_window = 0;
string cur_dir;
string app_home;
stack<string> forward_stack;
stack<string> back_stack;
vector<dirent> cur_dirs;

void list_files();
void update_list();
void travel();
void set_termios();
void command_mode();
void execute_cmd(string);
string cmd_delete_dir(string);
string cmd_delete_file(vector<string>);
string cmd_create_file(vector<string>);
string cmd_create_dir(vector<string>);
string cmd_rename(vector<string>);
string cmd_copy(vector<string>);
string cmd_copy_dir(string,string);
string cmd_move(vector<string>);
string cmd_search(vector<string>);
string cmd_search_dir(string,string);
void cmd_goto(vector<string>);
void winsz_handler(int);


string trimpath(string fp) {
    if(fp=="")
        return "";
    if(fp[fp.length()-1]=='/')
        fp = fp.substr(0,fp.length()-1);
    string fpath;
    if(fp[0]=='/')
        fpath = fp.substr(1,fp.length()-1);
    else if(fp[1]=='/' && (fp[0]=='.' || fp[0]=='~'))
        fpath = fp.substr(2,fp.length()-2);
    else if(fp=="." || fp=="~")
        fpath = "";
    else
        fpath = fp;
    return fpath;
}
