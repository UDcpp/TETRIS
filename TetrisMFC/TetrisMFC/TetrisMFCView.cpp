#pragma once
// TetrisMFCView.cpp: CTetrisMFCView 클래스의 구현
//

#include "pch.h"
#include "TetrisMFC.h"  // 🔹 애플리케이션 객체 포함
#include "TetrisMFCView.h"
#include "Tetromino.h"

IMPLEMENT_DYNCREATE(CTetrisMFCView, CView)

BEGIN_MESSAGE_MAP(CTetrisMFCView, CView)
    ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CTetrisMFCView::OnFilePrintPreview)
    ON_WM_CONTEXTMENU()
    ON_WM_RBUTTONUP()
    ON_WM_KEYDOWN()  // 🔹 키 입력 이벤트 추가
    ON_WM_CREATE()   // 🔹 윈도우 생성 시 타이머 설정
    ON_WM_TIMER()    // 🔹 타이머 이벤트 추가
    ON_WM_CREATE()
END_MESSAGE_MAP()

// CTetrisMFCView 생성자
CTetrisMFCView::CTetrisMFCView() noexcept
{
    srand((unsigned)time(NULL)); // 난수 생성기 초기화
    currentBlock = new Tetromino(rand() % 7); // 현재 블록 생성
    nextBlock = new Tetromino(rand() % 7); // 🔹 다음 블록 생성
}

// 소멸자
CTetrisMFCView::~CTetrisMFCView()
{
    delete currentBlock;
}

// 🔹 윈도우 생성 시 타이머 설정
int CTetrisMFCView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CView::OnCreate(lpCreateStruct) == -1)
        return -1;

    StartTimer(); // 🔹 타이머 시작
    return 0;
}
// 🔹 타이머 이벤트 (자동 낙하)
void CTetrisMFCView::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == 1) // 타이머 ID가 1인 경우
    {
        if (!CheckCollision(0, 1)) // 🔹 아래로 이동 가능하면 이동
        {
            currentBlock->y += 1;
        }
        else
        {
            FixBlock(); // 🔹 바닥에 닿으면 블록 고정
        }
        Invalidate(); // 화면 다시 그리기
    }
    CView::OnTimer(nIDEvent);
}

void CTetrisMFCView::StartTimer()
{
    SetTimer(1, 500, NULL); // 🔹 500ms마다 타이머 실행
}

// 🔹 타이머 정지 함수
void CTetrisMFCView::StopTimer()
{
    KillTimer(1); // 🔹 타이머 중지
}

// 🔹 블록 충돌 감지 함수
bool CTetrisMFCView::CheckCollision(int offsetX, int offsetY)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (currentBlock->shape[i][j])
            {
                int newX = currentBlock->x + j + offsetX;
                int newY = currentBlock->y + i + offsetY;

                // 벽 또는 바닥과 충돌 확인
                if (newX < 0 || newX >= BOARD_WIDTH || newY >= BOARD_HEIGHT)
                    return true;

                // 다른 블록과 충돌 확인
                if (board[newY][newX])
                    return true;
            }
        }
    }
    return false; // 충돌 없음
}

// 🔹 블록을 게임 보드에 고정
void CTetrisMFCView::FixBlock()
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (currentBlock->shape[i][j])
            {
                int newX = currentBlock->x + j;
                int newY = currentBlock->y + i;

                if (newY >= 0 && newY < BOARD_HEIGHT && newX >= 0 && newX < BOARD_WIDTH)
                    board[newY][newX] = 1; // 블록을 보드에 고정
            }
        }
    }

    ClearFullLines(); // 🔹 블록이 고정된 후 한 줄이 가득 찼는지 확인하고 삭제
    SpawnNewBlock();  // 🔹 새로운 블록 생성
    Invalidate(); // 화면 다시 그리기
}

