
#include "patchWorkerCallback.h"

extern "C" {
  #include "bspatch/bspatch.h"
}

// Public
PatchWorkerCallback::PatchWorkerCallback(Nan::Callback *callback, const std::string& oldfile, const std::string& newfile, const std::string& patchfile)
    : AsyncProgressWorkerBase(callback)
{
  _oldfile = oldfile;
  _newfile = newfile; 
  _patchfile = patchfile;
}

PatchWorkerCallback::~PatchWorkerCallback() 
{   }

void PatchWorkerCallback::Execute(const ExecutionProgress& progress) 
{
    char error[1024];
    memset(error, 0, sizeof error);

    PatchWorkerData data;
    data.percentage = 0;
    data.progressWorker = &progress;

    bspatch(error, _oldfile.c_str(), _newfile.c_str(), _patchfile.c_str(), &data, &PatchWorkerCallback::CCallback);
    _error = error; 
}

void PatchWorkerCallback::HandleProgressCallback(const int* data, size_t count) 
{
    if(data != nullptr)
    {
        Nan::HandleScope scope;
        v8::Local<v8::Value> argv[] = {
            v8::Number::New(v8::Isolate::GetCurrent(), *data),
            v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), _error.c_str())
        };

        callback->Call(2, argv, this->async_resource);
    }
}

void PatchWorkerCallback::HandleOKCallback() 
{
    v8::Local<v8::Value> argv[] = { 
        v8::Number::New(v8::Isolate::GetCurrent(), 100),
        v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), _error.c_str()) 
    };
    callback->Call(2, argv, this->async_resource);
}

