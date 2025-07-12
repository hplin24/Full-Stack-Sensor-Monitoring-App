#include <napi.h>
#include "DHT11Reader.h"

namespace
{
    DHT11Reader dht11;
}

Napi::Number InitComPort(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    ErrorCode err = ErrorCode::None;
    err = dht11.InitComPort();
    return Napi::Number::New(env, static_cast<int>(err));
}

Napi::Boolean IsComPortOk(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    const bool isOk = dht11.IsComPortOk();
    return Napi::Boolean::New(env, isOk);
}

Napi::Number ReInitComPort(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    ErrorCode err = ErrorCode::None;
    err = dht11.ReInitComPort();
    return Napi::Number::New(env, static_cast<int>(err));
}

Napi::Value RetrieveData(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    if (!info[0].IsObject()) {
        Napi::TypeError::New(env, "Expected object").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (dht11.IsComPortOk())
    {
        DHT11Data dht11Data;
        dht11Data.temp = 0;
        dht11Data.rh = 0;
        ErrorCode err = ErrorCode::None;
        err = dht11.RetrieveData(dht11Data);

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
    return env.Null();
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    exports.Set(Napi::String::New(env, "InitComPort"), Napi::Function::New(env, InitComPort));
    exports.Set(Napi::String::New(env, "IsComPortOk"), Napi::Function::New(env, IsComPortOk));
    exports.Set(Napi::String::New(env, "RetrieveData"), Napi::Function::New(env, RetrieveData));
    exports.Set(Napi::String::New(env, "ReInitComPort"), Napi::Function::New(env, ReInitComPort));
    return exports;
}

NODE_API_MODULE(addon, Init)