
# Thalia_ysassi_project3 Report
CS 219 Programming Assignment 3 (Spring 2025)

# Extra Credit
I extended my simulator to support branch insrtuctors by adding a 2nd bool variable (skipToBranch) that was enabled if a branch operation was found and valiabled using the NZCV flags. 

When the skipToBranch bool was enabled, the file line would compare each operation to my stored branchLabel. If they didn't match, the code would skip to the next line until it was found. Once found, the skipToBranch bool was set back to false and the code proceeded as normal.

# How to Run Program | Makefile Commands
**make main**: executable function \
**make clean**: removes executable 

# How the Code Works
My code is split into a main function with seven helper functions. Most of the helper functions are linked together to move onto the next operation as soon as possible, as it doesn't call the next function if the operands or operations are invalid. 

## Main()
Declares arrays for both the memory and the registers. Reads each line from input file, first storing the operation. It then prints the current file line. If skipToBranch is not enabled, ValidOperation is called to check if valid, and if it returns as true, the program then parsing the rest ofthe remaining line into individual numbers or the branch label. If the operation is not a branch, GetNumbers() is called to check if the operands are valid. If the function returns as true, computeOperation() is called to perform the operation and print the results. If the operation is a branch, the rest of the file line is read as the branch label and computeBranch() is called. 

### checkOpCondition()
This function is called if operation size == 5. Checks if the extra condition is recognized (GT, GE, LT, LE, EQ, or NE) as well as if the current status flags meet the condition's requirements. returns true if condition and requirements are met, false otherwise.

### validOperation()
Checks if operation is one of the supported operations (ADD, SUB, AND, ORR, EOR, LSR, LSL, MOV, MVN, CMP, LDR, STR, BNE, BEQ, BAL) and sets numCount to required amount of operands. if operation isn't supported, valid is set to false

If operation is BNE, BEQ, or BAL, variable branch is set to true (used in main)

function returns valid

### getNumbers()
If operation needs 3 operands, function individually reads each number to check if the first two values are registers and is stored into num1 and num2. Then checks if num3 is a register, if so, isReg is set to true. Otherwise, the value is stored into num3

If operation needs 2 operands, function individually reads first char to check if it is a register. If so, the value is stored into num1. Then checks if num2 is a register, if so, isReg is set to true. Otherwise, the value is stored into num2

Otherwise, if numCount != numbers.size(), error message is printed

### computeOperation()
Uses the operation and num1, num2, and num3 variables to compute and store the correct values in either the registers or memory arrays. Stores both an unsigned and signed value to check status flags if needed. If affectFlags is enabled, it called the updateFlags() function


### computeBranch()
Uses the operation and Z flag to decide if the branch is taken. Enabled skipToBranch bool if operation and conditions are valid.

### updateFlags()
Uses the unsigned and signed values to reset status flags. N, Z, and V have the same code no matter the equation as as set by checking if less than or equal to zero. If operation is CMP or SUB, the C flag is the opposite of the N flag's value, while if the operation is ADD the C flag is equal to the V flag. If the operation is LSR or LSL, the C flag is set depending on if the signed result is less than zero.

### printResults()
Prints the register, NZCV flags, and memory array after executing each line


# Logic and Functionality 

## Conditional Execution
1. GT (Greater Than) - function executes if Z == 0 and N == V
2. GE (Greater Than or Equal) - function executes if N == V
3. LT (Less Than) - function executes if N != V
4. LE (Less Than or Equal) - function executes if Z == 1 or N != V
5. EQ (Equal) - function executes if Z == 1
6. NE (Not Equal) - function executes if Z == 0

## Status Flags
The 'S' character may be added onto any of the following artimetic and logical operations. This will change the NZCV status flags depending on the operation's result.

## Three Operands (Arithmetic and Logical Operations)

1. ADD - Adds two numbers (num2 and num3), to be stored in the first given operand
2. SUB - Subtracts two numbers (num2 and num3), to be stored in the first given operand
3. AND - Compares two numbers, (num2 and num3), results in a 1 for every identical bit. Final answer is stored in the first given operand
4. ORR - Compares two numbers, (num2 and num3), results in a 1 for every bit equal to 1. Final answer is stored in the first given operand
5. EOR - Compares two numbers, (num2 and num3), results in a 1 if only one bit is equal to 1. Final answer is stored in the first given operand
6. LSL - shifts the register of num2 left by the value of num3
7. LSR - shifts the register of num2 right by the value of num3

## Two Operands
6. MOV - Stores a value (num2) into the first given operand (num1)
7. CMP - Performs the SUB operation, but only changes the status flags
8. LOAD - Stores the value of what is in the memory array at index num2 into the register at index num1
9. STORE - Stores the value of what is in  the register at index num1 into the memory array at index num2
10. MVN - Stores the NOT value of (num2) into the first given operand (num1)

