#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>
#include <string.h>


int main(int argc, char **  argv)
{
    
    const char * log_ident = "batBeep";
    /* This part of the code creates the daemon */
    /* Here we fork once so that the child is not the group leader */
    pid_t pID = fork();
    if(pID == -1)
    {
        fprintf(stderr, "Failed to fork in the first place");
        return 1;
    }
    else if(pID != 0)
    {
        exit(0);
    }

    // Then we make the child a new session leader with no controlling terminal.
    if(setsid() == -1)
    {
        fprintf(stderr, "Failed to start a new session");
        return 1;
    }

    /* Here we fork again so that we guarentee we can not reaquire a controlling 
     * terminal */
    switch (fork()) {
    case -1: 
            fprintf(stderr, "Failed to fork in the second place");
            return -1;
    case 0:
            break;
    default:
            exit(0);
    }
    openlog(log_ident, LOG_CONS, LOG_DAEMON);

    /* Clear the file mode creation mask */
    umask(0);

    /* Change the working directory to somewhere that 
     * probably won't be unmounted while this program runs */
    chdir("/sys/class/power_supply/BAT1");

    /* Close the STD File Descriptors */ 
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    /* Open back up the 0, 1, 2 just in case somebody accidently 
     * uses them */

    int fd = open("/dev/null", O_RDWR);
    if(fd != 0)
    {
        syslog(LOG_ERR, "Error FD did not match");
        return 1;
    }
    if (dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO)
        return 1;
    if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO)
        return 1;


    FILE * current_power, * full_power, * power_status;
    char * temp_cmd;
    temp_cmd = "aplay -q ~/Sounds/beep-06.wav";
    

    while (1)
    {
        if(!(current_power = fopen("/sys/class/power_supply/BAT1/charge_now", "r")))
        {
            syslog(LOG_ERR, "Error in opening current_power file");
            system(temp_cmd);
            return 1;
        }

        if(!(full_power = fopen("/sys/class/power_supply/BAT1/charge_full", "r")))
        {
            syslog(LOG_ERR, "Error in opening full_power file");
            system(temp_cmd);
            return 1;
        }

        if(!(power_status = fopen("/sys/class/power_supply/BAT1/status", "r")))
        {
            syslog(LOG_ERR, "Error in opening status file");
            system(temp_cmd);
            return 1;
        }

        unsigned int current, full;
        char status_string[15];
        int beeped = 0;
        

        current = full = 1;

        if(!fscanf(current_power, "%u", &current))
        {
            syslog(LOG_ERR, "Error in reading current_power file");
        }
        
        if(!fscanf(full_power, "%u", &full))
        {
            syslog(LOG_ERR, "Error in reading full_power file");
        }

        if(!fscanf(power_status, "%s", status_string))
        {
            syslog(LOG_ERR, "Error in reading status file");
        }

        if(current > full)
        {
            syslog(LOG_ERR, "Runtime error: Current charge greater than Max Capacity");
            return 1;
        } 

#ifdef DEBUG
        printf("Current: %u\n", current);
        printf("Full: %u\n", full);
#endif
        float percentage = (float) current / (float) full;
#ifdef DEBUG
        printf("\n%f\n", percentage);
#endif

        if((percentage > (float) 0.8005 && !strcmp(status_string, "Charging")) || 
                                        (percentage < (float) 0.3995 && !strcmp(status_string, "Discharging")))
        {
            system(temp_cmd);
            syslog(LOG_DEBUG, "The percentage is: %f", percentage);
        }

        sleep(5);
        fclose(full_power);
        fclose(current_power);
        fclose(power_status);

    }

        

    return 0;

}
