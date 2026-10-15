#include "pch.h"
#include "Game\05.Component\Animator\CAnimator.h"

#include "Game\05.Component\Animator\Animation\CAnimation.h"

#include <set>
#include "Game\02.Manager\07.ResMgr\CResMgr.h"

CAnimator::CAnimator(CObj* _pOwner)
    : CComponent(_pOwner)
    , m_pCurAnim(nullptr)
    , m_bRepeat(false)
{
}

CAnimator::CAnimator(const CAnimator& _other)
	: CComponent(_other)
	, m_mapAnim{}
	, m_pCurAnim(_other.m_pCurAnim)
	, m_bRepeat(_other.m_bRepeat)
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

CAnimation* CAnimator::CreateFrameAnimation(const wstring& _strName, CTexture* _pAtlas, const vector<tAnimFrm>& _vecFrm)
{
	// 예전 에디터용 함수 두 벌(CreateEditUpper/LowerAnimation)은 DeleteAnimation이 맵의 첫 원소를 지우지 못해
	// 두 번째부터 insert가 실패했다. 새 애니메이션은 새고 화면은 이전 애니메이션을 계속 재생했다 (MS-R20)
	DeleteAnimation(_strName);

	CAnimation* pAnim = new CAnimation(this);
	pAnim->SetName(_strName);
	pAnim->SetAtlas(_pAtlas);
	pAnim->SetAllFrmInfo(_vecFrm);
	m_mapAnim.insert(make_pair(_strName, pAnim));
	return pAnim;
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
	DeleteAnimation(pAnim->GetName());
	m_mapAnim.insert(make_pair(pAnim->GetName(), pAnim));

	return pAnim;
}

void CAnimator::DeleteAnimation(const wstring& _strName)
{
	// 예전 조건은 find != begin() && find != end() 라 맵의 첫 원소는 지우지 못했다 (MS-R20)
	const auto iter = m_mapAnim.find(_strName);
	if (iter == m_mapAnim.end())
		return;

	if (m_pCurAnim == iter->second)
		m_pCurAnim = nullptr;
	delete iter->second;
	m_mapAnim.erase(iter);
}