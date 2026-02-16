#pragma once

#include "pch.h"
#include "Debugger/DebugTypes.h"

#define DUMMYCPU
#define LynxCpu DummyLynxCpu
#include "Lynx/LynxCpu.h"
#undef LynxCpu
#undef DUMMYCPU
