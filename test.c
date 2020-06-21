#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mempool.h"
#include "stoj.h"

typedef struct
{
    char c;
    int  i;
    double d;
    char str[100];
}A;
typedef struct
{
    unsigned char c[10];
    A a;
}B;

SERIAL_STRUCT(A, 
        BASIC_TYPE(JSON_NUMBER,c) 
        BASIC_TYPE(JSON_NUMBER,i) 
        BASIC_TYPE(JSON_FLOAT,d) 
        BASIC_TYPE(JSON_STRING,str) 
        );
SERIAL_STRUCT(B, 
        ARRAY_TYPE(JSON_NUMBER,c) 
        STRUCT_TYPE(A,a) 
        );

B b;
B bb = {
            .c = {1,-2,223,5},
            .a = {
                .c = 'p',
                .i = -999,
                .d = 98.77,
                .str = "hello world"
            }
};



int main()
{
#if 1
    print_B(&bb);
    char*str = B_to_str(&bb);
    printf("%s",str);
    str_to_B(str,&b);
    print_B(&b);
#endif
   

#if 0
    mem_handle handle;

    handle = pinit(10,10);
    char *str = pmalloc(handle);
    memset(str,9,11);

    //MEM_CHECK_EXIT(str);
    ppool_info_print(handle);
    pfree(str);


    pdeinit(handle);
#endif
    return 0;
}
