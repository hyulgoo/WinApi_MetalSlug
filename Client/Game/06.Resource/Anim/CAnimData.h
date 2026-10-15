#pragma once
#include "Game\06.Resource\CRes.h"

class CTexture;

// .anim 파일 한 개의 내용. 엔진(CAnimData::Load)과 애니메이션 에디터가 같은 코드로 읽고 쓴다
struct tAnimFile
{
    wstring          strName;        // 애니메이션 이름 = 파일 이름 = 게임에서 Play하는 이름
    wstring          strAtlasKey;    // 아틀라스 텍스처 키
    wstring          strAtlasPath;   // 아틀라스 텍스처 경로 (content 폴더 기준)
    vector<tAnimFrm> vecFrm;
};

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

    // 파일 형식: UTF-16LE(BOM 없음), 줄바꿈 \n, 값은 공백으로 나뉜 토큰.
    // 읽지 못하면(파일 없음, 잘린 파일, 잘못된 숫자) false와 이유. 오류 알림은 부르는 쪽이 한다
    static bool ReadFile(const wstring& _strFullPath, tAnimFile& _tOut, wstring& _strError);
    // 프레임이 없거나 이름·키·경로가 비었거나 공백이 있으면(다시 읽을 수 없다) 파일을 건드리지 않고 false.
    // 임시 파일에 다 쓴 뒤 바꿔치기해서, 쓰다가 실패해도 원래 파일이 남는다
    static bool WriteFile(const wstring& _strFullPath, const tAnimFile& _tIn, wstring& _strError);

private:
    // 파일을 읽지 못하거나 프레임이 없으면 오류를 남기고 E_FAIL (프레임이 없는 데이터로 남는다)
    virtual int Load(const wstring& _strFilePath) override;

public:
    CAnimData();
    virtual ~CAnimData() override;

    friend class CResMgr;
};
