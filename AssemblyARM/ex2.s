.text
    .global	main
    .extern scanf
    .extern printf
    .extern strlen
    .extern	getchar
    .extern sprintf


@ r0 => address of buffer
@ r1 => length of the buffer
@ writes nulls to the specified buffer
reset_buff:
    push	{r0-r2, lr}

    add		r1, r1, r0
    mov		r2, #0
reset_loop:						/* loop through the buffer r1 times */
    strb	r2, [r0], #1		/* and set nulls */
    cmp		r0, r1
    bne		reset_loop

    pop		{r0-r2, pc}


@ r0 => char
@ counts occurences and sets spaces
count_occurences:
    push	{r1-r5, lr}
    
    ldr		r1, =input
    mov		r3, #0
    mov		r4, #32				/* ascii for space */
count_loop:
    ldrb	r2, [r1], #1		/* load a character from the input buffer */
    cmp		r2, #0				/* if null, leave */
    beq		return
    cmp		r2, #32				/* if space or less, ignore */
    ble		count_loop
    cmp		r2, r0				/* if the char is the one we are looking for */
    addeq	r3, r3, #1			/* if it is, increment the occurence counter */
    streqb	r4, [r1, #-1]		/* and set a space to its position */

    b		count_loop

return:
    mov		r0, r3
    pop		{r1-r5, pc}


@ this is the function we want to optimize (be as short as possible)
@ counts the characters of the input buff into the key and value buffs
parse:
    push	{r0-r4, lr}

    ldr		r0, =key
    ldr		r1, =value
    ldr		r2, =input
parse_loop:
    ldrb	r3, [r2], #1		/* grab a character */
    cmp		r3, #0
    beq		ret					/* if null, leave */
    cmp		r3, #32
    ble		parse_loop			/* if less/equal than/to ascii 32, skip */
    strb	r3, [r0], #1		/* store to buffer with keys */

    push	{r0}
    mov		r0, r3
    bl		count_occurences	/* find how many times the same character is found in the string */
    strb	r0, [r1], #1		/* save this value to the 'value' buffer */
    pop		{r0}

    b		parse_loop

ret:
    pop		{r0-r4, pc}


@ r1 => the address of the 6byte buffer to be written in the file
@ r4 holds the fd globaly
write_buff:
    push	{r0-r3, lr}

    mov		r0, r4			/* r4 holds the fd from before */
    mov		r2, #6			/* write 6 bytes */
    mov		r7, #4			/* sys_write */
    swi		0

    pop		{r0-r3, pc}


@ r2 => character ascii value
@ r3 => counter value
@ formats a line and writes it to the file
format_and_write:
    push	{r0-r1, lr}	

    ldr		r0, =outbuff
    ldr		r1, =format2
    bl		sprintf			/* sprintf formats the outbuff properly */

    ldr		r1, =outbuff	/* writes a line */
    bl		write_buff

    //ldr		r0, =outbuff	/* Uncomment these two lines to be verbose */
    //bl		printf

    pop		{r0-r1, pc}


@ print the "count" buffer to the outfile with proper format
print_counter:
    push	{r0-r7, lr}

    ldr		r0, =key
    ldr		r1, =value
print_loop:
    ldrb	r2, [r0], #1
b3:
    cmp		r2, #0
    beq		done

    ldrb	r3, [r1], #1

    bl		format_and_write

    b		print_loop

done:
    pop		{r0-r7, pc}


@ MAIN
main:
    push	{ip, lr}

    ldr		r0, =filename	/* open the file for writing */
    ldr		r1, =0x241		/* O_WRONLY | O_CREAT | O_TRUNC*/
    ldr		r2, =0x1c0		/* S_IRWXU */
    mov		r7, #5
    swi		0

    mov		r4, r0			/* save the file descriptor to r4 */

program_loop:

    ldr		r0, =key		/* memset to zero the key buffer */
    mov		r1, #32
    bl		reset_buff

    ldr		r0, =value		/* memset to zero the value buffer */
    mov		r1, #32
    bl		reset_buff

    ldr		r0, =prompt		/* print prompt message */
    bl		printf

    ldr		r0, =format		/* get input string */
    ldr		r1,	=input
    bl		scanf
    
    bl		getchar			/* trash out the trailing newline */

    ldr		r0, =input		/* place a null terminator at the 33th byte */
    mov		r2, #0			/* of the input buffer, in case that the  */
    strb	r2, [r0, #32]	/* user has entered more than 32 bytes */

    ldr		r0, =input		/* check the length of the input string */
    bl		strlen
    cmp		r0, #1			/* if one character entered, check if 'Q'||'q' */
    beq		possible_quit
    cmp		r0, #0			/* if no characters entered, loop back */
    beq		program_loop
nah_its_good:

    bl		parse			/* input string is then parsed by the parse function */

    bl		print_counter	/* print results */

    ldr		r1, =separator	/* print the separator */
    bl		write_buff

    b		program_loop

possible_quit:
    ldr		r0, =input
    ldrb	r0, [r0]
    cmp		r0, #113
    beq		end				/* 'Q' entered, goes to end */
    cmp		r0, #81
    bne		nah_its_good	/* neither 'Q' nor 'q' entered, jumps back */

end:						/* 'q' or 'Q' entered, closing file and returning 0 */
    mov		r0, r4			/* get the file descriptor */
    mov		r7, #6			/* sys_close */
    swi		0

    mov		r0, #0
    pop		{ip, pc}


.data
    prompt:		.asciz	"\nPlease input a string of up to 32 chars long: "
    format:		.asciz	"%[^\n]"
    format2:	.asciz	"%c->%d\n"

    filename:	.asciz	"count.out"

    separator:	.asciz	"    \n"

    outbuff:	.asciz	"\0\0\0\0\0\0"

    key:		.asciz	"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    value:		.asciz	"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    input:		.asciz	"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    