## Branch Operations
11. BNE - Takes the branch if the Zero Flag (Z) is set to 0
12. BEQ - Takes the branch if the Zero Flag (Z) is set to 1
13. BAL - Will always take the branch


# Test Cases
1. MOV R0,#0x14 \
Moves #0x14 into register 0 \
R0=0x14
2. MOV R1,#0xA \
Moves #0xA into register 1 \
R0=0x14, R1=0xA
3. CMP R0, R1 \
Performs SUB operation but only sets NZCV flags \
NZCV: 0010 \
R0=0x14, R1=0xA
4. ADDGT R2, R0, R1 \
From last CMP operation, R0 is greater than R1, so ADD operation is executed \
NZCV: 0010 \
R0=0x14, R1=0xA, R2=0x1E
5. ADDEQ R3, R0, R1 \
From last CMP operation, R0 is NOT equal to R1, so ADD operation is not executed \
NZCV: 0010 \
R0=0x14, R1=0xA, R2=0x1E
6. SUBLT R4, R0, R1 \
From last CMP operation, R0 is NOT less than R1, so SUB operation is not executed \
NZCV: 0010 \
R0=0x14, R1=0xA, R2=0x1E
7. ADDS R0,R1,R2 \
Performs ADD operation with R1 and R2, and is stored R0. S is added, so status flags are updated \
NZCV: 0000 \
R0=0x28, R1=0xA, R2=0x1E
8. CMP R2, #50 \
Performs SUB operation but only sets NZCV flags \
NZCV: 1000 \
R0=0x14, R1=0xA, R2=0x1E
9. ANDGT R3, R2, #1 \
From last CMP operation, R2 is NOT greater than 50 \
NZCV: 1000 \
R0=0x14, R1=0xA, R2=0x1E
10. ORREQ R4, R2, #2 \
From last CMP operation, R2 is NOT equal to 50 \
NZCV: 1000 \
R0=0x14, R1=0xA, R2=0x1E
11. EORLT R5, R2, #3 \
From last CMP operation, R2 is less than 50 \
Performs the EOR operation with R2 and 3, stores value in R5 \
NZCV: 1000 \
R0=0x14, R1=0xA, R2=0x1E, R3=0x0, R4=0x0, R5=0x1D
12. ORRS R3,R2,#15 \
Performs ORR operation and updates status flags \
NZCV: 0000 \
R0=0x14, R1=0xA, R2=0x1E, R3=0x1F, R4=0x0, R5=0x1D
13. CMP R5, #0x1D \
Performs SUB operation but only changes the status flags \
NZCV: 0110 \
R0=0x14, R1=0xA, R2=0x1E, R3=0x1F, R4=0x0, R5=0x1D
14. LSRGE R7, R5, #2 \
From last CMP operation, R5 is equal to 0x1D \
Performs the LSR operation with R5 and 2, stores value in R7 \
NZCV: 0110 \
R0=0x14, R1=0xA, R2=0x1E, R3=0x1F, R4=0x0, R5=0x1D, R6=0x0, R7=0x7
15. LSLNE R8, R5, #3 \
From last CMP operation, R5 and 0x1D are NOT not equal, so the operation is not performed \
NZCV: 0110 \
R0=0x14, R1=0xA, R2=0x1E, R3=0x1F, R4=0x0, R5=0x1D, R6=0x0, R7=0x7
16. CMP R8, R9 \
Performs SUB operation but only updates status flags \
NZCV: 0110 \
R0=0x14, R1=0xA, R2=0x1E, R3=0x1F, R4=0x0, R5=0x1D, R6=0x0, R7=0x7
17. MOVGT R10, R8 \
From the last CMP operation, R8 is NOT greater than R9, so the MOV operation is not performed \
NZCV: 0110 \
R0=0x14, R1=0xA, R2=0x1E, R3=0x1F, R4=0x0, R5=0x1D, R6=0x0, R7=0x7
18. MVNEQ R11, R8 \
From the last CMP operation, R8 is equal to R9, so the MVN operation is performed \
NZCV: 0110 \
R0=0x14, R1=0xA, R2=0x1E, R3=0x1F, R4=0x0, R5=0x1D, R6=0x0, R7=0x7, R8=0x0, R9=0x0, R10=0x0, R11=0xFFFFFFFF
19. MOV R6,#0x104 \
Moves 0x104 into register 6 \
NZCV: 0110 \
R0=0x14, R1=0xA, R2=0x1E, R3=0x1F, R4=0x0, R5=0x1D, R6=0x104, R7=0x7, R8=0x0, R9=0x0, R10=0x0, R11=0xFFFFFFFF
20. CMP R7, R2 \
Performs SUB operation but only updates status flags \
NZCV: 1000 \
R0=0x14, R1=0xA, R2=0x1E, R3=0x1F, R4=0x0, R5=0x1D, R6=0x104, R7=0x7, R8=0x0, R9=0x0, R10=0x0, R11=0xFFFFFFFF
21. LDRGT R3, [R6] \
From last CMP operation, R7 is NOT greater than R2, so LDR operation is not performed \
NZCV: 1000 \
R0=0x14, R1=0xA, R2=0x1E, R3=0x1F, R4=0x0, R5=0x1D, R6=0x104, R7=0x7, R8=0x0, R9=0x0, R10=0x0, R11=0xFFFFFFFF
22. STRNE R3, [R6] \
From the last CMP operation, R7 is not equal to R2, so STR operation is performed \
NZCV: 1000 \
R0=0x14, R1=0xA, R2=0x1E, R3=0x1F, R4=0x0, R5=0x1D, R6=0x104, R7=0x7, R8=0x0, R9=0x0, R10=0x0, R11=0xFFFFFFFF \
Memory: ----,0x1F,----,----,----
23. CMP R2, R7 \
Performs the SUB operation but only updates the status flags \
NZCV: 0010 \
R0=0x14, R1=0xA, R2=0x1E, R3=0x1F, R4=0x0, R5=0x1D, R6=0x104, R7=0x7, R8=0x0, R9=0x0, R10=0x0, R11=0xFFFFFFFF \
Memory: ----,0x1F,----,----,----
24. LDRGT R9, [R6] \
From last CMP operation, R2 is greater than R7, so LDR operation is performed \
NZCV: 0010 \
R0=0x14, R1=0xA, R2=0x1E, R3=0x1F, R4=0x0, R5=0x1D, R6=0x104, R7=0x7, R8=0x0, R9=0x1F, R10=0x0, R11=0xFFFFFFFF \
Memory: ----,0x1F,----,----,----
25. STRNE R3, [R6] \
From last CMP operation, R2 is not equal to R7, so STR operation is performed \
NZCV: 0010 \
R0=0x14, R1=0xA, R2=0x1E, R3=0x1F, R4=0x0, R5=0x1D, R6=0x104, R7=0x7, R8=0x0, R9=0x1F, R10=0x0, R11=0xFFFFFFFF \
Memory: ----,0x1F,----,----,----

