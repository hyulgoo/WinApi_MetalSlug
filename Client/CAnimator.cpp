#include "pch.h"
#include "CAnimator.h"

#include "CAnimation.h"



CAnimator::CAnimator(CObj* _pOwner)
	: CComponent(_pOwner)
	, m_pCurAnim(nullptr)
	, m_bRepeat(false)
{
}

CAnimator::CAnimator(const CAnimator& _other)
	: CComponent(_other)
	, m_pCurAnim(_other.m_pCurAnim)
	, m_bRepeat(_other.m_bRepeat)
	, m_mapAnim{}
{
	map<wstring, CAnimation*>::const_iterator iter = _other.m_mapAnim.begin();
	for (; iter != _other.m_mapAnim.end(); ++iter)
	{
		// Anim를 복사하고
		CAnimation* pAnim = iter->second->Clone();
		// 재생정보를 리셋하고
		pAnim->Reset();
		// 담당 애니메이터를 복사된 이 애니메이터로 한다
		pAnim->m_pAnimator = this;
		// 복사된 Anim(담당이 바뀐)와 원래의 이름인 first를 짝지어 넣는다.
		m_mapAnim.insert(make_pair(iter->first, pAnim));
	}
	
	if (nullptr != m_pCurAnim)
	{
		Play(_other.m_pCurAnim->GetName(), m_bRepeat);
	}
}

CAnimator::~CAnimator()
{ // 이름과 애니메이션을 담고 있는 조합배열에서 포인터 변수인 CAnimation* = iter->second를 삭제
	map<wstring, CAnimation*>::iterator iter = m_mapAnim.begin();
	for (;iter != m_mapAnim.end(); ++iter)
	{
		delete iter->second;
	}
}

void CAnimator::tick()
{
	// 현재 Anim가 nullptr이면 return
	if (nullptr == m_pCurAnim)
		return;
	// 현재 Anim의 출력이 마지막까지 끝났고 반복재생이 true로 돼있다면 Reset
	if (m_pCurAnim->IsFinish() && m_bRepeat)
	{
		m_pCurAnim->Reset();
	}
	// 현재 Anim tick
	m_pCurAnim->tick();
	if (m_bLink == true && m_pCurAnim->IsFinish())
	{
		Play(m_strLinkName, true);
		GetAnimation()->m_tPrintMode = m_tLinkPrint;
		if (m_tLinkDir)
			GetAnimation()->SetPlayRight();
		else
			GetAnimation()->SetPlayLeft();
	}
}

void CAnimator::render(HDC _dc)
{
	if (nullptr == m_pCurAnim)
		return;
	m_pCurAnim->render(_dc);
}

void CAnimator::final_tick()
{
}

void CAnimator::SetLink(const wstring& _strLinkName, SETPRINT _tUpDownNormal, bool _bDirection)
{
	 m_bLink = true; 
	 m_strLinkName = _strLinkName;
	 SetLinkAnim(_tUpDownNormal, _bDirection);
}

void CAnimator::SetLinkAnim(SETPRINT _tUpDownNormal, bool _bDirection)
{
	m_tLinkPrint = _tUpDownNormal;
	m_tLinkDir = _bDirection;
}


void CAnimator::Play(const wstring& _strName, bool _bRepeat)
{
	CAnimation* pAnim = FindAnimation(_strName);
	if (nullptr == pAnim)
	{
		MessageBox(nullptr, L"해당 Animation 없음", L"Animation 재생 오류", MB_OK);
		return;
	}

	m_pCurAnim = pAnim;
	m_pCurAnim->Reset();
	m_bRepeat = _bRepeat;
}

void CAnimator::CreateAnimation(const wstring& _strName, CTexture* _pAtlas, Vec2 _vLeftTop, Vec2 _vSize, Vec2 _vOffset, int _iMaxFrmCount, float _fDuration)
{
	CAnimation* pAnim = FindAnimation(_strName);
	if (pAnim != nullptr)
		DeleteAnimation(_strName);

	pAnim = new CAnimation(this);
	pAnim->init(_strName, _pAtlas, _vLeftTop, _vSize, _vOffset, _iMaxFrmCount, _fDuration);
	m_mapAnim.insert(make_pair(_strName, pAnim));
}

void CAnimator::CreateEditUpperAnimation(vector<tAnimFrm> _vecFrm, CTexture* _Atlas)
{	
	CAnimation* pAnim = FindAnimation(L"WorkAnim");
	if (pAnim != nullptr)
		DeleteAnimation(L"WorkAnim");

	pAnim = new CAnimation(this);
	pAnim->SetName(L"WorkAnim");
	pAnim->SetAtlas(_Atlas);
	pAnim->m_vecFrm.resize(_vecFrm.size());

	for (size_t i = 0; i < _vecFrm.size(); ++i)
	{
		pAnim->m_vecFrm[i] = _vecFrm[i];
	}
	m_mapAnim.insert(make_pair(L"WorkAnim", pAnim));
}

void CAnimator::CreateEditLowerAnimation(vector<tAnimFrm> _vecFrm, CTexture* _Atlas)
{
	CAnimation* pAnim = FindAnimation(L"WorkAnim1");
	if (pAnim != nullptr)
		DeleteAnimation(L"WorkAnim1");

	pAnim = new CAnimation(this);
	pAnim->SetName(L"WorkAnim1");
	pAnim->SetAtlas(_Atlas);
	pAnim->m_vecFrm.resize(_vecFrm.size());

	for (size_t i = 0; i < _vecFrm.size(); ++i)
	{
		pAnim->m_vecFrm[i] = _vecFrm[i];
	}
	m_mapAnim.insert(make_pair(L"WorkAnim1", pAnim));

}

CAnimation* CAnimator::FindAnimation(const wstring& _strName)
{
	map<wstring, CAnimation*>::iterator iter = m_mapAnim.find(_strName);

	if (iter == m_mapAnim.end())
		return nullptr;

	return iter->second;
}

CAnimation* CAnimator::LoadAnimaton(wstring _strRelativePath)
{
	CAnimation* pAnim = new CAnimation(this);
	pAnim->Load(_strRelativePath);

	m_mapAnim.insert(make_pair(pAnim->GetName(), pAnim));

	return pAnim;
}

void CAnimator::DeleteAnimation(const wstring& _strName)
{
	if (m_mapAnim.find(_strName) != m_mapAnim.begin() && m_mapAnim.find(_strName) != m_mapAnim.end())
	{
		delete m_mapAnim.find(_strName)->second;
		m_mapAnim.erase(_strName);
	}
}