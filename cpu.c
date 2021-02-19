#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"

/* Set this flag to 1 to enable debug messages */
#define ENABLE_DEBUG_MESSAGES 1

/*
 * Function creates and initializes APEX cpu.
 *
 */
APEX_CPU*
APEX_cpu_init(const char* filename)
{
  if (!filename) {
    return NULL;
  }

  APEX_CPU* cpu = malloc(sizeof(*cpu));
  if (!cpu) {
    return NULL;
  }

  /* Initialize PC, Registers and all pipeline stages */
  cpu->pc = 4000;
  memset(cpu->regs, 0, sizeof(int) * 32);
  memset(cpu->regs_valid, 1, sizeof(int) * 32);
  memset(cpu->stage, 0, sizeof(CPU_Stage) * NUM_STAGES);
  memset(cpu->data_memory, 0, sizeof(int) * 4000);

  /* Parse input file and create code memory */
  cpu->code_memory = create_code_memory(filename, &cpu->code_memory_size);

  if (!cpu->code_memory) {
    free(cpu);
    return NULL;
  }

  if (ENABLE_DEBUG_MESSAGES) {
       printf("\n");
    printf("-----------------------------------------------------\n");
      fprintf(stderr,
            "APEX_CPU : Initialized APEX CPU, loaded %d instructions\n",
            cpu->code_memory_size);
      printf("-----------------------------------------------------\n");
      printf("\n");
       printf("\n");
      printf("-----------------------------------------------------\n");
      printf("-----------------------------------------------------\n");
    fprintf(stderr, "APEX_CPU : Printing Code Memory...............\n");
      printf("-----------------------------------------------------\n");
      printf("-----------------------------------------------------\n");
    printf("%-9s %-9s %-9s %-9s %-9s %-9s\n", "opcode", "rd", "rs1", "rs2","rs3" , "imm");

    for (int i = 0; i < cpu->code_memory_size; ++i) {
      printf("%-9s %-9d %-9d %-9d %-9d %-9d\n",
             cpu->code_memory[i].opcode,
             cpu->code_memory[i].rd,
             cpu->code_memory[i].rs1,
             cpu->code_memory[i].rs2,
             cpu->code_memory[i].rs3,
             cpu->code_memory[i].imm);
    }
       printf("-----------------------------------------------------\n");
      printf("\n");
      printf("\n");
  }

  /* Make all stages busy except Fetch stage, initally to start the pipeline */
  for (int i = 1; i < NUM_STAGES; ++i) {
    cpu->stage[i].busy = 1;
  }
    for(int v=0;v<32;v++)
    {
        cpu->regs_valid[v]=1;
    }

  return cpu;
}

/*
 * This function de-allocates APEX cpu.
 *
 */
void
APEX_cpu_stop(APEX_CPU* cpu)
{
  free(cpu->code_memory);
  free(cpu);
}

/* Converts the PC(4000 series) into
 * array index for code memory
 *
 */
int
get_code_index(int pc)
{
  return (pc - 4000) / 4;
}

static void
print_instruction(CPU_Stage* stage)
{
  if (strcmp(stage->opcode, "STORE") == 0) {
    printf(
      "%s,R%d,R%d,#%d ", stage->opcode, stage->rs1, stage->rs2, stage->imm);
  }
    if (strcmp(stage->opcode, "STR") == 0) {
      printf(
        "%s,R%d,R%d,R%d ", stage->opcode, stage->rs1, stage->rs2, stage->rs3);
    }
    if (strcmp(stage->opcode, "LDR") == 0) {
      printf(
        "%s,R%d,R%d,R%d ", stage->opcode, stage->rd, stage->rs1, stage->rs2);
    }
    if (strcmp(stage->opcode, "LOAD") == 0) {
      printf(
        "%s,R%d,R%d,#%d ", stage->opcode, stage->rd, stage->rs1, stage->imm);
    }
    if (strcmp(stage->opcode, "ADD") == 0) {
      printf(
        "%s,R%d,R%d,R%d ", stage->opcode, stage->rd, stage->rs1, stage->rs2);
    }
    if (strcmp(stage->opcode, "ADDL") == 0) {
      printf(
        "%s,R%d,R%d,#%d ", stage->opcode, stage->rd, stage->rs1, stage->imm);
    }
    if (strcmp(stage->opcode, "SUB") == 0) {
      printf(
        "%s,R%d,R%d,R%d ", stage->opcode, stage->rd, stage->rs1, stage->rs2);
    }
    if (strcmp(stage->opcode, "SUBL") == 0) {
      printf(
        "%s,R%d,R%d,#%d ", stage->opcode, stage->rd, stage->rs1, stage->imm);
    }
    if (strcmp(stage->opcode, "MUL") == 0) {
      printf(
        "%s,R%d,R%d,R%d ", stage->opcode, stage->rd, stage->rs1, stage->rs2);
    }
    if (strcmp(stage->opcode, "AND") == 0) {
      printf(
        "%s,R%d,R%d,R%d ", stage->opcode, stage->rd, stage->rs1, stage->rs2);
    }
    if (strcmp(stage->opcode, "OR") == 0) {
      printf(
        "%s,R%d,R%d,R%d ", stage->opcode, stage->rd, stage->rs1, stage->rs2);
    }
    if (strcmp(stage->opcode, "XOR") == 0) {
      printf(
        "%s,R%d,R%d,R%d ", stage->opcode, stage->rd, stage->rs1, stage->rs2);
    }

  if (strcmp(stage->opcode, "MOVC") == 0) {
    printf("%s,R%d,#%d ", stage->opcode, stage->rd, stage->imm);
  }
    if (strcmp(stage->opcode, "BZ") == 0) {
       printf(
         "%s,#%d ", stage->opcode,stage->imm);
     }
    if (strcmp(stage->opcode, "BNZ") == 0) {
      printf(
        "%s,#%d ", stage->opcode,stage->imm);
    }
    if (strcmp(stage->opcode, "JUMP") == 0) {
      printf(
        "%s,R%d,#%d ", stage->opcode,stage->rs1,stage->imm);
    }
    if (strcmp(stage->opcode, "HALT") == 0) {
      printf(
        "%s", stage->opcode);
    }
    if (strcmp(stage->opcode, "") == 0) {
         printf("   ");
       }
       if (strcmp(stage->opcode, "") == 0) {
         printf("EMPTY");
       }
}

