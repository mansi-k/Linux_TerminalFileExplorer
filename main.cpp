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
    if(cur_dir[cur_dir.length()-1]!='/')
        cur_dir += '/';
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
    scr_rows = terminal.ws_row;
    scr_cols = terminal.ws_col;
    term_row = terminal.ws_row - 10;
    term_col = terminal.ws_col;
    struct dirent *dent;
    DIR *cdir = opendir(cur_dir.c_str());
    if (cdir == NULL) {
//        cout << "Directory cannot be opened" << endl;
        perror("");
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
    closedir(cdir);
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
        cur_fpath = cur_dir + cur_files[i];
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
                }
                else {
                    pid_t pid = fork();
                    if (pid == 0) {
                        close(2);
                        execlp("vi", "vi", fpath.c_str(), NULL);
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
                xcor = 1;
                ycor = 1;
                setcout(term_row+6,1);
                cout << "***** NORMAL MODE *****" << endl;
            }
            cursor;
            fflush(0);
        }
        else {

        }
    }
}

void command_mode() {
    setcout(term_row+6,1);
    cout << "***** COMMAND MODE *****" << endl;
    xcor = term_row+7;
    while(xcor<=scr_rows) {
        cursor;
        clr_line;
        xcor++;
    }
    xcor = term_row+7;
    cursor;
    int cmdstx = xcor;
    fflush(0);
    char ipch;
    bool cmplt = false;
    string input;
    while(1) {
        ipch = getchar();
        if(ipch == 27) {
            return;
        }

        if(ipch == 10) { //enter
            if(input=="") {
                cout << "inif";
                while(xcor>=cmdstx) {
                    cursor;
                    clr_line;
                    xcor--;
                }
                xcor++;
//                cout << "here";
            }
            else {
                xcor++;
//            ycor=1;
                cursor;
//            cout << "command = " << input;
                cmplt = true;
                execute_cmd(input);
                input = "";
//                cout << "cameback";
            }
        }
        else if(ipch == 127) { //backspace
            input = input.substr(0,input.length()-1);
            cursor;
            clr_line;
            cout << input;
        }
        else {
            input += ipch;
            if(cmplt) {
                while(xcor>=cmdstx) {
                    cursor;
                    clr_line;
                    xcor--;
                }
                cmplt = false;
                xcor++;
//                ycor = 1;
            }
//            cursor;
            cout << ipch;
//            ycor++;
        }

    }
}

void execute_cmd(string input) {
//    cout << input;
    vector<string> cmd_words;
    string fullcmd = input;
    string word;
    string status;
    istringstream iss(input);
    for(string s; iss >> s; ) {
        cmd_words.push_back(s);
    }
    if(cmd_words[0] == "delete_dir") {
        status = cmd_delete_file(cmd_words);
    }
    else if(cmd_words[0] == "delete_file") {
        status = cmd_delete_file(cmd_words);
    }
    else if(cmd_words[0] == "create_file") {
        status = cmd_create_file(cmd_words);
    }
    else if(cmd_words[0] == "create_dir") {
        status = cmd_create_dir(cmd_words);
    }
    else if(cmd_words[0] == "rename") {
        status = cmd_rename(cmd_words);
    }
    else if(cmd_words[0] == "copy") {
        status = cmd_copy(cmd_words);
    }
    else if(cmd_words[0] == "move") {
        status = cmd_move(cmd_words);
    }
    if(status != "") {
        cout << status;
    }
}

string cmd_delete_file(vector<string> cw_vect) {
    string ret="Files deleted!", fpath;
    struct stat sfst;
    for(int j=1;j<cw_vect.size();j++) {
        fpath = create_fpath(cw_vect[j],"");
        stat(fpath.c_str(), &sfst);
        if ((sfst.st_mode & S_IFMT) & S_IFDIR) {
            ret = cmd_delete_dir(fpath);
        }
        int status = remove(fpath.c_str());
        if(status != 0) {
//            ret = "Failed! " + fpath;
            perror("\n");
            ret = "";
        }
    }
    return ret;
}

string cmd_delete_dir(string ddir) {
    string ret = "Files deleted!";
    string dpath;
    struct dirent *dent;
    struct stat sfst;
    DIR *cdir = opendir(ddir.c_str());
    if (cdir==NULL) {
        perror("");
        ret = "";
        return ret;
    }
    while((dent = readdir(cdir)) != NULL) {
        if (string(dent->d_name) != "." && string(dent->d_name) != "..") {
            dpath = ddir + "/" + dent->d_name;
            stat(dpath.c_str(), &sfst);
            if ((sfst.st_mode & S_IFMT) & S_IFDIR) {
                cmd_delete_dir(dpath);
            }
            int status = remove(dpath.c_str());
            if(status != 0) {
                perror("\n");
                ret = "";
            }
        }
    }
    closedir(cdir);
    return ret;
}

