#ifndef BASEACTION_H
#define BASEACTION_H
#include "lowlvlenginescript.h"


class BaseAction : public LowLvlEngineScript
{
    ~BaseAction();
public:
    BaseAction();
    virtual void init();
};

#endif // BASEACTION_H
