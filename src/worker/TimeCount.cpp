#include "StdAfx.h"
#include "TimeCount.h"

namespace worker
{
    void CTimeCount::Start() 
    {
        this->tc.Start();
    }

    void CTimeCount::Stop() 
    {
        this->tc.Stop();
    }

    double CTimeCount::ElapsedMilliseconds() 
    {
        return this->tc.ElapsedTime().count();
    }

    std::wstring CTimeCount::Formatted() 
    {
        return util::CTimeCount::Format(this->tc.ElapsedTime());
    }
}
