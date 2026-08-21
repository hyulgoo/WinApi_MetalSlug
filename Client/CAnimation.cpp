#include "pch.h"
#include "CAnimation.h"

#include "CObj.h"
#include "CAnimator.h"
#include "CTexture.h"
#include "CCamera.h"
#include "CTimeMgr.h"
#include "CPathMgr.h"
#include "CResMgr.h"
#include "CEngine.h"
#include <cmath>


CAnimation::CAnimation(CAnimator* _pAnimator)
	: m_pAnimator(_pAnimator)
	, m_iCurFrm(0)
	, m_fAccTime(0.f)
	, m_bFinish(false)
	, m_pAtlas(nullptr)
	, m_pBackBuffer(nullptr)
	, m_fmagni(3.5)
	, m_bReverse(false)
	, m_vOffset{}
	, m_tPrintMode(SETPRINT::NORMAL)
{
	// 애니메이션을 키운 후 마젠타색을 없애기 위한 백버퍼
	Vec2 vResolution = CEngine::GetInst()->GetResolution();
	m_pBackBuffer = CResMgr::GetInst()->CreateTexture(L"AnimBackBuffer", (int)vResolution.x, (int)vResolution.y);
	SetDCBrushColor(m_pBackBuffer->GetDC(), RGB(255, 0, 255));
	HBRUSH hBrush = (HBRUSH)GetStockObject(DC_BRUSH);
	Rectangle(m_pBackBuffer->GetDC(), -1, -1, (int)vResolution.x + 1, (int)vResolution.y + 1);
}

CAnimation::~CAnimation()
{
}

void CAnimation::init(const wstring& _strName, CTexture* _pAtlas, Vec2 _vLeftTop, Vec2 _vSize, Vec2 _vOffset, int _iMaxFrmCount, float _fDuration)
{
	// 출력할 애니메이션 초기화, 이름 입력 및 텍스쳐 시작위치, 자를 사이즈, 지속시간 입력하여 넣기
	SetName(_strName);
	m_pAtlas = _pAtlas;

	for (int i = 0; i < _iMaxFrmCount; ++i)
	{
		tAnimFrm frm = {};

		frm.vLeftTop = Vec2(_vLeftTop.x + (float)i * _vSize.x, _vLeftTop.y);
		frm.vSize = _vSize;
		frm.vOffset = _vOffset;
		frm.fDuration = _fDuration;
		m_vecFrm.push_back(frm);
	}
}

void CAnimation::tick()
{
	if (m_bFinish) // 프레임 재생이 끝났다면 return
		return;
	m_fAccTime += DT;

	if (m_vecFrm[m_iCurFrm].fDuration < m_fAccTime) // AccTime이 재생 지속시간을 넘어가면 다음 프레임으로
	{
		++m_iCurFrm;
		m_fAccTime = 0.f;

		if (m_vecFrm.size() <= m_iCurFrm) // 프레임 사이즈와 같거나 커지면 현재 프레임을 최대로 고정하고 
		{	
			m_iCurFrm = (int)(m_vecFrm.size() - 1);
			m_bFinish = true;
		}
	}
}

