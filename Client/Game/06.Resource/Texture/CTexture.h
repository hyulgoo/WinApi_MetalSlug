#pragma once
#include "Game\06.Resource\CRes.h"

struct tColor
{
    BYTE red   = 0;
    BYTE green = 0;
    BYTE blue  = 0;

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
    HBITMAP m_hOldBit;     // DC를 만들 때 원래 선택돼 있던 비트맵. 지우기 전에 되돌린다

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
    void        Release();

    // 24비트 DIB 섹션과 그것을 선택한 메모리 DC를 만든다
    static bool CreateDIB(UINT _iWidth, UINT _iHeight, HBITMAP& _hBit, HDC& _hDC, HBITMAP& _hOldBit);

public:
    CTexture();
    virtual ~CTexture() override;

    friend class CResMgr;
};
