#pragma once
#include "CRes.h"

struct tColor
{
    BYTE red;
    BYTE green;
    BYTE blue;
};

class CTexture :
    public CRes
{
private:
    HBITMAP     m_hBit;
    HDC         m_hDC;

    BITMAP      m_tBitmapInfo;

public:  
    UINT Width() { return m_tBitmapInfo.bmWidth; }
    UINT Height() { return m_tBitmapInfo.bmHeight; }

    HDC GetDC() { return m_hDC; }

    void Resize(UINT _width, UINT _height);
    tColor GetPixel(UINT _ix, UINT _iy);

private:
    virtual int Load(const wstring& _strFilePath) override;
    void Create(UINT _iWidth, UINT _iHeight);
public:
    CTexture();
    ~CTexture();

   friend class CResMgr;
};