void CAnimation::render(HDC _dc)
{
	Vec2 vPos = {};
	vPos = m_pAnimator->GetOwner()->GetPos();

	tAnimFrm frm = m_vecFrm[m_iCurFrm];
	if (m_tPrintMode == SETPRINT::UPPER)
	{
		m_vOffset = Vec2(-5.7f, -25.f);
	}

	else if (m_tPrintMode == SETPRINT::LOWER)
	{
		m_vOffset = Vec2(-5.7f, 0.f);
	}

	else if (m_tPrintMode == SETPRINT::NORMAL)
	{
		m_vOffset = {};
	}

	if (m_bReverse == true)
	{
		StretchBlt(m_pBackBuffer->GetDC()
			, 0, 0
			, int(frm.vSize.x * m_fmagni), int(frm.vSize.y * m_fmagni)
			, m_pAtlas->GetDC()
			, int(frm.vLeftTop.x + frm.vSize.x - 1)
			, int(frm.vLeftTop.y)
			, -int(frm.vSize.x)
			, int(frm.vSize.y), SRCCOPY);
		TransparentBlt(_dc
			, int(vPos.x - (frm.vOffset.x * m_fmagni) - (m_vOffset.x * m_fmagni) - (frm.vSize.x * m_fmagni))
			, int(vPos.y + (frm.vOffset.y * m_fmagni) + (m_vOffset.y * m_fmagni))
			, int(frm.vSize.x * m_fmagni)
			, int(frm.vSize.y * m_fmagni)
			, m_pBackBuffer->GetDC()
			, 0, 0
			, int(frm.vSize.x * m_fmagni), int(frm.vSize.y * m_fmagni)
			, RGB(255, 0, 255));
	}
	else if (m_bReverse == false)
	{
		StretchBlt(m_pBackBuffer->GetDC()
			, 0, 0
			, int(frm.vSize.x * m_fmagni), int(frm.vSize.y * m_fmagni)
			, m_pAtlas->GetDC()
			, int(frm.vLeftTop.x)
			, int(frm.vLeftTop.y)
			, int(frm.vSize.x)
			, int(frm.vSize.y), SRCCOPY);
		TransparentBlt(_dc
			, int(vPos.x + (frm.vOffset.x * m_fmagni) + (m_vOffset.x * m_fmagni))
			, int(vPos.y + (frm.vOffset.y * m_fmagni) + (m_vOffset.y * m_fmagni))
			, int(frm.vSize.x * m_fmagni)
			, int(frm.vSize.y * m_fmagni)
			, m_pBackBuffer->GetDC()
			, 0, 0
			, int(frm.vSize.x * m_fmagni), int(frm.vSize.y * m_fmagni)
			, RGB(255, 0, 255));
	}
	
}
void CAnimation::NextFrm()
{
	if (m_iCurFrm == m_vecFrm.size() - 1)
		return;
	++m_iCurFrm;
}
void CAnimation::PrevFrm()
{
	if (m_iCurFrm == 0)
		return;
	--m_iCurFrm;
}
void CAnimation::SetCurFrmInfo(tAnimFrm _Other)
{
	m_vecFrm[m_iCurFrm] = _Other;
}
void CAnimation::SetAllFrmInfo(vector<tAnimFrm> _Other)
{
	m_vecFrm = _Other;
}
// dc, 텍스쳐 좌상단, 자를 사이즈, 텍스쳐 dc, 자르기 시작할 위치, 자를 사이즈, 제외할 색깔
void CAnimation::Save(const wstring& _strRelativePath)
{
	wstring strFilepath = CPathMgr::GetInst()->GetContentPath();
	strFilepath += _strRelativePath;

	// 파일 입출력
	FILE* pFile = nullptr;
	errno_t iErrNum = _wfopen_s(&pFile, strFilepath.c_str(), L"wb");

	if (nullptr == pFile)
	{
		wchar_t szStr[256] = {};
		wsprintf(szStr, L"Animation Save 실패, Error Number : %d", iErrNum);
		MessageBox(nullptr, szStr, L"파일 저장 실패", MB_OK);
		return;
	}

	// Animation 이름 저장
	fwprintf_s(pFile, L"\n");

	fwprintf_s(pFile, L"[ANIMATION_NAME]\n");
	fwprintf_s(pFile, GetName().c_str());
	fwprintf_s(pFile, L"\n\n");

	// 아틀라스 텍스쳐 키값 저장	
	fwprintf_s(pFile, L"[ATLAS_KEY]\n");
	fwprintf_s(pFile, m_pAtlas->GetKey().c_str());
	fwprintf_s(pFile, L"\n\n");

	fwprintf_s(pFile, L"[ATLAS_PATH]\n");
	fwprintf_s(pFile, m_pAtlas->GetRelativePath().c_str());
	fwprintf_s(pFile, L"\n\n");


	// 프레임 정보 저장
	wchar_t szNum[50] = {};

	fwprintf_s(pFile, L"[FRAME_COUNT]\n");

	size_t iFrmCount = m_vecFrm.size();
	_itow_s((int)iFrmCount, szNum, 50, 10);

	fwprintf_s(pFile, szNum);
	fwprintf_s(pFile, L"\n\n");

	for (size_t i = 0; i < iFrmCount; ++i)
	{
		fwprintf_s(pFile, L"[%zd_FRAME]\n", i);

		// LEFT_TOP
		fwprintf_s(pFile, L"[LEFT_TOP]\n");
		fwprintf_s(pFile, L"%.1f %.1f\n", m_vecFrm[i].vLeftTop.x, m_vecFrm[i].vLeftTop.y);

		// SIZE
		fwprintf_s(pFile, L"[SIZE]\n");
		fwprintf_s(pFile, L"%.1f %.1f\n", m_vecFrm[i].vSize.x, m_vecFrm[i].vSize.y);

		// OFFSET
		fwprintf_s(pFile, L"[OFFSET]\n");
		fwprintf_s(pFile, L"%.1f %.1f\n", m_vecFrm[i].vOffset.x, m_vecFrm[i].vOffset.y);

		// DURATION
		fwprintf_s(pFile, L"[DURATION]\n");
		fwprintf_s(pFile, L"%.2f\n", m_vecFrm[i].fDuration);

		fwprintf_s(pFile, L"\n");
	}

	fclose(pFile);
}

