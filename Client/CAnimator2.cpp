#include "pch.h"
#include "CAnimator2.h"

#include "CAnimation.h"



CAnimator2::CAnimator2(CObj* _pOwner)
	: CComponent(_pOwner)
	, m_pCurAnim(nullptr)
	, m_bRepeat(false)
{
}

CAnimator2::CAnimator2(const CAnimator2& _other)
	: CComponent(nullptr)
	, m_pCurAnim(_other.m_pCurAnim)
	, m_bRepeat(_other.m_bRepeat)
{
	map<wstring, CAnimation*>::const_iterator iter = _other.m_mapAnim.begin();
	for (; iter != _other.m_mapAnim.end(); ++iter)
	{
		// Anim를 복사하고
		CAnimation* pAnim = iter->second->Clone();
		// 재생정보를 리셋하고
		pAnim->Reset();
		// 담당 애니메이터를 복사된 이 애니메이터로 한다
		pAnim->m_pAnimator2 = this;
		// 복사된 Anim(담당이 바뀐)와 원래의 이름인 first를 짝지어 넣는다.
		m_mapAnim.insert(make_pair(iter->first, pAnim));
	}

	if (nullptr != m_pCurAnim)
	{
		Play(_other.m_pCurAnim->GetName(), m_bRepeat);
	}
}

CAnimator2::~CAnimator2()
{ // 이름과 애니메이션을 담고 있는 조합배열에서 포인터 변수인 CAnimation* = iter->second를 삭제
	map<wstring, CAnimation*>::iterator iter = m_mapAnim.begin();
	for (;iter != m_mapAnim.end(); ++iter)
	{
		delete iter->second;
	}
}

void CAnimator2::tick()
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
}

void CAnimator2::render(HDC _dc)
{
	if (nullptr == m_pCurAnim)
		return;
	m_pCurAnim->render(_dc);
}

void CAnimator2::final_tick()
{
}

void CAnimator2::Play(const wstring& _strName, bool _bRepeat)
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

void CAnimator2::CreateAnimation(const wstring& _strName, CTexture* _pAtlas, Vec2 _vLeftTop, Vec2 _vSize, Vec2 _vOffset, int _iMaxFrmCount, float _fDuration)
{
	CAnimation* pAnim = FindAnimation(_strName);
	assert(!pAnim);

	pAnim = new CAnimation(this);
	pAnim->init(_strName, _pAtlas, _vLeftTop, _vSize, _vOffset, _iMaxFrmCount, _fDuration);
	m_mapAnim.insert(make_pair(_strName, pAnim));
}

void CAnimator2::CreateEditAnimation(vector<tAnimFrm> _vecFrm, CTexture* _Atlas)
{
	CAnimation* pAnim = FindAnimation(L"WorkAnim2");
	assert(!pAnim);

	pAnim = new CAnimation(this);
	pAnim->SetName(L"WorkAnim2");
	pAnim->SetAtlas(_Atlas);
	pAnim->m_vecFrm.resize(_vecFrm.size());
	for (size_t i = 0; i < _vecFrm.size(); ++i)
	{
		pAnim->m_vecFrm[i] = _vecFrm[i];
	}
	m_mapAnim.insert(make_pair(L"WorkAnim2", pAnim));

}

CAnimation* CAnimator2::FindAnimation(const wstring& _strName)
{
	map<wstring, CAnimation*>::iterator iter = m_mapAnim.find(_strName);

	if (iter == m_mapAnim.end())
		return nullptr;

	return iter->second;
}

CAnimation* CAnimator2::LoadAnimaton(wstring _strRelativePath)
{
	CAnimation* pAnim = new CAnimation(this);

	pAnim->Load(_strRelativePath);

	m_mapAnim.insert(make_pair(pAnim->GetName(), pAnim));

	return pAnim;
}