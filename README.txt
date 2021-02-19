---------------------------------------------------------------------------------
CPU Pipeline Simulator
---------------------------------------------------------------------------------
A simple implementation of 5 Stage APEX Pipeline


Notes:
----------------------------------------------------------------------------------
1) This code is a simple implementation of 5 Stage CPU Pipeline. 
	 
	 Fetch -> Decode -> Execute -> Memory -> Writeback
	 
	 You can read, modify and build upon given codebase to add other features as
	 required in project description. You are also free to write your own 
	 implementation from scratch.

2) All the stages have latency of one cycle. There is a single functional unit in 
	 EX stage which perform all the arithmetic and logic operations.

3) Logic to check data dependencies has not be included. You have to implement it.


How to compile and run
----------------------------------------------------------------------------------
1) go to terminal, cd into project directory and type 'make' to compile project
2) Run using ./apex_sim <input file name>



