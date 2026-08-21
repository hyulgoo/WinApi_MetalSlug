#pragma once

class CObj;
bool IsValid(CObj*& _pTarget);

void SaveWString(const wstring& _str, FILE* _pFile);
void LoadWString(wstring& _str, FILE* _pFile);
void Instantiate(CObj* _pNewObj, const Vec2& _vPos, LAYER _eLayer);
void Saturate(float& _float);
void ChangeLevel(LEVEL_TYPE _eNextLevel);