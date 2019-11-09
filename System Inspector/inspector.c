/**
 * @file
 *
 * System inspector implementation: a Unix utility that inspects the system it
 * runs on and creates a summarized report for the user using the proc pseudo
 * file system.
 *
 * See specification here: https://www.cs.usfca.edu/~mmalensek/cs326/assignments/project-1.html
 */

#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <math.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include "debug.h"

#define BUF_SZ 1024


/* Function prototypes */
void print_usage(char *argv[]);
void get_uptime(double num[2]);
int read_line(char *str, size_t, int fd);
void print_uptime();
double cpu_usage();
void loadavg();
char *trimwhitespace(char *str);
void taskcount();
char* itoa(int i, char b[]);


/**
 * This struct is a collection of booleans that controls whether or not the
 * various sections of the output are enabled.
 */
struct view_opts {
    bool hardware;
    bool live_view;
    bool system;
    bool task_list;
};


/**
 * Returns index of the first occurence of the delimeter
 * @param char* str1 - string to be read from
 * @param char* delm - delimeter to look for
 *
 *@return index of the first occurence of the delimeter
 */
int get_index(char* str1, char* delm){
    return strcspn(str1, delm);
}

/**
 * Tokenizer functions breaks up given string
 * according to the delimeter and saves it in
 * a pointer list and returns the size of the list
 * so the list can be iterated
 * @param char* retval[] - list to add the tokens
 * @param char* str1 - string to be tokenized
 * @param char* delm - delimeter
 *
 * @return the size of retval
 */
int tokenize(char* retval[], char* str1, char* delm){
    //    char* retval[BUF_SZ];
    int rv = 0;
    int n = get_index(str1, delm);          //get index at end of string
    
    while(n != strlen(str1)){               // keep iterating until the index matches end of string
        
        
        char* temp = malloc((n+1)*sizeof(*temp));           //allocate size for the first string
        int i;
        for(i = 0; i < n; i++) {            //copy string into temp value;
            temp[i] = str1[i];
        }
        temp[i] = '\0';
        char* temp2 = malloc((strlen(str1) - n+1)*sizeof(*temp2));          // allocate size of the rest of string for new variable
        for(int j = n+1, k = 0;  j < strlen(str1)+1; j++, k++){             // copy the remmaining string into a temporary variable to later manipulate the orginal string
            temp2[k] = str1[j];
        }
        retval[rv++] = temp;            // save the tokenized word( converted to floats) into a list we will return
        str1 = temp2;                   // Shorten str1 so the while loop runs for the rest of the string
        n = get_index(str1, delm);      // update the new starting index using the length of the next word
        
    }
    retval[rv++] = str1; // save the last value of the string to our return list.
    
    return rv;
    
}

/**
 * Reads one line of a file given File descriptor
 * @param char *str - the line that needs to be reade
 * @param size_t size - size of Buffer
 * @param int fd - the file desrciptor
 *
 * @return the size of line
 */
int read_line(char *str, size_t size, int fd) {
    
    int total = 0;
    char t;
    while (read(fd, &t, 1) > 0 ) { // if read is zero it is EOF
        
        *(str + total) = t;
        ++total;
        
        if((total + 2) > size){
            *(str + total) = '\0';
            return total;
        }
        
        if(t == '\n' ){
            break;
        }
        
        
    }
    *(str + total - 1) = '\0';
    return total;
    
}

/**
 * Prints help/program usage information.
 *
 * This output is displayed if there are issues with command line option parsing
 * or the user passes in the -h flag.
 */
void print_usage(char *argv[])
{
    printf("Usage: %s [-ahrst] [-l] [-p procfs_dir]\n" , argv[0]);
    printf("\n");
    printf("Options:\n"
           "    * -a              Display all (equivalent to -rst, default)\n"
           "    * -h              Help/usage information\n"
           "    * -l              Live view. Cannot be used with other view options.\n"
           "    * -p procfs_dir   Change the expected procfs mount point (default: /proc)\n"
           "    * -r              Hardware Information\n"
           "    * -s              System Information\n"
           "    * -t              Task Information\n");
    printf("\n");
    
}