/* Debug function which dumps the cpu stage
 * content
 */
static void
print_stage_content(char* name, CPU_Stage* stage)
{
  printf("%-15s: pc(%d) ", name, stage->pc);
  print_instruction(stage);
  printf("\n");
}

/*
 *  Fetch Stage of APEX Pipeline
 *
 */
int
fetch(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[F];
  if (!stage->busy && !stage->stalled) {
    /* Store current PC in fetch latch */
    stage->pc = cpu->pc;

    /* Index into code memory using this pc and copy all instruction fields into
     * fetch latch
     */
    APEX_Instruction* current_ins = &cpu->code_memory[get_code_index(cpu->pc)];
    strcpy(stage->opcode, current_ins->opcode);
    stage->rd = current_ins->rd;
    stage->rs1 = current_ins->rs1;
    stage->rs2 = current_ins->rs2;
    stage->rs3 = current_ins->rs3;
    stage->imm = current_ins->imm;
    stage->rd = current_ins->rd;

    /* Update PC for next instruction */
    if(!cpu->stage[DRF].stalled)
    {
        cpu->pc += 4;
     
   
    /* Copy data from fetch latch to decode latch*/
        cpu->stage[DRF] = cpu->stage[F];
    }
      if(cpu->display)
      {
    if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Fetch", stage);
    }
      }
  }
    else if(stage->stalled)
    {
        stage->pc = cpu->pc;

        /* Index into code memory using this pc and copy all instruction fields into
         * fetch latch
         */
        APEX_Instruction* current_ins = &cpu->code_memory[get_code_index(cpu->pc)];
        strcpy(stage->opcode, current_ins->opcode);
        stage->rd = current_ins->rd;
        stage->rs1 = current_ins->rs1;
        stage->rs2 = current_ins->rs2;
        stage->rs3 = current_ins->rs3;
        stage->imm = current_ins->imm;
        stage->rd = current_ins->rd;
        
        
       if(cpu->display)
          {
             stage->pc=0;
            strcpy(stage->opcode,"");
        if (ENABLE_DEBUG_MESSAGES) {
            print_stage_content("Fetch", stage);
        }
          }
        
    }
  return 0;
}                   

/*
 *  Decode Stage of APEX Pipeline
 */
