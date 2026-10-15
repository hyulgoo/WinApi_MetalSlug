#include "pch.h"
#include "Game\06.Resource\Texture\CTexture.h"

#include "Game\01.Engine\CEngine.h"

CTexture::CTexture()
    : m_hBit(nullptr)
    , m_hDC(nullptr)
    , m_hOldBit(nullptr)
    , m_tBitmapInfo{}
{
}

CTexture::~CTexture()
{
    Release();
}

void CTexture::Release()
{
    // DC에 선택된 비트맵은 지우면 안 된다(문서상 실패): 원래 비트맵을 되돌린 뒤 비트맵과 DC를 지운다
    if (nullptr != m_hDC)
    {
        if (nullptr != m_hOldBit)
            SelectObject(m_hDC, m_hOldBit);
        DeleteDC(m_hDC);
    }
    if (nullptr != m_hBit)
        DeleteObject(m_hBit);

    m_hBit        = nullptr;
    m_hDC         = nullptr;
    m_hOldBit     = nullptr;
    m_tBitmapInfo = {};
}

bool CTexture::CreateDIB(const UINT _iWidth, const UINT _iHeight, HBITMAP& _hBit, HDC& _hDC, HBITMAP& _hOldBit)
{
    // DIB 섹션으로 생성해, bmBits로 픽셀 버퍼에 직접 접근할 수 있게 함
    BITMAPINFO tBmi              = {};
    tBmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    tBmi.bmiHeader.biWidth       = static_cast<LONG>(_iWidth);
    tBmi.bmiHeader.biHeight      = static_cast<LONG>(_iHeight); // 양수면 bottom-up DIB
    tBmi.bmiHeader.biPlanes      = 1;
    tBmi.bmiHeader.biBitCount    = 24;
    tBmi.bmiHeader.biCompression = BI_RGB;

    void* pBits = nullptr;
    _hBit       = CreateDIBSection(CEngine::GetInst()->GetMainDC(), &tBmi, DIB_RGB_COLORS, &pBits, nullptr, 0);
    _hDC        = CreateCompatibleDC(CEngine::GetInst()->GetMainDC());
    _hOldBit    = static_cast<HBITMAP>(SelectObject(_hDC, _hBit));
    return nullptr != _hBit && nullptr != _hDC;
}

void CTexture::Resize(const UINT _width, const UINT _height)
{
    // 크기가 같으면 다시 만들 필요가 없다 (스테이지·에디터에 들어갈 때마다 불린다)
    if (static_cast<LONG>(_width) == m_tBitmapInfo.bmWidth && static_cast<LONG>(_height) == m_tBitmapInfo.bmHeight)
        return;

    // 새 해상도로 화면을 담을 비트맵(DIB 섹션)과 DC를 새로 생성
    HBITMAP hNewBit = nullptr;
    HDC     hNewDC  = nullptr;
    HBITMAP hNewOld = nullptr;
    CreateDIB(_width, _height, hNewBit, hNewDC, hNewOld);

    // 새 DC에 기존 그림 복사
    BitBlt(hNewDC, 0, 0, m_tBitmapInfo.bmWidth, m_tBitmapInfo.bmHeight, m_hDC, 0, 0, SRCCOPY);

    // 기존 비트맵과 DC를 해제하고 새 것으로 바꾼다
    Release();
    m_hBit    = hNewBit;
    m_hDC     = hNewDC;
    m_hOldBit = hNewOld;
    GetObject(m_hBit, sizeof(BITMAP), &m_tBitmapInfo);
}


int CTexture::Load(const wstring& _strFilePath)
{
    // Bitmap 로드
    m_hBit = static_cast<HBITMAP>(LoadImage(nullptr, _strFilePath.c_str(), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE));
    GetObject(m_hBit, sizeof(BITMAP), &m_tBitmapInfo);

    // Bitmap을 그려 줄 DC 생성. 원래 선택돼 있던 비트맵은 해제할 때 되돌린다
    m_hDC     = CreateCompatibleDC(CEngine::GetInst()->GetMainDC());
    m_hOldBit = static_cast<HBITMAP>(SelectObject(m_hDC, m_hBit));

    return S_OK;
}

void CTexture::Create(const UINT _iWidth, const UINT _iHeight)
{
    CreateDIB(_iWidth, _iHeight, m_hBit, m_hDC, m_hOldBit);
    GetObject(m_hBit, sizeof(BITMAP), &m_tBitmapInfo);
}

tColor CTexture::GetPixel(const int _ix, const int _iy) const
{
    const int iWidth  = static_cast<int>(m_tBitmapInfo.bmWidth);
    const int iHeight = static_cast<int>(m_tBitmapInfo.bmHeight);

    // DIB 섹션이 아니거나(bmBits == nullptr) 범위를 벗어나면 유효한 픽셀이 아니므로 샘플링하지 않음
    if (nullptr == m_tBitmapInfo.bmBits || _ix < 0 || _iy < 0 || _ix >= iWidth || _iy >= iHeight)
        return tColor{0, 0, 0};

    // DIB는 bottom-up이므로 y좌표를 뒤집어서 변환
    const int iFlippedY = iHeight - (_iy + 1);

    // 한 줄의 픽셀 바이트 수 계산 후 4바이트 단위로 정렬
    int iWideByte = iWidth * static_cast<int>(sizeof(tColor));
    iWideByte     = (iWideByte + 3) & ~3;

    BYTE*       pByte  = static_cast<BYTE*>(m_tBitmapInfo.bmBits);
    const BYTE* pPixel = pByte + static_cast<size_t>(iWideByte) * iFlippedY + (static_cast<size_t>(_ix) * 3);

    // DIB 픽셀은 BGR 순서로 저장되어 있으므로 그 순서에 맞춰 매핑
    tColor tRet = {};
    tRet.blue  = pPixel[0];
    tRet.green = pPixel[1];
    tRet.red   = pPixel[2];
    return tRet;
}