string cmd_create_file(vector<string> cw_vect) {
    string dstn = cw_vect[cw_vect.size()-1];
    int fdi;
    string ret = "Files created!";
    string fpath;
    for(int j=1;j<cw_vect.size()-1;j++) {
        fpath = create_fpath(cw_vect[j],dstn);
        fdi = open(fpath.c_str(),O_RDONLY | O_CREAT,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
        if(fdi < 0) {
//            ret = "Failed!";
            perror("\n");
            ret = "";
        }
        else
            close(fdi);
    }
    return ret;
}

string cmd_create_dir(vector<string> cw_vect) {
    string dstn = cw_vect[cw_vect.size()-1];
    int status;
    string ret = "Directories created!";
    string dpath;
    for(int j=1;j<cw_vect.size()-1;j++) {
        dpath = create_fpath(cw_vect[j],dstn);
        status = mkdir(dpath.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if(status != 0) {
//            ret = "Failed!";
            perror("\n");
            ret = "";
        }
    }
    return ret;
}

string cmd_rename(vector<string> cw_vect) {
    int status;
    string fold, fnew;
    string ret = "Renamed!";
    fold = create_fpath("",cw_vect[1]);
    fnew = create_fpath("",cw_vect[2]);
    status = rename(fold.c_str(),fnew.c_str());
    if(status!=0) {
        perror("\n");
        ret = "";
    }
    return ret;
}

string cmd_copy(vector<string> cw_vect) {
    int sfdi, dfdi, rblock, sch1, sch2;
    string ret = "Files copied!";
    string spath, dpath;
    char chunk[1024];
    struct stat sfst;
    for(int j=1;j<cw_vect.size()-1;j++) {
        dpath = create_fpath(cw_vect[j],cw_vect[cw_vect.size()-1]);
        spath = create_fpath(cw_vect[j],"");
        stat(spath.c_str(), &sfst);
        if ((sfst.st_mode & S_IFMT) & S_IFDIR) {
            ret = cmd_copy_dir(spath,dpath);
            continue;
        }
        else {
            sfdi = open(spath.c_str(), O_RDONLY);
            dfdi = open(dpath.c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
            while ((rblock = read(sfdi, chunk, sizeof(chunk))) > 0) {
                write(dfdi, chunk, rblock);
            }
            sch1 = chown(dpath.c_str(), sfst.st_uid, sfst.st_gid);
            sch2 = chmod(dpath.c_str(), sfst.st_mode);
            if (sfdi < 0 || dfdi < 0 || sch1 != 0 || sch2 != 0) {
                perror("");
                ret = "";
            }
            close(sfdi);
            close(dfdi);
        }
    }
    return ret;
}

string cmd_copy_dir(string sdir,string ddir) {
    int sfdi, dfdi, rblock, sch1, sch2;
    string ret = "Files copied!";
    string spath,dpath;
    char chunk[1024];
    struct dirent *dent;
    struct stat sfst;
    DIR *cdir = opendir(sdir.c_str());
    DIR *dsdr = opendir(ddir.c_str());
    if (cdir==NULL) {
        perror("");
        ret = "";
        return ret;
    }
    if(dsdr==NULL) {
        int mks = mkdir(ddir.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if(mks!=0) {
            perror("");
            ret = "";
            return ret;
        }
    }
    while((dent = readdir(cdir)) != NULL) {
        if(string(dent->d_name)!="." && string(dent->d_name)!="..") {
            dpath = ddir + "/" + dent->d_name;
            spath = sdir + "/" + dent->d_name;
            stat(spath.c_str(), &sfst);
            if ((sfst.st_mode & S_IFMT) & S_IFDIR) {
                cmd_copy_dir(spath,dpath);
                continue;
            } else {
                sfdi = open(spath.c_str(), O_RDONLY);
                dfdi = open(dpath.c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
                while ((rblock = read(sfdi, chunk, sizeof(chunk))) > 0) {
                    write(dfdi, chunk, rblock);
                }
                sch1 = chown(dpath.c_str(), sfst.st_uid, sfst.st_gid);
                sch2 = chmod(dpath.c_str(), sfst.st_mode);
                if (sfdi < 0 || dfdi < 0 || sch1 != 0 || sch2 != 0) {
                    perror("");
                    ret = "";
                }
                close(sfdi);
                close(dfdi);
            }
        }
    }
    closedir(cdir);
    closedir(dsdr);
    return ret;
}

string cmd_move(vector<string> cw_vect) {
    string ret = "Files moved!";
    string st1,st2;
    st1 = cmd_copy(cw_vect);
    vector<string> cmdvec;
    for(auto it=cw_vect.begin();it!=cw_vect.end()-1;it++) {
        cmdvec.push_back(*it);
    }
    st2 = cmd_delete_file(cmdvec);
    if(st1=="" || st2=="")
        return "";
    return ret;
}









