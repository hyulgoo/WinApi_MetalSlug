#pragma once

class CTexture;

class CEngine
{
	SINGLE(CEngine);

private:	
	// static CEngine* m_pInst;
	HWND		m_hMainWnd;
	HDC			m_hDC;
	POINT		m_ptResolution;
	HPEN		m_arrPen[static_cast<UINT>(PEN_TYPE::END)];

	CTexture*	m_pMemTex;
	CTexture*	m_BackGroundTex;

public:
	HWND  GetMainWnd() const { return m_hMainWnd; }
	HDC   GetMainDC() const { return m_hDC; }
	HPEN  GetPen(PEN_TYPE _type) const { return m_arrPen[static_cast<UINT>(_type)]; }
	POINT GetResolution() const { return m_ptResolution; }
	void  ChangeWindowSize(UINT _width, UINT _height);
	HDC   GetMemTexDC() const;

public:
	void init(HWND _hwnd, UINT _iWidth, UINT _iHeight);
	void progress() const;

private:
	void tick() const;	// 갱신
	void render() const;	// 렌더링
	void CreatePenBrush();
};

