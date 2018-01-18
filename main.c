#include <stdio.h>
#include <stdlib.h>
#include <string.h>     // for strcmp, strlen, strcpy, strcat, ...

// Imports for Forks
#include <unistd.h>
#include <signal.h>     // for kill (killing child processes)

// Imports for Shared Memory Segment
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>

// Imports for ncurses functionality
#include <ncurses.h>
#include <sys/ioctl.h>  // for max window size
#include <pthread.h>    // for mutex locks

// Imports for Alarm and Time Panel
#include <time.h>
#include <sys/time.h>

int task1();
int task2(); void signal_handler(int sig);
int task3();

// Shared Memory Segment structs:
struct alarmInfo{
    int colour;
    char message[32];
    int alarmLC;
};

struct timeZones{
    char USAtime[64];
    char MALTAtime[64];
    char TOKYOtime[64];
};

// Global Variables:

// Internal Shell variable, time between every Time Panel refresh
unsigned int refreshTime = 1;

// Two timespec structs which are used to calculate the difference between 2 consecutive SIGALRMs
struct timespec time1;
struct timespec time2;

// The y-size (height) of the Alarm Panel
int alarmY;

// Boolean value (stored as int) which terminates all infinite loops when exiting the program, thus allowing
// the methods to complete the clean up tasks after the end of their loops
int runLoop = 1;

// Declaring the printLock Mutex Lock as a global variable
pthread_mutex_t printLock;

int main(void){
    pthread_mutex_init(&printLock, NULL);

    // Declaring the signal handler that will be receiving & handling SIGALRM calls from the presblock daemon
    signal(SIGALRM, (__sighandler_t) signal_handler);
    // Displaying the Process ID that presblock needs to be provided
    printf("Please enter this PID inside presblock: %d", getpid());
    fflush(stdout);
    sleep(5);

    // Forking the Parent Process into 2 child processes
    pid_t child1, child2;

    if (!(child1 = fork())) {
        // first child - Alarm Panel
        task2();
        _exit(0);
    } else if (!(child2 = fork())) {
        // second child - Time Panel
        task3();
        _exit(0);
    } else {
        // parent
        task1();
    }

    // By setting runLoop to 0, all 'infinite' loops will terminate
    runLoop = 0;
    // Give the methods some time to execute the code after the 'infinite' loops (clean up) before killing the processes
    sleep(2);

    // Killing off the child processes - (SIGTERM can be used to let processes clean up)
    kill(child1, SIGKILL);
    kill(child2, SIGKILL);

    return 0;
}

