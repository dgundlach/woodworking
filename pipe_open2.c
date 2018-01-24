//Unix version
//returns 1 on success, 0 on failure
//use this one to open both an input and output pipe to the executable
//
int pipe_open_2(char *cmd,            //duplicate this in argv[0]
                char *argv[],         //last argv[] must be NULL
                const char *type,     //"p" for execvp()
                FILE **pfp_read,      //read file handle returned
                FILE **pfp_write,     //write file handle returned
                int *ppid,            //process id returned
                int *fd_to_close_in_child,        //[in], array of fd
                int fd_to_close_in_child_count)   //number of valid fd
{
    int j, pfd_read[2], pfd_write[2];


    assert(pfp_read);
    assert(pfp_write);
    assert(cmd);
    assert(argv);
    assert(type);
    assert(ppid);
    assert(!fd_to_close_in_child_count || fd_to_close_in_child);
    if (!pfp_read || !pfp_write) return 0;
    *pfp_read=NULL;
    *pfp_write=NULL;
    if (!cmd || !argv || !type || !ppid) return 0;
    if (pipe(pfd_read)<0) return 0;
    if (pipe(pfd_write)<0) {
        close(pfd_read[0]);
        close(pfd_read[1]);
        return 0;
    }
    if ((*ppid=(int)fork())<0) {
        close(pfd_read[0]);
        close(pfd_read[1]);
        close(pfd_write[0]);
        close(pfd_write[1]);
        return 0;
    }
    if (!*ppid) {
        //child continues
        if (STDOUT_FILENO!=pfd_read[1]) {
            dup2(pfd_read[1],STDOUT_FILENO);
            close(pfd_read[1]);
        }
        close(pfd_read[0]);
        if (STDIN_FILENO!=pfd_write[0]) {
            dup2(pfd_write[0],STDIN_FILENO);
            close(pfd_write[0]);
        }
        close(pfd_write[1]);
        for (j=0; j<fd_to_close_in_child_count; j++)
            close(fd_to_close_in_child[j]);
        if (strstr(type,"p")) execvp(cmd, argv);
        else execv(cmd, argv);
        _exit(127); //execv() failed
    }
    //parent continues
    close(pfd_read[1]);
    close(pfd_write[0]);
    if (!(*pfp_read=fdopen(pfd_read[0],"r"))) {
        close(pfd_read[0]);
        close(pfd_write[1]);
        return 0;
    }
    if (!(*pfp_write=fdopen(pfd_write[1],"w"))) {
        fclose(*pfp_read);  //closing this also closes pfd_read[0]
        *pfp_read=NULL;
        close(pfd_write[1]);
        return 0;
    }
    return 1;
}    //pipe_open_2()



//Unix version
//returns 1 on success, 0 on failure
//
int pipe_close_2(FILE *fp_read,   //returned from pipe_open()
                 FILE *fp_write,  //returned from pipe_open()
                 int pid,         //returned from pipe_open()
                 int *result)     //can be NULL
{
    int res1, res2, status;


    if (result) *result=255;
    if (fp_read) res1=fclose(fp_read);
    else res1=0;
    if (fp_write) res2=fclose(fp_write);
    else res2=0;
    if (res1==EOF || res2==EOF) return 0;
    if (!pid) return 0;
    while (waitpid((pid_t)pid,&status,0)<0) if (EINTR!=errno) return 0;
    if (result && WIFEXITED(status)) *result=WEXITSTATUS(status);
    return 1;
}    //pipe_close_2()
