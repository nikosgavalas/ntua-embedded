.text

	.align		4

	.global		_strlen
	.type		_strlen, %function
	.global		_strcpy
	.type		_strcpy, %function
	.global		_strcat
	.type		_strcat, %function
	.global		_strcmp
	.type		_strcmp, %function

/*
	r0:	input string
*/
_strlen:
	push	{r1-r7, lr}
	mov		r1, r0
	mov		r0, #0

	// Iterate until \0
	strlen_loop:
		ldrb	r2,	[r1], #1
		cmp		r2, #0
		beq		strlen_loop_end
		add		r0, #1
		b		strlen_loop

	strlen_loop_end:

	pop		{r1-r7, pc}

/*
	r0:	dest
	r1:	src
	returns: dest
*/
_strcpy:
	push	{r1-r7, lr}

	mov		r2, r0

	// Iterate and copy over until \0
	strcpy_loop:
		ldrb	r3, [r1], #1
		strb	r3, [r2], #1

		cmp		r3, #0
		bne		strcpy_loop

	pop		{r1-r7, pc}

/*
	r0:	dest
	r1:	src
	returns: dest
*/
_strcat:
	push	{r1-r7, lr}

	mov		r2, r0

	// Reach the \0 of dest
	reach_term:
		ldrb	r3, [r2], #1
		cmp		r3, #0
		bne		reach_term
	
	// Move one back to point to \0
	sub		r2, r2, #1

	strcat_loop:
		ldrb	r3, [r1], #1
		strb	r3, [r2], #1

		cmp		r3, #0
		bne		strcat_loop

	pop		{r1-r7, pc}

/*
	r0: first string
	r1: seconds string
*/
_strcmp:
	push	{r1-r7, lr}

	mov		r2, r0

	strcmp_loop:
		ldrb	r3, [r2], #1	// s1 char
		ldrb	r4, [r1], #1	// s2 char

		subs	r0, r3, r4

		bne		strcmp_loop_end

		// Chars are equal, check if we reached the end
		cmp		r3, #0
		bne		strcmp_loop


	strcmp_loop_end:

	pop		{r1-r7, pc}











