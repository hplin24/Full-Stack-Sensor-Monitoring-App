#include <napi.h>
#include "STM32DHT11Reader.h"

namespace
{
    STM32DHT11Reader reader;
}

Napi::Number Init(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    EC ec = EC::None;
    ec = reader.Init();
    return Napi::Number::New(env, static_cast<int>(ec));
}

Napi::Number ReInit(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    EC ec = EC::None;
    ec = reader.ReInit();
    return Napi::Number::New(env, static_cast<int>(ec));
}

Napi::Boolean IsInit(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, reader.IsInit());
}

Napi::Number UpdateData(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    EC ec = EC::None;
    ec = reader.UpdateData();
    return Napi::Number::New(env, static_cast<int>(ec));
}

Napi::Number GetTemperature(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    return Napi::Number::New(env, reader.GetTemperature());
}

Napi::Number GetHumidity(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    return Napi::Number::New(env, reader.GetHumidity());
}

Napi::Object RegisterExports(Napi::Env env, Napi::Object exports)
{
    exports.Set(Napi::String::New(env, "Init"), Napi::Function::New(env, Init));
    exports.Set(Napi::String::New(env, "ReInit"), Napi::Function::New(env, ReInit));
    exports.Set(Napi::String::New(env, "IsInit"), Napi::Function::New(env, IsInit));
    exports.Set(Napi::String::New(env, "UpdateData"), Napi::Function::New(env, UpdateData));
    exports.Set(Napi::String::New(env, "GetTemperature"), Napi::Function::New(env, GetTemperature));
    exports.Set(Napi::String::New(env, "GetHumidity"), Napi::Function::New(env, GetHumidity));
    return exports;
}

NODE_API_MODULE(stm32_dht11_reader, RegisterExports)
