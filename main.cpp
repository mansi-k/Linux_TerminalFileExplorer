#include "customheader.h"

vector<string> cur_files;
struct termios initialrsettings, newrsettings;
int exit_code = 0;
int to, from, findex=0;

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
    exit_code = list_files();
    xcor = 1;
    ycor = 1;
    pos();
    fflush(0);
    cls;
    return exit_code;
}

int list_files() {
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal);
    term_row = terminal.ws_row - 10;
    term_col = terminal.ws_col;
    struct dirent *dent;
    DIR *cdir = opendir(cur_dir.c_str());
    if (cdir == NULL) {
        cout << "Directory cannot be opened" << endl;
        return -1;
    }
    cur_files.clear();
    while ((dent = readdir(cdir)) != NULL) {
        cur_files.push_back(string(dent->d_name));
    }
    sort(cur_files.begin(),cur_files.end());
    string fullpath = cur_dir;
    findex = 0;
    from = cur_window;
    if (cur_files.size() <= term_row)
        to = cur_files.size() - 1;
    else
        to = term_row;
    update_list(fullpath);
    tcgetattr(fileno(stdin), &initialrsettings);
    newrsettings = initialrsettings;
    newrsettings.c_lflag &= ~ICANON;
    newrsettings.c_lflag &= ~ECHO;
    if (tcsetattr(fileno(stdin), TCSAFLUSH, &newrsettings) != 0)
    {
        fprintf(stderr, "Could not set attributes\n");
    }
    pos();
    fflush(0);
    travel(fullpath);
    return 0;
}

void update_list(string fullpath) {
    struct stat f_stat;
    string cur_fpath, fname, fperm, ftype, fmody, fuser, fgrp;
    long long fsize;
    char szu='B', ftyp='-';
    xcor = 1;
    ycor = 1;
    cls;
    pos();
    cout << fullpath << endl;
    cout << left << setw(35) << "Name" << setw(10) << "Size" << setw(10) << "User" << setw(10) << "Group" << setw(15) << "Type/Perm" << setw(10) << "Modified" << endl;
    for (int i = from; i <= to; i++,findex++) {
        szu='B';
        ftyp='-';
        fperm = "";
        cur_fpath = fullpath + "/" + cur_files[i];
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
//    cout << findex << endl;
    pos();
    fflush(0);
}

void travel(string fullpath) {
    while(1) {
        char ch[3] = { 0 };
        read(STDIN_FILENO, ch, 3);
        if(ch[0] == 'q') {
            tcsetattr(fileno(stdin), TCSANOW, &initialrsettings);
            xcor = 1;
            ycor = 1;
            pos();
            fflush(0);
            cls;
            exit(0);
        }
        else if (xcor>1 && ch[0]==27 && ch[1]=='[' && ch[2]=='A') {
            xcor--;
            pos();
        }
        else if(xcor<=(findex-from+1) && ch[0]==27 && ch[1]=='[' && ch[2]=='B') {
            xcor++;
            pos();
        }
        else if(ch[0]=='h') {
            findex = 0;
            cur_dir = app_home;
            list_files();
        }
        else if(ch[0]=='l') {
            if(cur_files.size()>findex+1) {
                from = findex;
                if (cur_files.size()-findex+1 <= term_row)
                    to = cur_files.size() - 1;
                else
                    to = term_row + findex;
                update_list(fullpath);
            }
        }
        else if(ch[0]=='k') {
            if(findex > term_row+1) {
                from -= term_row+1;
                to -= to%(term_row+1)+1;
                findex = from;
                update_list(fullpath);
            }
        }
        fflush(0);
    }
}
