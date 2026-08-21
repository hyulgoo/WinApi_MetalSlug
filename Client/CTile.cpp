#include "pch.h"
#include "CTile.h"

#include "CResMgr.h"
#include "CTexture.h"

CTile::CTile()
	: m_pAtlas(nullptr)
	, m_iImgIdx(-1)
{
}

CTile::~CTile()
{
}

void CTile::tick()
{
}

void CTile::render(HDC _dc)
{
    // 이미지가 없거나 이미지인덱스가 재초기화 되지 않으면 리턴
	if (-1 == m_iImgIdx || m_pAtlas == nullptr)
		return;
	// 타일 최초 렌더링 위치
	Vec2 vPos = CCamera::GetInst()->GetRenderPos(GetPos());
	// 행 최대개수를 구하고 
    // 인덱스 / 행 개수는 현재 열
    // 인덱스 % 행 개수는 현재 행
    int iMaxRow = m_pAtlas->Width() / TILE_SIZE;
    int iCurCol = m_iImgIdx / iMaxRow;
    int iCurRow = m_iImgIdx % iMaxRow;

    // 위에서 구한 현재 행, 열에 TILE_SIZE를 곱해 출력
    BitBlt(_dc
        , (int)vPos.x
        , (int)vPos.y
        , TILE_SIZE, TILE_SIZE
        , m_pAtlas->GetDC()
        , iCurRow * TILE_SIZE
        , iCurCol * TILE_SIZE
        , SRCCOPY);
}

void CTile::Save(FILE* _pFile)
{
    // 위치 저장
    Vec2 vPos = GetPos();
    fwrite(&vPos, sizeof(Vec2), 1, _pFile);

    // 이미지 유무 저장
    bool bAtlas = m_pAtlas;
    fwrite(&bAtlas, sizeof(bool), 1, _pFile);

    // 이미지가 있다면 키와 경로를 저장
    if (bAtlas)
    {
        wstring strKey = m_pAtlas->GetKey();
        SaveWString(strKey, _pFile);

        wstring strRelativePath = m_pAtlas->GetRelativePath();
        SaveWString(strRelativePath, _pFile);
    }

    // 타일인덱스를 저장
    fwrite(&m_iImgIdx, sizeof(int), 1, _pFile);
}

void CTile::Load(FILE* _pFile)
{
    // 위치를 읽어온 후 SetPos해줌
    Vec2 vPos;
    fread(&vPos, sizeof(Vec2), 1, _pFile);
    SetPos(vPos);

    // 이미지가 있다면 키와 경로를 읽어오고 이미지 아틀라스를 세팅함
    bool bAtlas = false;
    fread(&bAtlas, sizeof(bool), 1, _pFile);

    if (bAtlas)
    {
        wstring strKey, strRelativePath;
        LoadWString(strKey, _pFile);
        LoadWString(strRelativePath, _pFile);
        m_pAtlas = CResMgr::GetInst()->LoadTexture(strKey, strRelativePath);
    }

    // 이미지인덱스 번호를 읽어옴
    fread(&m_iImgIdx, sizeof(int), 1, _pFile);
}

void CTile::SetImgIdx(int _imgidx)
{
    assert(m_pAtlas);

    int iRow = m_pAtlas->Width() / TILE_SIZE;
    int iCol = m_pAtlas->Height() / TILE_SIZE;
    int iImgMaxCount = iRow * iCol;

    assert(!(iImgMaxCount <= _imgidx));

    m_iImgIdx = _imgidx;
}

void CTile::AddImgIdx()
{
    assert(m_pAtlas);

    int iRow = m_pAtlas->Width() / TILE_SIZE;
    int iCol = m_pAtlas->Height() / TILE_SIZE;
    int iImgMaxCount = iRow * iCol;

    ++m_iImgIdx;

    if (iImgMaxCount <= m_iImgIdx)
    {
        m_iImgIdx = 0;
    }
}
