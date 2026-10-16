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
    tBmi.bmiHeader.biBitCount    = 32;   // 화면·불러온 텍스처(모두 32비트)와 같게. 24비트면 블릿마다 변환이 든다
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
    if (nullptr == m_hBit)
    {
        // 파일이 없거나 BMP가 아니다. 원인을 남긴다 (예전에는 실패를 보지 않고 빈 텍스처를 캐시했다)
        const wstring strMsg = L"텍스처를 불러오지 못함 : " + _strFilePath;
        ReportError(L"텍스처 로드 실패", strMsg.c_str());
        return E_FAIL;
    }
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

void CTexture::Tint(const COLORREF _tColor)
{
    if (nullptr == m_tBitmapInfo.bmBits || 32 != m_tBitmapInfo.bmBitsPixel)
        return;

    // 앞서 이 비트맵에 그린 GDI 작업(복사)이 끝난 뒤 픽셀을 고친다
    GdiFlush();

    // 32비트 DIB 픽셀은 메모리에 B, G, R, (빈 바이트) 순서라 DWORD로 읽으면 0x00RRGGBB
    constexpr DWORD MAGENTA = 0x00FF00FF;
    const DWORD     iR      = GetRValue(_tColor);
    const DWORD     iG      = GetGValue(_tColor);
    const DWORD     iB      = GetBValue(_tColor);
    DWORD*          pPixel  = static_cast<DWORD*>(m_tBitmapInfo.bmBits);
    const size_t    iCount  = static_cast<size_t>(m_tBitmapInfo.bmWidth) * static_cast<size_t>(m_tBitmapInfo.bmHeight);
    for (size_t i = 0; i < iCount; ++i)
    {
        const DWORD iColor = pPixel[i] & 0x00FFFFFF;
        if (MAGENTA == iColor)
            continue;

        const DWORD iLum = (((iColor >> 16) & 0xFF) * 299 + ((iColor >> 8) & 0xFF) * 587 + (iColor & 0xFF) * 114) / 1000;
        DWORD       iOut = ((iR * iLum / 255) << 16) | ((iG * iLum / 255) << 8) | (iB * iLum / 255);
        if (MAGENTA == iOut)
            iOut = MAGENTA - 1;   // 칠한 색이 투명색이 되지 않게
        pPixel[i] = iOut;
    }
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

    // 픽셀 하나의 바이트 수(24비트 3, 32비트 4). 한 줄은 4바이트 단위로 정렬돼 있다
    // (예전에는 24비트만 가정해, 32비트로 저장된 충돌 맵을 직접 읽으면 엉뚱한 곳을 읽었다)
    const int iBytePerPixel = m_tBitmapInfo.bmBitsPixel / 8;
    if (iBytePerPixel < 3)
        return tColor{0, 0, 0};
    const int iWideByte = (iWidth * iBytePerPixel + 3) & ~3;

    BYTE*       pByte  = static_cast<BYTE*>(m_tBitmapInfo.bmBits);
    const BYTE* pPixel = pByte + static_cast<size_t>(iWideByte) * iFlippedY + static_cast<size_t>(_ix) * iBytePerPixel;

    // DIB 픽셀은 BGR 순서로 저장되어 있으므로 그 순서에 맞춰 매핑
    tColor tRet = {};
    tRet.blue  = pPixel[0];
    tRet.green = pPixel[1];
    tRet.red   = pPixel[2];
    return tRet;
}
