#ifndef _APEX_CPU_H_
#define _APEX_CPU_H_
/**
 *  cpu.h
 *  Contains various CPU and Pipeline Data structures
 */

enum
{
  F,
  DRF,
  EX1,
  EX2,
  MEM1,
  MEM2,
  WB,
  NUM_STAGES
};

/* Format of an APEX instruction  */
typedef struct APEX_Instruction
{
  char opcode[128];	// Operation Code
  int rd;		    // Destination Register Address
  int rs1;		    // Source-1 Register Address
  int rs2;		    // Source-2 Register Address
  int rs3;          // Source-3 Register Address

  int imm;		    // Literal Value
    int halt;
    int stall;
} APEX_Instruction;

/* Model of CPU stage latch */
typedef struct CPU_Stage
{
  int pc;		    // Program Counter
  char opcode[128];	// Operation Code
  int rs1;		    // Source-1 Register Address
  int rs2;		    // Source-2 Register Address
  int rs3;            // Source-3 Register Address
  int rd;		    // Destination Register Address
  int imm;		    // Literal Value
  int rs1_value;	// Source-1 Register Value
  int rs2_value;	// Source-2 Register Value
  int rs3_value;    // Source-3 Register Value
  int buffer;		// Latch to hold some value
  int mem_address;	// Computed Memory Address
  int busy;		    // Flag to indicate, stage is performing some action
  int stalled;		// Flag to indicate, stage is stalled
    int stall;
    int ZF;
    int flush;
    int ari;
    int set_flow;
  
} CPU_Stage;

/* Model of APEX CPU */
typedef struct APEX_CPU
{
  char rdis[128];
    int bz_pc;
    int rem;
    int display;
    int Toflush;
    int bufRem;
    
    int clock_cycles;
  /* Clock cycles elasped */
  
    int clock;

  /* Current program counter */
  int pc;

  /* Integer register file */
  int regs[32];
  int regs_valid[32];

  /* Array of 5 CPU_stage */
  CPU_Stage stage[7];

  /* Code Memory where instructions are stored */
  APEX_Instruction* code_memory;
  int code_memory_size;

  /* Data Memory */
  int data_memory[4096];

  /* Some stats */
  int ins_completed;
    
    int ZF;
    int flush;
    int ari;
    int loop;
    int stall;
} APEX_CPU;

APEX_Instruction*
create_code_memory(const char* filename, int* size);

APEX_CPU*
APEX_cpu_init(const char* filename);

int
APEX_cpu_run(APEX_CPU* cpu);

void
APEX_cpu_stop(APEX_CPU* cpu);

int
fetch(APEX_CPU* cpu);

int
decode(APEX_CPU* cpu);

int
execute1(APEX_CPU* cpu);
int
execute2(APEX_CPU* cpu);

int
memory1(APEX_CPU* cpu);
int
memory2(APEX_CPU* cpu);
int
writeback(APEX_CPU* cpu);

int Regs_Valid(APEX_CPU* cpu);

void Display(APEX_CPU *cpu);
void Simulate(APEX_CPU *cpu);
void display_Reg(APEX_CPU* cpu);
void display_Mem(APEX_CPU* cpu);

int flush(APEX_CPU* cpu);
int flowDependency(APEX_CPU* cpu);


#endif
