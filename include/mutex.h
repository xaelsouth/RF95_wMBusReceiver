#ifndef __MUTEX_H__
#define __MUTEX_H__

typedef volatile uint32_t mutex_t;

void mutex_lock(mutex_t *mutex);
void mutex_unlock(mutex_t *mutex);
bool mutex_trylock(mutex_t *mutex);

#endif /* __MUTEX_H__ */
