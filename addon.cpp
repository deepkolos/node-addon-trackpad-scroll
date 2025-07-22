#include <napi.h>
#include <CoreGraphics/CoreGraphics.h>
#include <CoreFoundation/CoreFoundation.h>

Napi::FunctionReference jsCallback;

CGEventRef eventCallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon)
{
    if (type == kCGEventScrollWheel)
    {
        double deltaY = CGEventGetDoubleValueField(event, kCGScrollWheelEventPointDeltaAxis1);
        double deltaX = CGEventGetDoubleValueField(event, kCGScrollWheelEventPointDeltaAxis2);

        int64_t phase = CGEventGetIntegerValueField(event, kCGScrollWheelEventScrollPhase);
        int64_t momentumPhase = CGEventGetIntegerValueField(event, kCGScrollWheelEventMomentumPhase);
        bool isTrackpad = (phase != 0) || (momentumPhase != 0);

        if (!jsCallback.IsEmpty())
        {
            Napi::Env env = jsCallback.Env();
            Napi::HandleScope scope(env);
            Napi::Object obj = Napi::Object::New(env);
            obj.Set("deltaX", Napi::Number::New(env, deltaX));
            obj.Set("deltaY", Napi::Number::New(env, deltaY));
            obj.Set("isTrackpad", Napi::Boolean::New(env, isTrackpad));
            jsCallback.Call({obj});
        }
    }
    return event;
}

void StartListening(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsFunction())
    {
        Napi::TypeError::New(env, "Function expected").ThrowAsJavaScriptException();
        return;
    }

    jsCallback = Napi::Persistent(info[0].As<Napi::Function>());

    CFMachPortRef eventTap;
    CFRunLoopSourceRef runLoopSource;

    eventTap = CGEventTapCreate(kCGSessionEventTap, kCGHeadInsertEventTap, kCGEventTapOptionDefault,
                                kCGEventMaskForAllEvents, eventCallback, NULL);
    if (!eventTap)
    {
        Napi::Error::New(env, "Failed to create event tap").ThrowAsJavaScriptException();
        return;
    }

    runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
    CGEventTapEnable(eventTap, true);

    CFRunLoopRun();
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    exports.Set(Napi::String::New(env, "onScroll"), Napi::Function::New(env, StartListening));
    return exports;
}

NODE_API_MODULE(addon, Init)