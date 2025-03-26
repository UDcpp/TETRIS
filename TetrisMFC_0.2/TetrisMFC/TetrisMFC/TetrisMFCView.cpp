
// TetrisMFCView.cpp: CTetrisMFCView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "TetrisMFC.h"
#endif

#include "TetrisMFCDoc.h"
#include "TetrisMFCView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTetrisMFCView

IMPLEMENT_DYNCREATE(CTetrisMFCView, CView)

BEGIN_MESSAGE_MAP(CTetrisMFCView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CTetrisMFCView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CTetrisMFCView 생성/소멸

CTetrisMFCView::CTetrisMFCView() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CTetrisMFCView::~CTetrisMFCView()
{
}

BOOL CTetrisMFCView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CTetrisMFCView 그리기

// CTetrisMFCView.cpp 내부 OnDraw 함수 수정
void CTetrisMFCView::OnDraw(CDC* pDC)
{
	CTetrisMFCDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CRect clientRect;
	GetClientRect(&clientRect);

	int boardWidthPx = CTetrisMFCDoc::BOARD_WIDTH * CTetrisMFCDoc::CELL_SIZE;
	int boardHeightPx = CTetrisMFCDoc::BOARD_HEIGHT * CTetrisMFCDoc::CELL_SIZE;
	int boardOffsetX = (clientRect.Width() - boardWidthPx) / 2;
	int boardOffsetY = (clientRect.Height() - boardHeightPx) / 2;

	// ✅ 메인화면 - 시작 이미지 출력
	if (!pDoc->isStarted)
	{
		CBitmap bitmap;
		bitmap.LoadBitmap(IDB_START); // 리소스 ID는 Start.bmp 등록 시 사용한 ID
		CDC memDC;
		memDC.CreateCompatibleDC(pDC);
		CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);

		BITMAP bmp;
		bitmap.GetBitmap(&bmp);
		double scale = min((double)clientRect.Width() / bmp.bmWidth, (double)clientRect.Height() / bmp.bmHeight);
		int newWidth = (int)(bmp.bmWidth * scale);
		int newHeight = (int)(bmp.bmHeight * scale);
		int x = (clientRect.Width() - newWidth) / 2;
		int y = (clientRect.Height() - newHeight) / 2;

		pDC->StretchBlt(x, y, newWidth, newHeight, &memDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
		memDC.SelectObject(pOldBitmap);
		return;
	}

	// 게임 화면 배경 흰색
	pDC->FillSolidRect(&clientRect, RGB(255, 255, 255));

	// 격자
	CPen pen(PS_SOLID, 1, RGB(0, 0, 0));
	CPen* pOldPen = pDC->SelectObject(&pen);
	for (int y = 0; y <= CTetrisMFCDoc::BOARD_HEIGHT; ++y)
	{
		int yPos = boardOffsetY + y * CTetrisMFCDoc::CELL_SIZE;
		pDC->MoveTo(boardOffsetX, yPos);
		pDC->LineTo(boardOffsetX + boardWidthPx, yPos);
	}
	for (int x = 0; x <= CTetrisMFCDoc::BOARD_WIDTH; ++x)
	{
		int xPos = boardOffsetX + x * CTetrisMFCDoc::CELL_SIZE;
		pDC->MoveTo(xPos, boardOffsetY);
		pDC->LineTo(xPos, boardOffsetY + boardHeightPx);
	}
	pDC->SelectObject(pOldPen);

	// 현재 블록
	COLORREF color = CTetrisMFCDoc::BLOCK_COLORS[pDoc->currentBlockType];
	for (int y = 0; y < CTetrisMFCDoc::BLOCK_SIZE; ++y)
	{
		for (int x = 0; x < CTetrisMFCDoc::BLOCK_SIZE; ++x)
		{
			if (pDoc->currentBlock[y][x] == 1)
			{
				int drawX = boardOffsetX + (pDoc->currentX + x) * CTetrisMFCDoc::CELL_SIZE;
				int drawY = boardOffsetY + (pDoc->currentY + y) * CTetrisMFCDoc::CELL_SIZE;
				if (drawY < 0) continue;
				CBrush brush(color);
				CBrush* pOldBrush = pDC->SelectObject(&brush);
				pDC->Rectangle(drawX, drawY, drawX + CTetrisMFCDoc::CELL_SIZE, drawY + CTetrisMFCDoc::CELL_SIZE);
				pDC->SelectObject(pOldBrush);
			}
		}
	}

	// 고정된 블록
	for (int y = 0; y < CTetrisMFCDoc::BOARD_HEIGHT; ++y)
	{
		for (int x = 0; x < CTetrisMFCDoc::BOARD_WIDTH; ++x)
		{
			int blockType = pDoc->board[y][x];
			if (blockType > 0)
			{
				COLORREF color = CTetrisMFCDoc::BLOCK_COLORS[blockType - 1];
				int drawX = boardOffsetX + x * CTetrisMFCDoc::CELL_SIZE;
				int drawY = boardOffsetY + y * CTetrisMFCDoc::CELL_SIZE;
				CBrush brush(color);
				CBrush* pOldBrush = pDC->SelectObject(&brush);
				pDC->Rectangle(drawX, drawY, drawX + CTetrisMFCDoc::CELL_SIZE, drawY + CTetrisMFCDoc::CELL_SIZE);
				pDC->SelectObject(pOldBrush);
			}
		}
	}

	// 정보 출력
	int infoX = boardOffsetX + boardWidthPx + 20;
	CString scoreText, levelText, highScoreText;
	scoreText.Format(_T("Score: %d"), pDoc->score);
	levelText.Format(_T("Level: %d"), pDoc->level);
	highScoreText.Format(_T("High Score: %d"), pDoc->highScore);
	pDC->TextOutW(infoX, boardOffsetY, scoreText);
	pDC->TextOutW(infoX, boardOffsetY + 30, levelText);
	pDC->TextOutW(infoX, boardOffsetY + 60, highScoreText);
	pDC->TextOutW(infoX, boardOffsetY + 110, _T("Next:"));

	COLORREF nextColor = CTetrisMFCDoc::BLOCK_COLORS[pDoc->nextBlockType];
	for (int y = 0; y < CTetrisMFCDoc::BLOCK_SIZE; ++y)
	{
		for (int x = 0; x < CTetrisMFCDoc::BLOCK_SIZE; ++x)
		{
			if (pDoc->nextBlock[y][x] == 1)
			{
				int drawX = infoX + x * CTetrisMFCDoc::CELL_SIZE;
				int drawY = boardOffsetY + 130 + y * CTetrisMFCDoc::CELL_SIZE;
				CBrush brush(nextColor);
				CBrush* pOldBrush = pDC->SelectObject(&brush);
				pDC->Rectangle(drawX, drawY, drawX + CTetrisMFCDoc::CELL_SIZE, drawY + CTetrisMFCDoc::CELL_SIZE);
				pDC->SelectObject(pOldBrush);
			}
		}
	}

	// 일시정지/게임오버 메시지
	if (pDoc->isPaused || pDoc->isGameOver)
	{
		CRect messageRect;
		GetClientRect(&messageRect);
		int oldBkMode = pDC->SetBkMode(TRANSPARENT);
		CFont bigFont, smallFont;
		bigFont.CreatePointFont(300, _T("Arial Black"));
		smallFont.CreatePointFont(150, _T("Arial"));
		CFont* pOldFont = pDC->SelectObject(&bigFont);
		CString mainText, subText;
		COLORREF mainColor, subColor;
		if (pDoc->isPaused)
		{
			mainText = _T("PAUSED");
			subText = _T("Press ESC to Resume");
			mainColor = RGB(255, 140, 0);
			subColor = RGB(200, 0, 0);
		}
		else
		{
			mainText = _T("GAME OVER");
			subText = _T("Press R to Restart");
			mainColor = RGB(200, 0, 0);
			subColor = RGB(80, 80, 80);
		}
		pDC->SetTextColor(mainColor);
		pDC->DrawText(mainText, messageRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		pDC->SelectObject(&smallFont);
		CRect subRect = messageRect;
		subRect.top += 100;
		pDC->SetTextColor(subColor);
		pDC->DrawText(subText, subRect, DT_CENTER | DT_TOP | DT_SINGLELINE);
		pDC->SelectObject(pOldFont);
		pDC->SetBkMode(oldBkMode);
	}
}






// CTetrisMFCView 인쇄


void CTetrisMFCView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CTetrisMFCView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CTetrisMFCView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CTetrisMFCView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}

void CTetrisMFCView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CTetrisMFCView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CTetrisMFCView 진단

#ifdef _DEBUG
void CTetrisMFCView::AssertValid() const
{
	CView::AssertValid();
}

void CTetrisMFCView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTetrisMFCDoc* CTetrisMFCView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTetrisMFCDoc)));
	return (CTetrisMFCDoc*)m_pDocument;
}
#endif //_DEBUG


