/*
 *  bkegen.h
 *
 *  Created by Bruno Keymolen on 1/17/08.
 *
 *  (C) Copyright, 2008 Bruno Keymolen. 
 *
 *
 *  Contact: bruno.keymolen@gmail.com
 */

#ifndef BKEGEN_H
#define BKEGEN_H

#include <pthread.h>
#include <sys/times.h>

//#define BKE_DEBUG 10
//#define BKE_DEBUG_MALLOC

/*
 * Linux / MacOS Posix
 */

//Memory
#ifdef BKE_DEBUG_MALLOC
	int bke_Debug_free(void *region, char* file, int line);
	
	#define BKE_MALLOC(s) malloc(s)
	#define BKE_FREE(s) bke_Debug_free(s, __FILE__, __LINE__)
#else
	#define BKE_MALLOC(s) malloc(s)
	#define BKE_FREE(s) free(s)
#endif

//Time
long GetTickCount();


//Mutex
#define BKE_MUTEX pthread_mutex_t*
#define BKE_MUTEX_EMPTY NULL

#define BKE_MUTEX_CREATE(pmutex) \
{ \
	pmutex = (BKE_MUTEX)malloc(sizeof(pthread_mutex_t)); \
	pthread_mutexattr_t attributes; \
	pthread_mutexattr_init(&attributes); \
	pthread_mutexattr_settype(&attributes, PTHREAD_MUTEX_RECURSIVE); \
	pthread_mutex_init(pmutex, &attributes); \
	pthread_mutexattr_destroy(&attributes); \
}

#define BKE_MUTEX_DESTROY(pmutex) \
{ \
	pthread_mutex_destroy( pmutex ); \
	free(pmutex); \
}

#ifdef BKE_DEBUG
	int pthread_Debug_mutex_unlock(pthread_mutex_t *mutex, char* file, int line);
	int pthread_Debug_mutex_lock(pthread_mutex_t *mutex, char* file, int line);

	#define BKE_MUTEX_LOCK(m) pthread_Debug_mutex_lock(m, __FILE__, __LINE__)
	#define BKE_MUTEX_UNLOCK(m) pthread_Debug_mutex_unlock(m, __FILE__, __LINE__)
#else
	#define BKE_MUTEX_LOCK(m) pthread_mutex_lock(m)
	#define BKE_MUTEX_UNLOCK(m) pthread_mutex_unlock(m)
#endif



//Thread
#define BKE_THREAD pthread_t*

#define BKE_THREAD_CREATE(pthread, pfunc, parg) \
{ \
	pthread = (pthread_t*)malloc(sizeof(pthread_t)); \
	pthread_create(pthread, NULL, pfunc, (void*)parg); \
}

#define BKE_THREAD_DESTROY(pthread) \
{ \
	pthread_exit(NULL); \
	free(pthread); \
}


//Types
//
//#ifndef BOOL
//	typedef int BOOL;
//#endif
//#ifndef TRUE
//	#define TRUE 1
//#endif
//#ifndef FALSE
//	#define FALSE 0
//#endif



#ifndef U8
	#define U8 unsigned char
#endif

#ifndef U16
	#define U16 unsigned short
#endif

#ifndef U32
	#define U32 unsigned int
#endif

#ifndef u8
	#define u8 unsigned char
#endif

#ifndef u16
	#define u16 unsigned short
#endif

#ifndef u32
	#define u32 unsigned int
#endif

#ifndef S8
	#define S8 signed char
#endif

#ifndef S16
	#define S16 signed short
#endif

#ifndef s32
	#define s32 signed int
#endif

#ifndef s8
	#define s8 signed char
#endif

#ifndef s16
	#define s16 signed short
#endif

#ifndef s32
	#define s32 signed int
#endif


#endif //BKEGEN_H
