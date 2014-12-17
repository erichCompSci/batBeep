#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **  argv)
{
    FILE * current_power, * full_power;
    char * temp_cmd;
    int i = 0;
    temp_cmd = "aplay -q ~/Sounds/beep-06.wav";
    

    while (i < 30)
    {
        if(!(current_power = fopen("/sys/class/power_supply/BAT1/charge_now", "r")))
        {
            fprintf(stderr, "Error in opening current_power file");
            return 1;
        }

        if(!(full_power = fopen("/sys/class/power_supply/BAT1/charge_full", "r")))
        {
            fprintf(stderr, "Error in opening full_power file");
            return 1;
        }

        unsigned int current, full;
        if(!fscanf(current_power, "%u", &current))
        {
            fprintf(stderr, "Error in reading current_power file");
            return 1;
        }
        
        if(!fscanf(full_power, "%u", &full))
        {
            fprintf(stderr, "Error in reading full_power file");
            return 1;
        }

        if(current > full)
        {
            fprintf(stderr, "Runtime error: Current charge greater than Max Capacity");
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

        if(percentage > (float) 0.8005 || percentage < (float) 0.3995)
        {
            system(temp_cmd);
        }

        sleep(3);
        fclose(full_power);
        fclose(current_power);

        
        ++i;
    }

        



}
