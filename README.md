# Chasm
Assembler for the Chip8 assembly language. 

The Chip8 Assembly language contains the amazing amount of 29 mnemonics! These are:
#CLS 
Clears the screen
#JMP address/label
Jumps to a label or an address
#RET 
Returns from subroutine
#CALL address/label
Calls the subroutine at that address/calls subroutine with that label
#SKPE 
(Skip on equal. Skips next instruction if the register is equal to a register/number)
SKPD (Skip on different. Skips next instruction if the register is different from a register/number)
MOV (Moves a number, or the contents of another register, to the specified register)
ADD (Adds a register to a register/number and puts the result in the first register.)
OR (Logically ORs two registers and puts the result in the first register)
AND (Logically ANDs two registers and puts the result in the first register)
XOR (Logically XORs two register and puts the result in the first register)
SUB (Same as the ADD instruction but instead of adding it substracts)
SHR (Shifts right by one bit the contents of the specified register)
SHL (Shifts left by one bit the contents of the specified register)
STI (Set I. Sets the I pointer to the specified address)
JMPO (Jump offset. Jump instruction with an offset of the value stored in V0)
RND (Generates a random number and logically ANDs it with a byte)
DRW Vx, Vy, N (Draw instruction. Draws the sprite at I, with a height of N, at the coordinates stored in Vx and Vy)
SKPK (Skip on key. Skips next instruction if the key stored at the specified register is the key currently being pressed)
SKPNK (Skip on not key. Skips next instruction if the key stored at the specified register is NOT the key currently being pressed)
GDLY (Store the value of the delay coiunter in the specified register)
SDLY (Set delay timer to the value stored in the specified register)
GKEY (Waits for a key press and stores it in the specified register)
SSND (Sets the sound counter)
ADDI (Adds the value stored in the specified register to the I pointer)
BCD (Stores the BCD representation of the value stored in the specified register in I, I+1 and I+3, for the hundreds, the tens and the digits respectively)
REGD (Regdump. Dumps the contents of V0 to Vx in I to I + x. Does not modify the value of I)
REGL (Regload. Loads V0 to VX with I to I+x. Does not Modify the value of I either)

You can use the . symbol to specify the origin of a chunk of code.

The @ symbol means that you want to load a certain address with a certain byte, for example:
```
  @0055 40
```
This loads the address 0x0055 with the byte 0x40.

The $ symbol is used in address dependent instructions to specify the address:
```
  jmp $0201 //jumps to address 0x0201
```

Labels can be declared with an id followed by a colon:
  main:
    -code-
  label2:
    -more code-
  label3: but_also_label4:
    -even more code-

The current version only supports hexadecimal numbers, but i'll  add decimal and binary number support.






Chasm stands for Chip8 Assembly.
It can also signify how abbysmal this is.