int task1(){
    WINDOW * mainwin;
    WINDOW * promptPanel, * outputPanel;
    WINDOW * alarmPanel, * colourPanel;
    WINDOW * timePanel;

    // Getting maximum dimensions of terminal
    //getmaxyx(mainwin, mainwinY, mainwinX);
    struct winsize mainwinSize;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &mainwinSize);

    // Setting sizes of every window
    int mainwinY = mainwinSize.ws_row, mainwinX = mainwinSize.ws_col;

    int promptY  = mainwinY/4, promptX = mainwinX;      // Y=24 or 43 - X=80 or 143 (windowed 24*80)(Fullscreen 43*143)
    int outputY = mainwinY/2, outputX = mainwinX;
    //int outputY = 80, outputX = 256;

    alarmY = mainwinY/4; int alarmX = mainwinX*3/8;
    int colourY = mainwinY/4, colourX = mainwinX/8;

    int timeY = mainwinY/4, timeX = mainwinX/2;

    // Initialize ncurses
    if ( (mainwin = initscr()) == NULL ) {
        fprintf(stderr, "Error initialising ncurses.\n");
        exit(EXIT_FAILURE);
    }

    // Switch off echoing
    //noecho();
    // Switch off cursor
    curs_set(0);

    // subwin(WINDOW, sizeY, sizeX, locationY, locationX);
    // Initializing the prompt panel
    promptPanel = subwin(mainwin, promptY, promptX, (mainwinY*3/4), 0);
    // add a border to the prompt panel
    box(promptPanel, 0, 0);
    wrefresh(promptPanel);
    // Initializing the output panel
    outputPanel = subwin(mainwin, outputY, outputX, (mainwinY*1/4), 0);
    box(outputPanel, 0, 0);
    wrefresh(outputPanel);

    // Initializing the alarm panel
    alarmPanel = subwin(mainwin, alarmY, alarmX, 0, timeX);
    box(alarmPanel, 0, 0);
    wrefresh(alarmPanel);
    // Initializing the colour panel
    colourPanel = subwin(mainwin, colourY, colourX, 0, (timeX+alarmX));
    box(colourPanel, 0, 0);
    wrefresh(colourPanel);

    // Initializing the time panel
    timePanel = subwin(mainwin, timeY, timeX, 0, 0);
    box(timePanel, 0, 0);
    wrefresh(timePanel);



    // Private Shared Memory Segments

    // Alarm Panel Private Shared Memory Segment identifier
    key_t alarmKey = 0x0001;
    size_t alarmSize = sizeof(struct alarmInfo);   // alarmY*alarmX (+colourY)
    int alarm_shmid = shmget(alarmKey, alarmSize, 0666);
    if (alarm_shmid < 0) {
        perror("shmget");
        exit(1);
    }
    // Attaching to the Private Shared Memory Segment which is storing the Alarm Panel
    struct alarmInfo * alarm_shm = (struct alarmInfo *) shmat(alarm_shmid, NULL, 0);
    if (alarm_shm == (struct alarmInfo *) -1) {
        perror("shmat");
        exit(1);
    }

    // Time Panel Private Shared Memory Segment identifier
    key_t timeKey = 0x0002;
    size_t timeSize = sizeof(struct timeZones);   // timeY*timeX
    int time_shmid = shmget(timeKey, timeSize, 0666);
    if (time_shmid < 0) {
        perror("shmget");
        exit(1);
    }
    // Attaching to the Private Shared Memory Segment which is storing the Time Panel
    struct timeZones * time_shm = (struct timeZones *) shmat(time_shmid, NULL, 0);
    if (time_shm == (struct timeZones *) -1) {
        perror("shmat");
        exit(1);
    }


    // Starting Colours in ncurses
    start_color();

    // Change the RGB values of the colour YELLOW to those of the colour orange
    init_color(COLOR_YELLOW, 1000, 647, 0);

    // Defining the colour pairs which will be used for the alarm colour bar
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    init_pair(2, COLOR_BLACK, COLOR_RED);
    init_pair(3, COLOR_BLACK, COLOR_YELLOW);
    init_pair(4, COLOR_BLACK, COLOR_GREEN);
    init_pair(5, COLOR_BLACK, COLOR_BLUE);

    // Alarm Panel Updater - Reads from Alarm Shared Memory Segment and outputs to Alarm Panel
    pid_t alarmPanelMGR = fork();
    if (alarmPanelMGR < 0){
        mvwprintw(alarmPanel, 1, 1, "Fork Failed");
    } else if (alarmPanelMGR == 0){
        alarm_shm->alarmLC = 1;
        while(runLoop == 1) {
            sleep(1);

            pthread_mutex_lock(&printLock);
            // Printing the time from the shared memory segment + Alarm Received
            mvwprintw(alarmPanel, alarm_shm->alarmLC, 1, "[%s] Alarm Received",alarm_shm->message);
            // Changing the colour of the alarm panel
            wbkgd(colourPanel, COLOR_PAIR(alarm_shm->colour));
            // Printing that the alarm has been handled
            mvwprintw(alarmPanel, (alarm_shm->alarmLC + 1), 1, "[%s] Alarm Handled",alarm_shm->message);
            wrefresh(colourPanel);
            wrefresh(alarmPanel);
            pthread_mutex_unlock(&printLock);
        }
    }


    // Time Panel Updater - Reads from Time Shared Memory Segment and outputs to Time Panel
    pid_t timePanelMGR = fork();
    if (timePanelMGR < 0){
        mvwprintw(timePanel, 1, 1, "Fork Failed");
    } else if (timePanelMGR == 0){
        while(runLoop == 1) {
            sleep(refreshTime);

            pthread_mutex_lock(&printLock);
            mvwprintw(timePanel, 1, 1, time_shm->USAtime);
            mvwprintw(timePanel, 2, 1, time_shm->MALTAtime);
            mvwprintw(timePanel, 3, 1, time_shm->TOKYOtime);
            wrefresh(timePanel);
            pthread_mutex_unlock(&printLock);
        }
    }


    // Char which stores the character inputted by the user
    char inputChar;
    // Array of characters which will store the command entered by the user
    char command[20];
    // Array of characters which will store the argument entered by the user
    char argument[200];

    // internal shell variables
    char prompt[32];
    char path[256];
    //unsigned int refreshTime;
    char buffer[16];
    int buffery;
    int bufferx;

    // default values of shell internal variables (set)
    strcpy(prompt,"OK");
    //refreshTime = 1;
    strcpy(buffer, "80x256");
    sscanf(buffer, "%dx%d",&buffery,&bufferx);  // buffery=80;bufferx=256;

    // accessing a file to store output in
    FILE * outputFP;
    outputFP = fopen("output", "w");

    FILE * systemFP;

    // counter which stores which line the program is on in the Prompt Panel(for cursor)
    int promptLC = 1;
    int outputLC = 1;
    int i, j;   // counters
    char var[10];   // will store which internal variable will be changed
    char temp[256];   // used as a temporary character array
    do{
        // Outputting the prompt (eg: OK>)
        pthread_mutex_lock(&printLock);
        // clear the line you will start writing to
        wmove(promptPanel, promptLC, 1); wclrtoeol(promptPanel); box(promptPanel, 0, 0);
        mvwprintw(promptPanel, promptLC, 1, "%s>",prompt);
        wrefresh(promptPanel);
        pthread_mutex_unlock(&printLock);
        //wnoutrefresh(promptPanel);
        //doupdate();

        // Getting user input
        pthread_mutex_lock(&printLock);
        //mvwscanw(promptPanel, promptLC, (int) strlen(prompt)+2, "%s %180[^\n]s",command,argument);
        //pthread_mutex_unlock(&printLock);
        i=0;
        // Getting input character by character
        do{
            // Get the user inputted character and store it
            inputChar = (char) getch();
            if (inputChar == 127 && i != 0) {   //KEY_BACKSPACE
                i--;
                temp[i] = '\0';
                wmove(promptPanel, promptLC, (int) (strlen(prompt)+2+i+1)); wclrtoeol(promptPanel); box(promptPanel, 0, 0);
                wrefresh(promptPanel);
            } else if (inputChar == 13){
                temp[i] = '\0';
                break;
            } else {
                temp[i] = inputChar;
                i++;
                // echo the user's input in the prompt panel
                mvwaddch(promptPanel, promptLC, (int) (strlen(prompt)+2+i), inputChar);
                wrefresh(promptPanel);
            }
        } while(inputChar != '\n'); // do this until the user presses 'Enter'
        temp[i] = '\0';
        sscanf(temp, "%s %180[^\n]s",command,argument);
        wrefresh(promptPanel);
        pthread_mutex_unlock(&printLock);



        // Handling the user's chosen command
        pthread_mutex_lock(&printLock);
        if (strcmp(command, "chdir") == 0) {
            strcpy(temp, getcwd(0,0));
            chdir(argument);
            mvwprintw(outputPanel, outputLC, 1, "Directory changed from: %s to: %s",temp,getcwd(0,0));
            fprintf(outputFP, "Directory changed from: %s to: %s\n",temp,getcwd(0,0));
        } else if (strcmp(command, "shdir") == 0){
            mvwprintw(outputPanel, outputLC, 1, "Current Directory: %s",getcwd(0,0));
            fprintf(outputFP, "Current Directory: %s\n",getcwd(0,0));
        } else if (strcmp(command, "print") == 0){
            //mvwaddstr(outputPanel, outputLC, 1, argument);
            mvwprintw(outputPanel, outputLC, 1, "%s",argument);
            fprintf(outputFP, "%s\n",argument);
        } else if (strcmp(command, "printvar") == 0){
            if (strcmp(argument, "prompt") == 0){
                mvwprintw(outputPanel, outputLC, 1, "prompt: %s",prompt);
                fprintf(outputFP, "prompt: %s\n",prompt);
            } else if (strcmp(argument, "path") == 0){
                mvwprintw(outputPanel, outputLC, 1, "path: %s",path);
                fprintf(outputFP, "path: %s\n",path);
            } else if (strcmp(argument, "refresh") == 0){
                mvwprintw(outputPanel, outputLC, 1, "refresh: %u",refreshTime);
                fprintf(outputFP, "refresh: %u\n",refreshTime);
            } else if (strcmp(argument, "buffer") == 0){
                mvwprintw(outputPanel, outputLC, 1, "buffer: %dx%d",buffery,bufferx);
                fprintf(outputFP, "buffer: %dx%d\n",buffery,bufferx);
            }
        } else if (strcmp(command, "set") == 0){
            // finding out which variable will be set and what value it will be set to
            i = 0, j = 0;
            while(argument[i] != '=') {
                var[j] = argument[i];
                i++;
                j++;
            }
            var[j] = '\0';
            i++;    // for = sign
            j = 0;
            while(argument[i] != '\0') {
                temp[j] = argument[i];
                i++;
                j++;
            }
            temp[j] = '\0';

            //sscanf(argument, "%16[^=]s=%180[^\n]s",var,temp);

            // var now holds the variable to be set
            // temp holds the value of the variable
            if (strcmp(var, "prompt") == 0){
                strcpy(prompt, temp);
                mvwprintw(outputPanel, outputLC, 1, "prompt was set to: %s",prompt);
                fprintf(outputFP, "prompt was set to: %s\n",prompt);
            } else if (strcmp(var, "path") == 0){
                strcpy(path, temp);
                mvwprintw(outputPanel, outputLC, 1, "path was set to: %s",path);
                fprintf(outputFP, "path was set to: %s\n",path);
            } else if (strcmp(var, "refresh") == 0){
                refreshTime = atoi(temp);
                mvwprintw(outputPanel, outputLC, 1, "refresh was set to: %u",refreshTime);
                fprintf(outputFP, "refresh was set to: %u\n",refreshTime);
            } else if (strcmp(var, "buffer") == 0){
                sscanf(temp, "%dx%d",&buffery,&bufferx);
                wresize(outputPanel, buffery, bufferx); wrefresh(outputPanel);
                mvwprintw(outputPanel, outputLC, 1, "buffer was set to: %dx%d",buffery,bufferx);
                fprintf(outputFP, "buffer was set to: %dx%d\n",buffery,bufferx);
            }
        } else if (strcmp(command, "move") == 0){
            mvwprintw(outputPanel, outputLC, 1, "Window was moved by %d",atoi(argument));
            fprintf(outputFP, "Window was moved by %d\n",atoi(argument));
        } else if (strcmp(command, "exit") == 0){
            mvwprintw(outputPanel, outputLC, 1, "Orange Wave will now exit");
            fprintf(outputFP, "Orange Wave will now exit");
        } else{     // external command
            strcpy(temp, command); strcat(temp, " "); strcat(temp, argument);
            mvwprintw(outputPanel, outputLC, 1, "%s was not found as a built-in function, trying to run as an external command",temp);
            fprintf(outputFP, "%s was not found as a built-in function, trying to run as an external command\n",temp);
            // redirecting the output of the system call to the tempOut File
            strcat(temp, " > tempOut");
            // emptying tempOut File
            systemFP = fopen("tempOut", "w");
            fclose(systemFP);
            // Making the system call with the user's command + output redirection
            system(temp);
            // reading from the text file and outputting the result
            systemFP = fopen("tempOut", "r");
            strcpy(temp, "");
            fgets(temp, 250, systemFP);
            fclose(systemFP);
            mvwprintw(outputPanel, ++outputLC, 1, "%s",temp);
            fprintf(outputFP, "%s\n",temp);
        }
        wrefresh(outputPanel);
        pthread_mutex_unlock(&printLock);

        // if the Line Counter for the Prompt Panel has reached the end, then start from the beginning/top again
        if(promptLC < (promptY-2)){
            promptLC++;
        } else{
            promptLC = 1;
        }

        if(outputLC < (outputY-2)){
            outputLC++;
        } else{
            outputLC = 1;
        }

    }while(strcmp(command, "exit") != 0);   // ==0 means they are equal, so != will loop until exit is entered

    // Clean up after ourselves
    delwin(promptPanel);
    delwin(outputPanel);
    delwin(alarmPanel);
    delwin(colourPanel);
    delwin(timePanel);
    delwin(mainwin);
    endwin();
    refresh();

    // Close the File
    fclose(outputFP);

    // Detach the Shared Memory segments
    shmdt(alarm_shm);
    shmdt(time_shm);

    // killing off the child processes
    kill(alarmPanelMGR, SIGKILL);
    kill(timePanelMGR, SIGKILL);

    return 0;
}