/**
 * Reads version file and prints
 * kernal version
 */
void printkernel(){
    char kernel[BUF_SZ];
    char* smallList[BUF_SZ];
    int fd2 = open("version", O_RDONLY);
    
    read_line(kernel, BUF_SZ, fd2);
    tokenize(smallList, kernel, " ");
    
    printf("Kernel Version: %s\n", smallList[2]);
    close(fd2);
}

/**
 * Reads hostname file and prints
 * hostname
 */
void hostname(){
    char hostname[BUF_SZ];
    int fd = open("sys/kernel/hostname", O_RDONLY);
    
    read_line(hostname, BUF_SZ, fd);
    
    printf("Hostname: %s\n", hostname);
    close(fd);
}

/**
 * Prints the uptime by caling the getuptime function
 * and prints according to specific format
 * @param double list to update values
 */
void print_uptime(){
    double d[2];
    double uptime;
    int years;
    int days;
    int hours;
    int minutes;
    int seconds;
    
    
    get_uptime(d);
    //    Idle = d[1];
    
    uptime = d[0];
    years = (int)uptime/31536000;
    days = ((int)uptime % 31536000)/86400;
    hours = (((int)uptime % 31536000)%86400)/3600;
    minutes = ((((int)uptime % 31536000)%86400)%3600)/60;
    seconds = (((int)uptime % 31536000)%86400)%60;
    
    if (years == 0) {
        if (days == 0) {
            if (hours == 0) {
                printf("Uptime: %d minutes, %d seconds", minutes, seconds);
            } else {
                printf("Uptime: %d hours, %d minutes, %d seconds\n", hours, minutes, seconds);
            }
        } else {
            if (hours == 0) {
                printf("Uptime:%d days, %d minutes, %d seconds\n", days, minutes, seconds);
            } else {
                printf("Uptime:%d days, %d hours, %d minutes, %d seconds\n", days, hours, minutes, seconds);
            }
        }
    } else {
        printf("Uptime: %d years, ", years);
        
        
        if (days == 0) {
            if (hours == 0) {
                printf("%d minutes, %d seconds\n", minutes, seconds);
            } else {
                printf("%d hours, %d minutes, %d seconds\n", hours, minutes, seconds);
            }
        } else {
            if (hours == 0) {
                printf("%d days, %d minutes, %d seconds\n", days, minutes, seconds);
            } else {
                printf(" %d days, %d hours, %d minutes, %d seconds\n", days, hours, minutes, seconds);
            }
        }
    }
}


/**
 * Calculates the uptime by reading the uptime file in relative
 * proc location
 * @param double list to update values
 */

void get_uptime(double uptimeList[2]) {
    char uptime[BUF_SZ];
    char * p[BUF_SZ];
    int fd = open("uptime", O_RDONLY);
    read_line(uptime, BUF_SZ, fd);
    
    tokenize(p, uptime, " ");
    
    
    uptimeList[0] = atof(p[0]);
    uptimeList[1] = atof(p[1]);
}



/**
 * Calculates the CPU usage
 * by getting the uptime after one second intervals
 * @return the cpu usage time
 */
double cpu_usage(){
    double first[2];
    double second[2];
    double cpu_time;
    
    get_uptime(first);
    sleep(1);
    get_uptime(second);
    
    if((second[1] - first[1]) == 0){
        cpu_time = 0;
    }else{
        cpu_time = 1 - ((second[0] - first[0]) / (second[1] - first[1]));
    }
    
    return cpu_time;
}

/**
 * Calculates the CPU usage
 * by getting the uptime- used for
 * liveview
 * @param double list to update the values
 *
 * @return the cpu usage time
 */
double cpu_usage_liveview(double first[2]){
    double second2[2];
    double cpu_time;
    get_uptime(second2);
    
    cpu_time = 1 - ((second2[0] - first[0]) / (second2[1] - first[1]));
    
    return cpu_time;
}


