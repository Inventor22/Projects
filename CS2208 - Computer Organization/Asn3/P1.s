;Name:  Dustin Dobransky
;Alias: ddobran
;Id:	250575030
;Date:	26/02/15
;
;		Description
;		This program verifies UPC codes.

		AREA first_example, CODE, READONLY
ENTRY

		ADR r0,UPC		 ; store the address of the UPC code in r0
		MOV r1,#0		 ; store 0 in r1
		
addloop	LDRB r3,[r0,r1]  ; load r3 with [r0+r1] -> UPC start address + offset
		SUB r3,#"0"      ; subtract ascii char '0' (decimal 48) from r3 char to get integer value
		AND r6,r1,#1 	 ; determine whether the number is odd of even, by bitwise AND-ing with 1
		CMP r6,#1	 	 ; Compare r6 and 1.  If == 0, number is even.  else, odd
		BEQ sum2	 	 ; If the number is odd...
sum1	ADD r4,r3        ; r4 += r3 -> add the digit to the second counter (r4)
		b doneSum		 ; branch to the next command
sum2	ADD r5,r3		 ; if the number is even, add the digit to the first counter (r3)
doneSum	ADD r1,#1		 ; increment the UPC digit index
		CMP r1,#11		 ; if we've reached the end-1 of the UPC string
		BNE addloop		 ; then continue
		
		ADD r4, r4, r4, LSL #1 ; compute r1 = r1 + r1*2 == r1*3
		ADD r4,r5        ; add first sum and second sum, store in first sum
		SUB r4,#1        ; subtract 1 from result
		
divloop	CMP r4,#10       ; if the number is less than 10, break.
		BLT done	     ; else subtract by 10 and loop
		sub r4,#10
		b divloop
done	
		
		RSB r4, r4, #9   ;  r4 = 9 - r4;
		
		LDRB r1,[r0,#11] ; load the 12th digit of the UPC code into r1
		SUB r1,#48       ; convert from char to int
		CMP r4,r1        ; compare to computed value from above (r4)
		BNE Incorr       ; if r1 != r4, branch to Incorr label
Correct	MOV r0,#1		 ; store 1 in r0, to indicate a valid UPC
		b loop	
Incorr  MOV r0,#0		 ; store 0 in r0, to indicate an invalid UPC
		
loop	b	loop
		
UPC		DCB "013800150738" ;UPC string
;UPC		DCB	"060383755577"
;UPC		DCB	"065633454712"
		
		END
			