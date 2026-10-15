#pragma once

class CTexture;
class CAnimData;

class CResMgr
{
	SINGLE(CResMgr);
private:
	map<wstring, CTexture*>	 m_mapTex;
	map<wstring, CAnimData*> m_mapAnim;		// 키: animation 폴더 기준 상대 경로 (구분자 \, 소문자)

public:
	CTexture* FindTexture(const wstring& _strKey);
	CTexture* LoadTexture(const wstring& _strKey, const wstring& _strRelativePath);
	CTexture* CreateTexture(const wstring& _strKey, UINT _iWidth, UINT _Height);

	// .anim 파일을 처음 찾을 때 한 번만 읽어 두고 같은 데이터를 돌려준다.
	// 읽지 못하면 오류를 한 번 남기고 프레임이 없는 데이터(이름은 파일 이름)를 돌려준다
	const CAnimData* LoadAnimData(const wstring& _strRelativePath);
	size_t           GetAnimDataCount() const { return m_mapAnim.size(); }

	// 애니메이션 에디터가 _strFullPath에 저장한 뒤 부른다. 이미 읽어 둔 파일이면 같은 데이터에 다시 읽는다
	// (읽어 둔 것을 계속 쓰면 에디터에서 고친 내용이 다시 실행할 때까지 게임에 보이지 않는다)
	void ReloadAnimData(const wstring& _strFullPath);
};