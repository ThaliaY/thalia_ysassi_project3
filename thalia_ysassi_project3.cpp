// Author: Thalia Ysassi
// Date: 4/23/25
// Function: Replicate an ALU with C++. Includes error messages for invalid cases. 

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath> 
using namespace std;

// Status Flags for operations
struct flags {
    int N_flag = 0;
    int Z_flag = 0;
    int C_flag = 0;
    int V_flag = 0;
};

// Function Prototypes
bool checkOpCondition(const string& opCondition, flags& statusFlags);
bool validOperation(string& operation, uint32_t& numCount,  bool& affectFlags, bool& branch, flags& statusFlags);
bool getNumbers(uint32_t* registers, const string& operation, const vector<string>& numbers, uint32_t& num1, uint32_t& num2, uint32_t& num3, bool& isReg);
void computeOperation(const string& operation, uint32_t& num1, uint32_t& num2, uint32_t& num3, flags& statusFlags, uint32_t* registers, uint32_t* memory, bool& isReg, bool& affectFlags);
void computeBranch(bool& skipToBranch, const string& operation, flags& statusFlags, const string& branchLabel);
void updateFlags(const string& operation, uint32_t unsigned_result, int32_t signed_result, flags& statusFlags);
void printResults(uint32_t* registers, uint32_t* memory, flags& statusFlags);