int
decode(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[DRF];
   if(cpu->stage[DRF].stalled)
    {
        cpu->stage[DRF].stalled=0;
    }
  if (!stage->busy && !stage->stalled) {
      
/* Read data from register file for store */
  if (strcmp(stage->opcode, "STORE") == 0) {
      if(cpu->regs_valid[cpu->stage[DRF].rs1]&&cpu->regs_valid[cpu->stage[DRF].rs2])
      {
          stage->ari=0;
          cpu->stage[DRF].stalled=0;
          cpu->stage[F].stalled=0;
        stage->rs1_value=cpu->regs[stage->rs1];
      //cpu->regs_valid[stage->rs1]=0;
        stage->rs2_value=cpu->regs[stage->rs2];
        //cpu->regs_valid[stage->rs2]=0;
      }
      else
      {
          cpu->stage[DRF].stalled=1;
          cpu->stage[F].stalled=1;
          
          
      }
  }
      if (strcmp(stage->opcode, "STR") == 0) {
          if(cpu->regs_valid[cpu->stage[DRF].rs1]&&cpu->regs_valid[cpu->stage[DRF].rs2]&&cpu->regs_valid[cpu->stage[DRF].rs3])
          {
              stage->ari=0;
          stage->rs1_value=cpu->regs[stage->rs1];
            //cpu->regs_valid[stage->rs1]=0;
          stage->rs2_value=cpu->regs[stage->rs2];
           // cpu->regs_valid[stage->rs2]=0;
          stage->rs3_value=cpu->regs[stage->rs3];
           // cpu->regs_valid[stage->rs3]=0;
           cpu->stage[DRF].stalled=0;
          cpu->stage[F].stalled=0;
          }
          else
          {
              cpu->stage[DRF].stalled=1;
              cpu->stage[F].stalled=1;
              
              
          }
          
          
      }
      
      if (strcmp(stage->opcode, "LDR") == 0) {
          if(cpu->regs_valid[cpu->stage[DRF].rs1]&&cpu->regs_valid[cpu->stage[DRF].rs2])
          {
              stage->ari=0;
          stage->rs1_value=cpu->regs[stage->rs1];
           // cpu->regs_valid[stage->rs1]=0;
          stage->rs2_value=cpu->regs[stage->rs2];
            //cpu->regs_valid[stage->rs2]=0;
            //cpu->regs_valid[stage->rd]=0;
          }
          else
          {
              cpu->stage[DRF].stalled=1;
             cpu->stage[F].stalled=1;
             
              
          }
          
          
      }
      
      if (strcmp(stage->opcode, "LOAD") == 0) {
          if(cpu->regs_valid[cpu->stage[DRF].rs1])
          {
              stage->ari=0;
          stage->rs1_value=cpu->regs[stage->rs1];
            //cpu->regs_valid[stage->rs1]=0;
            //cpu->regs_valid[stage->rd]=0;
          }
          else
          {
              cpu->stage[DRF].stalled=1;
              cpu->stage[F].stalled=1;
             
          }
          
      }
      
      if (strcmp(stage->opcode, "ADD") == 0) {
          if(cpu->regs_valid[cpu->stage[DRF].rs1]&&cpu->regs_valid[cpu->stage[DRF].rs2])
          {
              stage->ari=1;
              cpu->stage[DRF].stalled=0;
              cpu->stage[F].stalled=0;
             stage->rs1_value=cpu->regs[stage->rs1];
            //cpu->regs_valid[stage->rs1]=0;
             stage->rs2_value=cpu->regs[stage->rs2];
            //cpu->regs_valid[stage->rs2]=0;
            //cpu->regs_valid[stage->rd]=0;
         }
          else
          {
              cpu->stage[DRF].stalled=1;
              cpu->stage[F].stalled=1;
             
            
              
          }
          }
      
      if (strcmp(stage->opcode, "ADDL") == 0) {
          if(cpu->regs_valid[cpu->stage[DRF].rs1])
          {
          stage->ari=1;
          cpu->stage[DRF].stalled=0;
          cpu->stage[F].stalled=0;
          stage->rs1_value=cpu->regs[stage->rs1];
            //cpu->regs_valid[stage->rs1]=0;
            //cpu->regs_valid[stage->rd]=0;
          }
          else
          {
              cpu->stage[DRF].stalled=1;
              cpu->stage[F].stalled=1;
              
              
          }
          
          
      }
      
      if (strcmp(stage->opcode, "SUB") == 0) {
       if(cpu->regs_valid[cpu->stage[DRF].rs1]&&cpu->regs_valid[cpu->stage[DRF].rs2])
       {
           stage->ari=1;
           cpu->stage[DRF].stalled=0;
           cpu->stage[F].stalled=0;
          stage->rs1_value=cpu->regs[stage->rs1];
         //cpu->regs_valid[stage->rs1]=0;
          stage->rs2_value=cpu->regs[stage->rs2];
        // cpu->regs_valid[stage->rs2]=0;
        // cpu->regs_valid[stage->rd]=0;
      }
       else
       {
           cpu->stage[DRF].stalled=1;
           cpu->stage[F].stalled=1;
         
         
           
       }
       }
      
        if (strcmp(stage->opcode, "SUBL") == 0) {
              if(cpu->regs_valid[cpu->stage[DRF].rs1])
              {
                  stage->ari=1;
              cpu->stage[DRF].stalled=0;
              cpu->stage[F].stalled=0;
              stage->rs1_value=cpu->regs[stage->rs1];
               // cpu->regs_valid[stage->rs1]=0;
                //cpu->regs_valid[stage->rd]=0;
              }
              else
              {
                  cpu->stage[DRF].stalled=1;
                  cpu->stage[F].stalled=1;
                 
                  
              }
              
              
          }
      
      if (strcmp(stage->opcode, "MUL") == 0) {
       if(cpu->regs_valid[cpu->stage[DRF].rs1]&&cpu->regs_valid[cpu->stage[DRF].rs2])
       {
           stage->ari=1;
           cpu->stage[DRF].stalled=0;
           cpu->stage[F].stalled=0;
          stage->rs1_value=cpu->regs[stage->rs1];
         //cpu->regs_valid[stage->rs1]=0;
          stage->rs2_value=cpu->regs[stage->rs2];
        // cpu->regs_valid[stage->rs2]=0;
         //cpu->regs_valid[stage->rd]=0;
      }
       else
       {
           cpu->stage[DRF].stalled=1;
           cpu->stage[F].stalled=1;
           
           
           
       }
       }

      
      if (strcmp(stage->opcode, "AND") == 0) {
       if(cpu->regs_valid[cpu->stage[DRF].rs1]&&cpu->regs_valid[cpu->stage[DRF].rs2])
       {
            stage->ari=0;
           cpu->stage[DRF].stalled=0;
           cpu->stage[F].stalled=0;
          stage->rs1_value=cpu->regs[stage->rs1];
       //  cpu->regs_valid[stage->rs1]=0;
          stage->rs2_value=cpu->regs[stage->rs2];
        // cpu->regs_valid[stage->rs2]=0;
         //cpu->regs_valid[stage->rd]=0;
      }
       else
       {
           cpu->stage[DRF].stalled=1;
           cpu->stage[F].stalled=1;
       
           
       }
       }

      
      if (strcmp(stage->opcode, "OR") == 0) {
       if(cpu->regs_valid[cpu->stage[DRF].rs1]&&cpu->regs_valid[cpu->stage[DRF].rs2])
       {
            stage->ari=0;
           cpu->stage[DRF].stalled=0;
           cpu->stage[F].stalled=0;
          stage->rs1_value=cpu->regs[stage->rs1];
        // cpu->regs_valid[stage->rs1]=0;
          stage->rs2_value=cpu->regs[stage->rs2];
       //  cpu->regs_valid[stage->rs2]=0;
         //cpu->regs_valid[stage->rd]=0;
      }
       else
       {
           cpu->stage[DRF].stalled=1;
           cpu->stage[F].stalled=1;
          
           
       }
       }

      
     if (strcmp(stage->opcode, "XOR") == 0) {
       if(cpu->regs_valid[cpu->stage[DRF].rs1]&&cpu->regs_valid[cpu->stage[DRF].rs2])
       {
            stage->ari=0;
           cpu->stage[DRF].stalled=0;
           cpu->stage[F].stalled=0;
          stage->rs1_value=cpu->regs[stage->rs1];
         //cpu->regs_valid[stage->rs1]=0;
          stage->rs2_value=cpu->regs[stage->rs2];
         //cpu->regs_valid[stage->rs2]=0;
         //cpu->regs_valid[stage->rd]=0;
      }
       else
       {
           cpu->stage[DRF].stalled=1;
           cpu->stage[F].stalled=1;
           
       }
       }
      
    /* No Register file read needed for MOVC */
    if (strcmp(stage->opcode, "MOVC") == 0) {
        if(cpu->regs_valid[cpu->stage[DRF].rd])
        {
             stage->ari=0;
            cpu->stage[DRF].stalled=0;
            cpu->stage[F].stalled=0;
            cpu->stage[DRF].stalled=0;
            cpu->stage[F].stalled=0;
            //cpu->regs_valid[stage->rd]=0;
        }
        else
        {
            cpu->stage[DRF].stalled=1;
            cpu->stage[F].stalled=1;
            
            
        }
        
    }
      
     if (strcmp(stage->opcode, "BZ") == 0 || (strcmp(stage->opcode, "BNZ") == 0))
     {
       
       cpu->loop =(stage->imm/4)-1;
      

       cpu->rem=stage->pc;
         if(cpu->stage[EX1].ari)
         { cpu->stall=1;
           cpu->bz_pc=cpu->stage[EX1].pc;
         }
           if(cpu->stall)
           {
             cpu->stage[DRF].stalled=1;
             cpu->stage[F].stalled=1;
         }
         else
         {
             cpu->stage[DRF].stalled=0;
             cpu->stage[F].stalled=0;
         }
     }
     
      if (strcmp(stage->opcode, "JUMP") == 0) {
            if(cpu->regs_valid[cpu->stage[DRF].rs1])
            {
                 stage->ari=0;
                cpu->stage[DRF].stalled=0;
                cpu->stage[F].stalled=0;
          stage->rs1_value=cpu->regs[stage->rs1];
            //cpu->regs_valid[stage->rs1]=0;
            }
          else
          {
              cpu->stage[DRF].stalled=1;
              cpu->stage[F].stalled=1;
              
         }
      }
      
      if (strcmp(stage->opcode, "HALT") == 0) {
           stage->ari=0;
          strcpy(cpu->stage[F].opcode,"");
          cpu->stage[F].stalled=1;
          cpu->stage[F].pc=0;
         

      }

    /*Copy data from decode latch to execute latch*/
    cpu->stage[EX1] = cpu->stage[DRF];

    if(cpu->display)
         {
             if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Decode/RF", stage);
    }
         }
  }
  else if(stage->stalled)
  {
      if(cpu->display)
           {
               if (ENABLE_DEBUG_MESSAGES) {
                   stage->pc=0;
                 strcpy(stage->opcode,"");
                cpu->stage[EX1] = cpu->stage[DRF];
         print_stage_content("Decode/RF", stage);
      }
           }
  
  }
      
  
  return 0;
}

