


/*
	REFER TO THE SUBMISSION INSTRUCTION FOR DETAILS

    Name 1: Thomas McRoberts
    UTEID 1: tpm627
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Instruction Level Simulator                         */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files: isaprogram   LC-3b machine language program file     */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void process_instruction();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1
#define DEBUG 1
/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
*/

#define WORDS_IN_MEM    0x08000 
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */


typedef struct System_Latches_Struct{

  int PC,		/* program counter */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P;		/* p condition bit */
  int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int INSTRUCTION_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {                                                    
  printf("----------------LC-3b ISIM Help-----------------------\n");
  printf("go               -  run program to completion         \n");
  printf("run n            -  execute program for n instructions\n");
  printf("mdump low high   -  dump memory from low to high      \n");
  printf("rdump            -  dump the register & bus values    \n");
  printf("?                -  display this help menu            \n");
  printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                

  process_instruction();
  CURRENT_LATCHES = NEXT_LATCHES;
  INSTRUCTION_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {                                      
  int i;

  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating for %d cycles...\n\n", num_cycles);
  for (i = 0; i < num_cycles; i++) {
    if (CURRENT_LATCHES.PC == 0x0000) {
	    RUN_BIT = FALSE;
	    printf("Simulator halted\n\n");
	    break;
    }
    cycle();
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed                 */
/*                                                             */
/***************************************************************/
void go() {                                                     
  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating...\n\n");
  while (CURRENT_LATCHES.PC != 0x0000) 
    cycle();
  RUN_BIT = FALSE;
  printf("Simulator halted\n\n");
}

/***************************************************************/ 
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {          
  int address; /* this is a byte address */

  printf("\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  printf("-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    printf("  0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  printf("\n");

  /* dump the memory contents into the dumpsim file */
  fprintf(dumpsim_file, "\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  fprintf(dumpsim_file, "-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    fprintf(dumpsim_file, " 0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */   
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {                               
  int k; 

  printf("\nCurrent register/bus values :\n");
  printf("-------------------------------------\n");
  printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
  printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  printf("Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  printf("\n");

  /* dump the state information into the dumpsim file */
  fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
  fprintf(dumpsim_file, "-------------------------------------\n");
  fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
  fprintf(dumpsim_file, "PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  fprintf(dumpsim_file, "Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    fprintf(dumpsim_file, "%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */  
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {                         
  char buffer[20];
  int start, stop, cycles;

  printf("LC-3b-SIM> ");

  scanf("%s", buffer);
  printf("\n");

  switch(buffer[0]) {
  case 'G':
  case 'g':
    go();
    break;

  case 'M':
  case 'm':
    scanf("%i %i", &start, &stop);
    mdump(dumpsim_file, start, stop);
    break;

  case '?':
    help();
    break;
  case 'Q':
  case 'q':
    printf("Bye.\n");
    exit(0);

  case 'R':
  case 'r':
    if (buffer[1] == 'd' || buffer[1] == 'D')
	    rdump(dumpsim_file);
    else {
	    scanf("%d", &cycles);
	    run(cycles);
    }
    break;

  default:
    printf("Invalid Command\n");
    break;
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {                                           
  int i;

  for (i=0; i < WORDS_IN_MEM; i++) {
    MEMORY[i][0] = 0;
    MEMORY[i][1] = 0;
  }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {                   
  FILE * prog;
  int ii, word, program_base;

  /* Open program file. */
  prog = fopen(program_filename, "r");
  if (prog == NULL) {
    printf("Error: Can't open program file %s\n", program_filename);
    exit(-1);
  }

  /* Read in the program. */
  if (fscanf(prog, "%x\n", &word) != EOF)
    program_base = word >> 1;
  else {
    printf("Error: Program file is empty\n");
    exit(-1);
  }

  ii = 0;
  while (fscanf(prog, "%x\n", &word) != EOF) {
    /* Make sure it fits. */
    if (program_base + ii >= WORDS_IN_MEM) {
	    printf("Error: Program file %s is too long to fit in memory. %x\n",
             program_filename, ii);
	    exit(-1);
    }

    /* Write the word to memory array. */
    MEMORY[program_base + ii][0] = word & 0x00FF;
    MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
    ii++;
  }

  if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

  printf("Read %d words from program into memory.\n\n", ii);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */ 
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename, int num_prog_files) { 
  int i;

  init_memory();
  for ( i = 0; i < num_prog_files; i++ ) {
    load_program(program_filename);
    while(*program_filename++ != '\0');
  }
  CURRENT_LATCHES.Z = 1;  
  NEXT_LATCHES = CURRENT_LATCHES;
    
  RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {                              
  FILE * dumpsim_file;

  /* Error Checking */
  if (argc < 2) {
    printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
           argv[0]);
    exit(1);
  }

  printf("LC-3b Simulator\n\n");

  initialize(argv[1], argc - 1);

  if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
    printf("Error: Can't open dumpsim file\n");
    exit(-1);
  }

  while (1)
    get_command(dumpsim_file);
    
}

/***************************************************************/
/* Do not modify the above code.
   You are allowed to use the following global variables in your
   code. These are defined above.

   MEMORY

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */

/***************************************************************/

int fetch();
int decode(int instr);
void setcc(int nzp);
int sext(int num, int bits);



int handleALU(int instr, int op);
void handleBR(int instr);
void handleLEA(int instr);
void handleJMP(int instr);
void handleJSR(int instr);


void handleTRAP(int instr);




void process_instruction(){
  /*  function: process_instruction
   *  
   *    Process one instruction at a time  
   *       -Fetch one instruction
   *       -Decode 
   *       -Execute
   *       -Update NEXT_LATCHES
   */     
    #if DEBUG
    printf("************************************************\n");
	printf("starting process\n");
    printf("************************************************\n");
	#endif
    int instruction = fetch();
    int opcode = decode(instruction);   
    #if DEBUG
    printf("Opcode = 0x%1x\n*********\n", opcode);
    #endif
    
    
    switch(opcode){
        case 1 :
        case 5 :
        case 9 : /* alu */
            setcc( handleALU(instruction, opcode) );
            break;
        case 0 : /* BR */
            handleBR(instruction);
            break;
        case 14 : /* LEA */
            handleLEA(instruction);
            break;
        case 12 : /* JMP */
            handleJMP(instruction);
            break;
        case 8 : /*  RTI, don't do ??? */
            printf("RTI Called, no simmulated handler\n");
            break;
        case 4 : /* JSR */
            handleJSR(instruction);            
            break;
        case 2 :
        case 6 :
        case 3 :
        case 7 : /* LD/ST */
            
            break;
        case 13 : /* SHF */
            
            break;
        case 15 : /* TRAP */ 
            handleTRAP(instruction);
            break;
        default:
            printf("error in decode: unknown opcode\n");
            break;




    }



}


int handleALU(int instr, int op){
    #if DEBUG
    printf("ALU handle\n");
    #endif
    int* DR; /* Destination Register */
    int SR1, OPSPEC; /* operands */
    int A = (instr >> 5) & 0x1;
    
    DR = &NEXT_LATCHES.REGS[ (instr >> 9) & 0x7 ];
    SR1 = CURRENT_LATCHES.REGS[ (instr >> 6) & 0x7];
    SR1 = Low16bits(SR1);

    OPSPEC = (A) ? sext( (instr & 0x1F) , 5) : CURRENT_LATCHES.REGS[instr & 0x7];
    printf("OPSPEC = 0x%4x =  %i\n",OPSPEC, OPSPEC);
    OPSPEC = Low16bits(OPSPEC);
    #if DEBUG
    printf("DR = %i SR1 = %i A = %i, OPSPEC = %i \n",*DR, SR1, A, OPSPEC);
    #endif
    
    switch(op){

    case 1 : /* ADD */
        *DR = SR1 + OPSPEC;
        break;
    case 5 : /* AND */
        *DR = SR1 & OPSPEC;
        break;
    case 9 : /* XOR */
        *DR = SR1 ^ OPSPEC;
        break;
    default : 
        printf("Error in handleALU(): invalid opcode");
        break;
    }

    *DR = Low16bits(*DR);


    int nzp = 0;
    if(*DR == 0){
        nzp += 2;
    }
    else if( ( (*DR) >> 15 ) == 1 ){
        nzp += 4;
    }
    else{
        nzp += 1;
    }
   
    #if DEBUG
    printf("DR = 0x%4x = %i, nzp = 0x%1x\n", *DR, *DR, nzp);
    #endif
    return nzp;

}


void handleBR(int instr){
    int n, z, p;
    n = (instr >> 11) & 0x1;
    z = (instr >> 10) & 0x1;
    p = (instr >> 9) & 0x1;
    if( (CURRENT_LATCHES.N && n) || (CURRENT_LATCHES.Z && z) || (CURRENT_LATCHES.P && p) ){
        #if DEBUG
        printf("Branching based on NZP = %1x%1x%1x, to offset %i which is location 0x%4x \n", n, z, p, sext ( (instr & 0x1FF), 9) , Low16bits(NEXT_LATCHES.PC +  (sext((instr & 0x1FF), 9) << 1) ) );
        #endif
        NEXT_LATCHES.PC = NEXT_LATCHES.PC + (sext( (instr & 0x1FF), 9) << 1 );
        NEXT_LATCHES.PC = Low16bits(NEXT_LATCHES.PC);
    }

}

void handleLEA(int instr){
    int* DR = &NEXT_LATCHES.REGS[ (instr >> 9) & 0x7];
    
    #if DEBUG
    printf("LEA handle. DR = %i, load from memory 0x%4x \n", ( (instr >> 9) & 0x7),  Low16bits( NEXT_LATCHES.PC + ( sext( (instr & 0x1FF), 9) << 1) ) );
    #endif
    
    *DR = NEXT_LATCHES.PC + ( sext( (instr & 0x1FF), 9) << 1);
    
    *DR = Low16bits(*DR);
}


void handleJMP(int instr){
    #if DEBUG
    printf("Jump handle");
    #endif
    NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[(instr >> 6) & 0x7];
    
    NEXT_LATCHES.PC = Low16bits(NEXT_LATCHES.PC);
}


void handleJSR(int instr){
    NEXT_LATCHES.REGS[7] = NEXT_LATCHES.PC;
    int A = ( (instr >> 11) & 0x1); 
    #if DEBUG
    printf("JSR handle\n");
    #endif

    if(A){
        NEXT_LATCHES.PC = NEXT_LATCHES.PC + ( sext((instr & 0x7FF) , 11) << 1);
        printf("(JSR) Jumping to 0x%4x\n", NEXT_LATCHES.PC);
    }
    else{
        int BaseR = CURRENT_LATCHES.REGS[(instr >> 6) & 0x7];
        NEXT_LATCHES.PC = BaseR;
        printf("(JSRR) Jumping to 0x%4x\n", NEXT_LATCHES.PC);
    }
    
    NEXT_LATCHES.PC = Low16bits(NEXT_LATCHES.PC);
}

void handleLDST(int instr){
    int A = (instr >> 11) & 0x1;
    if(A){
        NEXT_LATCHES.PC = NEXT_LATCHES.PC + ( (sext( (instr & 0x7FF) ,11) ) << 1);
    }
    int W = (instr >> 14) &0x1; /*  1 = word, 0 = byte  */
    int ST = 0;
    
    int BaseR = CURRENT_LATCHES.REGS[(instr >> 6) & 0x7]; 
    BaseR = Low16bits(BaseR);
    int offset6 = sext( (instr & 0x3F), 6);
    if(W){ /*  word  */
        offset6 = offset6 << 1;
    }
    int address = BaseR + offset6;

    if(ST){ /*  Store  */

    }
    else{ /*  Load  */
        int* DR = &NEXT_LATCHES.REGS[ (instr >> 9) & 0x7];  
        /* TODO: Check Memory shifts for everything */
    }



}





void handleTRAP(int instr){
    #if DEBUG
    printf("TRAP handle\n");
    #endif

    NEXT_LATCHES.REGS[7] = NEXT_LATCHES.PC;    
    NEXT_LATCHES.PC = MEMORY[( (instr & 0xFF) << 1)][0] + (MEMORY[( (instr & 0xFF) << 1)][1] << 8);
    #if DEBUG
    printf("PC at 0x%4x\n", NEXT_LATCHES.PC);
    #endif
}
    




int fetch(){
	#if DEBUG
	printf("address 0x%4x\n", CURRENT_LATCHES.PC);
	#endif
    int data = 0;
	data += MEMORY[CURRENT_LATCHES.PC >> 1][0];
	data += MEMORY[CURRENT_LATCHES.PC >> 1][1] << 8;
	#if DEBUG
	printf("Memory = 0x%4x\n", data);
	#endif
	NEXT_LATCHES.PC += 2;
	return data;
}



int decode(int instr){
    return ( (instr >> 12) & 0xF);
}


void setcc(int nzp){
    NEXT_LATCHES.N = ( (nzp >> 2) & 0x1);
    NEXT_LATCHES.Z = ( (nzp >> 1) & 0x1);
    NEXT_LATCHES.P = (nzp & 0x1);
}




int sext(int num, int bits){
    int sign = (num >> (bits - 1)) & 0x1; 
    
    int ret;
    ret = 0xFFFF << bits;

    if(sign){
        ret = ret | num;
    }
    else{
        ret = (~ret) & num;
    }

    #if DEBUG
    printf("sext: Original = 0x%4x, ret = 0x%4x\n", num, ret);
    #endif
    return ret;
}




