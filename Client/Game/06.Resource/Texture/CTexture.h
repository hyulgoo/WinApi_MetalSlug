#pragma once
#include "Game\06.Resource\CRes.h"

struct tColor
{
    BYTE red;
    BYTE green;
    BYTE blue;

    bool operator==(const tColor& _other) const
    {
        return red == _other.red && green == _other.green && blue == _other.blue;
    }

    bool operator!=(const tColor& _other) const
    {
        return !(*this == _other);
    }
};

class CTexture :
    public CRes
{
private:
    HBITMAP m_hBit;
    HDC     m_hDC;

    BITMAP m_tBitmapInfo;

public:
    UINT Width() const { return m_tBitmapInfo.bmWidth; }
    UINT Height() const { return m_tBitmapInfo.bmHeight; }

    HDC GetDC() const { return m_hDC; }

    void   Resize(UINT _width, UINT _height);
    tColor GetPixel(int _ix, int _iy) const;

private:
    virtual int Load(const wstring& _strFilePath) override;
    void        Create(UINT _iWidth, UINT _iHeight);

public:
    CTexture();
    virtual ~CTexture() override;

    friend class CResMgr;
};