/*
 *  Execute Stage of APEX Pipeline
 */
int
execute1(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[EX1];
  if (!stage->busy && !stage->stalled)
  {
      if (strcmp(stage->opcode, "HALT") == 0) {
          strcpy(cpu->stage[F].opcode,"");
          cpu->stage[F].stalled=1;
          cpu->stage[F].pc=0;
          strcpy(cpu->stage[DRF].opcode,"");
          cpu->stage[DRF].stalled=1;
          cpu->stage[DRF].pc=0;
         

      }

      if (strcmp(stage->opcode, "BZ") == 0 || strcmp(stage->opcode, "BNZ") == 0 ) {
          stage->buffer=(stage->imm+cpu->rem);
          
      }




if (strcmp(stage->opcode, "MOVC") == 0) {
                cpu->regs_valid[stage->rd]=0;
             
         }


if (strcmp(stage->opcode, "ADD") == 0) {
                cpu->regs_valid[stage->rd]=0;
             
         }
 


if (strcmp(stage->opcode, "ADDL") == 0) {
                cpu->regs_valid[stage->rd]=0;
             
         }
 


if (strcmp(stage->opcode, "SUB") == 0) {
                cpu->regs_valid[stage->rd]=0;
             
         }
 


if (strcmp(stage->opcode, "SUBL") == 0) {
                cpu->regs_valid[stage->rd]=0;
             
         }
 


if (strcmp(stage->opcode, "MUL") == 0) {
                cpu->regs_valid[stage->rd]=0;
             
         }
 


if (strcmp(stage->opcode, "AND") == 0) {
                cpu->regs_valid[stage->rd]=0;
             
         }
 
if (strcmp(stage->opcode, "OR") == 0) {
                cpu->regs_valid[stage->rd]=0;
             
         }
 


if (strcmp(stage->opcode, "XOR") == 0) {
                cpu->regs_valid[stage->rd]=0;
             
         }

if (strcmp(stage->opcode, "LDR") == 0) {
                cpu->regs_valid[stage->rd]=0;
             
         }
 

if (strcmp(stage->opcode, "LOAD") == 0) {
                cpu->regs_valid[stage->rd]=0;
             
         }
 
 


 
     
    /* Copy data from Execute1 latch to Execute 2 latch*/
    cpu->stage[EX2] = cpu->stage[EX1];

    if(cpu->display)
         {
             if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Execute1", stage);
    }
         }
  }
    else if(stage->stalled)
    {

        if(cpu->display)
             {
                 if (ENABLE_DEBUG_MESSAGES) {
                     stage->pc=0;
                     strcpy(stage->opcode,"");
                     cpu->stage[EX2] = cpu->stage[EX1];
                   print_stage_content("Execute1", stage);
        }
             }
        
    }
  return 0;
}

