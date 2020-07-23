#ifndef __STOJ_H__
#define __STOJ_H__

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "cJSON.h"

#define BASIC_GET_int_ELEMENT(to_struct, from_json, _element) \
    do{\
        tjson = cJSON_GetObjectItem(from_json, #_element);\
        if(tjson) (to_struct)->_element = (typeof ((to_struct)->_element))tjson->valueint; \
        else JSON_PRINTF("no num:%s\n",#_element);\
    }while(0)

#define BASIC_GET_string_ELEMENT(to_struct, from_json, _element) \
    do{\
        tjson = cJSON_GetObjectItem(from_json, #_element);\
        if(tjson&&((to_struct)->_element)) strcpy((to_struct)->_element, tjson->valuestring); \
        else JSON_PRINTF("no str:%s or null point\n",#_element); \
    }while(0)

#define BASIC_GET_double_ELEMENT(to_struct, from_json, _element) \
    do{\
        tjson = cJSON_GetObjectItem(from_json, #_element);\
        if(tjson) (to_struct)->_element = (typeof ((to_struct)->_element))tjson->valuedouble; \
        else JSON_PRINTF("no float:%s\n",#_element);\
    }while(0)

#define ARRAY_GET_int_ELEMENT(to_struct, from_json, _element, index) \
    (to_struct)->_element[index] = (typeof ((to_struct)->_element[0]))from_json->valueint;

#define ARRAY_GET_double_ELEMENT(to_struct, from_json, _element, index) \
    (to_struct)->_element[index] = (typeof ((to_struct)->_element[0]))from_json->valuedouble;

#define ARRAY_GET_string_ELEMENT(to_struct, from_json, _element, index) \
    if((to_struct)->_element[index])strcpy((to_struct)->_element[index], from_json->valuestring);

#define BASIC_GET_ARRAY(to_struct, from_json, type, _element) \
    do{\
        cJSON *array;\
        size_t index = 0, size = 0;\
        array = cJSON_GetObjectItem(from_json, #_element);\
        if(array) {\
            size = cJSON_GetArraySize(array);\
            while(index < size) {\
                tjson = cJSON_GetArrayItem(array, index);\
                if(tjson) {ARRAY_GET_##type##_ELEMENT(to_struct, tjson, _element, index++);}\
                else JSON_PRINTF("no arr:%s\n",#_element);\
            }\
        }\
    }while(0)

#define STRUCT_GET_ARRAY(to_struct, from_json, type, _element)\
    do{\
        cJSON *array;\
        size_t index = 0, tsize = 0, size = sizeof((to_struct)->_element)/sizeof((to_struct)->_element[0]);\
        array = cJSON_GetObjectItem(from_json, #_element);\
        if(array) {\
            tsize = cJSON_GetArraySize(array);\
            size = size>tsize?tsize:size;\
            while(index < size){\
                tjson = cJSON_GetArrayItem(array, index);\
                if(tjson) _serialize_##type(&((to_struct)->_element[index++]), &tjson, JSON_2_STRUCT);\
                else JSON_PRINTF("no struct_arr:%s\n",#_element);\
            }\
        }\
    }while(0)

#define STRUCT_GET_ELEMENT(to_struct, from_json, type,_element) \
    do{\
        tjson = cJSON_GetObjectItem(from_json, #_element);\
        if(tjson)_serialize_##type(&((to_struct)->_element), &tjson, JSON_2_STRUCT); \
        else JSON_PRINTF("no struct:%s\n",#_element);\
    }while(0)
        
#define BASIC_SET_int_ELEMENT(to_json, from_struct, _element) \
    do{\
        cJSON_AddNumberToObject(to_json, #_element, (from_struct)->_element);\
    }while(0)

#define BASIC_SET_double_ELEMENT(to_json, from_struct, _element) \
    do{\
        cJSON_AddNumberToObject(to_json, #_element, (from_struct)->_element);\
    }while(0)

#define BASIC_SET_string_ELEMENT(to_json, from_struct, _element)\
  do{\
    if((from_struct)->_element)\
      cJSON_AddStringToObject(to_json, #_element, (from_struct)->_element);\
    else\
      cJSON_AddNullToObject(to_json,#_element);\
  }while(0)

#define ARRAY_SET_int_ELEMENT(to_json, from_struct, _element, index) \
        cJSON_AddItemToArray(to_json, cJSON_CreateNumber((from_struct)->_element[index]));\

#define ARRAY_SET_double_ELEMENT ARRAY_SET_int_ELEMENT

#define ARRAY_SET_string_ELEMENT(to_json, from_struct, _element, index) \
    if((from_struct)->_element[index])\
      cJSON_AddItemToArray(to_json, cJSON_CreateString((from_struct)->_element[index]));\
    else\
      cJSON_AddItemToArray(to_json, cJSON_CreateNull());\

#define BASIC_SET_ARRAY(to_json, from_struct, type, _element)\
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

#define STRUCT_SET_ARRAY(to_json, from_struct, type, _element)\
    do{\
        size_t index = 0;\
        cJSON *ttjson;\
        tjson = cJSON_CreateArray();\
        if(tjson){\
            while(index < sizeof((from_struct)->_element)/sizeof((from_struct)->_element[0])){\
                _serialize_##type(&from_struct->_element[index++], &ttjson, STRUCT_2_JSON);\
                if(ttjson) cJSON_AddItemToArray(tjson, ttjson);\
            }\
            cJSON_AddItemToObject(to_json, #_element, tjson);\
        }\
    }while(0)

#define STRUCT_SET_ELEMENT(to_json, from_struct, type, _element) \
    do{\
        _serialize_##type(&from_struct->_element, &tjson, STRUCT_2_JSON);\
        if(tjson) cJSON_AddItemToObject(to_json, #_element, tjson);\
    }while(0)

#define _BASIC_TYPE(type,element_name)  if(opt==STRUCT_2_JSON){BASIC_SET_##type##_ELEMENT(json, tstruct, element_name);}else{BASIC_GET_##type##_ELEMENT(tstruct, json, element_name);}
#define _BASIC_ARRAY(type,element_name)  if(opt==STRUCT_2_JSON){BASIC_SET_ARRAY(json, tstruct, type, element_name);}else{BASIC_GET_ARRAY(tstruct, json,type, element_name);}
#define _STRUCT_TYPE(type,element_name) if(opt==STRUCT_2_JSON){STRUCT_SET_ELEMENT(json, tstruct, type, element_name);}else{STRUCT_GET_ELEMENT(tstruct, json, type, element_name);}
#define _STRUCT_ARRAY(type,element_name) if(opt==STRUCT_2_JSON){STRUCT_SET_ARRAY(json, tstruct, type, element_name);}else{STRUCT_GET_ARRAY(tstruct, json, type, element_name);}



#define STRUCT_2_JSON   (0)
#define JSON_2_STRUCT   (!STRUCT_2_JSON)

#define _SERIAL_STRUCT(struct_name ,...) \
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
    static inline cJSON* struct_name##_to_json(struct_name *_struct, const char *_info)\
    {\
        cJSON *_json;\
        if(!_struct){\
            JSON_PRINTF("null point struct [info:%s]\n", _info);\
            return NULL;\
        }\
        _serialize_##struct_name(_struct, &_json, STRUCT_2_JSON);\
        return _json;\
    }\
    static inline int json_to_##struct_name(cJSON *_json, struct_name *_struct, const char *_info)\
    {\
        if(!_json || !_struct){\
            JSON_PRINTF("null point json:%p,struct:%p [info:%s]\n",_json,_struct, _info);\
            return -1;\
        }\
        _serialize_##struct_name(_struct, &_json, JSON_2_STRUCT);\
    }\
    static inline char* struct_name##_to_str(struct_name *_struct, const char *_info)\
    {\
        cJSON *_json = struct_name##_to_json(_struct, _info);\
        char *_str = cJSON_Print(_json);;\
        cJSON_Delete(_json);\
        return _str;\
    }\
    static inline int str_to_##struct_name(const char *str, struct_name *_struct, const char *_info)\
    {\
        cJSON *_json;\
        _json = cJSON_Parse(str);\
        if(!_json){\
            JSON_PRINTF("str parse err [info:%s]:%s\n",_info, cJSON_GetErrorPtr());\
            return -1;\
        }\
        _serialize_##struct_name(_struct, &_json, JSON_2_STRUCT);\
        cJSON_Delete(_json);\
        return 0;\
    }\
    static inline int struct_name##_to_file(struct_name *_struct, const char *pathname, const char *_info)\
    {\
        char *str = struct_name##_to_str(_struct, _info);\
        if(!str) return -1;\
        int fd = open(pathname, O_CREAT|O_EXCL|O_RDWR);\
        if(fd < 0)\
        {\
            JSON_PRINTF("open err[info:%s]:%s\n", _info, pathname);\
            return -1;\
        }\
        write(fd, str, strlen(str));\
        close(fd);\
        return 0;\
    }\
    static inline int file_to_##struct_name(const char *pathname, struct_name *_struct, const char *_info)\
    {\
        char *str;\
        struct stat s;\
        if(!_struct) return -1;\
        int fd = open(pathname, O_RDONLY);\
        if(fd < 0){\
            JSON_PRINTF("open err[info:%s]:%s\n", _info, pathname);\
            return -1;\
        }\
        if(fstat(fd, &s)){\
            JSON_PRINTF("stat err[info:%s]:%s\n", _info, pathname);\
            close(fd);\
            return -1;\
        }\
        str = malloc(s.st_size);\
        read(fd, str, s.st_size);\
        close(fd);\
        str_to_##struct_name(str, _struct, _info);\
        free(str);\
        return 0;\
    }\
    static inline void print_##struct_name(struct_name *_struct, const char *_info)\
    {\
        char *str = struct_name##_to_str(_struct, _info);\
        printf("%s\n",str);\
        free(str);\
    }
#define _JSON_STR(x) #x
#define JSON_STR(x) _JSON_STR(x)
#define JSON_SLINE  JSON_STR(__LINE__)

#ifndef JSON_PRINTF
    #define JSON_PRINTF printf
#endif

#define JTYPE_NUMBER int 
#define JTYPE_FLOAT  double 
#define JTYPE_STRING string 

#define PBASIC_TYPE(type, element_name)
#define PBASIC_ARRAY(type, element_name)
#define PSTRUCT_TYPE(type, element_name)
#define PSTRUCT_ARRAY(type, element_name)
#define BASIC_TYPE(type, element_name)    _BASIC_TYPE(type, element_name)
#define BASIC_ARRAY(type, element_name)   _BASIC_ARRAY(type, element_name)
#define STRUCT_TYPE(type, element_name)   _STRUCT_TYPE(type, element_name)
#define STRUCT_ARRAY(type, element_name)  _STRUCT_ARRAY(type, element_name)
#define SERIAL_STRUCT(struct_type, args...) _SERIAL_STRUCT(struct_type, ##args)

#define struct2json(struct_type, pstruct)             struct_type##_to_json(pstruct, JSON_SLINE)
#define json2struct(struct_type, pjson, pstruct)      json_to_##struct_type(pjson, pstruct, JSON_SLINE)
#define struct2str(struct_type, pstruct)              struct_type##_to_str(pstruct, JSON_SLINE)
#define str2struct(struct_type, pstr, pstruct)        str_to_##struct_type(pstr, pstruct, JSON_SLINE)
#define struct2file(struct_type, pstruct, filepath)   struct_type##_to_file(pstruct, filepath, JSON_SLINE)
#define file2struct(struct_type, filepath, pstruct)   file_to_##struct_type(filepath, pstruct, JSON_SLINE)
#define printstruct(struct_type, pstruct)             print_##struct_type(pstruct, JSON_SLINE)
























#endif

