// Michael Harris - mi051467
// CDA3103C - mips datapath simulation

#include "spimcore.h"

/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
	if (ALUControl == 0b000) 			// add
		*ALUresult = A + B;
	
	else if (ALUControl == 0b001)		// sub
		*ALUresult = A - B;
	
	else if (ALUControl == 0b010) {		// slt
		int tempA = (int) A;
		int tempB = (int) B;
		
		if (tempA < tempB) 
			*ALUresult = 1;
		else 
			*ALUresult = 0;
	}
	
	else if (ALUControl == 0b011) 		// sltu
		if (A < B)
			*ALUresult = 1;
		else 
			*ALUresult = 0;
	
	else if (ALUControl == 0b100) 		// and
		*ALUresult = A & B;
	
	else if (ALUControl == 0b101) 		// or
		*ALUresult = A | B;
	
	else if (ALUControl == 0b110) 		// B << 16 bits
		*ALUresult = B << 16;
	
	else if (ALUControl == 0b111) 		// flip A
		*ALUresult = ~A;
		
	// set the Zero output appropriately 
	if (*ALUresult == 0) 
		*Zero = 1;
	else 
		*Zero = 0;
	
	return;
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
	// check if PC passed is word aligned
	if (PC % 4 != 0)
		return 1;
	
	*instruction = Mem[PC >> 2];
	
	return 0;
}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
	*op =    (instruction & 0b11111100000000000000000000000000) >> 26;	//31-26
	*r1 =    (instruction & 0b00000011111000000000000000000000) >> 21;	//25-21
	*r2 =    (instruction & 0b00000000000111110000000000000000) >> 16;	//20-16
	*r3 =    (instruction & 0b00000000000000001111100000000000) >> 11;	//15-11
	*funct =  instruction & 0b00000000000000000000000001111111;			//5-0
	*offset = instruction & 0b00000000000000001111111111111111;			//15-0
	*jsec =   instruction & 0b00000011111111111111111111111111;			//25-0
}



