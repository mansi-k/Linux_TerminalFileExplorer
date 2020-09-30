#include "customheader.h"

vector<string> cur_files;
char *cur_dir;
int exit_code = 0;

int main(int argc, char *argv[]) {
    if(argc == 2) {
        cur_dir = argv[1];
    }
    else if(argc == 1) {
        char cd[] = {'.','\0'};
        cur_dir = cd;
        getcwd(cur_dir, sizeof(cur_dir));
    }
    else {
        cout << "Argument is invalid" << endl;
    }
    exit_code = list_files();
    return exit_code;
}

int list_files() {
    struct dirent *dent;
    DIR *cdir = opendir(cur_dir);
    if (cdir == NULL) {
        printf("Directory cannot be opened");
        return -1;
    }
    cur_files.clear();
    clsr;
    while ((dent = readdir(cdir)) != NULL) {
//        if(strcmp("..",dent->d_name) == 0)
//            continue;
        cur_files.push_back(string(dent->d_name));

    }
    sort(cur_files.begin(),cur_files.end());
    struct stat f_stat;
    string fullpath = string(cur_dir);
    string cur_fpath, fname, fperm="", ftype, fmody, fuser, fgrp;
    long long fsize;
    char szu='B', ftyp='-';
    clsr;
    cout << left << setw(25) << "Name" << setw(10) << "Size" << setw(10) << "User" << setw(10) << "Group" << setw(15) << "Type/Perm" << setw(10) << "Modified" << endl;
    for(auto itr=cur_files.begin();itr!=cur_files.end();itr++) {
        szu='B';
        ftyp='-';
        cur_fpath = fullpath + "/" + *itr;
        stat(cur_fpath.c_str(),&f_stat);
        fname = *itr;
//        printf("%s\t",fname.c_str());
        cout << left << setw(25) << fname;
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
//        printf("%lld%c\t",fsize,szu);
        cout << setw(10) << to_string(fsize)+szu;
        fuser = getpwuid(f_stat.st_uid)->pw_name;
        cout << setw(10) << fuser;
        fgrp = getgrgid(f_stat.st_gid)->gr_name;
        cout << setw(10) << fgrp;
        if((f_stat.st_mode & S_IFMT) & S_IFDIR){
            ftyp = 'd';
        }
//        printf("%c",ftyp);
//        printf((f_stat.st_mode & S_IRUSR) ? "r" : "-");
//        printf((f_stat.st_mode & S_IWUSR) ? "w" : "-");
//        printf((f_stat.st_mode & S_IXUSR) ? "x" : "-");
//        printf((f_stat.st_mode & S_IRGRP) ? "r" : "-");
//        printf((f_stat.st_mode & S_IWGRP) ? "w" : "-");
//        printf((f_stat.st_mode & S_IXGRP) ? "x" : "-");
//        printf((f_stat.st_mode & S_IROTH) ? "r" : "-");
//        printf((f_stat.st_mode & S_IWOTH) ? "w" : "-");
//        printf((f_stat.st_mode & S_IXOTH) ? "x" : "-");
//        printf("\t");
        fperm = "";
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
        fmody = string(ctime(&f_stat.st_mtime)).substr(4,12);
//        printf(" %-12s ", fmody.c_str());
//        printf("\n");
        cout << setw(20) << fmody << endl;
    }
//    cout << left << setw(25) << "CwfregOL1" << setw(25) << "COferfeL2" << endl;
//    cout << left << setw(25) << "COLgg1" << setw(25) << "COL2" << endl;
    closedir(cdir);
    return 0;
}