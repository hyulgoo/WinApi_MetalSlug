#pragma once
#include "Game\06.Resource\CRes.h"

class CTexture;

// .anim 파일 하나의 내용(애니메이션 이름, 아틀라스, 프레임).
// CResMgr가 파일마다 한 번만 읽어 두고, 그 애니메이션을 쓰는 오브젝트들(CAnimation)이 함께 본다.
// 재생 상태(현재 프레임, 누적 시간, 방향)는 CAnimation이 따로 가진다
class CAnimData :
    public CRes
{
private:
    wstring          m_strName;
    CTexture*        m_pAtlas;
    vector<tAnimFrm> m_vecFrm;

public:
    const wstring&          GetName() const { return m_strName; }
    CTexture*               GetAtlas() const { return m_pAtlas; }
    const vector<tAnimFrm>& GetFrames() const { return m_vecFrm; }

private:
    // 파일을 읽지 못하거나 프레임이 없으면 오류를 남기고 E_FAIL (프레임이 없는 데이터로 남는다)
    virtual int Load(const wstring& _strFilePath) override;

public:
    CAnimData();
    virtual ~CAnimData() override;

    friend class CResMgr;
};
