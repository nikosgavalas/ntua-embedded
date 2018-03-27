.text

	.global 	main

	.include 	"lib.s"
	.extern 	printf
	.extern		scanf
	b			main


// Funcs


/*
 	r0: buffer addr
	r1: length
*/
clear_buffer:
	push	{r0-r7, lr}
	mov		r2, #0
	clear_loop:
		cmp		r1, #0
		beq		end_clear
		strb	r2, [r0], #1
		sub		r1, #1
		b		clear_loop
	end_clear:

	pop		{r0-r7, pc}

/*
	r0: string addr
*/
transform:
	push 	{r0-r7, lr}

	parsing:

		ldrb	r2, [r0]

		cmp		r2, #65
		blt		under_upper
		cmp		r2, #90
		bgt		above_upper

			// Is upper case
			add		r2, r2, #32
			b		final

		above_upper:

			cmp		r2, #97
			blt		final
			cmp		r2, #122
			bgt		final

			// Is lower case
			sub		r2, #32

			b 		final

		under_upper:

			// Check if number
			cmp		r2, #48
			blt		final
			cmp		r2, #57
			bgt		final

			// Is number

			add		r2, #5
			cmp		r2, #57
			subgt	r2, #10

		final:

		strb	r2, [r0], #1

		cmp		r0, r1
		bne		parsing

	// Strap a terminator at the end
	mov		r2, #0
	strb	r2, [r0]
last:
	pop		{r0-r7, pc}


main:

main_loop:

	// Print prompt
	ldr		r0, =output_str
	bl		printf

	// Read input until newline
	ldr		r0, =format
	ldr		r1, =input_str
	bl		scanf

	// Trash the newline from the buffer
	ldr		r0, =format2
	ldr		r1, =tmp
	bl		scanf
	

	ldr		r0, =input_str
	ldr		r1, =input_end


	ldrb	r2, [r0] 		// first char
	ldrb	r3, [r0, #1]	// second char

	// Check if we only read a 'Q'
	cmp		r3, #0
	bne		do
	ldrb	r2,[r0]
	cmp		r2,#113
	beq		end
	cmp		r2,#81
	beq		end

do:
	// Transform the string
	ldr		r0, =input_str
	ldr		r1, =input_end
	bl		transform

	// Print the transformed string
	ldr		r0, =input_str
	bl		printf

	// Print a newline
	ldr		r0, =newline
	bl		printf

	// Fill the buffer with \0 to prepare for the next iteration
	ldr		r0, =input_str
	mov		r1, #8
	bl		clear_buffer

	b		main_loop

end:

_exit #0


.data
	newline: 	.ascii "\n\0"
	format:		.string "%[^\n]"
	format2:	.string "%c"

	output_str: .ascii "Please input a string 32 char long: \0"
	output_len 	= . - output_str

	input_str: .ascii "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
	input_end	= .

	tmp:		.ascii "\0"



