#include "customheader.h"

vector<string> cur_files;
struct termios initialrsettings, newrsettings;
struct stat f_stat;
int exit_code = 0;
int to, from;

int main(int argc, char *argv[]) {
    char *tmp_cur_dir;
    if(argc == 2) {
        tmp_cur_dir = argv[1];
    }
    else if(argc == 1) {
        char buff[FILENAME_MAX];
        getcwd(buff, FILENAME_MAX);
        tmp_cur_dir = buff;
    }
    else {
        cout << "Argument is invalid" << endl;
    }
    cur_dir = string(tmp_cur_dir);
    app_home = cur_dir;
    set_termios();
    list_files();
    travel();
    xcor = 1;
    ycor = 1;
    cursor;
    fflush(0);
    cls;
    return 0;
}

void set_termios() {
    tcgetattr(fileno(stdin), &initialrsettings);
    newrsettings = initialrsettings;
    newrsettings.c_lflag &= ~ICANON;
    newrsettings.c_lflag &= ~ECHO;
    if (tcsetattr(fileno(stdin), TCSAFLUSH, &newrsettings) != 0)
    {
        fprintf(stderr, "Could not set attributes\n");
    }
}

void list_files() {
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal);
    term_row = terminal.ws_row - 10;
    term_col = terminal.ws_col;
    struct dirent *dent;
    DIR *cdir = opendir(cur_dir.c_str());
    if (cdir == NULL) {
        cout << "Directory cannot be opened" << endl;
        return;
    }
    cur_files.clear();
    while ((dent = readdir(cdir)) != NULL) {
        cur_files.push_back(string(dent->d_name));
    }
    sort(cur_files.begin(),cur_files.end());
    from = cur_window;
    if (cur_files.size() <= term_row)
        to = cur_files.size() - 1;
    else
        to = term_row;
    cursor;
    update_list();
    cursor;
    fflush(0);
//    travel(fullpath);
}

void update_list() {
    string cur_fpath, fname, fperm, ftype, fmody, fuser, fgrp;
    long long fsize;
    char szu='B', ftyp='-';
    xcor = 1;
    ycor = 1;
    cls;
    cursor;
    cout << cur_dir << endl;
    cout << left << setw(35) << "Name" << setw(10) << "Size" << setw(10) << "User" << setw(10) << "Group" << setw(15) << "Type/Perm" << setw(10) << "Modified" << endl;
    for (int i = from; i <= to; i++) {
        szu='B';
        ftyp='-';
        fperm = "";
        cur_fpath = cur_dir + "/" + cur_files[i];
        stat(cur_fpath.c_str(),&f_stat);
        fname = cur_files[i];
        cout << left << setw(35) << fname;
        fsize = f_stat.st_size;
        if(fsize >= (1<<30)) {
            fsize /= (1 << 30);
            szu = 'G';
        }
        else if(fsize >= (1<<20)) {
            fsize /= (1 << 20);
            szu = 'M';
        }
        else if(fsize >= (1<<10)) {
            fsize /= (1 << 10);
            szu = 'K';
        }
        cout << setw(10) << to_string(fsize)+szu;
        fuser = getpwuid(f_stat.st_uid)->pw_name;
        cout << setw(10) << fuser;
        fgrp = getgrgid(f_stat.st_gid)->gr_name;
        cout << setw(10) << fgrp;
        if((f_stat.st_mode & S_IFMT) & S_IFDIR){
            ftyp = 'd';
        }
        fperm += (f_stat.st_mode & S_IRUSR) ? "r" : "-";
        fperm += (f_stat.st_mode & S_IWUSR) ? "w" : "-";
        fperm += (f_stat.st_mode & S_IXUSR) ? "x" : "-";
        fperm += (f_stat.st_mode & S_IRGRP) ? "r" : "-";
        fperm += (f_stat.st_mode & S_IWGRP) ? "w" : "-";
        fperm += (f_stat.st_mode & S_IXGRP) ? "x" : "-";
        fperm += (f_stat.st_mode & S_IROTH) ? "r" : "-";
        fperm += (f_stat.st_mode & S_IWOTH) ? "w" : "-";
        fperm += (f_stat.st_mode & S_IXOTH) ? "x" : "-";
        cout << setw(15) << ftyp+fperm;
        fmody = string(ctime(&f_stat.st_mtime)).substr(0,24);
        cout << setw(25) << fmody << endl;
    }
    setcout(term_row+6,1);
    if(!cmd_mode)
        cout << "***** NORMAL MODE *****" << endl;
    else
        cout << "***** COMMAND MODE *****" << endl;
    cursor;
}