// CTetrisMFCView 메시지 처리기

void CTetrisMFCView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// 게임은 아직 시작되지 않았기 때문에 타이머를 설정하지 않음
	// isStarted == false 상태에서는 메인 화면만 표시
	// Enter 키를 누르면 타이머 시작됨 (PreTranslateMessage에서 처리함)
}



void CTetrisMFCView::OnTimer(UINT_PTR nIDEvent)
{
	CTetrisMFCDoc* pDoc = GetDocument();

	// 🔹 아직 게임이 시작되지 않았으면 타이머 무시
	if (!pDoc->isStarted)
		return;

	// 🔹 게임 오버 처리
	if (pDoc->isGameOver)
	{
		KillTimer(1); // 타이머 멈춤
		AfxMessageBox(_T("Game Over!"), MB_OK | MB_ICONINFORMATION);
		return;
	}

	// 🔹 자동 낙하 처리
	if (nIDEvent == 1)
	{
		if (!pDoc->IsBlockColliding())
		{
			pDoc->currentY++;
		}
		else
		{
			pDoc->FixCurrentBlock();
			pDoc->ClearFullLines();
		}
		Invalidate();

		// 속도 재조정
		KillTimer(1);
		int speed = max(100, 500 - (pDoc->level * 50));
		SetTimer(1, speed, NULL);
	}

	CView::OnTimer(nIDEvent);
}



