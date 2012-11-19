#include <node.h>
#include <v8.h>

#ifndef UNIX
    #define UNIX
    #define UNIX_64
#endif

#include <scctype.h>
#include <sccex.h>
#include "toimage.h"

using namespace v8;

void toimage_convert (uv_work_t* req) {
    
    // init the baton pointer
    toimage_convert_baton* baton = (toimage_convert_baton*) req->data;
    
    VTDWORD loadOptions = OI_INIT_DEFAULT | OI_INIT_NOSAVEOPTIONS | OI_INIT_NOLOADOPTIONS;
        
    // initialize data access module, which performs some disk io
    if (DAInitEx(DATHREAD_INIT_PTHREADS, loadOptions) == DAERR_OK) {
        
        VTHDOC documentHandle;
        
        // open the source document
        if (DAOpenDocument(&documentHandle, IOTYPE_UNIXPATH, baton->source, 0) == SCCERR_OK) {
            
            VTHEXPORT exportHandle;
            
            SCCERR foo = EXOpenExport(documentHandle, FI_PNG, IOTYPE_UNIXPATH, baton->destination, 0, 0, NULL, 0, &exportHandle);
            
            printf("error: %d\n", foo);
            
            // convert the source document
            if (foo == SCCERR_OK) {
                
                printf("exporting");
                
                if (EXRunExport(exportHandle) == SCCERR_OK) {
                    
                    printf("woot");
                    
                    baton->success = true;
                    
                }
                
                // close the export engine
                EXCloseExport(exportHandle);
                
            }
            
            // close the opened document
            DACloseDocument(documentHandle);
            
        }
        
        DADeInit();
        
    }
    
}

void toimage_convert_end (uv_work_t* req) {
    
    HandleScope scope;
    
    // init the baton pointer
    toimage_convert_baton* baton = (toimage_convert_baton*) req->data;
    
    // init callback params
    Local<Value> argv[2];
    
    if (baton->success == true) {
        
        argv[0] = Local<Value>::New(Null());
        argv[1] = Local<Value>::New(Boolean::New(true));
        
    } else {
        
        argv[0] = Exception::Error(String::New("failed to convert file"));
        argv[1] = Local<Value>::New(Boolean::New(false));
        
    }
    
    // execute callback function
    node::MakeCallback(Context::GetCurrent()->Global(), baton->callback, 2, argv);
    
    delete baton;
    
}

Handle<Value> convert (const Arguments& args) {
    
    HandleScope scope;
    
    if (args.Length() != 3)
        ThrowException(Exception::SyntaxError(String::New("expected three parameters")));
    
    if (!args[0]->IsString() || !args[1]->IsString() || !args[2]->IsFunction())
        ThrowException(Exception::TypeError(String::New("expected str, str, func parameters")));
    
    String::Utf8Value source (args[0]->ToString());
    String::Utf8Value destination (args[1]->ToString());
    
    // instanciate data baton
    toimage_convert_baton* baton = new toimage_convert_baton;
    
    // set async work data thing
    baton->req.data = (void*) baton;
    
    // init string memory locations
    baton->source = new char[source.length() + 1];
    baton->destination = new char[destination.length() + 1];
    
    // set baton properties
    baton->success = false;;
    baton->callback = Persistent<Function>::New(Local<Function>::Cast(args[2]));
    
    // copy in string paths
    strncpy((char*)memset(baton->source, '\0', source.length() + 1), *source, source.length());
    strncpy((char*)memset(baton->destination, '\0', destination.length() + 1), *destination, destination.length());
    
    // initiate async work on thread pool
    uv_queue_work(uv_default_loop(), &baton->req, toimage_convert, toimage_convert_end);
    
    return scope.Close(Undefined());
    
}

void initialize (Handle<Object> target) {
    
    target->Set(String::NewSymbol("convert"), FunctionTemplate::New(convert)->GetFunction());
    
}

NODE_MODULE(toimage, initialize)