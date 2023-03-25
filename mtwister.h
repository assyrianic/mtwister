#ifndef __MTWISTER_H
#	define __MTWISTER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

enum {
	STATE_VECTOR_LENGTH = 624,
	STATE_VECTOR_M      = 397,
	
	UPPER_MASK          = 0x80000000,
	LOWER_MASK          = 0x7fffffff,
	TEMPERING_MASK_B    = 0x9d2c5680,
	TEMPERING_MASK_C    = 0xefc60000,
};


typedef struct MTRand {
	size_t mt[STATE_VECTOR_LENGTH];
	size_t index;
} MTRand;

static inline void _MT_SeedRand(MTRand *const rand, size_t const seed) {
	/**
	 * set initial seeds to mt[STATE_VECTOR_LENGTH] using the generator
	 * from Line 25 of Table 1 in: Donald Knuth, "The Art of Computer
	 * Programming," Vol. 2 (2nd Ed.) pp.102.
	 */
	rand->mt[0] = seed & 0xFFffFFffUL;
	for( rand->index=1; rand->index < STATE_VECTOR_LENGTH; rand->index++ ) {
		rand->mt[rand->index] = (6069 * rand->mt[rand->index - 1]) & 0xFFffFFffUL;
	}
}

static inline MTRand MT_SeedRand(size_t const seed) {
	MTRand rand = {0};
	_MT_SeedRand(&rand, seed);
	return rand;
}

/**
 * Generates a pseudo-randomly generated long.
 */
static inline size_t MT_GenRand_UInt(MTRand *const rand) {
	size_t y = 0;
	size_t const mag[2] = {0x0, 0x9908b0df}; /** mag[x] = x * 0x9908b0df for x = 0,1 */
	if( rand->index >= STATE_VECTOR_LENGTH ) {
		/** generate STATE_VECTOR_LENGTH words at a time */
		if( rand->index >= STATE_VECTOR_LENGTH+1 ) {
			_MT_SeedRand(rand, 4357);
		}
		
		int kk = 0;
		for( ; kk < (STATE_VECTOR_LENGTH - STATE_VECTOR_M); kk++ ) {
			y = (rand->mt[kk] & UPPER_MASK) | (rand->mt[kk + 1] & LOWER_MASK);
			rand->mt[kk] = rand->mt[kk + STATE_VECTOR_M] ^ (y >> 1) ^ mag[y & 0x1];
		}
		for( ; kk < STATE_VECTOR_LENGTH-1; kk++ ) {
			y = (rand->mt[kk] & UPPER_MASK) | (rand->mt[kk+1] & LOWER_MASK);
			rand->mt[kk] = rand->mt[kk + (STATE_VECTOR_M - STATE_VECTOR_LENGTH)] ^ (y >> 1) ^ mag[y & 0x1];
		}
		
		y = (rand->mt[STATE_VECTOR_LENGTH - 1] & UPPER_MASK) | (rand->mt[0] & LOWER_MASK);
		rand->mt[STATE_VECTOR_LENGTH - 1] = rand->mt[STATE_VECTOR_M - 1] ^ (y >> 1) ^ mag[y & 0x1];
		rand->index = 0;
	}
	
	y = rand->mt[rand->index++];
	y ^= (y >> 11);
	y ^= (y << 7) & TEMPERING_MASK_B;
	y ^= (y << 15) & TEMPERING_MASK_C;
	y ^= (y >> 18);
	return y;
}

/**
 * Generates a pseudo-randomly generated double in the range [0..1].
 */
static inline double MT_GenRand_F64(MTRand *const rand) {
	return ( double )(MT_GenRand_UInt(rand)) / 0xFFffFFffUL;
}

#ifdef __cplusplus
}
#endif

#endif /** __MTWISTER_H */