/**
 * Prints out the percentage bar
 * for the CPU usage
 * @param number to fill the percentage till
 */
void percentage_bar(double percentage) {
    int num_hash = (percentage*100)/5;
    int num_hip = 20 - num_hash;
    int i = 0, j = 0;
    printf("[");
    while(i < num_hash){
        printf("#");
        i++;
    }
    while (j < num_hip) {
        printf("-");
        j++;
    }
    printf("]");
    printf(" %.1f%%", (100*percentage));
    
}

/**
 * Prints out the percentage bar
 * for the memory usage
 * @param number to fill the percentage till
 */
void percentage_bar_mem(double percentage) {
    int num_hash = (percentage*100)/5;
    int num_hip = 20 - num_hash;
    int i = 0, j = 0;
    printf("[");
    while(i < num_hash){
        printf("#");
        i++;
    }
    while (j < num_hip) {
        printf("-");
        j++;
    }
    printf("]");
    printf(" %.1f%%", (100*percentage));
}

/**
 * Trims the whitespace from given string
 * @param any string containing white space
 *
 * @return the string without whitespace
 */
char *trimwhitespace(char *str)
{
    char *end;
    
    // Trim leading space
    while(isspace((unsigned char)*str)) str++;
    
    if(*str == 0)  // All spaces?
        return str;
    
    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    
    // Write new null terminator character
    end[1] = '\0';
    
    return str;
}

/**
 * Finds the percentage of memory used
 * by reading the meminfo file from relative
 * proc directory and returns the percentage
 * of memory being used
 * @return percentage of memory being used
 */
double mem_usage(){
    char memTot[BUF_SZ];
    
    char* listTotal[BUF_SZ];
    
    double totalmem;
    double freemem;
    double used;
    
    
    int fd = open("meminfo", O_RDONLY);
    
    while(read_line(memTot, BUF_SZ, fd) != 0){
        char* temp;
        
        tokenize(listTotal, memTot, ":");
        temp = trimwhitespace(listTotal[0]);
        
        if(strcmp(temp, "MemFree") == 0){
            freemem = atof(listTotal[1]);
            
        }
        if(strcmp(temp, "MemTotal") == 0){
            totalmem = atof(listTotal[1]);
        }
    }
    
    used = totalmem - freemem;
    if(used < 0){
        return 0;
    }
    return used/totalmem;
}


/**
 * Parses the cpuinfo file from
 * relative proc directory and prints to
 * console the CPU Model and Processing
 * Units found then calls loadavg function
 */
void parse_cpuinfo(){
    int proc_count = 0;
    char model_name[BUF_SZ] ="";
    int fd = open("cpuinfo", O_RDONLY);
    char line[BUF_SZ];
    int found = 0;
    
    
    while(read_line(line, BUF_SZ, fd) != 0){
        int leng=0;
        char* tokens[strlen(line) +1];
        leng = tokenize(tokens, line, " :\t");
        if(((strcmp(tokens[0],"model") == 0)) && ((strcmp(tokens[1],"name") == 0)) && (found == 0) ){ // Iter through the rest of tokens list and concat the next token
            found = 1;
            for(int i = 4; i< leng; i++){
                strcat(model_name, tokens[i]);
                strcat(model_name, " ");
            }
        }
        if(strcmp(tokens[0], "processor") == 0){
            proc_count++;
        }
    }
    printf("\nCPU Model: %s\n", model_name);
    printf("Processing Units: %d\n", proc_count);
    loadavg();
}



/**
 * This is the live view function, repeatedly
 * calls percentage bars to be printed
 */
void live_view(){
    printf("\e[?25l");
    printf("Live View/Memory View\n");
    printf("--------------------\n");
    
    double cpu_sample;
    double cpu_1[2];
    double mem2;
    
    
    
    mem2 = mem_usage();
    
    get_uptime(cpu_1);
    cpu_sample = cpu_1[0];
    
    while(true) {
        double mem3;
        sleep(1);
        loadavg();
        cpu_sample = cpu_usage_liveview(cpu_1);
        mem3 = mem_usage();
        printf("CPU Usage:\t");
        percentage_bar(cpu_sample);
        printf("\nMemory Usage:\t");
        percentage_bar_mem(((mem3+mem2)/2));
        mem2 = mem3;
        printf("\r\033[A\033[A");
    }
    printf("\e[?25h");
}


