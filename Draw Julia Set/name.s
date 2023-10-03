    .data
	.equ s_name1, 0
	.equ s_name2, 15
issue_start: .asciz "*****Print Name*****\n"
team:  .asciz "Team 32"
name1: .asciz "Kaiyu Wu"
name2: .asciz "Pinyi Pu"
name3: .asciz "Meiling Jiang"
enter: .asciz "\n"
issue_end: .asciz "*****End Print*****\n"

    .text
    .global name
    @.global team
    @.global name1
    @.global name2
    @.global name3

name:
      stmfd sp!, {r4-r7, lr}
      mov r4, r0                 @ r4 = address of name1
	  mov r5, r1                 @ r5 = address of name2
	  mov r6, r2                 @ r6 = address of name3
	  mov r7, r3                 @ r7 = address of team
	  
      movhs r3, sp               @ backup sp, r3 = sp

                                 @ reset cpsr
      mrs   r0, cpsr             @ read the cpsr into r0
      bic   r0, r0, #0xf0000000  @ clear all of the flags
      msr   cpsr_f, r0           @ write the flags field to cpsr

      mov   r0, sp               @ r0 = sp
      adcs  r13, r1, r2          @ r13 = r1 + r2 + c
      mov   sp, r3               @ sp = r3

      add   r0, r1, r2, lsl #0
      sub   r0, r1, r2, lsr r1
      sub   r0, r1, #0

      ldr   r0,  =issue_start
      bl    printf               @ print "*****Print Name*****\n"

      ldr   r0,  =team
      bl    printf               @ print "Team 32"
      ldr r0, =enter
      bl printf
	  
      ldr   r0,  =name1
      bl    printf               @ print name1
      ldr r0, =enter
      bl printf
	  
      ldr   r0,  =name2
      bl    printf               @ print name2
      ldr r0, =enter
      bl printf
	  
      ldr   r0,  =name3
      bl    printf               @ print name3
      ldr r0, =enter
      bl printf
	  
      ldr   r0,  =issue_end
      bl    printf               @ print "*****End Print*****\n"
	  
	  mov r0, r4
	  ldr r1, =name1
	  bl strcpy
	  
	  mov r0, r5
	  ldr r1, =name2
	  bl strcpy

	  mov r0, r6
	  ldr r1, =name3
	  bl strcpy
	  
	  mov r0, r7
	  ldr r1, =team
	  bl strcpy
	  
      ldmfd sp!, {r4-r7, lr}
      mov   pc,  lr
