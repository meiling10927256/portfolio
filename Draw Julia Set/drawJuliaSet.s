
.data 

.text 
  .global	drawJuliaSet
   
drawJuliaSet:
        stmfd sp!, {r4-r11,lr}       @ backup r4-r11 & return address
		mov r11, r0                  @ r11 = cx
		mov r10, r1                  @ r10 = cy
		mov r4, r2                   @ r4 = frame
        mov r9, #0                   @ x=0
		adds r14, r0, r15            @ !!!!!!!
		
loopx : cmp r9, #640                 @ if ( x < width )
        bge finishx
        mov r8, #0                   @ y=0		

loopy : cmp r8, #480                 @ if ( y < height )
        bge finishy
		
        mov r2, #320                 @ r2 = width >> 1
        sub r7, r9, r2               @ zx = (x - (width>>1)
		ldr r12, .constant           @ r12 = 1500
		mul r7, r7, r12              @ zx = zx * 1500 = 1500 * (x - (width>>1)
		mov r0, r7                   @ r0 = 1500 * (x - (width>>1)
		mov r1, #320                 @ r1 = (width>>1)
		bl	__aeabi_idiv             @ r0 = 1500 * (x - (width>>1) / (width>>1)
		mov r7, r0                   @ zx = 1500 * (x - (width>>1) / (width>>1)
		mov r2, #240                 @ r2 = height >> 1
		sub r6, r8, r2               @ zy = (y - (height>>1)
		mov r12,#1000                @ r12 = 1000
		mul r6, r6, r12              @ zy = zy * 1000 = 1000 * (y - (height>>1))
		mov r0, r6                   @ r0 = 1000 * (y - (height>>1) 
		mov r1, #240                 @ r1 = (height>>1
		bl	__aeabi_idiv             @ r0 = 1000 * (y - (height>>1) / (height>>1)
		mov r6, r0                   @ zy = 1000 * (y - (height>>1) / (height>>1)
        mov r5, #255                 @ i = maxIter
		
while : mul r0, r7, r7               @ r0 = zx * zx
		mul r1, r6, r6               @ r1 = zy * zy
		add r0, r0, r1               @ r0 = zx * zx + zy * zy
		ldr r1, .constant+4         @ r1 = 4000000
        cmp r0, r1                   @ if ( zx * zx + zy * zy < 4000000 )
		bge finishwhile
		cmp r5, #0                   @ if ( i > 0 )	
        ble finishwhile		
		
        mul r1, r6, r6               @ r1 = zy * zy
		mul r0, r7, r7               @ r0 = zx * zx
        sub r0, r0, r1               @ r0 = (zx * zx - zy * zy ) 
        mov r1, #1000                @ r1 = 1000
        bl	__aeabi_idiv             @ r0 = (zx * zx - zy * zy) / 1000
        add r3, r0, r11              @ r3 = tmp = (zx * zx - zy * zy) / 1000 + cx
        mov r2, r7, lsl #1           @ r2 = zx * 2
		mov r7, r3                   @ zx = tmp
		mul r0, r2, r6               @ r0 = 2 * zx * zy 
		mov r1, #1000                @ r1 = 1000
		bl	__aeabi_idiv             @ r0 = 2 * zx * zy / 1000
		add r6, r0, r10              @ zy = 2 * zx * zy / 1000 + cy

		sub r5, r5, #1               @ i--
		b  while
		
finishwhile:    
        and r0, r5, #0xff            @ r0 = i & 0xff
        mov r1, r0, lsl #8           @ r1 = (i & 0xff) << 8
        orr r1, r1, r0               @ r1 = color = (i & 0xff) << 8	| (i & 0xff) 
		ldr r0, .constant+8          @ r0 = 0xffff
        bic r2, r0, r1               @ r2 = color = (~color)&0xffff 
		
		mov r0, r4                   @ r0 = frame
		mov r1, #1280                @ r1 = 640 * 2 byte 
		mul r1, r1, r8               @ r1 = 1280*y
		add r0, r0, r1               @ r0 = frame + 1280*y 
		add r0, r0, r9, lsl #1       @ r0 = frame + 1280*y + 2x
		
		strh r2, [r0]
		
		add r8, r8, #1               @ y++
        b loopy
		
finishy : 
        add r9, r9, #1               @ x++
        b loopx 
		
finishx :
        addvs   r0, r1, r2, lsl #0
        subne   r0, r1, r2, lsr r1
        suble   r0, r1, #0
		
        ldmfd sp!, {r4-r11, lr}
		mov pc, lr
		
@------
.constant : 
    .word 1500     @.constant+0
	.word 4000000  @.constant+4
	.word 0xffff   @.constant+8
	