void travel() {
    while(1) {
        if(!cmd_mode) {
            char ch[3] = {0};
            read(STDIN_FILENO, ch, 3);
            if (ch[0] == 'e') {
                tcsetattr(fileno(stdin), TCSANOW, &initialrsettings);
                xcor = 1;
                ycor = 1;
                cursor;
                fflush(0);
                cls;
                exit(0);
            }
            else if (xcor > 1 && ch[0] == 27 && ch[1] == '[' && ch[2] == 'A') { //up
                xcor--;
            }
            else if (xcor <= (to - from + 2) && ch[0] == 27 && ch[1] == '[' && ch[2] == 'B') { //down
                xcor++;
            }
            else if (ch[0] == 'l') {
                if (cur_files.size() > to + 1) {
                    from++;
                    to++;
                    update_list();
                }
            }
            else if (ch[0] == 'k') {
                if (from > 0) {
                    from--;
                    to--;
                    update_list();
                }
            }
            else if (ch[0] == 27 && ch[1] == '[' && ch[2] == 'D') { //left
                if (!back_stack.empty() && back_stack.top() != cur_dir) {
                    forward_stack.push(cur_dir);
                    cur_dir = back_stack.top();
                    if (back_stack.size() > 1)
                        back_stack.pop();
                    list_files();
                }
            }
            else if (ch[0] == 27 && ch[1] == '[' && ch[2] == 'C') {
                if (!forward_stack.empty()) {
                    back_stack.push(cur_dir);
                    cur_dir = forward_stack.top();
                    forward_stack.pop();
                    list_files();
                }
            }
            else if (xcor > 2 && ch[0] == '\n') {
                string fpath = cur_dir + cur_files[from + xcor - 3];
                stat(fpath.c_str(), &f_stat);
                if ((f_stat.st_mode & S_IFMT) & S_IFDIR) {
                    if (cur_files[from + xcor - 3] == ".")
                        continue;
                    else if (cur_files[from + xcor - 3] == "..") {
                        if (back_stack.empty() || back_stack.top() != cur_dir)
                            back_stack.push(cur_dir);
                        int lidx = cur_dir.find_last_of('/', cur_dir.length() - 2);
                        cur_dir = cur_dir.substr(0, lidx) + "/";
                        list_files();
                    } else {
                        if (back_stack.empty() || back_stack.top() != cur_dir)
                            back_stack.push(cur_dir);
                        cur_dir = fpath + "/";
                        list_files();
                    }
                    while (!forward_stack.empty())
                        forward_stack.pop();
                } else {
                    pid_t pid = fork();
                    if (pid == 0) {
                    close(2);
                    execlp("vi", "vi", fpath.c_str(), NULL);
//                    execlp("xdg-open", "xdg-open", fpath.c_str(), NULL);
//                    tcsetattr(fileno(stdin), TCSANOW, &initialrsettings);
//                    execlp("vi", "vi", fpath.c_str(), NULL);
//                    execv(fpath.c_str(),"vi");
//                    set_termios();
                        exit(0);
                    }
                    int status;
                    waitpid(pid, &status, 0);
                    while(!WIFEXITED(status))
                        continue;
                }
            }
            else if (ch[0] == 127) { //backspace
                if (back_stack.empty() || back_stack.top() != cur_dir)
                    back_stack.push(cur_dir);
                int lidx = cur_dir.find_last_of('/', cur_dir.length() - 2);
                cur_dir = cur_dir.substr(0, lidx) + "/";
                while (!forward_stack.empty())
                    forward_stack.pop();
                list_files();
            }
            else if (ch[0] == 'h') {
                if (back_stack.empty() || back_stack.top() != cur_dir)
                    back_stack.push(cur_dir);
                cur_dir = app_home;
                while (!forward_stack.empty())
                    forward_stack.pop();
                list_files();
            }
            else if (ch[0] == 58) { //:
                command_mode();
            }
            cursor;
            fflush(0);
        }
        else {

        }
    }
}

void command_mode() {
    cmd_mode = true;
//    newrsettings.c_lflag &= ECHO;
//    tcsetattr(fileno(stdin), TCSAFLUSH, &newrsettings);
    tcsetattr(fileno(stdin), TCSANOW, &initialrsettings);
    setcout(term_row+6,1);
    cout << "***** COMMAND MODE *****" << endl;
    xcor = term_row+7;
    cursor;
    fflush(0);
    char c;
    string input="";
    while(c = getchar()) {
        input += c;
        if(c == 033) {
            setcout(term_row+6,1);
//            newrsettings.c_lflag &= ~ECHO;
//            tcsetattr(fileno(stdin), TCSAFLUSH, &newrsettings);
            set_termios();
            cmd_mode = false;
            cout << "***** NORMAL MODE *****" << endl;
            xcor = 1;
            ycor = 1;
            cursor;
            fflush(0);
            return;
        }
        if(c=='\n') {
            setcout(term_row + 8, 1);
            cout << input;
        }
    }
//    setcout(term_row+3,1);
//    cout << "mnsi";
}