int
execute2(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[EX2];
  if (!stage->busy && !stage->stalled) {
     if (strcmp(stage->opcode, "HALT") == 0) {
          strcpy(cpu->stage[F].opcode,"");
          cpu->stage[F].stalled=1;
          cpu->stage[F].pc=0;
          strcpy(cpu->stage[DRF].opcode,"");
          cpu->stage[DRF].stalled=1;
          cpu->stage[DRF].pc=0;
         strcpy(cpu->stage[EX1].opcode,"");
         cpu->stage[EX1].stalled=1;
         cpu->stage[EX1].pc=0;
         

      }
      

      if (strcmp(stage->opcode, "JUMP") == 0) {

          stage->buffer=(stage->rs1_value+stage->imm);
          if(stage->buffer > 4000&& stage->buffer<(4000+(cpu->code_memory_size*4)))
          {
          cpu->pc=stage->buffer;
          flush(cpu);
          }
          
      }
      
     


     if (strcmp(stage->opcode, "MOVC") == 0) {
             stage->buffer=stage->imm+0;
             
         }

         /* Store */
         if (strcmp(stage->opcode, "STORE") == 0) {
           stage->mem_address=(stage->rs2_value+stage->imm);
       }
         if (strcmp(stage->opcode, "STR") == 0) {
              stage->mem_address=(stage->rs2_value+stage->rs3_value);
          }
         if (strcmp(stage->opcode, "LDR") == 0) {
              stage->mem_address=(stage->rs1_value+stage->rs2_value);
          }
         if (strcmp(stage->opcode, "LOAD") == 0) {
              stage->mem_address=(stage->rs1_value+stage->imm);
          }
         if (strcmp(stage->opcode, "ADD") == 0) {
                stage->buffer=(stage->rs1_value+stage->rs2_value);
            }
         if (strcmp(stage->opcode, "ADDL") == 0) {
             stage->buffer=(stage->rs1_value+stage->imm);
         }
         if (strcmp(stage->opcode, "SUB") == 0) {
             stage->buffer=(stage->rs1_value-stage->rs2_value);
         }
         if (strcmp(stage->opcode, "SUBL") == 0) {
             stage->buffer=(stage->rs1_value-stage->imm);
         }
         if (strcmp(stage->opcode, "MUL") == 0) {
             stage->buffer=(stage->rs1_value*stage->rs2_value);
         }
      
      if (strcmp(stage->opcode, "AND") == 0) {
          stage->buffer=(stage->rs1_value&stage->rs2_value);
      }
      
      if (strcmp(stage->opcode, "OR") == 0) {
          stage->buffer=(stage->rs1_value|stage->rs2_value);
      }
      
      if (strcmp(stage->opcode, "XOR") == 0) {
          stage->buffer=(stage->rs1_value^stage->rs2_value);
      }
      if (strcmp(stage->opcode, "BZ") == 0) {
           if(cpu->ZF)
            {
                 flush(cpu);
              cpu->Toflush=1;
              cpu->bufRem=stage->buffer;
           
              if(stage->imm < 0) {
          cpu->ins_completed = cpu->ins_completed + cpu->loop;
             }

             else{
              cpu->ins_completed = cpu->ins_completed-cpu->loop;
            }

                  
            }
         }

         if (strcmp(stage->opcode, "BNZ") == 0) {
           if(!cpu->ZF)
            {
              flush(cpu);
               cpu->Toflush=1;
              cpu->bufRem=stage->buffer;
           
              if(stage->imm < 0) {
                cpu->ins_completed = cpu->ins_completed + cpu->loop;
              }
               else
               {
              cpu->ins_completed = cpu->ins_completed-cpu->loop;
            }

                  
            }
         }

      

    
         
    /* Copy data from Execute latch to Memory latch*/
    cpu->stage[MEM1] = cpu->stage[EX2];

    if(cpu->display)
         {
             if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Execute2", stage);
    }
         }
  }
    else if(stage->stalled)
    {

        if(cpu->display)
             {
                 if (ENABLE_DEBUG_MESSAGES) {
                     stage->pc=0;
                     strcpy(stage->opcode,"");
                     cpu->stage[MEM1] = cpu->stage[EX2];
            print_stage_content("Execute2", stage);
        }
             }
        
    }
  return 0;
}

