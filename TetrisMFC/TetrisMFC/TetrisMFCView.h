// TetrisMFCView.h: CTetrisMFCView 클래스의 인터페이스
//

#pragma once

#include <afxwin.h>
#include "Tetromino.h"
#include "TetrisMFCDoc.h"  // 🔹 문서 클래스 포함

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20

class CTetrisMFCView : public CView
{
protected: // serialization에서만 만들어집니다.
	CTetrisMFCView() noexcept;
	DECLARE_DYNCREATE(CTetrisMFCView)
	int score = 0;      // 현재 점수 저장
	int level = 1;      // 🔹 현재 난이도 (레벨)
	int dropSpeed = 500; // 🔹 현재 블록 낙하 속도 (ms 단위)

	void UpdateScore(int linesCleared); // 점수 업데이트 함수
	void IncreaseLevel();               // 🔹 난이도 증가 함수
	Tetromino* nextBlock; // 🔹 다음 블록 저장
	void DrawNextBlock(CDC* pDC); // 🔹 다음 블록을 그리는 함수


	// 특성입니다.
public:
	CTetrisMFCDoc* GetDocument() const;

	// 작업입니다.
public:
	Tetromino* currentBlock; // 현재 블록 추가

protected:  // 🔹 게임 보드 데이터 추가 위치 (protected 섹션)
	int board[BOARD_HEIGHT][BOARD_WIDTH] = { 0 }; // 게임 보드 배열 (0 = 빈 공간, 1 = 블록)

	void FixBlock(); // 블록을 고정하는 함수
	bool CheckCollision(int offsetX, int offsetY); // 충돌 감지 함수
	void SpawnNewBlock(); // 새 블록 생성 함수
	void ClearFullLines();  // 🔹 한 줄이 가득 차면 삭제하는 함수
	void StartTimer(); // 🔹 타이머 시작 함수
	void StopTimer(); // 🔹 타이머 정지 함수

	// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC) override;  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

	// 구현입니다.
public:
	virtual ~CTetrisMFCView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	// 생성된 메시지 맵 함수
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags); // 키 입력 추가
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct); // 🔹 윈도우 생성 시 타이머 설정
	afx_msg void OnTimer(UINT_PTR nIDEvent); // 🔹 타이머 이벤트 처리
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // TetrisMFCView.cpp의 디버그 버전
inline CTetrisMFCDoc* CTetrisMFCView::GetDocument() const
{
	return reinterpret_cast<CTetrisMFCDoc*>(m_pDocument);
}
#endif
