#pragma once

class CObj;
bool IsValid(CObj*& _pTarget);

void SaveWString(const wstring& _str, FILE* _pFile);
void LoadWString(wstring& _str, FILE* _pFile);
void Instantiate(CObj* _pNewObj, const Vec2& _vPos, LAYER _eLayer);
void Saturate(float& _float);
void ChangeLevel(LEVEL_TYPE _eNextLevel);

// 사용자에게 알리거나 묻는 창. 자동 테스트에서는 창을 띄우지 않는다
void ReportError(const wchar_t* _szTitle, const wchar_t* _szMsg);             // 테스트: 로그에 기록 (CTestMgr.cpp)
bool AskYesNo(const wchar_t* _szTitle, const wchar_t* _szMsg);                 // 테스트: 예
bool ShowFileDialog(OPENFILENAME& _tOfn, bool _bSave);                        // 테스트: 시나리오의 dialog 경로
