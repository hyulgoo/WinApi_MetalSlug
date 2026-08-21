#pragma once
#include "CLevel.h"
class CStage01Level :
    public CLevel
{
public:
    virtual void init() override;
    virtual void tick() override;
    virtual void Enter() override;
    virtual void Exit() override;

public:
    CStage01Level();
    ~CStage01Level();
};

