#pragma once
#include "Game\06.Resource\CRes.h"
class CSound :
    public CRes
{
private:

private:
    virtual int Load(const wstring& _strFilePath) override {}
    
public:
    CSound()                   = default;
    virtual ~CSound() override = default;
};