/**
 * This checks if given number
 * is a digit
 * @param const char *s - any character
 * @return
 */
int digits_only(const char *s) {
    while (*s) {
        if (isdigit(*s++) == 0) {
            return 0;
        }
    }
    return 1;
}


/**
 * Checks the relative proc directory
 * and counts all process's then prints
 * to console
 */
void taskcount() {
    DIR *d;
    d = opendir("./");
    
    int taskcount= 0;
    
    
    if (d) {
        struct dirent *dir;
        while ((dir = readdir(d)) != NULL){
            
            if(digits_only(dir->d_name)){
                taskcount++;
            }
            
        }
    }
    printf("\nTasks Running: %d\n", taskcount-3);
}


/**
 * This functions prints all the
 * process found in the relative
 * proc directory and finds all
 * information Including PID, Task
 * name, Tasks and state then prints
 * in a specified format
 */
void num_process() {
    DIR *d;
    
    d = opendir("./");
    
    if (d) {
        int fd;
        char line[BUF_SZ];
        struct dirent *dir;
        char taskname[26];
        char* words[BUF_SZ];
        char user[16];
        char task[10];
        
        char state[13] = " ";
        char PID[6];
        int leng;
        
        printf("%5s | %12s | %25s | %15s | %s \n", "PID", "State", "Task Name", "User", "Tasks");
        printf("------+--------------+---------------------------+-----------------+-------");
        
        while ((dir = readdir(d)) != NULL){
            
            if (digits_only(dir->d_name)){
                strcat(dir->d_name,"/status");
                fd = open(dir->d_name, O_RDONLY);
                
                while(read_line(line, BUF_SZ, fd) != 0){
                    
                    leng = tokenize(words, line, ":");
                    
                    if(strcmp(words[0], "Name") == 0){
                        strncpy(taskname, trimwhitespace(words[leng - 1]),25);
                        strcat(taskname,"\0");
                    }
                    
                    if(strcmp(words[0], "State") == 0) {
                        char temp[13];
                        char *t2;
                        strcpy(temp, words[leng-1]);
                        t2 = trimwhitespace(temp);
                        
                        if(strcmp(t2, "S (sleeping)") == 0){
                            strncpy(state,"sleeping", 12);
                            strcat(state,"\0");
                        }else if(strcmp(t2, "I (idle)") == 0){
                            strncpy(state,"idle",12);
                            strcat(state,"\0");
                        }
                        else if(strcmp(t2, "A (active)") == 0){
                            strncpy(state,"active",12);
                            strcat(state,"\0");
                        }
                        else if(strcmp(t2, "Z (zombie)") == 0){
                            strncpy(state,"zombie",12);
                            strcat(state,"\0");
                        }
                        else if(strcmp(t2, "X (dead)") == 0){
                            strncpy(state,"dead",12);
                            strcat(state,"\0");
                        }
                        else if(strcmp(t2, "D (disk sleep)") == 0){
                            strncpy(state,"disk sleep",12);
                            strcat(state,"\0");
                        }
                        
                    }
                    
                    if(strcmp(words[0], "Pid") == 0){
                        strncpy(PID, trimwhitespace(words[leng -1]), 6);
                        strcat(PID,"\0");
                    }
                    
                    if(strcmp(words[0], "Uid") == 0){
                        if(getpwuid(atoi(words[leng -1])) == NULL){
                            itoa(atoi(words[leng -1]), user);
                            
                        }else{
                            strncpy(user, trimwhitespace(getpwuid(atoi(words[leng -1]))->pw_name), 15);
                            strcat(user,"\0");
                            
                        }
                    }
                    if(strcmp(words[0], "Threads") == 0) {
                        strncpy(task, trimwhitespace(words[leng -1]),9);
                        strcat(task,"\0");
                    }
                    
                }
            }
            if(strcmp(PID, "") !=0) {
                printf("\n%5s | %12s | %25s | %15s | %s ",
                       PID, state, taskname, user, task);
            }
        }
        
        //        closedir(d);
    }
    
}



