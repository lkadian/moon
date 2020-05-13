% --------------------------------------------------------------%
% Start of function definition: factorialinteger
% --------------------------------------------------------------%
      factorialinteger sw -4(r14), r15
% storing intnum 1
      addi r2, r0, 1
      sw -12(r14), r2

% n == lit1
      lw r3, -24(r14)
      lw r4, -12(r14)
      ceq r5, r3, r4
      sw -28(r14), r5

% if
      lw r1, -28(r14)
      bz r1, else1
% then
% storing intnum 1
      addi r1, r0, 1
      sw -16(r14), r1

% return
      lw r1, -16(r14)
      sw 0(r14),r1
      j endif1
% else
      else1
% storing intnum 1
      addi r1, r0, 1
      sw -20(r14), r1

% n - lit3
      lw r2, -24(r14)
      lw r3, -20(r14)
      sub r4, r2, r3
      sw -32(r14), r4

% Function call to factorialinteger
      lw r1, -32(r14)
      sw -64(r14), r1
      addi r14, r14, -40
      jl r15, factorialinteger
      subi r14, r14, -40
      lw r1, -40(r14)
      sw -36(r14), r1
% n * temp3
      lw r2, -24(r14)
      lw r3, -36(r14)
      mul r4, r2, r3
      sw -40(r14), r4

% return
      lw r1, -40(r14)
      sw 0(r14),r1
      endif1
      lw r15, -4(r14)
      jr r15
% --------------------------------------------------------------%
% End of function definition: factorialinteger
% --------------------------------------------------------------%
% --------------------------------------------------------------%
% Start of function definition: wrapperinteger
% --------------------------------------------------------------%
      wrapperinteger sw -4(r14), r15
% Function call to factorialinteger
      lw r1, -12(r14)
      sw -40(r14), r1
      addi r14, r14, -16
      jl r15, factorialinteger
      subi r14, r14, -16
      lw r1, -16(r14)
      sw -16(r14), r1
% return
      lw r1, -16(r14)
      sw 0(r14),r1
      lw r15, -4(r14)
      jr r15
% --------------------------------------------------------------%
% End of function definition: wrapperinteger
% --------------------------------------------------------------%
% --------------------------------------------------------------%
% Start of main
% --------------------------------------------------------------%
      entry
      addi r14, r0, topaddr

% storing intnum 5
      addi r1, r0, 5
      sw -4(r14), r1

% Function call to wrapperinteger
      lw r1, -4(r14)
      sw -24(r14), r1
      addi r14, r14, -12
      jl r15, wrapperinteger
      subi r14, r14, -12
      lw r1, -12(r14)
      sw -8(r14), r1
% x = temp6
      lw r1, -8(r14)
      sw -12(r14), r1

% writing to stdout
      lw r1, -12(r14)
      jl r15, putint

      hlt
% --------------------------------------------------------------%
% End of main
% --------------------------------------------------------------%
%
%
% --------------------------------------------------------------%
% putint                                                        %
% --------------------------------------------------------------%
% Write an integer number to stdout. Transform the number into
% ASCII string taking the sign into account.
% Entry : integer number -> r1
% Exit : none.
%
putint	align
	add	r2,r0,r0		% Initialize buffer's index i 
	cge	r3,r1,r0		% True if N >= 0
	bnz	r3,putint1		% Branch if True (N >= 0)
	sub	r1,r0,r1		% N = -N
putint1	modi	r4,r1,10		% Rightmost digit
	addi	r4,r4,48		% Convert to ch
	divi	r1,r1,10		% Truncate rightmost digit
	sb	putint9(r2),r4		% Store ch in buffer
	addi	r2,r2,1			% i++
	bnz	r1,putint1		% Loop if not finished
	bnz	r3,putint2		% Branch if True (N >= 0)
	addi	r3,r0,45
	sb	putint9(r2),r3		% Store '-' in buffer
	addi	r2,r2,1			% i++
	add	r1,r0,r0		% Initialize output register (r1 = 0)
putint2	subi	r2,r2,1			% i--
	lb	r1,putint9(r2)		% Load ch from buffer
	putc	r1			% Output ch
	bnz	r2,putint2		% Loop if not finished
	addi r2,r0,10     % print a newline
	putc r2           % print a newline
	jr	r15			% return to the caller
putint9	res	12			% loacl buffer (12 bytes)
	align

