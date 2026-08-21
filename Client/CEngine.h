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
	HPEN		m_arrPen[(UINT)PEN_TYPE::END];

	CTexture*	m_pMemTex;
	CTexture*	m_BackGroundTex;

public:
	HWND GetMainWnd() { return m_hMainWnd; }
	HDC GetMainDC() { return m_hDC; }
	HPEN GetPen(PEN_TYPE _type) { return m_arrPen[(UINT)_type]; }
	POINT GetResolution() { return m_ptResolution; }
	void ChangeWindowSize(UINT _width, UINT _height);
	HDC GetMemTexDC();

public:
	void init(HWND _hwnd, UINT _iWidth, UINT _iHeight);
	void progress();

private:
	void tick();	// °»½Å
	void render();	// ·»´õ¸µ
	void CreatePenBrush();
};