void CAnimation::Load(const wstring& _strFileName)
{
	wstring strFilepath = CPathMgr::GetInst()->GetContentPath();
	strFilepath += L"animation\\";
	strFilepath += _strFileName;

	// 파일 입출력
	FILE* pFile = nullptr;
	errno_t iErrNum = _wfopen_s(&pFile, strFilepath.c_str(), L"rb");

	if (nullptr == pFile)
	{
		wchar_t szStr[256] = {};
		wsprintf(szStr, L"Animation Save 실패, Error Number : %d", iErrNum);
		MessageBox(nullptr, szStr, L"파일 저장 실패", MB_OK);
		return;
	}

	size_t iFrameCount = 0;
	wstring strAtlasKey;
	wstring strAltasRelativePath;

	while (true)
	{
		wchar_t szBuffer[256] = {};
		fwscanf_s(pFile, L"%s", szBuffer, 256);

		if (!wcscmp(szBuffer, L"[ANIMATION_NAME]"))
		{
			fwscanf_s(pFile, L"%s", szBuffer, 256);
			SetName(szBuffer);
		}

		else if (!wcscmp(szBuffer, L"[ATLAS_KEY]"))
		{
			fwscanf_s(pFile, L"%s", szBuffer, 256);
			strAtlasKey = szBuffer;
		}

		else if (!wcscmp(szBuffer, L"[ATLAS_PATH]"))
		{
			fwscanf_s(pFile, L"%s", szBuffer, 256);
			strAltasRelativePath = szBuffer;
		}

		else if (!wcscmp(szBuffer, L"[FRAME_COUNT]"))
		{
			fwscanf_s(pFile, L"%zd", &iFrameCount);

			for (size_t i = 0; i < iFrameCount; ++i)
			{
				tAnimFrm frm = {};

				while (true)
				{
					wchar_t szBuffer[256] = {};
					fwscanf_s(pFile, L"%s", szBuffer, 256);

					if (!wcscmp(szBuffer, L"[LEFT_TOP]"))
					{
						fwscanf_s(pFile, L"%f %f", &frm.vLeftTop.x, &frm.vLeftTop.y);
					}

					else if (!wcscmp(szBuffer, L"[SIZE]"))
					{
						fwscanf_s(pFile, L"%f %f", &frm.vSize.x, &frm.vSize.y);
					}

					else if (!wcscmp(szBuffer, L"[OFFSET]"))
					{
						fwscanf_s(pFile, L"%f %f", &frm.vOffset.x, &frm.vOffset.y);
					}

					else if (!wcscmp(szBuffer, L"[DURATION]"))
					{
						fwscanf_s(pFile, L"%f", &frm.fDuration);
						break;
					}
				}
				m_vecFrm.push_back(frm);
			}
			break;
		}
	}

	m_pAtlas = CResMgr::GetInst()->LoadTexture(strAtlasKey, strAltasRelativePath);

	fclose(pFile);
}