int main () {
    uint32_t memory[5] = {0x0, 0x0, 0x0, 0x0, 0x0};  // initalize memory array to 0
    uint32_t registers[12] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};  // initialize register array to 0
    uint32_t num1, num2, num3 = 0;  // stores first, second, and third valid number
    uint32_t numCount = 0;  //stores amount of numbers needed
    vector<string> fileLines;  // stores each individual line
    string branchLabel;  // stores branch label to compare until branch is hit
    flags statusFlags;  // Stores the values for the N,Z,C, and V flags
    bool isReg = false;  // Used to see if final value is a register number or immediate value
    bool affectFlags = false;  // used to see if 'S' oppcode has been added to the operation
    bool branch = false;  // used to skip to compute branch function
    bool skipToBranch = false;  // used to skip file lines until the branch is found
    
    ifstream fin("pp3_input.txt");  // input file
    
    if (!fin.eof()) {
        string line;  // stores individual line before stored into fileLines
        // read file input
        while (getline(fin, line)) {
            fileLines.push_back(line);
        }
    }
        fin.close();
    
        cout << "--- RESULTS ---" << endl;
        cout << endl;

        for (int i = 0; i < fileLines.size(); i++) {
            stringstream ss(fileLines[i]);  // Current line in file
            string operation;  // stores the valid operation
            string number;  // stores the next number in fileLine
            vector<string> numbers;  // stores the string of numbers
            ss >> operation;  // store current operation

            cout << fileLines[i] << endl;  // print current fileLine

            if (skipToBranch && branchLabel == operation) {  // Compares branchLabel to operation
                skipToBranch = false;  // disables bool so that the function will process normally
                ss >> operation;  // Gets the true operation as the original operation was the branch
            }
            
            if (!skipToBranch) {  // If there is no branch or if the branch was found, proceed to process line
            if (validOperation(operation, numCount, affectFlags, branch, statusFlags)) {  // checks if operation is valid
                ss.ignore();  // ignore extra whitespace char
                if (!branch) {
                    while (getline(ss, number, ',')) {  // get each number
                        if (ss.peek() == ' ') {  // ignores whitespace between commas
                            ss.ignore();
                        }
                        numbers.push_back(number);  // add to number list
                    }
                    if (getNumbers(registers, operation, numbers, num1, num2, num3, isReg)) {  // get and valiadate numbers
                        computeOperation(operation, num1, num2, num3, statusFlags, registers, memory, isReg, affectFlags);  // If all numbers are valid, compute the final operation
                    }
                } else {
                    getline(ss, branchLabel);  // if operation is a branch, store branch name into branch label
                    computeBranch(skipToBranch, operation, statusFlags, branchLabel);  // validates branch
                }
            }
            printResults(registers, memory, statusFlags);
            cout << endl;
        }
        }
        return 0;
    }
    
    
    //// Helper Functions Definitions ////
    bool checkOpCondition(const string& opCondition, flags& statusFlags) {
        bool valid = false;
        if (opCondition == "GT" && statusFlags.Z_flag == 0 && statusFlags.N_flag == statusFlags.V_flag) {
            valid = true;
        } else if (opCondition == "GE" && statusFlags.N_flag == statusFlags.V_flag) {
            valid = true;
        } else if (opCondition == "LT" && statusFlags.N_flag != statusFlags.V_flag) {
            valid = true;
        } else if (opCondition == "LE" && statusFlags.Z_flag == 1 && statusFlags.N_flag == statusFlags.V_flag) {
            valid = true;
        } else if (opCondition == "EQ" && statusFlags.Z_flag == 1) {
            valid = true;
        } else if (opCondition == "NE" && statusFlags.Z_flag == 0) {
            valid = true;
        }
        return valid;
    }

    bool validOperation(string& operation, uint32_t& numCount, bool& affectFlags, bool& branch, flags& statusFlags) {
        string opCondition;  // Stores extra condition if size == 5
        bool valid = false;  // Used to check if operation is valid
        affectFlags = false;  // enabled if 'S' char is found in operation
        branch = false;  // enabled if operation is BNE, BEQ, BAL

        if (operation.size() == 5) { // check if operation size is 5
            opCondition = operation.substr(3,2);  // split operation into operation and into opCondition
            if (checkOpCondition(opCondition, statusFlags)) {
                operation = operation.substr(0,3);  // Get original operation
            }
        } else if ((operation.size() == 4 && operation[3] == 'S') || operation == "CMP") {
            affectFlags = true;  // enabled to call updateFlags() function
            operation = operation.substr(0,3);  // get original operation
        }
        
        if (operation.size() == 3) {
            if (operation == "ADD" || operation == "SUB" || operation == "AND" ||operation == "ORR" 
                || operation == "EOR" || operation == "LSR" || operation == "LSL") {
                valid = true;
                numCount = 3;
            } else if (operation == "MOV" || operation == "MVN" || operation == "CMP" || operation == "LDR" || operation == "STR") {
                valid = true;
                numCount = 2;
            } else if (operation == "BNE" || operation == "BEQ" || operation == "BAL") {
                valid = true;
                numCount = 1;
                branch = true;  // Used in main to call computeBranch() function
            } else {
                cout << "Unsupported Operation" << endl;
            }
    }   
        return valid;
    }
    
    bool getNumbers(uint32_t* registers, const string& operation, const vector<string>& numbers, uint32_t& num1, uint32_t& num2, uint32_t& num3, bool& isReg) {
        isReg = false;  // Reset to false when checking each line
        bool valid = true; // Used to check if all numbers are valid
        stringstream ss;

        if (operation == "ADD" || operation == "SUB" || operation == "AND" ||operation == "ORR" 
            || operation == "EOR" || operation == "LSL" || operation == "LSR") {
            if (numbers.size() == 3) {
                ss << numbers [0];
                if (ss.get() == 'R') {
                    ss >> hex >> num1;
                    ss.clear();

                    ss << numbers[1];
                    if (ss.get() == 'R') {
                        ss >> hex >> num2;  // According to input file, will always be an immediate value
                        ss.clear();

                        ss << numbers[2];
                        if (ss.get() == 'R') {
                            ss >> hex >> num3;  // According to input file, will always be an immediate value
                            isReg = true;
                        } else {
                            ss >> hex >> num3;
                        }
                    } else {
                        cout << "Invalid Instruction." << endl;
                        valid = false;
                    }
                }
            } else {
                cout << "Invalid Instruction. Invalid Operand Count." << endl;
                valid = false;
            }
         } else if (operation == "MOV" || operation == "MVN" || operation == "CMP" || operation == "LDR" || operation == "STR") {
                if (numbers.size() == 2) {  // 2 numbers for binary or 1 number and 1 numShift for LSL & LSR
                    ss << numbers [0];
                    if (ss.get() == 'R') {
                        ss >> num1;
                        ss.clear();

                        ss << numbers[1];
                        if (ss.peek() == '#') {
                            ss.ignore();
                            isReg = true;  // sets as true because it is the memory index and not the value
                            ss >> hex >> num2;  // According to input file, will always be an immediate value
                        }
                        if (ss.peek() == 'R') {
                            ss.ignore();
                            ss >> num2;
                            num2 = registers[num2];
                        }
                        if (ss.peek() == '[') {
                            ss.ignore();
                            ss.ignore();
                            ss >> num2;
                            num2 = registers[num2];
                        }
                    
                        if (operation == "LDR" || operation == "STR")  {
                            if (num2 > 0x110 || num2 < 0x100) {  // checks if memory is valid
                                cout << "Invalid Instruction. Memory out-of-range." << endl;
                                valid = false;
                            }
                        }
                    }
                }
            }
        return valid;
}

    
    void computeOperation(const string& operation, uint32_t& num1, uint32_t& num2, 
         uint32_t& num3, flags& statusFlags, uint32_t* registers, uint32_t* memory, bool& isReg, bool& affectFlags) {
            uint32_t unsigned_result = 0;
            int32_t signed_result = 0;  

            if (operation != "MOV" && operation != "MVN" && operation != "CMP" && operation != "LDR" && operation != "STR") {
                num2 = registers[num2];  // changes num2 to value at register[num2] instead of the index
                if (isReg) {
                    num3 = registers[num3];  // changes num3 to value at register[num3] instead of the index
                }
            } else if (operation == "LDR" || operation == "STR") {
                num2 = ((num2 - 0x100) / 4);  // get the correct index for the memory array
            }

        // Binary Operations
        if (operation == "ADD") {
            registers[num1] = num2 + num3;
            signed_result = num2 + num3;
        } else if (operation == "SUB") {
            registers[num1] = num2 - num3;
            signed_result = num2 - num3;
        } else if (operation == "AND") {
            registers[num1] = num2 & num3;
            signed_result = num2 & num3;
        } else if (operation == "ORR") {
            registers[num1] = num2 | num3;
            signed_result = num2 | num3;
        } else if (operation == "EOR") {
            registers[num1] = num2 ^ num3;
            signed_result = num2 ^ num3;
        } else if (operation == "MOV" ) {
            registers[num1] = num2;
            signed_result = num2;
        } else if (operation == "MVN") {
            registers[num1] = ~num2;
            signed_result = ~num2;
        } else if (operation == "CMP") {
            unsigned_result = registers[num1] - num2;
            signed_result = registers[num1] - num2;
        } else if (operation == "LDR") {
            registers[num1] = memory[num2];
        } else if (operation == "STR") {
            memory[num2] = registers[num1];
        } else if (operation == "LSL") {
            registers[num1] = num2 << num3;
        } else if (operation == "LSR") {
            registers[num1] = num2 >> num3;
        }
    

        if (affectFlags) {  // Change the staus flags if S opcode was attached
            if (operation != "CMP") {
                unsigned_result = registers[num1];
            }
            updateFlags(operation, unsigned_result, signed_result, statusFlags);
        }
    }

    void computeBranch(bool& skipToBranch, const string& operation, flags& statusFlags, const string& branchLabel) {
        skipToBranch = false;
        if (operation == "BNE") {
            if (statusFlags.Z_flag == 0) {
                skipToBranch = true;
            }
        } else if (operation == "BEQ") {
            if (statusFlags.Z_flag == 1) {
                skipToBranch = true;
            }
        } else if (operation == "BAL") {
            skipToBranch = true;
        }
    }

    void updateFlags(const string& operation, uint32_t unsigned_result, int32_t signed_result, flags& statusFlags) {
        // N and Z are affected by all operations
        statusFlags.N_flag = (signed_result < 0) ? 1 : 0;
        statusFlags.Z_flag = (unsigned_result == 0) ? 1 : 0;
        statusFlags.V_flag = (signed_result < (pow(2, 31) - 1) || signed_result > pow(2, -31)) ? 0 : 1;

        if (operation == "CMP" || operation == "SUB") {
            statusFlags.C_flag = (statusFlags.N_flag == 1) ? 0 : 1;
        } else if (operation == "ADD") {
            statusFlags.C_flag = (statusFlags.V_flag == 0) ? 0 : 1;

        } else if (operation == "LSL" || operation =="LSR") {
            if (signed_result < 0) {
                statusFlags.C_flag = 0;
            } else {
                statusFlags.C_flag = 1;
            }
        }
    }

    void printResults(uint32_t* registers, uint32_t* memory, flags& statusFlags) {
        // print final results
        cout << "Register array: " << endl;  // print registers
        
        for (int i = 0; i < 12; i++) {
            cout << "R" << dec << i << "=0x" << hex << registers[i] << " ";
            if (i == 5) {
                cout << endl;
            }
        }
        cout << endl;

        cout << "NZCV: " << statusFlags.N_flag << statusFlags.Z_flag << statusFlags.C_flag << statusFlags.V_flag << endl;
        cout << "Memory array: " << endl;  // print memory
        for (int i = 0; i < 5; i++) {
            if (memory[i] == 0) {
                cout << "___";
            } else {
                cout << "0x" << hex << memory[i];
            }
            
            if (i != 4) {
                cout << ",";
            }
        }
        cout << endl;
    }