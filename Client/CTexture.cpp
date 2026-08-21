#include "pch.h"
#include "CTexture.h"

#include "CEngine.h"

CTexture::CTexture()
	: m_hBit(nullptr)
	, m_hDC(nullptr)
	, m_tBitmapInfo {}
{
}

CTexture::~CTexture()
{
	DeleteObject(m_hBit);
	DeleteDC(m_hDC);
}

void CTexture::Resize(UINT _width, UINT _height)
{
	// 새로운 해상도로 화면을 생성할 비트맵, DC 생성
	HBITMAP hNewBit = CreateCompatibleBitmap(CEngine::GetInst()->GetMainDC(), _width, _height);
	HDC		hNewDC = CreateCompatibleDC(CEngine::GetInst()->GetMainDC());
	HBITMAP hPrevBit = (HBITMAP)SelectObject(hNewDC, hNewBit);
	DeleteObject(hPrevBit);

	// 새로운 DC로 기존 그림 복사
	BitBlt(hNewDC, 0, 0, m_tBitmapInfo.bmWidth, m_tBitmapInfo.bmHeight, m_hDC, 0, 0, SRCCOPY);

	// 이전 비트맵 및 DC 삭제
	DeleteObject(m_hBit);
	DeleteDC(m_hDC);

	// 새로운 비트맵과 DC를 기본으로 설정
	m_hBit = hNewBit;
	m_hDC = hNewDC;

	// 비트맵 정보 갱신
	GetObject(m_hBit, sizeof(BITMAP), &m_tBitmapInfo);
}


int CTexture::Load(const wstring& _strFilePath)
{
	// Bitmap 로딩
    m_hBit = (HBITMAP)LoadImage(nullptr, _strFilePath.c_str(), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	GetObject(m_hBit, sizeof(BITMAP), &m_tBitmapInfo);

    // Bitmap 과 연결 시킬 DC 생성
    m_hDC = CreateCompatibleDC(CEngine::GetInst()->GetMainDC());
    HBITMAP hPrevBit = (HBITMAP)SelectObject(m_hDC, m_hBit);
    DeleteObject(hPrevBit);

	return S_OK;
}

void CTexture::Create(UINT _iWidth, UINT _iHeight)
{
	m_hBit = CreateCompatibleBitmap(CEngine::GetInst()->GetMainDC(), _iWidth, _iHeight);

	m_hDC = CreateCompatibleDC(CEngine::GetInst()->GetMainDC());

	HBITMAP hPrevBit = (HBITMAP)SelectObject(m_hDC, m_hBit);
	DeleteObject(hPrevBit);

	GetObject(m_hBit, sizeof(BITMAP), &m_tBitmapInfo);

}

tColor CTexture::GetPixel(UINT _ix, UINT _iy)
{
	//비트맵 정보 가져오기
	UINT iWidth = m_tBitmapInfo.bmWidth;
	UINT iHeight = m_tBitmapInfo.bmHeight;
	_iy = iHeight - (_iy + 1);
	//픽셀 개수 계산
	UINT iWideByte = iWidth * sizeof(tColor);
	//4바이트로 맞춰주기
	iWideByte += 4 - iWideByte % 4;
	//픽셀 입력
	BYTE* pByte = (BYTE*)m_tBitmapInfo.bmBits;
	tColor* pColor = (tColor*)(pByte + iWideByte * (_iy)+(_ix * 3));
	return *pColor;
}
