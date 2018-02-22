#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <stdio.h>
#include <tchar.h>

#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <algorithm>
#include <thread>

#include "logger\Log.h"
#include "utilities\StringHelper.h"
#include "utilities\TimeCount.h"
#include "utilities\Utilities.h"
#include "avs\src\Avs2Raw\Avs2Raw.h"
#include "configuration\Configuration.h"
#include "worker\Worker.h"
#include "ArgvParser.h"
