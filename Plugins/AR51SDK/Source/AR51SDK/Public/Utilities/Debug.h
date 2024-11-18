#pragma once

#include "CoreMinimal.h"
#include <iostream>
#include <string>
#include "Engine.h"


DECLARE_LOG_CATEGORY_CLASS(AR51SDK, All, All);

class AR51SDK_API Debug
{
public:
    static void Display(const FString& msg, FColor color = FColor::Green)
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 10.f, color, msg, true, FVector2D::UnitVector * 1.5f);
        }
    }

    template<typename FormatType, typename... ArgsType>
    inline static void Log(const FormatType& format, const FString& function, int line, ArgsType... args)
    {
        FString msg = FString::Printf(TEXT("%s [%d]: %s"), *function, line, *FString::Printf(format, args...));
        UE_LOG(AR51SDK, Log, TEXT("%s"), *msg);
    }


    template<typename FormatType, typename... ArgsType>
    inline static void Error(const FormatType& format, const FString& function, int line, ArgsType... args)
    {
        FString msg = FString::Printf(TEXT("%s [%d]: %s"), *function, line, *FString::Printf(format, args...));
        UE_LOG(AR51SDK, Error, TEXT("%s"), *msg);
    }
};


#define LOG1(format) Debug::Log(TEXT(format), FString(__FUNCTION__), __LINE__, "r", "DFSAFD")
#define LOG2(format, ...) Debug::Log(TEXT(format), FString(__FUNCTION__), __LINE__, __VA_ARGS__)

#define EXPAND( x ) x

#define COUNT_ARGS(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define GET_MACRO(...)  EXPAND( COUNT_ARGS(__VA_ARGS__, LOG2, LOG2, LOG2, LOG2, LOG2, LOG2, LOG2, LOG2, LOG2, LOG1) )


#define LOG(...)   EXPAND(GET_MACRO(__VA_ARGS__) (__VA_ARGS__))

//void A() {
//    Debug::Log(TEXT("FDASF %d"), FString("fdsf"), 50, 1);
//}