/**
 * Converts integer to string
 * @param int i - integer to be converted
 * @param char b - string to add to
 *
 * @return returns the modified string
 */
char* itoa(int i, char b[]){
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}



/**
 * This functions finds the average
 * load times by reading loadavg
 */
void loadavg() {
    int fd = open("loadavg", O_RDONLY);
    char line[BUF_SZ];
    
    printf("Load Average (1/5/15 min): ");
    while(read_line(line, BUF_SZ, fd) != 0){
        char* tokens[strlen(line) +1];
        tokenize(tokens, line, " ");
        for(int s = 0; s < 3; s++) {
            printf("%s", tokens[s]);
        }
        printf("\n");
    }
}


/**
 * Main program entrypoint. Reads command line options and runs the appropriate
 * subroutines to display system information.
 */
int main(int argc, char *argv[])
{
    /* Default location of the proc file system */
    char *procfs_loc = "/proc";
    /* Set to true if we are using a non-default proc location */
    bool alt_proc = false;
    
    struct view_opts defaults = { true, false, true, true };
    struct view_opts options = { false, false, false, false };
    
    int c;
    opterr = 0;
    while ((c = getopt(argc, argv, "ahlp:rst")) != -1) {
        opterr++;
        
        switch (c) {
            case 'a':
                options = defaults;
                break;
            case 'h':
                print_usage(argv);
                return 0;
            case 'l':
                options.live_view = true;
                break;
            case 'p':
                procfs_loc = optarg;
                alt_proc = true;
                break;
            case 'r':
                options.hardware = true;
                
                break;
            case 's':
                options.system = true;
                break;
            case 't':
                options.task_list = true;
                break;
            case '?':
                if (optopt == 'p') {
                    fprintf(stderr,
                            "Option -%c requires an argument.\n", optopt);
                } else if (isprint(optopt)) {
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                } else {
                    fprintf(stderr,
                            "Unknown option character `\\x%x'.\n", optopt);
                }
                print_usage(argv);
                return 1;
            default:
                abort();
        }
        
        
    }
    
    if (alt_proc == true) {
        LOG("Using alternative proc directory: %s\n", procfs_loc);
        
        /* Remove two arguments from the count: one for -p, one for the
         * directory passed in: */
        argc = argc - 2;
    }
    
    if (argc <= 1) {
        /* No args (or -p only). Enable default options: */
        options = defaults;
    }
    if (chdir(procfs_loc) == -1) {
        perror("ERROR CHDIR");
        return -1;
    }
    if (options.system) {
        printf("System Information\n--------------------\n");
        hostname();
        printkernel();
        print_uptime();
        cpu_usage();
    }
    if (options.hardware) {
        printf("Hardware Information\n--------------------\n");
        double r;
        double p;
        r = mem_usage();
        parse_cpuinfo();
        p = cpu_usage();
        
        printf("\nCPU Usage: ");
        percentage_bar(p);
        printf("\nMemory Usage: ");
        percentage_bar_mem(r);
        printf(" (%.1f GB / 500.0GB)\n", r);
    }
    if (options.task_list) {
        printf("Task Information\n--------------------\n");
        
        taskcount();
        num_process();
    }
    
    if (options.live_view == true) {
        /* If live view is enabled, we will disable any other view options that
         * were passed in. */
        options = defaults;
        options.live_view = true;
        live_view();
        LOGP("Live view enabled. Ignoring other view options.\n");
    } else {
        LOG("View options selected: %s%s%s\n",
            options.hardware ? "hardware " : "",
            options.system ? "system " : "",
            options.task_list ? "task_list" : "");
    }
    
    return 0;
}

