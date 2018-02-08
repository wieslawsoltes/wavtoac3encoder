#pragma once

#include <string>
#include "utilities\TimeCount.h"

namespace worker
{
    class CTimeCount
    {
        util::CTimeCount tc;
    public:
        void Start();
        void Stop();
        double ElapsedMilliseconds();
        std::wstring Formatted();
    };
}
