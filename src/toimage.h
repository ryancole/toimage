#ifndef _TOIMAGE_H_
#define _TOIMAGE_H_

#include <v8.h>

using namespace v8;

struct toimage_convert_baton {
    
    uv_work_t req;
    
    bool success;
    char* source;
    char* destination;
    Persistent<Function> callback;
    
};

#endif