/*
 *  Memory Stage of APEX Pipeline
 */
int
memory1(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[MEM1];
  if (!stage->busy && !stage->stalled) {
      
      

      if (strcmp(stage->opcode, "HALT") == 0) {
             strcpy(cpu->stage[F].opcode,"");
             cpu->stage[F].stalled=1;
             cpu->stage[F].pc=0;
             strcpy(cpu->stage[DRF].opcode,"");
             cpu->stage[DRF].stalled=1;
             cpu->stage[DRF].pc=0;
            strcpy(cpu->stage[EX1].opcode,"");
            cpu->stage[EX1].stalled=1;
            cpu->stage[EX1].pc=0;
          strcpy(cpu->stage[EX2].opcode,"");
          cpu->stage[EX2].stalled=1;
          cpu->stage[EX2].pc=0;
            

         }
         
      
      //cpu->ZF=0;
    /* Copy data from decode latch to execute latch*/
    cpu->stage[MEM2] = cpu->stage[MEM1];

    if(cpu->display)
         {
             if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Memory1", stage);
    }
         }
  }
    else if(stage->stalled)
    {

        if(cpu->display)
             {
                 if (ENABLE_DEBUG_MESSAGES) {
                     stage->pc=0;
                     strcpy(stage->opcode,"");
                     cpu->stage[MEM2] = cpu->stage[MEM1];
            print_stage_content("Memory1", stage);
        }
             }
    }
        
  return 0;
}


int
memory2(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[MEM2];
  if (!stage->busy && !stage->stalled) {
    
  
        if (strcmp(stage->opcode, "HALT") == 0) {
                 strcpy(cpu->stage[F].opcode,"");
                 cpu->stage[F].stalled=1;
                 cpu->stage[F].pc=0;
                 strcpy(cpu->stage[DRF].opcode,"");
                 cpu->stage[DRF].stalled=1;
                 cpu->stage[DRF].pc=0;
                strcpy(cpu->stage[EX1].opcode,"");
                cpu->stage[EX1].stalled=1;
                cpu->stage[EX1].pc=0;
              strcpy(cpu->stage[EX2].opcode,"");
              cpu->stage[EX2].stalled=1;
              cpu->stage[EX2].pc=0;
          strcpy(cpu->stage[MEM1].opcode,"");
                     cpu->stage[MEM1].stalled=1;
                     cpu->stage[MEM1].pc=0;
                

             }
      /* Store */
     if (strcmp(stage->opcode, "STORE") == 0) {
             cpu->data_memory[stage->mem_address]=stage->rs1_value;
          // cpu->regs_valid[stage->rs1]=1;
          // cpu->regs_valid[stage->rs2]=1;
             
         }
           if (strcmp(stage->opcode, "STR") == 0) {
               cpu->data_memory[stage->mem_address]=stage->rs1_value;
                // cpu->regs_valid[stage->rs1]=1;
               //  cpu->regs_valid[stage->rs2]=1;
              //   cpu->regs_valid[stage->rs3]=1;
               
           }
           if (strcmp(stage->opcode, "LDR") == 0) {
               cpu->regs[stage->rd]=cpu->data_memory[stage->mem_address];
                // cpu->regs_valid[stage->rs1]=1;
               //  cpu->regs_valid[stage->rs2]=1;
                 cpu->regs_valid[stage->rd]=1;
               
           }
           if (strcmp(stage->opcode, "LOAD") == 0) {
               cpu->regs[stage->rd]=cpu->data_memory[stage->mem_address];
                 //cpu->regs_valid[stage->rs1]=1;
               cpu->regs_valid[stage->rd]=1;
               
           }
    /* Copy data from decode latch to execute latch*/
    cpu->stage[WB] = cpu->stage[MEM2];

    if(cpu->display)
         {
             if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Memory2", stage);
    }
         }
  }
  
    else if(stage->stalled)
    {

        if(cpu->display)
             {
                 if (ENABLE_DEBUG_MESSAGES) {
                     stage->pc=0;
                     strcpy(stage->opcode,"");
                     cpu->stage[WB] = cpu->stage[MEM2];
            print_stage_content("Memory2", stage);
        }
             }
    }
        
  return 0;
}

