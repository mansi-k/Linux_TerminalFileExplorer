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

string create_fpath(string fp, string dp) {
    string fpath = "";
    if(dp=="") {
        fpath = app_home + fp;
//        cout << 4;
    }
    else if(dp[0]=='/') {
        fpath = app_home + dp.substr(1,dp.length()-1) + "/" + fp;
//        cout << 1;
    }
    else if(dp[0]=='.') {
        fpath = app_home + fp;
//        cout << 2;
    }
    else if(dp[0]=='~') {
        fpath = app_home + dp.substr(1,dp.length()-1) + "/" + fp;
//        cout << 3;
    }
    else {
        fpath = app_home + dp + fp;
//        cout << 5;
    }
    return fpath;
}
