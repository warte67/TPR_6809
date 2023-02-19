/* Math.h ***************************************
 *
 *   Implements IEEE 754 floating point math processing.
 *		Both half and single precision should be implemented.
 *		Single precision comes from standard C++, but
 *		Half precision comes from the 16-bit "Half-precision
 *		floating-point library" by Christian Rau under the
 *		MIT liscense. (https://half.sourceforge.net/index.html)
 *
 * Copyright (C) 2023 by Jay Faries
 ************************************/

#pragma once
#ifndef __MATH_H__
#define __MATH_H__


class Bus;
class Memory;


class Math : public REG
{
	friend class Bus;
	friend class Memory;

public:

	Math();
	Math(Word offset, Word size);
	virtual ~Math();

	// static members
	static Byte OnCallback(REG* reg, Word ofs, Byte data, bool bWasRead);
	static Math* Assign_Math(MemoryMap* memmap, DWord& offset);
	static Word MapDevice(MemoryMap* memmap, Word offset);

	// abstract members
	virtual void OnInitialize() override;				// runs once after all devices are created
	virtual void OnEvent(SDL_Event* evnt) override;		// fires per SDL_Event
	virtual void OnCreate() override;					// fires when the object is created/recreated
	virtual void OnDestroy() override;					// fires when the object is destroyed/lost focus
	virtual void OnUpdate(float fElapsedTime) override;	// fires each frame, for updates
	//virtual void OnRender() override;					// render the current frames texture
	virtual void OnQuit() override;						// fires on exit -- reverses OnInitialize()

private:
	Bus* bus = nullptr;
	Memory* memory = nullptr;

};


#endif // __MATH_H__


/****** NOTES: **************************************

	Floating Point Unit:

		Both single and half-precision should be implemented.

		From what I can tell, 11 (+1 null) bytes are required to store a
		string that represents any half-precision value. 
		16 (+1 null) bytes are required to store any single-precision value.


	Random:
		MATH_RNDINT = (Word) 16-bit random integer (on read)
		MATH_RNDHALF = (Half) 16-bit random half-precision value (on read)

	Data:
		Half-Precision A0 and A1:
			MATH_ASC_A	= (17-Bytes) ASCII Representation of A
			MATH_INT_A1	= (Word)	A1: MSWord Integer Representation of A
			MATH_INT_A0	= (Word)	A0: LSWord Integer Representation of A

		Half-Precision B0 and B1:
			MATH_ASC_B	= (17-Bytes) ASCII Representation of B
			MATH_INT_B1	= (Word)	B1: MSWord Integer Representation of B
			MATH_INT_B0	= (Word)	B0: LSWord Integer Representation of B

		Half-Precision Output (O):
			MATH_HASC_OUT	= (17-Bytes) ASCII Representation of O, "XXXXX.YYYYY" + NULL
			MATH_INT_OUT1	= (Word)	MSWord output portion of Integer Output
			MATH_INT_OUT0	= (Word)	LSWord output portion of Integer Output



		Error Flags:
			MATH_ERR_FLAGS: (Byte)
				bit 0 = (INVALID) domain error: invalid input for operation
				bit 1 = (DIVBYZERO) pole error: finite input produced infinite result
				bit 2 = (OVERFLOW) result too large to represent finitely
				bit 3 = (UNDERFLOW) subnormal or zero result after rounding
				bit 4 = (INEXACT) result was rounded to be representable
				bit 5 = (ZERO) all bits of result are logic zero
				bit 6 = (NEGATIVE) the result of an arithmetic operation is negative
				bit 7 = (CARRY) carry/borrow result from addition/subtraction

		Operation: (Byte) Write Only Command. Operates on A and B, result in O.
			$00 = NONE		-- no effect
			$01 = EQUAL		-- (A == B)
			$02 = NOT_EQUAL	-- (A != B)
			$03 = GREATER_THAN	-- (A > B)
			$04 = LESS_THAN	-- (A < B)
			$05 = LESS_EQUAL	-- (A <= B)
			$06 = GREATER_EQUAL -- (A >= B)
			$07 = PLUS		-- (A + B)
			$08 = MINUS		-- (A - B)
			$09 = MULTIPLY	-- (A * B)
			$0A = DIVIDE	-- (A / B)
			$0B = MODULO	-- (A % B)	integer only, remainder after division
			$0C = ABSOLUTE	-- abs(A)
			$0D = MAX		-- max(A, B)
			$0E = MIN		-- min(A, B)
			$0F = LOG		-- log(A)	natural logarithm 
			$10 = LOG10		-- log10(A) common logarithm
			$11 = LOG2		-- log2(A)	binary logarithm
			$12 = SQRT		-- sqrt(A)	square root of A
			$13 = RSQRT		-- rsqrt(A)	inverse square root of A
			$14 = CBRT		-- cbrt(A)	cubit root
			$15 = HYPOT		-- hypot(A, B)	hypotenuse function
			$16 = POW		-- pow(A, B)	power function
			$17 = SIN		-- sin(A)	sine of A
			$18 = COS		-- cos(A)	cosine of A
			$19 = TAN		-- tan(A)	tangent of A
			$1A = ASIN		-- asin(A)	arc sine of A
			$1B = ACOS		-- acos(A)	arc cosine of A
			$1C = ATAN		-- atan(A)	arc tangent of A
			$1D = SINH		-- sinh(A)	hyberbolic sine of A
			$1E = COSH		-- cosh(A)	hyperbolic cosine of A
			$1F = TANH		-- tanh(A)	hyperbolic tangent of A
			$20 = ASINH		-- asinh(A) hyperbolic area sine of A
			$21 = ACOSH		-- acosh(A)	hyperbolic area cosine of A
			$22 = ATANH		-- atanh(A) hyperbolic area tangent of A
			$23 = CEIL		-- ceil(A)	Nearest integer not less than half value
			$24 = FLOOR		-- floor(A) Nearest integer not greater than half value
			$25 = TRUNC		-- trunc(A) Nearest integer not greater in magnitude than half value
			$26 = ROUND		-- round(A)	Nearest integer
			$27 = LOGB		-- logb(A)	integer extract exponent
			$28 - $7F		-- reserved / not used

			$80 - $FF		-- set bit 7 for single-precision operations
				(idea: If the most significant word is non-zero, it means
					it's a 32-bit single-precision float. Otherwise it's a 
					16-bit half-precision float.)

	Half-Precision ASCII Format:
		"XXXXX.YYYYY"		11-bytes + 1 for null-termination

	Single-Precision ASCII Format:
		"XXXXXXX.YYYYYYYY"	16-bytes + 1 for null-termination

*****************************************************/