int task2(){

    // Shared memory segment

    // Alarm Panel Private Shared Memory Segment identifier
    key_t alarmKey = 0x0001;
    size_t alarmSize = sizeof(struct alarmInfo);   //alarmY*alarmX
    // Create the Private Shared Memory Segment
    int alarm_shmid = shmget(alarmKey, alarmSize, IPC_CREAT | 0666);
    if (alarm_shmid < 0) {
        perror("shmget");
        exit(1);
    }
    // Attach to the segment
    struct alarmInfo * alarm_shm = (struct alarmInfo *) shmat(alarm_shmid, NULL, 0);
    if (alarm_shm == (struct alarmInfo *) -1) {
        perror("shmat");
        exit(1);
    }

    // Set the current time for the presblock
    clock_gettime(CLOCK_MONOTONIC, &time2);

    // Does not allow the Shared Memory Segment to be destroyed until the program is ready to exit
    while(runLoop == 1){
        pause();
    }

    // Destroying the Shared Memory Segment when we are done
    if(shmctl(alarm_shmid, IPC_RMID ,NULL) == -1) {
        perror("shmctl");
        exit(1);
    }

    return 0;
}
// Signal Handling method for the presblock daemon
void signal_handler(int sig){

    // Attaching to the Alarm Shared Memory Segment
    key_t alarmKey = 0x0001;
    size_t alarmSize = sizeof(struct alarmInfo);   // alarmY*alarmX (+colourY?)
    int alarm_shmid = shmget(alarmKey, alarmSize, 0666);
    if (alarm_shmid < 0) {
        perror("shmget");
        exit(1);
    }
    // Attaching to the Private Shared Memory Segment which is storing the Alarm Panel
    struct alarmInfo * alarm_shm = (struct alarmInfo *) shmat(alarm_shmid, NULL, 0);
    if (alarm_shm == (struct alarmInfo *) -1) {
        perror("shmat");
        exit(1);
    }


    if (sig == SIGALRM){
        time1 = time2;
        clock_gettime(CLOCK_MONOTONIC, &time2);

        int timeDiff = time2.tv_sec - time1.tv_sec;

        // Decide which colour pair to display based on the interarrival time and store it in the Shared Memory Segment
        if (timeDiff < 5){
            // white
            alarm_shm->colour = 1;
        } else if (timeDiff>=5 && timeDiff<10){
            // red
            alarm_shm->colour = 2;
        } else if (timeDiff>=10 && timeDiff<15){
            // orange
            alarm_shm->colour = 3;
        } else if (timeDiff>=15 && timeDiff<=20){
            // green
            alarm_shm->colour = 4;
        } else {
            // blue
            alarm_shm->colour = 5;
        }

        // Store the time at which the alarm was received in the Shared Memory Segment
        strftime(alarm_shm->message, 31, "%H:%M:%S", gmtime(&time2.tv_sec));

        // Change the y-coordinate at which the alarm prompts will be printed inside tha alarm panel
        if(alarm_shm->alarmLC < (alarmY-4)){
            alarm_shm->alarmLC += 2;
        } else{
            alarm_shm->alarmLC = 1;
        }

    } else {
        perror("Unexpected Signal Received");
    }

    // Detach from the shared memory segment
    shmdt(alarm_shm);
}

