#include "StdAfx.h"
#include "TimeCount.h"
#include "utilities\TimeCount.h"

namespace worker
{
    class CTimeCountImpl : public ITimeCount
    {
        util::CTimeCount tc;
    public:
        void Start()
        {
            this->tc.Start();
        }
        void Stop()
        {
            this->tc.Stop();
        }
        double ElapsedMilliseconds()
        {
            return this->tc.ElapsedTime().count();
        }
        std::wstring Formatted()
        {
            return util::CTimeCount::Format(this->tc.ElapsedTime());
        }
    };

    CTimeCount::CTimeCount()
    {
        this->tc = std::make_unique<CTimeCountImpl>();
    }

    void CTimeCount::Start() 
    {
        this->tc->Start();
    }

    void CTimeCount::Stop() 
    {
        this->tc->Stop();
    }

    double CTimeCount::ElapsedMilliseconds() 
    {
        return this->tc->ElapsedMilliseconds();
    }

    std::wstring CTimeCount::Formatted() 
    {
        return this->tc->Formatted();
    }
}
