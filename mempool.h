#ifndef __MEMPOOL_H__
#define __MEMPOOL_H__

#include <pthread.h>
#include "list.h"



typedef struct _mem_node
{
    list_node node;
    const char *priv;
}mem_node;

typedef struct _mem_head
{
    list_node head_node;
    pthread_mutex_t mutex;
    void *data_arr;
    int   size;
    int   num;
}mem_head;

typedef mem_head *mem_handle;

void* _pmalloc(mem_handle handle, const char *priv);
void _pfree(void *data, const char *str);
int _pdata_check(void *data, const char *str);



#define MEM_MAXNUM 1024
#define MEM_USERCHECK 1
#define __STR(x)  #x
#define _STR(x)  __STR(x)
#define _SLINE   _STR(__LINE__)
#define _MEM_PRIV (__FILE__":"_SLINE)

#if(MEM_USERCHECK)
    #define MEM_CHECK(ptr) _pdata_check((ptr), _MEM_PRIV)
#else
    #define MEM_CHECK(ptr) 1
#endif
#define MEM_CHECK_EXIT(ptr) (MEM_CHECK(ptr)||(abort(),0))

mem_handle pinit(int num, int size);
void pdeinit(mem_handle handle);
#define pmalloc(handle)  _pmalloc((handle), _MEM_PRIV)
#define pfree(data)     _pfree((data), _MEM_PRIV)
void pdata_info_print(void *pdata);
void ppool_info_print(mem_handle handle);










#endif

