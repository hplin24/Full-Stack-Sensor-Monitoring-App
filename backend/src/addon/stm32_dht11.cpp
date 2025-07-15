#include <napi.h>
#include "DHT11Reader.h"

namespace
{
    DHT11Reader dht11Reader;
}

Napi::Number Initialize(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    ErrorCode err = ErrorCode::None;
    err = dht11Reader.Init();
    return Napi::Number::New(env, static_cast<int>(err));
}

Napi::Boolean IsInit(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, dht11Reader.IsInit());
}

Napi::Number ReInit(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    ErrorCode err = ErrorCode::None;
    err = dht11Reader.ReInit();
    return Napi::Number::New(env, static_cast<int>(err));
}

Napi::Value GetData(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    if (!info[0].IsObject()) {
        Napi::TypeError::New(env, "Expected object").ThrowAsJavaScriptException();
        return env.Null();
    }

    DHT11Data dht11Data;
    dht11Data.temp = 0;
    dht11Data.rh = 0;
    ErrorCode err = ErrorCode::None;
    err = dht11Reader.GetData(dht11Data);

    if (err == ErrorCode::Ok)
    {
        Napi::Object obj = info[0].As<Napi::Object>();
        if (obj.Has("temp") && obj.Get("temp").IsNumber())
            obj.Set("temp", Napi::Number::New(env, static_cast<int>(dht11Data.temp)));
        if (obj.Has("rh") && obj.Get("rh").IsNumber())
            obj.Set("rh", Napi::Number::New(env, static_cast<int>(dht11Data.rh)));
    }

    return Napi::Number::New(env, static_cast<int>(err));
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    exports.Set(Napi::String::New(env, "Initialize"), Napi::Function::New(env, Initialize));
    exports.Set(Napi::String::New(env, "IsInit"), Napi::Function::New(env, IsInit));
    exports.Set(Napi::String::New(env, "ReInit"), Napi::Function::New(env, ReInit));
    exports.Set(Napi::String::New(env, "GetData"), Napi::Function::New(env, GetData));
    return exports;
}

NODE_API_MODULE(addon, Init)