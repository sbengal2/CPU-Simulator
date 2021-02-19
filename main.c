
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"

int
main(int argc, char const* argv[])
{
  if (argc != 4) {
    fprintf(stderr, "APEX_Help : Usage %s <input_file> Display/Simulate <clock-cycles> \n", argv[0]);
    exit(1);
  }
    
    if(strcmp(argv[2], "Display") == 0 || strcmp(argv[2], "display") == 0)
    {
        APEX_CPU* cpu = APEX_cpu_init(argv[1]);
         if (!cpu) {
           fprintf(stderr, "APEX_Error : Unable to initialize CPU\n");
           exit(1);
         }
        cpu->clock_cycles=atoi(argv[3]);
        Display(cpu);
        
    }
    else if(strcmp(argv[2], "Simulate") == 0 || strcmp(argv[2], "simulate") == 0)
    {
        APEX_CPU* cpu = APEX_cpu_init(argv[1]);
         if (!cpu) {
           fprintf(stderr, "APEX_Error : Unable to initialize CPU\n");
           exit(1);
         }
        cpu->clock_cycles=atoi(argv[3]);
        Simulate(cpu);
        
    }
    else{
      fprintf(stderr, "APEX_Help : Usage %s <input_file> Display/Simulate <clock-cycles> \n", argv[0]);
        exit(1);
        
    }
      return 0;

}

 
void Display(APEX_CPU* cpu)
    {
        cpu->display=1;
        APEX_cpu_run(cpu);
        APEX_cpu_stop(cpu);
    }


void Simulate(APEX_CPU* cpu)
  {
      APEX_cpu_run(cpu);
      APEX_cpu_stop(cpu);
      
  }