BOOL CTetrisMFCView::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		CTetrisMFCDoc* pDoc = GetDocument();
		switch (pMsg->wParam)
		{

		case VK_RETURN: // 엔터 키 → 게임 시작
			if (!pDoc->isStarted)
			{
				pDoc->isStarted = true;
				pDoc->score = 0;
				pDoc->level = 0;
				pDoc->isPaused = false;
				pDoc->isGameOver = false;

				// 블록 준비
				pDoc->nextBlockType = rand() % 7;
				for (int y = 0; y < CTetrisMFCDoc::BLOCK_SIZE; ++y)
				{
					for (int x = 0; x < CTetrisMFCDoc::BLOCK_SIZE; ++x)
					{
						pDoc->nextBlock[y][x] = CTetrisMFCDoc::BLOCK_SHAPES[pDoc->nextBlockType][y][x];
					}
				}
				pDoc->GenerateRandomBlock();
				pDoc->PlayBGM();  // ✅ 배경음악 재생

				// 타이머 시작
				int speed = max(100, 500 - (pDoc->level * 50));
				SetTimer(1, speed, NULL);
				Invalidate();
			}
			return TRUE;

		case VK_RIGHT:
			if (pDoc->CanMove(1, 0))
				pDoc->currentX++;
			Invalidate();
			return TRUE;

		case VK_LEFT:
			if (pDoc->CanMove(-1, 0))
				pDoc->currentX--;
			Invalidate();
			return TRUE;

		case VK_DOWN:
			if (pDoc->CanMove(0, 1))
				pDoc->currentY++;
			Invalidate();
			return TRUE;

		case VK_UP: // ↑
			pDoc->RotateBlock();
			Invalidate();
			return TRUE;

		case VK_SPACE: // 즉시 낙하
			while (pDoc->CanMove(0, 1))
			{
				pDoc->currentY++;
			}
			// 충돌 직전 위치에 도달한 상태니까 이제 고정
			pDoc->FixCurrentBlock();
			pDoc->ClearFullLines();
			Invalidate();
			return TRUE;

		case VK_ESCAPE:
			if (!pDoc->isGameOver)
			{
				if (pDoc->isPaused)
				{
					// 다시 시작
					int speed = max(100, 500 - (pDoc->level * 50));
					SetTimer(1, speed, NULL);
					pDoc->isPaused = false;

					pDoc->PlayBGM();  // ✅ 다시 재생
				}
				else
				{
					// 일시정지
					KillTimer(1);
					pDoc->isPaused = true;

					pDoc->StopBGM();  // ✅ 멈춤
				}

				Invalidate(); // ✅ 여기 꼭 필요함!
			}
			return TRUE;


		case 'R': // 게임 오버일 때 재시작
			if (pDoc->isGameOver)
			{
				// 보드 초기화
				for (int y = 0; y < CTetrisMFCDoc::BOARD_HEIGHT; ++y)
				{
					for (int x = 0; x < CTetrisMFCDoc::BOARD_WIDTH; ++x)
					{
						pDoc->board[y][x] = 0;
					}
				}

				pDoc->score = 0;
				pDoc->level = 0;
				pDoc->isPaused = false;
				pDoc->isGameOver = false;

				// next 블록 새로 만들기
				pDoc->nextBlockType = rand() % 7;
				for (int y = 0; y < CTetrisMFCDoc::BLOCK_SIZE; ++y)
				{
					for (int x = 0; x < CTetrisMFCDoc::BLOCK_SIZE; ++x)
					{
						pDoc->nextBlock[y][x] = CTetrisMFCDoc::BLOCK_SHAPES[pDoc->nextBlockType][y][x];
					}
				}

				pDoc->GenerateRandomBlock();

				int speed = max(100, 500 - (pDoc->level * 50));
				SetTimer(1, speed, NULL);
				Invalidate();
			}
			return TRUE;
		}
	}

	return CView::PreTranslateMessage(pMsg);
}