/*
 *  Writeback Stage of APEX Pipeline
 *
 */
int
writeback(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[WB];
  if (!stage->busy && !stage->stalled) {
      if (strcmp(stage->opcode, "HALT") == 0) {
          cpu->ins_completed = cpu->code_memory_size - 1;
             strcpy(cpu->stage[F].opcode,"");
             cpu->stage[F].stalled=1;
             cpu->stage[F].pc=0;
             strcpy(cpu->stage[DRF].opcode,"");
             cpu->stage[DRF].stalled=1;
             cpu->stage[DRF].pc=0;
            strcpy(cpu->stage[EX1].opcode,"");
            cpu->stage[EX1].stalled=1;
            cpu->stage[EX1].pc=0;
          strcpy(cpu->stage[EX2].opcode,"");
          cpu->stage[EX2].stalled=1;
          cpu->stage[EX2].pc=0;
      strcpy(cpu->stage[MEM1].opcode,"");
                 cpu->stage[MEM1].stalled=1;
                 cpu->stage[MEM1].pc=0;
          strcpy(cpu->stage[MEM2].opcode,"");
          cpu->stage[MEM2].stalled=1;
          cpu->stage[MEM2].pc=0;
            

         }

       /* Update register file */
    if (strcmp(stage->opcode, "MOVC") == 0) {
        cpu->regs[stage->rd]=stage->buffer;
          cpu->regs_valid[stage->rd]=1;
    }
      if (strcmp(stage->opcode, "ADD") == 0) {
        cpu->regs[stage->rd] = stage->buffer;
           // cpu->regs_valid[stage->rs1]=1;
           // cpu->regs_valid[stage->rs2]=1;
            cpu->regs_valid[stage->rd]=1;
            if(cpu->bz_pc==stage->pc)
        {
          cpu->stall=0;
        }
          if(stage->buffer)
          {
              cpu->ZF=0;
              stage->ZF=0;
          }
          else{
             cpu->ZF=1;
             stage->ZF=1;
       
          }
      }
      if (strcmp(stage->opcode, "ADDL") == 0) {
        if(cpu->bz_pc==stage->pc)
        {
          cpu->stall=0;
        }
        cpu->regs[stage->rd] = stage->buffer;
            //cpu->regs_valid[stage->rs1]=1;
            cpu->regs_valid[stage->rd]=1;
         if(stage->buffer)
                 {
                     cpu->ZF=0;
                     stage->ZF=0;
                 }
                 else{
                    cpu->ZF=1;
                     stage->ZF=1;
                 }
          
      }
      if (strcmp(stage->opcode, "SUB") == 0) {
       
          if(cpu->bz_pc==stage->pc)
          {
              cpu->stall=0;
          }
        
        cpu->regs[stage->rd] = stage->buffer;
           // cpu->regs_valid[stage->rs1]=1;
           // cpu->regs_valid[stage->rs2]=1;
            cpu->regs_valid[stage->rd]=1;
          if(stage->buffer)
                   {
                       cpu->ZF=0;
                       stage->ZF=0;
                   }
                   else
                   {
                       cpu->ZF=1;
                       stage->ZF=1;
                   }
      }
      if (strcmp(stage->opcode, "SUBL") == 0) {
        if(cpu->bz_pc==stage->pc)
        {
          cpu->stall=0;
        }
        cpu->regs[stage->rd] = stage->buffer;
          //  cpu->regs_valid[stage->rs1]=1;
            cpu->regs_valid[stage->rd]=1;
         if(stage->buffer)
                 {
                     cpu->ZF=0;
                     stage->ZF=0;
                 }
                 else{
                     cpu->ZF=1;
                     stage->ZF=1;
                 }
      }
      if (strcmp(stage->opcode, "MUL") == 0) {
        if(cpu->bz_pc==stage->pc)
        {
          cpu->stall=0;
        }
        cpu->regs[stage->rd] = stage->buffer;
            //cpu->regs_valid[stage->rs1]=1;
           // cpu->regs_valid[stage->rs2]=1;
            cpu->regs_valid[stage->rd]=1;
          if(stage->buffer)
                   {
                     cpu->ZF=0;
                       stage->ZF=0;
                   }
                   else{
                       cpu->ZF=1;
                       stage->ZF=1;
                   }
        
      }
      if (strcmp(stage->opcode, "AND") == 0) {
      cpu->regs[stage->rd] = stage->buffer;
         // cpu->regs_valid[stage->rs1]=1;
         // cpu->regs_valid[stage->rs2]=1;
          cpu->regs_valid[stage->rd]=1;
      }
          if (strcmp(stage->opcode, "OR") == 0) {
          cpu->regs[stage->rd] = stage->buffer;
              //cpu->regs_valid[stage->rs1]=1;
           //   cpu->regs_valid[stage->rs2]=1;
              cpu->regs_valid[stage->rd]=1;
          }
              if (strcmp(stage->opcode, "XOR") == 0) {
              cpu->regs[stage->rd] = stage->buffer;
                  //cpu->regs_valid[stage->rs1]=1;
                  //cpu->regs_valid[stage->rs2]=1;
                  cpu->regs_valid[stage->rd]=1;
              }

              if (strcmp(stage->opcode, "JUMP") == 0) {
                 cpu->stage[DRF].stalled=0;
                 cpu->stage[EX1].stalled=0;
         
                
              }

    cpu->ins_completed++;

    if(cpu->display)
         {
             if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Writeback", stage);
    }
         }
  }
         else if(stage->stalled)
    {

        if(cpu->display)
             {
                 if (ENABLE_DEBUG_MESSAGES) {
                     stage->pc=0;
                     strcpy(stage->opcode,"");
            print_stage_content("Writeback", stage);
        }
             }
    }
         
  return 0;
}

