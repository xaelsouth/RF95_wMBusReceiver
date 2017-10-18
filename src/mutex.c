#include <stdbool.h>
#include <stdint.h>
#include <cmsis/cmsis_device.h>
#include "mutex.h"

#define UNLOCKED		0u
#define LOCKED			1u

inline void mutex_lock(mutex_t *mutex)
{
	while (__LDREXW(mutex) == UNLOCKED) __STREXW(LOCKED, mutex);
	__CLREX();
}

inline void mutex_unlock(mutex_t *mutex)
{
	while (__LDREXW(mutex) == LOCKED) __STREXW(UNLOCKED, mutex);
	__CLREX();
}

inline bool mutex_trylock(mutex_t *mutex)
{
	if (__LDREXW(mutex) == UNLOCKED)
		return __STREXW(LOCKED, mutex) == 0; //   0: Function succeeded, 1: Function failed
	else
		__CLREX();

	return false;
}
