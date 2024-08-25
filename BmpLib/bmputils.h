#pragma once

namespace BmpZipper
{

struct IProgressNotifier
{
    virtual ~IProgressNotifier() = default;

    virtual void init(int min, int max) = 0;

    virtual void notifyProgress(int current) = 0;
};

}