/*
 *  APEX CPU simulation loop
 *
 */
int
APEX_cpu_run(APEX_CPU* cpu)
{
  while (1) {

    /* All the instructions committed, so exit */
    if (cpu->ins_completed == cpu->code_memory_size || cpu->clock==cpu->clock_cycles) {
      printf("\n");
      printf("\n");
        printf("-----------------------------------------------------\n");
      printf("-------------(apex) >> Simulation Complete------------\n");
        printf("-----------------------------------------------------\n");
        
        display_Reg(cpu);
        display_Mem(cpu);
      break;
    }


    if(cpu->display)
    {
      if (ENABLE_DEBUG_MESSAGES) {
          
      printf("-----------------------------------------------------\n");
      printf("Clock Cycle #: %d\n", cpu->clock);
      printf("-----------------------------------------------------\n");
    }
    }

    if(cpu->Toflush)
    {
      cpu->pc=cpu->bufRem;
      cpu->stage[F].stalled=0;
      cpu->stage[DRF].stalled=0;
      cpu->stage[EX1].stalled=0;
      //cpu->pc=cpu->bufRem;
      
      cpu->Toflush=0;
    }
    writeback(cpu);
    memory2(cpu);
    memory1(cpu);
    execute2(cpu);
    execute1(cpu);
    decode(cpu);
    fetch(cpu);
    cpu->clock++;
  }

  return 0;
}

void display_Reg(APEX_CPU* cpu)
{
    
    printf("\n");
    printf("\n");
    printf("------------------------------------------------------\n");
    printf("-------------------| REGISTER-FILE |------------------\n");
    printf("------------------------------------------------------\n");
    printf(" %-9s %-9s %-9s\n", "\tREGISTER\t", "VALUE \t" ,"\tSTATUS \t");
     printf("------------------------------------------------------\n");
     for (int r = 0; r < 16; r++)
    {
      if(cpu->regs_valid[r]==1)
      {
        strcpy(cpu->rdis,"VALID");

      }
      else
      {
        strcpy(cpu->rdis,"INVALID");
      }
      
        printf("\tREG %d\t \t   %-9d\t %-9s\t \n",r,cpu->regs[r],cpu->rdis);
        printf("\n");
    
}
    printf("------------------------------------------------------\n");

}
void display_Mem(APEX_CPU* cpu)
{
    printf("\n");
    printf("\n");
    printf("------------------------------------------------------\n");
    printf("-----------------|  DATA-MEMORY  |--------------------\n");
    printf("------------------------------------------------------\n");
    printf(" %-9s %-9s \n", "\t \tMEMORY\t", "\tVALUE \t");
    printf("------------------------------------------------------\n");

       for (int r = 0; r < 100; r++)
    {
        printf("\t \tMEM %d\t \t   %-9d\t \n",r,cpu->data_memory[r]);
         printf("\n");
    }
    printf("------------------------------------------------------\n");

}
int flush(APEX_CPU* cpu)
{
    strcpy(cpu->stage[DRF].opcode,"");
          cpu->stage[DRF].stalled=1;
          cpu->stage[DRF].pc=0;
         strcpy(cpu->stage[EX1].opcode,"");
         cpu->stage[EX1].stalled=1;
         cpu->stage[EX1].pc=0;
          strcpy(cpu->stage[F].opcode,"");
         cpu->stage[F].stalled=1;
         cpu->stage[F].pc=0;
  return 0;
}

