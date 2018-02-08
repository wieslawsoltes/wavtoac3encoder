#pragma once

#include <string>
#include <memory>

namespace worker
{
    class ITimeCount
    {
    public:
        virtual void Start() = 0;
        virtual void Stop() = 0;
        virtual double ElapsedMilliseconds() = 0;
        virtual std::wstring Formatted() = 0;
    };

    class CTimeCount
    {
    private:
        std::unique_ptr<ITimeCount> tc;
    public:
        CTimeCount();
    public:
        void Start();
        void Stop();
        double ElapsedMilliseconds();
        std::wstring Formatted();
    };
}
