// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen
// by writing 'black' in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen by writing
// 'white' in every pixel;
// the screen should remain fully clear as long as no key is pressed.

//// Replace this comment with your code.
	// n takes the starting number of the screen register, i takes the number of screen registers
	// KBD
(LOOPX)
	@KBD
	D=M
	@LOOP1_ENTER
	D;JNE
	@LOOP2_ENTER
	0;JMP
// Make screen black
(LOOP1_ENTER)
	@SCREEN
	D=A
	@n
	M=D
	@8191
	D=A
	@i
	M=D
(LOOP1)
	// set 1's to register within screen
	@n
	A=M
	M=-1
	// if i == 0 go to LOOPX
	@i
	D=M
	@LOOPX
	D;JEQ
	// i-=1, n+=1;
	@n
	M=M+1
	@i
	M=M-1
	// go to LOOP
	@LOOP1
	0;JMP
// Make screen white
(LOOP2_ENTER)
	@SCREEN
	D=A
	@n
	M=D
	@8191
	D=A
	@i
	M=D
(LOOP2)
	// set 0's to register within screen
	@n
	A=M
	M=0
	// if i == 0 go to LOOPX
	@i
	D=M
	@LOOPX
	D;JEQ
	// i-=1, n+=1;
	@n
	M=M+1
	@i
	M=M-1
	// got to LOOP2
	@LOOP2
	0;JMP
(END)
	@END
	0;JMP
	
	
