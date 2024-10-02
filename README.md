# Chasm

Chasm is an assembler for the Chip8 assembly language. Chasm stands for Chip8 assembly. Sometimes it also stands for how abysmal the code is. Before looking at the instruction set, I'd like to make some things clear:
- Vx and Vy are used to represent any register from V0 to Vf.
- Num is used to indicate an immediate value.
- Whenever you see label/address or Vy/Num that just means that either one of those can be used, not at the same time tho. 

# Instruction Mnemonics

**CLS** Clear screen. <br/>
**RET** Return from subroutine.<br/>
**JMP label/address** Jumps to the specified label or address.<br/>
**CALL label/address** Calls the specified subroutine with that label/address.<br/>
**SKPE Vx, Vy/Num** Compares register Vx to register Vy/num and skips the next instruction if equal.<br/>
**SKPD Vx, Vy/Num** Like SKPE but skips the next instrucion on different.<br/>
**MOV Vx, VY/Num** Moves the contents of Vy/the value num into the register Vx.<br/>
**ADD Vx, Vy/Num** Adds the contents of Vy/the value of Num to Vx, and stores the result in Vx. Sets Vf to 1 if there is an overflow.<br/>
**OR Vx, Vy** Logically ORs the contents of Vx with the contents of Vy, and stores the result in Vx.<br/>
**AND Vx, Vy** Logically ANDs the contents of Vx with the contents of Vy, and stores the result in Vx.<br/>
**XOR Vx, Vy** Logically XORs the contents of Vx with the contents of Vy, and stores the result in Vx.<br/>
**SUB Vx, Vy** Substracts the contents of Vy from Vx (Vx - Vy), and stores the result in Vx. Sets Vf to 1 if there is an underflow.<br/>
**SHR Vx** Shifts the contents of Vx to the right by one bit, and stores the result in Vx. Stores the most important bit previous to shifting in Vf.<br/>
**SHL Vx** Shifts the contents of Vx to the left by one bit, and stores the result in Vx. Stores the least important bit previous to shifting in Vf.<br/>
**STI address** Sets the I pointer to the specified address.<br/>
**JMPO label/address** Jumps to the specified label/address with an offset of V0.<br/>
**RND Vx, Num** Logically ANDs a randomly generated number with Num, and stores the result in Vx.<br/>
**DRW Vx, Vy, N** Draws the sprite (with a height of N) pointed to by the I pointer, at the position (Vx, Vy).<br/>
**SKPK Vx** Skips the next instruction if the key currently being pressed is the same as the key stored in Vx.<br/>
**SKPNK Vx** Skips the next instruction if the key currently being pressed is different from the key stored in Vx.<br/>
**GDLY Vx** Stores the value of the Delay Timer in the Vx register.<br/>
**SDLY Vx** Sets the Delay Timer to the value stored in the Vx register.<br/>
**GKEY Vx** Waits for a key press and stores the key in the Vx register.<br/>
**SSND Vx** Sets the Sound Timer to the value stored in the Vx register.<br/>
**ADDI Vx** Increments the pointer I by the value stored in Vx.<br/>
**BCD Vx** Dumps the BCD representation to the address pointed to by the I pointer. The hundreds digit get stored in I, the tens digit gets stored in (I+1) and the ones digit get stored in (I+2).<br/>
**REGL Vx** Loads the registers V0-Vx with the contents pointed to by the I pointer, increasing I by 1 for each register written.<br/>
**REGD Vx** Dumps the contents of the V0-Vx registers starting at the address pointed to by the I pointer, increasing I by 1 for each address written.<br/>

## Labels and addresses

A label is declared in the standard assembly way; you write the name for the label followed by a colon.
```
label1:
	//some code
	mov V0, 1
	add V0, 4
```
A block of code can also have several labels:
```
label1:label1_1:label1_1_1:
	//some more code
	mov v0, 0
	mov v1, v0
	drw v0, v1, 5
```
To indicate an address, the address number must be preceded by a $:
```
main:
	mov v0, 0
	rnd v1, 7
	jmp $0055
```
To indicate the origin of a block of code the address must be preceded by a .:
```
.0300
ADD V0, V1
SHR V0
```
To write directly to an address you can use the @ symbol followed by the address and the byte that you want to write:
```
@0055 55
@0056 56
@0057 57
```
This is most probably not very safe.
Every single explicitly written address will be treated literally. This means that no offset will be applied to them.
Labels have a 0x200 offset applied to them. 
	 
## Things that will be implemented
- Currently Chasm only supports hex numbers, so it interprets every single number written as hex. Support for binary and decimal numbers will be added.

- Chasm also assumes that the Chip8 implementation uses a 4KB memory, and cannot write to anything beyond the address 0x0fff. Support for Chip8s with larger memory will be added.
- The Vx = Vy - Vx instruction has not been added yet. Support for this instruction will be added.<br/>
Chasm is a work in progress and my first "compiler" so any feedback is welcome! And thanks for checking it out.

