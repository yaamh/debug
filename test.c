#include <stdlib.h>
#include <string.h>
#include "mempool.h"


int main()
{
    mem_handle handle;

    handle = pinit(10,10);
    char *str = pmalloc(handle);
    memset(str,9,11);

    //MEM_CHECK_EXIT(str);
    ppool_info_print(handle);
    pfree(str);


    pdeinit(handle);
    return 0;
}