Extra Credit Operations 

26. CMP R7, #7
Performs the SUB operation but only updates the status flags \
NZCV: 0110 \
R0=0x14, R1=0xA, R2=0x1E, R3=0x1F, R4=0x0, R5=0x1D, R6=0x104, R7=0x7, R8=0x0, R9=0x1F, R10=0x0, R11=0xFFFFFFFF \
Memory: ----,0x1F,----,----,----
27. BEQ SKIP \
From the last CMP operation, R7 and 7 are equal, so we will skip until we find the SKIP branch
NZCV: 0110 \
R0=0x14, R1=0xA, R2=0x1E, R3=0x1F, R4=0x0, R5=0x1D, R6=0x104, R7=0x7, R8=0x0, R9=0x1F, R10=0x0, R11=0xFFFFFFFF \
Memory: ----,0x1F,----,----,----
28. ADD R2, R2, #1 \
Skipped \
29 SKIP SUB R3, R3, #1
Skip Branch found. Performs the SUB operation with R3 and 1. Stores final value in R3 \
NZCV: 0110 \
R0=0x14, R1=0xA, R2=0x1E, R3=0x1E, R4=0x0, R5=0x1D, R6=0x104, R7=0x7, R8=0x0, R9=0x1F, R10=0x0, R11=0xFFFFFFFF \
Memory: ----,0x1F,----,----,----


# Invalid Cases in File
For this assignment, there are no cases that are invalid. However, there are multiple cases that do not execute because they didn't pass the second operation condition.

Operations that were skipped are:
5. ADDEQ R3, R0, R1
6. SUBLT R4, R0, R1
9. ANDGT R3, R2, #1
10. ORREQ R4, R2, #2
15. LSLNE R8, R5, #3
17. MOVGT R10, R8
21. LDRGT R3, [R6]
28. ADD R2, R2, #1

# Input Text
```
MOV R0,#0x14
MOV R1,#0xA
CMP R0, R1
ADDGT R2, R0, R1
ADDEQ R3, R0, R1
SUBLT R4, R0, R1
ADDS R0,R1,R2
CMP R2, #50
ANDGT R3, R2, #1
ORREQ R4, R2, #2
EORLT R5, R2, #3
ORRS R3,R2,#15
CMP R5, #0x1D
LSRGE R7, R5, #2
LSLNE R8, R5, #3
CMP R8, R9
MOVGT R10, R8
MVNEQ R11, R8
MOV R6,#0x104
CMP R7, R2
LDRGT R3, [R6]
STRNE R3, [R6]
CMP R2, R7
LDRGT R9, [R6]
STRNE R3, [R6]
CMP R7, #7
BEQ SKIP
ADD R2, R2, #1
SKIP SUB R3, R3, #1
```