int task3(){
    // Shared memory segment

    // Time Panel Private Shared Memory Segment identifier
    key_t timeKey = 0x0002;
    size_t timeSize = sizeof(struct timeZones);   //timeY*timeX
    // Create the Private Shared Memory Segment
    int time_shmid = shmget(timeKey, timeSize, IPC_CREAT | 0666);
    if (time_shmid < 0) {
        perror("shmget");
        exit(1);
    }
    // Attach to the segment
    struct timeZones * time_shm = (struct timeZones *) shmat(time_shmid, NULL, 0);
    if (time_shm == (struct timeZones *) -1) {
        perror("shmat");
        exit(1);
    }

    struct timeval USAtime;
    struct timeval MALTAtime;
    struct timeval TOKYOtime;

    // while global is not 1
    while (runLoop == 1) {
        sleep(refreshTime);

        // Getting the current epoch time storing it in USAtime
        gettimeofday(&USAtime, NULL);
        // Reducing 6 Hours worth of seconds from the epoch time to adjust for the Time Zone
        USAtime.tv_sec -= 6*(60*60);
        // Storing the formatted time in the Shared Memory Segment
        sprintf(time_shm->USAtime, "WHITE HOUSE [USA]: %s",ctime((const time_t *) &USAtime.tv_sec));

        gettimeofday(&MALTAtime, NULL);
        MALTAtime.tv_sec += 1*(60*60);
        sprintf(time_shm->MALTAtime, "MALTA [MSIDA]: %s",ctime((const time_t *) &MALTAtime.tv_sec));

        gettimeofday(&TOKYOtime, NULL);
        TOKYOtime.tv_sec += 9*(60*60);
        sprintf(time_shm->TOKYOtime, "JAPAN [TOKYO]: %s",ctime((const time_t *) &TOKYOtime.tv_sec));
    }

    // Destroying the Shared Memory Segment when we are done
    if(shmctl(time_shmid, IPC_RMID ,NULL) == -1) {
        perror("shmctl");
        exit(1);
    }

    return 0;
}