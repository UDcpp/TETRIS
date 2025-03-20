#include "pch.h"
#include "Tetromino.h"
#include <cstring>


// 생성자: 블록의 형태를 설정하고 초기 위치를 지정
Tetromino::Tetromino(int type) {
    SetShape(type);
    x = 3; // 초기 위치 (가로 방향 중앙)
    y = 0;
}

// 블록 모양 정의
void Tetromino::SetShape(int type) {
    memset(shape, 0, sizeof(shape)); // 배열 초기화

    switch (type) {
    case 0: // I 블록
        shape[1][0] = shape[1][1] = shape[1][2] = shape[1][3] = 1;
        color = RGB(0, 255, 255);
        break;
    case 1: // O 블록
        shape[1][1] = shape[1][2] = shape[2][1] = shape[2][2] = 1;
        color = RGB(255, 255, 0);
        break;
    case 2: // T 블록
        shape[1][1] = shape[0][1] = shape[1][0] = shape[1][2] = 1;
        color = RGB(128, 0, 128);
        break;
    case 3: // L 블록
        shape[1][0] = shape[1][1] = shape[1][2] = shape[2][2] = 1;
        color = RGB(255, 165, 0);
        break;
    case 4: // J 블록
        shape[1][0] = shape[1][1] = shape[1][2] = shape[2][0] = 1;
        color = RGB(0, 0, 255);
        break;
    case 5: // S 블록
        shape[1][1] = shape[1][2] = shape[2][0] = shape[2][1] = 1;
        color = RGB(0, 255, 0);
        break;
    case 6: // Z 블록
        shape[1][0] = shape[1][1] = shape[2][1] = shape[2][2] = 1;
        color = RGB(255, 0, 0);
        break;
    }
}


void Tetromino::Rotate()
{
    int temp[4][4] = { 0 };

    // ?? 90도 회전 구현 (Transpose + Reverse Rows)
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            temp[j][3 - i] = shape[i][j];
        }
    }

    // ?? 회전 결과를 원래 배열에 적용
    memcpy(shape, temp, sizeof(shape));
}
