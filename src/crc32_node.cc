#include <node.h>
#include <node_buffer.h>
#include <string>

#include "./../lib/crc32.cc"

using namespace node;
using namespace v8;

uint8_t *BufferData(v8::Local<v8::Value> buf_val)
{
    return (uint8_t *)node::Buffer::Data(buf_val);
}
size_t BufferLength(v8::Local<v8::Value> buf_val)
{
    return node::Buffer::Length(buf_val);
}

void CRC32CheckSum(const FunctionCallbackInfo<Value> &args)
{
    Isolate *isolate = args.GetIsolate();
    if (!args[0]->IsObject())
    {
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Input Stream is Buffer required!")));
    }

    Handle<Value> stream = Handle<Value>::Cast(args[0]);
    Handle<Object> option = Handle<Object>::Cast(args[1]);

    std::string retType = "hex";
    Local<String> retTypeKey = String::NewFromUtf8(isolate, "retType");
    if (option->Has(retTypeKey) && option->Get(retTypeKey)->IsString())
    {
        String::Utf8Value retTypeNode(option->Get(retTypeKey)->ToString());
        retType = std::string(*retTypeNode);
    }

    uint8_t *bytes = BufferData(stream);
    size_t len = BufferLength(stream);
    if(len <= 0){
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Stream buffer can't be empty!")));
    }

    if (retType == "array")
    {
        uint8_t sumArry[2];
        // CRC32CheckSum(bytes, len, sumArry);

        v8::Local<Array> sumForReturn = v8::Array::New(isolate);
        sumForReturn->Set(0, v8::Number::New(isolate, sumArry[0]));
        sumForReturn->Set(1, v8::Number::New(isolate, sumArry[1]));

        args.GetReturnValue().Set(sumForReturn);
    }
    else if(retType == "int")
    {
        uint16_t sum;
        // CRC32CheckSum(bytes, len, &sum);
        args.GetReturnValue().Set(v8::Number::New(isolate, sum));
    }
    else
    {
        char *sumStr = CRC32CheckSum(bytes, len);
        args.GetReturnValue().Set(String::NewFromUtf8(isolate, sumStr));
    }
}

// void CRC32VerifySum(const FunctionCallbackInfo<Value> &args)
// {
//     Isolate *isolate = args.GetIsolate();
//     if (!args[0]->IsObject())
//     {
//         isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Input Stream is Buffer required!")));
//     }

//     Handle<Value> stream = Handle<Value>::Cast(args[0]);
//     uint8_t *bytes = BufferData(stream);
//     size_t len = BufferLength(stream);
//     if(len <= 0){
//         isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Stream buffer can't be empty!")));
//     }

//     bool isValid = false;
//     isValid = CRC32VerifySum(bytes, len);

//     args.GetReturnValue().Set(Boolean::New(isolate, isValid));
// }

void init(Local<Object> exports)
{
    NODE_SET_METHOD(exports, "checkSum", CRC32CheckSum);
    // NODE_SET_METHOD(exports, "verifySum", CRC32VerifySum);
}

NODE_MODULE(crc32, init)
