/*
Hack to get around linker definition issue. 
*/

#pragma once
namespace Tands
{
// DO NOT MODIFY THIS FILE

// Trans simulates processing a transaction
// It does not simulate this by using sleep -- that would free up the CPU.
// Thus, Trans does a silly computation to use up CPU cycles. Note that the 
// computation has to produce some result. If it doesn't, then a smart 
// compiler will notice this and delete (optimize) the code!

// The loop's computation (to waste time) is used to modify TransSave.
// TransSave is added to the wait time in Sleep -- a few billionths of a
// second. By doing this, something "real" comes from the computation, and
// the compiler is fooled.

extern int TransSave;

void Trans( int n ) {
	long i, j;

	// Use CPU cycles 
	j = 0;
	for( i = 0; i < n * 100000; i++ ) {
		j += i ^ (i+1) % (i+n);
	}

	TransSave += j;
	TransSave &= 0xff;
}
}