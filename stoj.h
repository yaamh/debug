#ifndef __STOJ_H__
#define __STOJ_H__

#include <string.h>
#include "cJSON.h"

#define BASIC_GET_int_ELEMENT(to_struct, from_json, _element) \
    do{\
        tjson = cJSON_GetObjectItem(from_json, #_element);\
        if(tjson) (to_struct)->_element = (typeof ((to_struct)->_element))tjson->valueint; \
        else (to_struct)->_element = (typeof ((to_struct)->_element))19951517;\
    }while(0)

#define BASIC_GET_char_ELEMENT BASIC_GET_int_ELEMENT

#define BASIC_GET_string_ELEMENT(to_struct, from_json, _element) \
    do{\
        tjson = cJSON_GetObjectItem(from_json, #_element);\
        if(tjson&&((to_struct)->_element)) strcpy((to_struct)->_element, tjson->valuestring); \
        else strcpy((to_struct)->_element, "19950517"); \
    }while(0)

#define BASIC_GET_double_ELEMENT(to_struct, from_json, _element) \
    do{\
        tjson = cJSON_GetObjectItem(from_json, #_element);\
        if(tjson) (to_struct)->_element = (typeof ((to_struct)->_element))tjson->valuedouble; \
        else (to_struct)->_element = (typeof ((to_struct)->_element))1995.1517;\
    }while(0)

#define ARRAY_GET_int_ELEMENT(to_struct, from_json, _element, index) \
    (to_struct)->_element[index] = (typeof ((to_struct)->_element[0]))from_json->valueint;

#define ARRAY_GET_char_ELEMENT ARRAY_GET_int_ELEMENT

#define ARRAY_GET_double_ELEMENT(to_struct, from_json, _element, index) \
    (to_struct)->_element[index] = (typeof ((to_struct)->_element[0]))from_json->valuedouble;

#define ARRAY_GET_ELEMENT(to_struct, from_json, type, _element) \
    do{\
        cJSON *array, *array_element;\
        size_t index = 0, size =0;\
        array = cJSON_GetObjectItem(from_json, #_element);\
        if(array) {\
            size = cJSON_GetArraySize(array);\
            while(index < size) {\
                array_element = cJSON_GetArrayItem(array, index);\
                if(array_element) {ARRAY_GET_##type##_ELEMENT(to_struct, array_element, _element, index++);}\
                else {memset((to_struct)->_element,0xff,sizeof((to_struct)->_element));}\
            }\
        }\
    }while(0)

#define STRUCT_GET_ELEMENT(type,element_name) \
    do{\
        tjson = cJSON_GetObjectItem(json, #element_name);\
        _serialize_##type(&((tstruct)->element_name), &tjson, JSON_2_STRUCT); \
    }while(0)
        
#define BASIC_SET_int_ELEMENT(to_json, from_struct, _element) \
    do{\
        cJSON_AddNumberToObject(to_json, #_element, (from_struct)->_element);\
    }while(0)

#define BASIC_SET_char_ELEMENT  BASIC_SET_int_ELEMENT

#define BASIC_SET_double_ELEMENT(to_json, from_struct, _element) \
    do{\
        cJSON_AddNumberToObject(to_json, #_element, (from_struct)->_element);\
    }while(0)

#define BASIC_SET_string_ELEMENT(to_json, from_struct, _element)\
    do{\
        cJSON_AddStringToObject(to_json, #_element, (from_struct)->_element);\
    }while(0)

#define ARRAY_SET_int_ELEMENT(to_json, from_struct, _element, index) \
    do{\
        cJSON_AddItemToArray(to_json, cJSON_CreateNumber((from_struct)->_element[index]));\
    }while(0)

#define ARRAY_SET_char_ELEMENT ARRAY_SET_int_ELEMENT

#define ARRAY_SET_double_ELEMENT ARRAY_SET_int_ELEMENT

#define ARRAY_SET_string_ELEMENT(to_json, from_struct, _element, index) \
    do{\
        cJSON_AddItemToArray(to_json, cJSON_CreateString((from_struct)->_element[index]));\
    }while(0)

#define ARRAY_SET_ELEMENT(to_json, from_struct, type, _element)\
    do{\
        size_t index = 0; \
        tjson = cJSON_CreateArray(); \
        if(tjson){\
            while(index < sizeof((from_struct)->_element)/sizeof((from_struct)->_element[0])){\
                ARRAY_SET_##type##_ELEMENT(tjson, from_struct, _element, index++);\
            }\
            cJSON_AddItemToObject(to_json, #_element, tjson);\
        }\
    }while(0)

#define STRUCT_SET_ELEMENT(type, element_name) \
    do{\
        _serialize_##type(&tstruct->element_name, &tjson, STRUCT_2_JSON);\
        if(tjson) cJSON_AddItemToObject(json, #element_name, tjson);\
    }while(0)

#define _BASIC_TYPE(type,element_name)  if(opt==STRUCT_2_JSON){BASIC_SET_##type##_ELEMENT(json, tstruct, element_name);}else{BASIC_GET_##type##_ELEMENT(tstruct, json, element_name);}
#define _ARRAY_TYPE(type,element_name)  if(opt==STRUCT_2_JSON){ARRAY_SET_ELEMENT(json, tstruct, type, element_name);}else{ARRAY_GET_ELEMENT(tstruct, json,type, element_name);}
#define _STRUCT_TYPE(type,element_name) if(opt==STRUCT_2_JSON){STRUCT_SET_ELEMENT(type, element_name);}else{STRUCT_GET_ELEMENT(type, element_name);}
#define BASIC_TYPE(type,element_name)  _BASIC_TYPE(type,element_name)
#define ARRAY_TYPE(type,element_name)  _ARRAY_TYPE(type,element_name)
#define STRUCT_TYPE(type,element_name) _STRUCT_TYPE(type,element_name)

#define STRUCT_2_JSON   (0)
#define JSON_2_STRUCT   (!STRUCT_2_JSON)

#define SERIAL_STRUCT(struct_name ,...) \
    static inline int _serialize_##struct_name(struct_name *in_out_struct, cJSON **in_out_json, int opt)\
    {\
        cJSON *json,*tjson;\
        struct_name *tstruct;\
        if(opt == STRUCT_2_JSON)\
        {\
            tstruct = in_out_struct;\
            json = cJSON_CreateObject();\
            *in_out_json = json;\
        }\
        else\
        {\
            tstruct = in_out_struct;\
            json = *in_out_json;\
            memset(tstruct, 0, sizeof(struct_name));\
        }\
        __VA_ARGS__;\
    }\
    static inline cJSON* struct_name##_to_json(struct_name *_struct)\
    {\
        cJSON *_json;\
        _serialize_##struct_name(_struct, &_json, STRUCT_2_JSON);\
        return _json;\
    }\
    static inline void json_to_##struct_name(cJSON *_json, struct_name *_struct)\
    {\
        _serialize_##struct_name(_struct, &_json, JSON_2_STRUCT);\
    }\
    static inline char* struct_name##_to_str(struct_name *_struct)\
    {\
        cJSON *_json;\
        char *_str;\
        _serialize_##struct_name(_struct, &_json, STRUCT_2_JSON);\
        _str = cJSON_Print(_json);\
        cJSON_Delete(_json);\
        return _str;\
    }\
    static inline void str_to_##struct_name(const char *str, struct_name *_struct)\
    {\
        cJSON *_json;\
        _json = cJSON_Parse(str);\
        _serialize_##struct_name(_struct, &_json, JSON_2_STRUCT);\
        cJSON_Delete(_json);\
    }\
    static inline void print_##struct_name(struct_name *_struct)\
    {\
        char *str = struct_name##_to_str(_struct);\
        printf("%s\n",str);\
        free(str);\
    }

#define JSON_NUMBER int 
#define JSON_FLOAT  double 
#define JSON_STRING string 



#endif

