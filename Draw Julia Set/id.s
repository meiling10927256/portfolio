
      .data
msg1: .asciz  "Please Enter Member 1 ID : \n"
msg2: .asciz  "Please Enter Member 2 ID : \n"
msg3: .asciz  "Please Enter Member 3 ID : \n"
msg4: .asciz  "** Please Enter Command **\n"
msg5: .asciz  "*****Print Team Member ID and Summation*****\n"
msg6: .asciz  "%d\n"
msg7: .asciz  "ID Summation = %d\n"
msg8: .asciz  "*****End Print*****\n"
msg9: .asciz  "*****Input ID*****\n"
scan_pattern1: .asciz "%d"
scan_pattern2: .asciz "%s"
enter: .asciz "\n"
ifp: .asciz  "p"

id1: .word 0
id2: .word 0
id3: .word 0
sum: .word 0
command: .word 0

return: .word 0

     .text
     .global id
	 
     @.global notp
     @.global id1
     @.global id2
     @.global id3
     @.global sum
     @.global add

id:
      stmfd sp!, {r4-r7,lr}
      mov r4, r0         @ r4 = address of id1
	  mov r5, r1         @ r5 = address of id2
	  mov r6, r2         @ r6 = address of id3
	  mov r7, r3         @ r7 = address of sum
	  
      ldr   r0, =msg9
      bl    printf       @ print "*****Input ID*****\n"

      ldr   r0, =msg1
      bl    printf       @ print "Please Enter Member 1 ID : \n"
 
      ldr   r0, =scan_pattern1    @ "%d"
      ldr   r1, =id1
      bl    scanf        @ scanf id1


      ldr   r0,  =msg2
      bl    printf       @ print "Please Enter Member 2 ID : \n"

      ldr   r0, =scan_pattern1
      ldr   r1, =id2
      bl    scanf        @scanf id2

      ldr   r0,  =msg3
      bl    printf       @ print "Please Enter Member 3 ID : \n"

      ldr   r0, =scan_pattern1
      ldr   r1, =id3
      bl    scanf        @ scanf id3

      @add----

      ldr   r0, =id1       @ r0 = address of id1
      ldr   r1, [r0], #2   @ r1 = id1, [r0] = [r0] + 2
      ldr   r0, =id2       @ r1 = address of id2
      ldr   r2, [r0], r3   @ r1 = id2, [r1] = [r1] + r3
      addvc   r1, r1, r2   @ r1 = r1 + r2, vc: v clear ( not overflow )

      ldr   r0, =id3       @ r0 = address of id3
      ldr   r0, [r0]       @ r0 = id3

      addpl r0, r0, r1     @ r0 = r0 + r1, pl: n clear
      ldr   r1, =sum       @ r1 = address of sum
      str   r0, [r1, #0]   @ sum = r0

      @---

      ldr   r0,  =msg4
      bl    printf         @ print "Please Enter Command\n"

      ldr   r0, =scan_pattern2  @ "%s"
      ldr   r1, =command
      bl    scanf          @ scanf command

      ldr   r0, =command @ r0 = address of command
      ldrb  r0, [r0]     @ r0 = command
      ldr   r1, =ifp     @ r1 = address of ifp
      ldrb  r1, [r1]     @ r1 = ifp
      cmp   r0, r1       @ cmp r0 & ifp
      blne  notp         @ if (command != p)

      ldreq   r0,  =msg5
      bl    printf       @ print "*****Print Team Member ID and Summation*****\n"

      ldr   r0, =msg6
      ldr   r1, =id1     @ r1 = address of id1
      ldr   r1, [r1]     @ r1 = id1
      bl    printf       @ print id1

      ldr   r0, =msg6
      ldr   r1, =id2     @ r1 = address of id2
      ldr   r1, [r1]     @ r1 = id2
      bl    printf       @ print id2

      ldr   r0, =msg6
      ldr   r1, =id3     @ r1 = address of id3
      ldr   r1, [r1]     @ r1 = id3
      bl    printf       @ print id3

      ldr r0, =enter
      bl printf

      ldr   r0, =msg7
      ldr   r1, =sum   @ r1 = address of sum
      ldr   r1, [r1]   @ r1 = sum
      bl    printf     @ print "ID Summation = %d\n"

      @---

notp: ldr   r0, =msg8
      bl    printf       @ print "*****End Print*****"

      ldr r0, =id1       @ r0 = address of id1
	  ldr r0, [r0]       @ r0 = id1
	  str r0, [r4]       @ [r4] = id1
	  
      ldr r0, =id2
	  ldr r0, [r0]
	  str r0, [r5]
	  
      ldr r0, =id3
	  ldr r0, [r0]
	  str r0, [r6]
	  
	  ldr r0, =sum
	  ldr r0, [r0]
	  str r0, [r7]
	  
      ldmfd sp!, {r4-r7,lr}
      mov   pc,  lr