/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
	if (op == 0) {				// r-types have an opcode of 000000
		controls->RegDst = 1;
		controls->Jump = 0;
		controls->Branch = 0;
		controls->MemRead = 0;
		controls->MemtoReg = 0;
		controls->ALUOp = 7;	//0b111 to indicate an r-type
		controls->MemWrite = 0;
		controls->ALUSrc = 0;
		controls->RegWrite = 1;
	}
	else if (op == 0b000010) {	// opcode 2: jump
		controls->RegDst = 2;
		controls->Jump = 1;
		controls->Branch = 2;
		controls->MemRead = 0;
		controls->MemtoReg = 2;
		controls->ALUOp = 0;
		controls->MemWrite = 0;
		controls->ALUSrc = 2;
		controls->RegWrite = 0;
	}
	else if (op == 0b000100) {	// opcode 4: beq
		controls->RegDst = 2;
		controls->Jump = 0;
		controls->Branch = 1;
		controls->MemRead = 0;
		controls->MemtoReg = 2;
		controls->ALUOp = 1;	//subtract
		controls->MemWrite = 0;
		controls->ALUSrc = 2;
		controls->RegWrite = 0;
	}
	else if (op == 0b001000) {	// opcode 8: addi
		controls->RegDst = 0;
		controls->Jump = 0;
		controls->Branch = 0;
		controls->MemRead = 0;
		controls->MemtoReg = 0;
		controls->ALUOp = 0;
		controls->MemWrite = 0;
		controls->ALUSrc = 1;
		controls->RegWrite = 1;
	}
	else if (op == 0b001010) {	// opcode 10: slti
		controls->RegDst = 0;
		controls->Jump = 0;
		controls->Branch = 0;
		controls->MemRead = 0;
		controls->MemtoReg = 0;
		controls->ALUOp = 2;	//slt
		controls->MemWrite = 0;
		controls->ALUSrc = 1;
		controls->RegWrite = 1;
	}
	else if (op == 0b001011) {	// opcode 11: sltiu
		controls->RegDst = 0;
		controls->Jump = 0;
		controls->Branch = 0;
		controls->MemRead = 0;
		controls->MemtoReg = 0;
		controls->ALUOp = 3;	//sltu
		controls->MemWrite = 0;
		controls->ALUSrc = 1;
		controls->RegWrite = 1;
	}
	else if (op == 0b001111) {	// opcode 15: load upper imm
		controls->RegDst = 0;
		controls->Jump = 0;
		controls->Branch = 0;
		controls->MemRead = 0;
		controls->MemtoReg = 0;
		controls->ALUOp = 6;	//shift ev left 16
		controls->MemWrite = 0;
		controls->ALUSrc = 1;
		controls->RegWrite = 1;
	}
	else if (op == 0b100011) {	// opcode 35: lw
		controls->RegDst = 0;
		controls->Jump = 0;
		controls->Branch = 0;
		controls->MemRead = 1;
		controls->MemtoReg = 1;
		controls->ALUOp = 0;
		controls->MemWrite = 0;
		controls->ALUSrc = 1;
		controls->RegWrite = 1;
	}
	else if (op == 0b101011) {	// opcode 43: sw
		controls->RegDst = 0;
		controls->Jump = 0;
		controls->Branch = 0;
		controls->MemRead = 0;
		controls->MemtoReg = 0;
		controls->ALUOp = 0;
		controls->MemWrite = 1;
		controls->ALUSrc = 1;
		controls->RegWrite = 0;
	}
	else
		return 1;
	
	return 0;
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
	*data1 = Reg[r1];
	*data2 = Reg[r2];
}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
	int temp = 0;
	
	if (offset >> 15 != 0)
		temp = 0b11111111111111110000000000000000;
	
	temp += offset;
	*extended_value = temp;
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
	char control;
	unsigned int temp2;
	
	// only care about the function field for r types, otherwise ALUOp is the control signal
	if (ALUOp==0 || ALUOp==1 || ALUOp==2 || ALUOp==3 || ALUOp==4 || ALUOp==5 || ALUOp==6) 
		control = ALUOp;
	
	// r-type instruction, so look at function field to determine control for ALU
	else if (ALUOp == 7) {	
		if (funct == 0b100000) 		//add
			control = 0;
			
		else if (funct == 0b100010) //sub
			control = 0b1;
			
		else if (funct == 0b100100)	//and
			control = 0b100;
			
		else if (funct == 0b100101)	//or
			control = 0b101;
			
		else if (funct == 0b101010)	//slt
			control = 0b010;
			
		else if (funct == 0b101011)	//sltu
			control = 0b011;
			
		else	//invalid function field
			return 1;
	}
	
	else
		return 1;
	
	// data1 is always the first input, the second depends on the ALUSrc control line
	if (ALUSrc == 1)
		temp2 = extended_value;
	else
		temp2 = data2;
	
	// call the ALU function with appropriate parameters
	ALU(data1, temp2, control, ALUresult, Zero);
	
	return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem) {
    
	// read memory
	if (MemRead == 1)
		if (ALUresult % 4 == 0 && ALUresult <= 0xFFFF) // check word alignment and address validity
			*memdata = Mem[ALUresult >> 2];
		else
			return 1;
    
	// write to memory
	if (MemWrite == 1)
		if (ALUresult % 4 == 0 && ALUresult <= 0xFFFF) // check word alignment and address validity
			Mem[ALUresult >> 2] = data2;
		else
			return 1;
    
	return 0;
}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg) {
    
	// only do anything if we are allowed to 
	if (RegWrite == 1) {	
	
		// data is coming from memory
		if (MemtoReg == 1)		
			// i-type, RegDst must be == 0
			Reg[r2] = memdata;							
		
		// data is coming from register
		else if (MemtoReg == 0)
			
			// r-type, write to r3 	
			if (RegDst == 1)
				Reg[r3] = ALUresult;
			// i-type, write to r2
			else						
				Reg[r2] = ALUresult;
	}
	
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
	// always add 4 to program counter
	*PC += 4;
	
	// if branch and 0 asserted, shift ev up twice and add to PC
	if (Branch == 1 && Zero == 1) 
		*PC = *PC + (extended_value << 2);
	
	// if jump asserted: Left shift bits of jsec by 2 and use upper 4 bits of PC
	if (Jump == 1) 
		*PC = ((*PC & 0b11110000000000000000000000000000) + (jsec << 2));
	
}
