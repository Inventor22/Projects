;Name:  Dustin Dobransky
;Alias: ddobran
;Id:	250575030
;Date:	26/02/15
;
;		Description
;		This program determines whether a string is a palindrome, ignoring all
;		non-alphabetic characters, including spaces and numbers.

;		Two pointers traverse the string, one at the beginning, the other at the end.
;		Pointer values are compared until either the beginning pointer is greater or
;		equal to the end pointer, or the pointed-to chars are not equal.  The appropropriate
;		flag is then set in register r0.

		AREA first_example, CODE, READONLY
ENTRY
		MOV r10, #"a"
		SUB r10, #"A"   ; calculate the difference between ascii 'a' and 'A'

		ADR r1,STRING   ; get start and end positions of string
		ADR r2,EoS
		SUB r2,#1       ; subtract 1 to get address of last char in string
						; -- Check if character is in bounds --
getL	LDRB r3,[r1],#1 ; load r3 with char pointed to by r1, increment r1
		CMP  r3,#"A"    ; compare to 'A'
		BLT	 getL       ; if less than 'A', get next char
		CMP	 r3,#"z"    ; compare to 'z'
		BGT	 getL       ; if greater than 'z', get next char
		
		CMP r3,#"Z"     ; convert lower case letters to upper case letter
		BLT getR
		sub r3,r10
		
getR	LDRB r4,[r2],#-1 ; same shit, different cow.  But go backwards from end of string
		CMP r4,#"A"
		BLT getR
		CMP r4,#"z"
		BGT getR
		
		CMP r4,#"Z"
		BLT cont
		sub r4,r10
		
cont	CMP r3,r4        ; compare the two characters
		BNE NoPal        ; If not equal, branch to NoPal
		CMP r1,r2		 ; compare char addresses.
		BGE Pal          ; if forward ptr is greater or equal to backwards ptr
		b getL			 ; then string is equal.  Continue looping otherwise
		
Pal		MOV r0,#1		 ; set palindrome status --> True
		b fin
NoPal	MOV r0,#0        ; palindrome --> False

fin		b fin

STRING	DCB "He lived as a devil, eh?" ; string
EoS		DCB 0x00		 ; end of string
		
		END
			