// 🔹 한 줄이 가득 찼을 때 삭제하는 함수
void CTetrisMFCView::ClearFullLines()
{
    int linesCleared = 0; // 🔹 삭제된 줄 개수 카운트

    for (int y = 0; y < BOARD_HEIGHT; y++)
    {
        bool isFull = true;

        // 🔹 현재 줄이 꽉 찼는지 확인
        for (int x = 0; x < BOARD_WIDTH; x++)
        {
            if (board[y][x] == 0) // 빈 칸이 있으면 꽉 찬 줄이 아님
            {
                isFull = false;
                break;
            }
        }

        // 🔹 한 줄이 가득 찼다면 삭제 후 위의 블록을 아래로 이동
        if (isFull)
        {
            linesCleared++; // 🔹 삭제된 줄 개수 증가

            for (int newY = y; newY > 0; newY--) // 한 칸씩 아래로 이동
            {
                for (int x = 0; x < BOARD_WIDTH; x++)
                {
                    board[newY][x] = board[newY - 1][x]; // 윗줄을 아래로 복사
                }
            }

            // 🔹 맨 윗줄을 비움 (빈 행으로 초기화)
            for (int x = 0; x < BOARD_WIDTH; x++)
            {
                board[0][x] = 0;
            }
        }
    }

    if (linesCleared > 0)
    {
        UpdateScore(linesCleared); // 🔹 삭제된 줄 수를 기반으로 점수 업데이트
    }
}


void CTetrisMFCView::UpdateScore(int linesCleared)
{
    switch (linesCleared)
    {
    case 1:
        score += 100; // 1줄 삭제 = 100점
        break;
    case 2:
        score += 300; // 2줄 삭제 = 300점
        break;
    case 3:
        score += 500; // 3줄 삭제 = 500점
        break;
    case 4:
        score += 800; // 4줄 (테트리스) = 800점
        break;
    default:
        break;
    }

    IncreaseLevel(); // 🔹 점수가 오를 때마다 난이도 증가 확인
    Invalidate(); // 🔹 화면 다시 그려 점수 표시 업데이트
}

void CTetrisMFCView::IncreaseLevel()
{
    int newLevel = score / 1000 + 1; // 🔹 1000점마다 레벨 증가

    if (newLevel > level) // 🔹 새 레벨이 현재 레벨보다 높으면 증가
    {
        level = newLevel;
        dropSpeed = max(100, 500 - (level - 1) * 50); // 🔹 최소 속도 100ms 제한
        KillTimer(1);
        SetTimer(1, dropSpeed, NULL); // 🔹 새로운 속도로 타이머 재설정
    }
}


// 🔹 새 블록 생성
void CTetrisMFCView::SpawnNewBlock()
{
    delete currentBlock; // 기존 블록 삭제
    currentBlock = nextBlock; // 🔹 다음 블록을 현재 블록으로 변경
    nextBlock = new Tetromino(rand() % 7); // 🔹 새로운 다음 블록 생성

    // 🔹 생성 직후 충돌하면 게임 오버 처리
    if (CheckCollision(0, 0))
    {
        AfxMessageBox(_T("Game Over!"));
        exit(0); // 게임 종료
    }
}


// 🔹 키 입력 처리 (블록 이동)
void CTetrisMFCView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    switch (nChar)
    {
    case VK_LEFT:  // ← 키: 블록을 왼쪽으로 이동
        if (!CheckCollision(-1, 0))
            currentBlock->x -= 1;
        break;
    case VK_RIGHT: // → 키: 블록을 오른쪽으로 이동
        if (!CheckCollision(1, 0))
            currentBlock->x += 1;
        break;
    case VK_DOWN:  // ↓ 키: 블록을 아래로 이동
        if (!CheckCollision(0, 1))
            currentBlock->y += 1;
        else
            FixBlock(); // 바닥 또는 다른 블록과 충돌하면 블록 고정
        break;
    case VK_UP:    // 🔹 ↑ 키: 블록 90도 회전
        currentBlock->Rotate();
        if (CheckCollision(0, 0)) // 회전 후 충돌하면 원상 복구
            currentBlock->Rotate();
        break;
    }

    Invalidate(); // 🔹 화면 다시 그리기 (OnDraw() 호출)
    CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

