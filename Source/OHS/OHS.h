#pragma once

#include "Engine.h"
#include "EngineMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(OHS, Log, All);

//로그가 출력되는 함수이름 및 위치를 알려줌
#define LINE_INFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))

#define CHECK_LOG(Expr) {UE_LOG(OHS, Error, TEXT("%s ASSERTION : %s"), *LINE_INFO, *FString::Printf(TEXT("'"#Expr"'")))}

//#define ABLOG(Verbosity, Format, ...) UE_LOG(ArenaBattle, Verbosity, TEXT("%s%s"), *ABLOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

//#define ABCHECK(Expr, ...) { if(!(Expr)){ABLOG(Error, TEXT("ASSERTION : %s"), TEXT("'"#Expr"'")); return __VA_ARGS__;}}