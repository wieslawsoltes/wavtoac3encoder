#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <utility>
#include <iostream>

namespace util
{
    class Option
    {
    public:
        int Id;
        std::vector<std::wstring> Args;
        int nParams;
    };

    class Result
    {
    public:
        int Id;
        std::wstring Arg;
        std::vector<std::wstring> Params;
    };

    class ArgvParser
    {
    public:
        bool bVerbose = false;
    public:
        int FindOption(const std::wstring& arg, const std::vector<Option>& options)
        {
            for (size_t i = 0; i < options.size(); i++)
            {
                auto& args = options[i].Args;
                auto it = std::find(std::begin(args), std::end(args), arg);
                if (it != std::end(args)) 
                    return i;
            }
            return -1;
        }
    public:
        bool HasDuplicates(std::vector<Result>& results)
        {
            std::sort(results.begin(), results.end(), [](Result& a, Result &b) { return a.Id < b.Id; });
            auto unique = std::unique(results.begin(), results.end(), [](Result& a, Result &b) { return a.Id == b.Id; });
            return unique != results.end();
        }
    public:
        int ParseOptions(const int argc, const wchar_t *argv[], const std::vector<Option> options, std::vector<Result>& results)
        {
            for (int i = 1; i < argc; i++)
            {
                std::wstring arg = argv[i];
                if (bVerbose)
                    std::wcout << L"Input arg: " << arg << std::endl;

                int index = FindOption(arg, options);
                if (index != -1)
                {
                    auto& option = options[index];
                    if (bVerbose)
                        std::wcout << L"Found option: " << arg << L", id: " << option.Id << L", params: " << option.nParams << std::endl;

                    std::vector<std::wstring> params;
                    if (option.nParams == -1)
                    {
                        // unlimited number of params expected, minimum one required
                        int j = i + 1;
                        while (j < argc)
                        {
                            std::wstring param = argv[j];
                            if (param[0] == '-')
                                break;

                            if (bVerbose)
                                std::wcout << L"Input param: " << param << std::endl;

                            params.emplace_back(param);
                            j++;
                            i++;
                        }
                        if (params.size() < 1)
                        {
                            if (bVerbose)
                                std::wcout << L"At leat one param is required" << std::endl;
                            return -1;
                        }
                    }
                    else if (option.nParams > 0)
                    {
                        // specific number of params required
                        if (i + option.nParams < argc)
                        {
                            for (int j = i + 1; j < i + option.nParams + 1; j++)
                            {
                                std::wstring param = argv[j];
                                if (bVerbose)
                                    std::wcout << L"Input param: " << param << std::endl;
                                if (param[0] == '-')
                                {
                                    if (bVerbose)
                                        std::wcout << L"Invalid param: " << param << std::endl;
                                    return -1;
                                }
                                
                                if (bVerbose)
                                    std::wcout << L"Found param: " << param << std::endl;
                                params.emplace_back(param);
                            }
                            i += option.nParams;
                        }
                        else
                        {
                            if (bVerbose)
                                std::wcout << L"Invalid number of params for: " << arg << std::endl;
                            return -1;
                        }
                    }

                    if (bVerbose)
                    {
                        std::wcout << L"Result: " << arg << L", id: " << option.Id << L", params:";
                        for (auto& param : params)
                            std::wcout << L" " << param;
                        std::wcout << std::endl << std::endl;
                    }

                    Result result { option.Id, arg, params };
                    results.emplace_back(result);
                }
                else
                {
                    if (bVerbose)
                        std::wcout << L"Invalid option: " << argv[i] << std::endl;
                    return -1;
                }
            }

            bool bHasDuplicates = HasDuplicates(results);
            if (bHasDuplicates)
            {
                if (bVerbose)
                    std::wcout << L"Duplicate options found." << std::endl;
                return -1;
            }

            return 0;
        }
    };
}