// 기타 MFC 기본 함수들...
BOOL CTetrisMFCView::PreCreateWindow(CREATESTRUCT& cs)
{
    return CView::PreCreateWindow(cs);
}

BOOL CTetrisMFCView::OnPreparePrinting(CPrintInfo* pInfo)
{
    return DoPreparePrinting(pInfo);
}

void CTetrisMFCView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CTetrisMFCView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CTetrisMFCView::AssertValid() const
{
    CView::AssertValid();
}

void CTetrisMFCView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

void CTetrisMFCView::OnDraw(CDC* pDC)
{
    int cellSize = 30;
    int rows = BOARD_HEIGHT, cols = BOARD_WIDTH;

    // 🔹 1️⃣ 격자 그리기
    for (int i = 0; i <= rows; i++) {
        pDC->MoveTo(0, i * cellSize);
        pDC->LineTo(cols * cellSize, i * cellSize);
    }
    for (int j = 0; j <= cols; j++) {
        pDC->MoveTo(j * cellSize, 0);
        pDC->LineTo(j * cellSize, rows * cellSize);
    }

    // 🔹 2️⃣ 고정된 블록(보드) 그리기
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (board[i][j]) {
                CBrush brush(RGB(100, 100, 100)); // 회색 블록
                CBrush* pOldBrush = pDC->SelectObject(&brush);

                pDC->Rectangle(
                    j * cellSize, i * cellSize,
                    (j + 1) * cellSize, (i + 1) * cellSize
                );

                pDC->SelectObject(pOldBrush);
            }
        }
    }

    // 🔹 3️⃣ 현재 블록 그리기
    if (currentBlock) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (currentBlock->shape[i][j]) {
                    CBrush brush(currentBlock->color);
                    CBrush* pOldBrush = pDC->SelectObject(&brush);

                    pDC->Rectangle(
                        (currentBlock->x + j) * cellSize,
                        (currentBlock->y + i) * cellSize,
                        (currentBlock->x + j + 1) * cellSize,
                        (currentBlock->y + i + 1) * cellSize
                    );

                    pDC->SelectObject(pOldBrush);
                }
            }
        }
    }

    // 🔹 4️⃣ 점수 및 레벨 표시
    CString scoreText, levelText;
    scoreText.Format(_T("Score: %d"), score);
    levelText.Format(_T("Level: %d"), level);
    pDC->TextOut(cols * cellSize + 20, 20, scoreText);
    pDC->TextOut(cols * cellSize + 20, 50, levelText);

    // 🔹 5️⃣ 다음 블록 미리보기 추가
    DrawNextBlock(pDC);
}


void CTetrisMFCView::DrawNextBlock(CDC* pDC)
{
    int cellSize = 20; // 🔹 다음 블록은 크기를 작게 표시
    int offsetX = BOARD_WIDTH * 30 + 50; // 🔹 오른쪽 여백에 표시
    int offsetY = 100;

    // 🔹 "Next Block" 텍스트 표시
    pDC->TextOut(offsetX, offsetY - 30, _T("Next Block:"));

    if (nextBlock)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                if (nextBlock->shape[i][j]) // 🔹 블록이 있는 부분만 그리기
                {
                    CBrush brush(nextBlock->color);
                    CBrush* pOldBrush = pDC->SelectObject(&brush);

                    pDC->Rectangle(
                        offsetX + j * cellSize,
                        offsetY + i * cellSize,
                        offsetX + (j + 1) * cellSize,
                        offsetY + (i + 1) * cellSize
                    );

                    pDC->SelectObject(pOldBrush);
                }
            }
        }
    }
}




void CTetrisMFCView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
    AFXPrintPreview(this);
#endif
}

void CTetrisMFCView::OnRButtonUp(UINT nFlags, CPoint point)
{
    ClientToScreen(&point);
    OnContextMenu(this, point);
}

void CTetrisMFCView::OnContextMenu(CWnd* pWnd, CPoint point)
{
#ifndef SHARED_HANDLERS
    theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}
