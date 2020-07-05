#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mempool.h"
#include "stoj.h"

typedef struct
{
    int a;
    int b;
}C;
typedef struct
{
    char c;
    int i;
    double d;
    char *str[3];
}A;

typedef struct
{
    unsigned char c[10];
    A a;
    C cc[3];
    C *ccc;
}B;


SERIAL_STRUCT(C, 
        BASIC_TYPE(JTYPE_NUMBER,a) 
        BASIC_TYPE(JTYPE_NUMBER,b) 
        );

SERIAL_STRUCT(A, 
        BASIC_TYPE(JTYPE_NUMBER,c) 
        BASIC_TYPE(JTYPE_NUMBER,i) 
        BASIC_TYPE(JTYPE_FLOAT,d) 
        BASIC_ARRAY(JTYPE_STRING,str) 
        );

SERIAL_STRUCT(B,
        BASIC_ARRAY(JTYPE_NUMBER, c)
        STRUCT_TYPE(A,a)
        STRUCT_ARRAY(C,cc)
        );

C c = {99,1111};
B b;
B bb = {
            .c = {1,-2,223,5},
            .a = {
                .c = 'p',
                .i = -999,
                .d = 98.77,
                .str = {"123","hello world"}
            },
            .cc = {11,22,33,44,55},
                .ccc = &c
};



int main()
{
#if 1
    printstruct(B,&bb);
    char*str = struct2str(B,&bb);
    printf("%s\n",str);
    str2struct(B,str,&b);
    printstruct(B,&b);
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
