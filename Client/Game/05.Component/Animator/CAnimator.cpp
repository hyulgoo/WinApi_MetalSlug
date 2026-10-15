#include "pch.h"
#include "Game\05.Component\Animator\CAnimator.h"

#include "Game\05.Component\Animator\Animation\CAnimation.h"

#include <set>
#include "Game\02.Manager\07.ResMgr\CResMgr.h"

CAnimator::CAnimator(CObj* _pOwner)
    : CComponent(_pOwner)
    , m_pCurAnim(nullptr)
    , m_bRepeat(false)
    , m_bLink(false)
    , m_tLinkPrint()
    , m_tLinkDir(false)
{
}

CAnimator::CAnimator(const CAnimator& _other)
	: CComponent(_other)
	, m_mapAnim{}
	, m_pCurAnim(_other.m_pCurAnim)
	, m_bRepeat(_other.m_bRepeat)
	, m_bLink(_other.m_bLink)
	, m_strLinkName(_other.m_strLinkName)
	, m_tLinkPrint(_other.m_tLinkPrint)
	, m_tLinkDir(_other.m_tLinkDir)
{
    for (auto& iter : _other.m_mapAnim)
    {
        CAnimation* pAnim = iter.second->Clone();
        // 재생상태를 초기화하고
        pAnim->Reset();
        // 복제된 애니메이션이 참조하는 애니메이터를 현재 애니메이터로 한다
        pAnim->m_pAnimator = this;
        // 복제된 Anim(포인터는 다름)을 원본과 같은 이름으로 first에 짝지어 넣는다.
        m_mapAnim.insert(make_pair(iter.first, pAnim));
    }
	if (nullptr != m_pCurAnim)
	{
		Play(_other.m_pCurAnim->GetName(), m_bRepeat);
	}
}

CAnimator::~CAnimator()
{ // 이름별로 애니메이션을 담고 있는 map에서, 실제로 지워야 하는 건 CAnimation* = iter->second이다
    for (const auto iter : m_mapAnim)
        delete iter.second;
    delete m_pMissingAnim;
}

void CAnimator::tick()
{
	// 현재 Anim이 nullptr이면 return
	if (nullptr == m_pCurAnim)
		return;
    
	// 현재 Anim tick. 반복 재생은 애니메이션이 남은 시간을 이어 가며 처음으로 되돌아간다
	m_pCurAnim->tick(m_bRepeat);
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

bool CAnimator::IsFinish() const
{
	return nullptr == m_pCurAnim || m_pCurAnim->IsFinish();
}

void CAnimator::render(const HDC _dc)
{
	if (nullptr == m_pCurAnim)
		return;
    
	m_pCurAnim->render(_dc);
}

void CAnimator::final_tick()
{
}

void CAnimator::SetLink(const wstring& _strLinkName, const SETPRINT _tUpDownNormal, const bool _bDirection)
{
	 m_bLink = true; 
	 m_strLinkName = _strLinkName;
	 SetLinkAnim(_tUpDownNormal, _bDirection);
}

void CAnimator::SetLinkAnim(const SETPRINT _tUpDownNormal, const bool _bDirection)
{
	m_tLinkPrint = _tUpDownNormal;
	m_tLinkDir = _bDirection;
}


void CAnimator::Play(const wstring& _strName, const bool _bRepeat)
{
	CAnimation* pAnim = FindAnimation(_strName);
	if (nullptr == pAnim)
	{
		// 이름마다 한 번만 알린다 (매 프레임 Play하는 곳이 있다)
		static std::set<wstring> s_setReported;
		if (s_setReported.insert(_strName).second)
		{
			const wstring strMsg = L"해당 Animation 없음 : " + _strName;
			ReportError(L"Animation 재생 실패", strMsg.c_str());
		}

		// 빈 애니메이션을 대신 재생한다. 아무것도 그리지 않고, 호출부가 바로 GetAnimation()으로 방향을 정해도
		// 안전하다 (예전에는 처음 재생부터 없으면 GetAnimation()이 nullptr라 호출부에서 크래시했다)
		if (nullptr == m_pMissingAnim)
			m_pMissingAnim = new CAnimation(this);
		m_pMissingAnim->SetName(_strName);
		pAnim = m_pMissingAnim;
	}

	m_pCurAnim = pAnim;
	m_pCurAnim->Reset();
	m_bRepeat = _bRepeat;
}

void CAnimator::CreateAnimation(const wstring& _strName, CTexture* _pAtlas, const Vec2& _vLeftTop, const Vec2& _vSize, const Vec2& _vOffset, const int _iMaxFrmCount, const float _fDuration)
{
	CAnimation* pAnim = FindAnimation(_strName);
	if (pAnim != nullptr)
		DeleteAnimation(_strName);

	pAnim = new CAnimation(this);
	pAnim->init(_strName, _pAtlas, _vLeftTop, _vSize, _vOffset, _iMaxFrmCount, _fDuration);
	m_mapAnim.insert(make_pair(_strName, pAnim));
}

void CAnimator::CreateEditUpperAnimation(const vector<tAnimFrm>& _vecFrm, CTexture* _Atlas)
{	
	CAnimation* pAnim = FindAnimation(L"WorkAnim");
	if (pAnim != nullptr)
		DeleteAnimation(L"WorkAnim");

	pAnim = new CAnimation(this);
	pAnim->SetName(L"WorkAnim");
	pAnim->SetAtlas(_Atlas);
	pAnim->m_vecFrm.resize(_vecFrm.size());

	for (size_t i = 0; i < _vecFrm.size(); ++i)
		pAnim->m_vecFrm[i] = _vecFrm[i];
    
	m_mapAnim.insert(make_pair(L"WorkAnim", pAnim));
}

void CAnimator::CreateEditLowerAnimation(const vector<tAnimFrm>& _vecFrm, CTexture* _Atlas)
{
	CAnimation* pAnim = FindAnimation(L"WorkAnim1");
	if (pAnim != nullptr)
		DeleteAnimation(L"WorkAnim1");

	pAnim = new CAnimation(this);
	pAnim->SetName(L"WorkAnim1");
	pAnim->SetAtlas(_Atlas);
	pAnim->m_vecFrm.resize(_vecFrm.size());

	for (size_t i = 0; i < _vecFrm.size(); ++i)
		pAnim->m_vecFrm[i] = _vecFrm[i];
    
	m_mapAnim.insert(make_pair(L"WorkAnim1", pAnim));
}

CAnimation* CAnimator::FindAnimation(const wstring& _strName)
{
	const map<wstring, CAnimation*>::iterator iter = m_mapAnim.find(_strName);

	if (iter == m_mapAnim.end())
		return nullptr;

	return iter->second;
}

CAnimation* CAnimator::LoadAnimation(const wstring& _strRelativePath)
{
	// 프레임 데이터는 리소스 매니저가 파일마다 한 번만 읽어 공유한다. 이 오브젝트는 재생 상태만 갖는다
	CAnimation* pAnim = new CAnimation(this);
	pAnim->SetData(CResMgr::GetInst()->LoadAnimData(_strRelativePath));

	// 같은 이름이 이미 있으면 바꾼다 (예전에는 map::insert가 실패해 새로 만든 것이 샜다)
	const auto iter = m_mapAnim.find(pAnim->GetName());
	if (iter != m_mapAnim.end())
	{
		if (m_pCurAnim == iter->second)
			m_pCurAnim = nullptr;
		delete iter->second;
		m_mapAnim.erase(iter);
	}
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