#include "pch.h"
#include "Game\04.Object\Bullet\Effect\CAnimEffect.h"

#include "Game\05.Component\Animator\CAnimator.h"
#include "Game\05.Component\Animator\Animation\CAnimation.h"

CAnimEffect::CAnimEffect(const wstring& _strAnimFile, const bool _bPlayRight)
    : m_bPlayRight(_bPlayRight)
    , m_bStart(false)
{
    CreateAnimator();
    m_strAnim = GetAnimator()->LoadAnimation(_strAnimFile)->GetName();
}

CAnimEffect::~CAnimEffect()
{
}

void CAnimEffect::tick()
{
    if (!m_bStart)
    {
        m_bStart = true;
        GetAnimator()->Play(m_strAnim, false);
        if (m_bPlayRight)
            GetAnimator()->GetAnimation()->SetPlayRight();
        else
            GetAnimator()->GetAnimation()->SetPlayLeft();
    }

    CObj::tick();

    // 애니메이션이 끝나면 사라진다
    if (GetAnimator()->IsFinish() && !IsDead())
        SetDead();
}
