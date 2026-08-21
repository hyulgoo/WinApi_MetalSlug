#include "pch.h"
#include "CTexture.h"

#include "CEngine.h"

CTexture::CTexture()
    : m_hBit(nullptr)
    , m_hDC(nullptr)
    , m_tBitmapInfo{}
{
}

CTexture::~CTexture()
{
    DeleteObject(m_hBit);
    DeleteDC(m_hDC);
}

void CTexture::Resize(const UINT _width, const UINT _height)
{
    // 새 해상도로 화면을 담을 비트맵(DIB 섹션)과 DC를 새로 생성
    BITMAPINFO tBmi              = {};
    tBmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    tBmi.bmiHeader.biWidth       = static_cast<LONG>(_width);
    tBmi.bmiHeader.biHeight      = static_cast<LONG>(_height); // 양수면 bottom-up DIB
    tBmi.bmiHeader.biPlanes      = 1;
    tBmi.bmiHeader.biBitCount    = 24;
    tBmi.bmiHeader.biCompression = BI_RGB;

    void*         pBits    = nullptr;
    const HBITMAP hNewBit  = CreateDIBSection(CEngine::GetInst()->GetMainDC(), &tBmi, DIB_RGB_COLORS, &pBits, nullptr, 0);
    const HDC     hNewDC   = CreateCompatibleDC(CEngine::GetInst()->GetMainDC());
    const HBITMAP hPrevBit = static_cast<HBITMAP>(SelectObject(hNewDC, hNewBit));
    DeleteObject(hPrevBit);

    // 새 DC에 기존 그림 복사
    BitBlt(hNewDC, 0, 0, m_tBitmapInfo.bmWidth, m_tBitmapInfo.bmHeight, m_hDC, 0, 0, SRCCOPY);

    // 기존 비트맵과 DC 해제
    DeleteObject(m_hBit);
    DeleteDC(m_hDC);

    // 새 비트맵과 DC를 기본으로 세팅
    m_hBit = hNewBit;
    m_hDC  = hNewDC;

    // 비트맵 정보 갱신
    GetObject(m_hBit, sizeof(BITMAP), &m_tBitmapInfo);
}


int CTexture::Load(const wstring& _strFilePath)
{
    // Bitmap 로드
    m_hBit = static_cast<HBITMAP>(LoadImage(nullptr, _strFilePath.c_str(), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE));
    GetObject(m_hBit, sizeof(BITMAP), &m_tBitmapInfo);

    // Bitmap을 그려 줄 DC 생성
    m_hDC                  = CreateCompatibleDC(CEngine::GetInst()->GetMainDC());
    const HBITMAP hPrevBit = static_cast<HBITMAP>(SelectObject(m_hDC, m_hBit));
    DeleteObject(hPrevBit);

    return S_OK;
}

void CTexture::Create(const UINT _iWidth, const UINT _iHeight)
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
    m_hBit      = CreateDIBSection(CEngine::GetInst()->GetMainDC(), &tBmi, DIB_RGB_COLORS, &pBits, nullptr, 0);

    m_hDC = CreateCompatibleDC(CEngine::GetInst()->GetMainDC());

    const HBITMAP hPrevBit = static_cast<HBITMAP>(SelectObject(m_hDC, m_hBit));
    DeleteObject(hPrevBit);

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
    tColor tRet;
    tRet.blue  = pPixel[0];
    tRet.green = pPixel[1];
    tRet.red   = pPixel[2];
    return tRet;
}
