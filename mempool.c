#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "list.h"
#include "mempool.h"

#define MEM_MAGIC (0x19950517)
#define MEM_PRINTF(str,args...) printf(str,##args)
#define MEM_CHECK_MAGIC(ptr) ((*(int*)(ptr)) == MEM_MAGIC)

#define MEM_OFFPRIV(ptr) (&((mem_node*)(ptr))->priv)
#define MEM_OFFDATA(ptr) (((void*)(ptr))+sizeof(mem_node))
#define MEM_OFFMAGIC(ptr,datasize) (MEM_OFFDATA(ptr)+(datasize))
#define MEM_GETNODE(ptr) (((void*)(ptr))-sizeof(mem_node))

static int bprint = 0;

mem_handle pinit(int num, int size)
{
    int relsize = size + sizeof(mem_node) + sizeof(MEM_MAGIC);
    mem_head *handle;
    void *data_arr;
    void *sdata,*edata;
    char *env;

    env = getenv("MEM_PRINT");
    if(env)
        bprint = env[0] - '0';
    else
        bprint = 0;

    handle = malloc(sizeof(mem_head));
    if(!handle)
        return NULL;
    list_init(&handle->head_node);
    data_arr = calloc(num,relsize);
    if(!data_arr)
    {
        MEM_PRINTF("calloc\n");
        free(handle);
        return NULL;
    }
    for(sdata = data_arr, edata = data_arr + relsize * num;
            sdata != edata;sdata += relsize)
        list_add_prev(&handle->head_node, sdata);
    pthread_mutex_init(&handle->mutex, NULL);
    handle->size = size;
    handle->num = num;
    handle->data_arr = data_arr;

    return handle;
}
   
void pdeinit(mem_head *handle)
{
    if(!handle)
        return;
    pthread_mutex_destroy(&handle->mutex);
    free(handle->data_arr);
    free(handle);
}

void* _pmalloc(mem_handle handle,const char *priv)
{
    if(!handle)
        return NULL;

    pthread_mutex_lock(&handle->mutex);
    if(list_empty(&handle->head_node))
    {
        MEM_PRINTF("no mem\n");
        pthread_mutex_unlock(&handle->mutex);
        return NULL;
    }
    list_node *pnode = list_next(&handle->head_node);
    list_del(pnode);
    /* next指向head,prev指向自己的magic节点判定为正在被使用*/
    pnode->next = &handle->head_node;
    pnode->prev = MEM_OFFMAGIC(pnode, handle->size);
    pthread_mutex_unlock(&handle->mutex);

    const char **ppriv = MEM_OFFPRIV(pnode);
    *ppriv = priv;
    int *pmagic = MEM_OFFMAGIC(pnode, handle->size);
    *pmagic = MEM_MAGIC;

    return MEM_OFFDATA(pnode);
}

void _pfree(void *data, const char *str)
{
    if(!data)
        return;
    mem_node *pnode = MEM_GETNODE(data);
    if(!MEM_CHECK_MAGIC(pnode->node.prev))
    {
        MEM_PRINTF("free:magicv err[%s]---",str);
        pdata_info_print(data);
        return;
    }

    mem_head *handle = (mem_head*)pnode->node.next;
    pthread_mutex_lock(&handle->mutex);
    pnode->priv = NULL;
    list_add_prev(&handle->head_node, &pnode->node);
    pthread_mutex_unlock(&handle->mutex);
}

int _pdata_check(void *data, const char *str)
{
    mem_node *pnode = MEM_GETNODE(data);
    if(!MEM_CHECK_MAGIC(pnode->node.prev))
    {
        MEM_PRINTF("user check:magicv err[%s]---",str);
        pdata_info_print(data);
        return 0;
    }
    return 1;
}

void pdata_info_print(void *data)
{
    if(!data)
        return;
    mem_node *pnode = MEM_GETNODE(data);
    MEM_PRINTF("{{prev:%p},{next:%p},{priv:%s},{magic:%x}}\n",
            pnode->node.prev,
            pnode->node.next,
            pnode->priv,
            *((int*)pnode->node.prev));
    if(bprint)
    {
        unsigned char *saddr,*eaddr;
        MEM_PRINTF("data={");
        for(saddr = data,eaddr = (unsigned char*)pnode->node.prev;saddr!=eaddr;saddr++)
            MEM_PRINTF("[0x%x]",*saddr);
        MEM_PRINTF("}\n");
    }
}


void ppool_info_print(mem_handle handle)
{
    void *saddr,*eaddr;
    int relsize;
    if(!handle)
        return;
    relsize = handle->size + sizeof(mem_node) + sizeof(MEM_MAGIC);
    MEM_PRINTF("startaddr               endaddr                used       num=%d     size=%d \n",handle->num, handle->size);
    for(saddr = handle->data_arr, eaddr = saddr + relsize * handle->num;
            saddr != eaddr;saddr += relsize)
        MEM_PRINTF("%p          %p         %s\n",MEM_OFFDATA(saddr),MEM_OFFDATA(saddr)+handle->size,((mem_node*)saddr)->priv);
}


















