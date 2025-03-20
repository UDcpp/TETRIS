#pragma once
#include <windows.h> // COLORREF 사용

class Tetromino {
public:
    int shape[4][4]; // 블록의 형태를 저장하는 4x4 배열
    int x, y; // 블록의 현재 위치
    COLORREF color; // 블록 색상

    // 생성자
    Tetromino(int type);

    // 블록 형태 설정
    void SetShape(int type);

    void Rotate(); // 블록 회전 함수 선언

};