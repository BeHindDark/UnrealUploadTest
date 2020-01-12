#pragma once

#include "Engineminimal.h"

DECLARE_LOG_CATEGORY_EXTERN(WJT, Log, All);

#define CODECHECK(Expr) {UE_LOG(OHS, Error, TEXT("%s ASSERTION : %s"), *LINE_INFO, *FString::Printf(TEXT("'"#Expr"'")))}
