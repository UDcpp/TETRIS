
// TetrisMFCDoc.cpp: CTetrisMFCDoc 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "TetrisMFC.h"
#endif

#include "TetrisMFCDoc.h"

#include <propkey.h>
#include <fstream>

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int CTetrisMFCDoc::BLOCK_SHAPES[CTetrisMFCDoc::NUM_BLOCKS][CTetrisMFCDoc::BLOCK_SIZE][CTetrisMFCDoc::BLOCK_SIZE] =
{
    // I
    {
        {0,0,0,0},
        {1,1,1,1},
        {0,0,0,0},
        {0,0,0,0}
    },
    // O
    {
        {0,1,1,0},
        {0,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    // T
    {
        {0,1,0,0},
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    // S
    {
        {0,1,1,0},
        {1,1,0,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    // Z
    {
        {1,1,0,0},
        {0,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    // J
    {
        {1,0,0,0},
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    // L
    {
        {0,0,1,0},
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    }
};

const COLORREF CTetrisMFCDoc::BLOCK_COLORS[CTetrisMFCDoc::NUM_BLOCKS] =
{
    RGB(0, 255, 255),   // I - 시안
    RGB(255, 255, 0),   // O - 노랑
    RGB(128, 0, 128),   // T - 보라
    RGB(0, 255, 0),     // S - 초록
    RGB(255, 0, 0),     // Z - 빨강
    RGB(0, 0, 255),     // J - 파랑
    RGB(255, 165, 0)    // L - 주황
};

// CTetrisMFCDoc

IMPLEMENT_DYNCREATE(CTetrisMFCDoc, CDocument)

BEGIN_MESSAGE_MAP(CTetrisMFCDoc, CDocument)
END_MESSAGE_MAP()


// CTetrisMFCDoc 생성/소멸

CTetrisMFCDoc::CTetrisMFCDoc() noexcept
{
    // 보드 초기화
    for (int y = 0; y < BOARD_HEIGHT; ++y)
    {
        for (int x = 0; x < BOARD_WIDTH; ++x)
        {
            board[y][x] = 0;
        }
    }

    score = 0;
    LoadHighScore();
    level = 0;

    srand((unsigned)time(NULL)); // 랜덤 시드 초기화

    // ✅ 먼저 nextBlock 생성
    nextBlockType = rand() % 7;
    for (int y = 0; y < BLOCK_SIZE; ++y)
    {
        for (int x = 0; x < BLOCK_SIZE; ++x)
        {
            nextBlock[y][x] = BLOCK_SHAPES[nextBlockType][y][x];
        }
    }

    // ✅ 그런 다음 currentBlock으로 넘김
    GenerateRandomBlock();
    isGameOver = false;
    isPaused = false;
    isStarted = false;

}



void CTetrisMFCDoc::GenerateRandomBlock()
{
    // 현재 블록 → next 블록으로부터 가져오기
    currentBlockType = nextBlockType;
    for (int y = 0; y < BLOCK_SIZE; ++y)
    {
        for (int x = 0; x < BLOCK_SIZE; ++x)
        {
            currentBlock[y][x] = nextBlock[y][x];
        }
    }

    currentX = BOARD_WIDTH / 2 - 2;
    currentY = 0;

    // 새로운 next 블록 생성
    nextBlockType = rand() % 7;
    for (int y = 0; y < BLOCK_SIZE; ++y)
    {
        for (int x = 0; x < BLOCK_SIZE; ++x)
        {
            nextBlock[y][x] = BLOCK_SHAPES[nextBlockType][y][x];
        }
    }
}


void CTetrisMFCDoc::ClearFullLines()
{
    int linesCleared = 0;

    for (int y = BOARD_HEIGHT - 1; y >= 0; --y)
    {
        bool full = true;
        for (int x = 0; x < BOARD_WIDTH; ++x)
        {
            if (board[y][x] == 0)
            {
                full = false;
                break;
            }
        }

        if (full)
        {
            for (int row = y; row > 0; --row)
            {
                for (int col = 0; col < BOARD_WIDTH; ++col)
                    board[row][col] = board[row - 1][col];
            }
            for (int col = 0; col < BOARD_WIDTH; ++col)
                board[0][col] = 0;

            ++linesCleared;
            ++y; // 같은 줄 다시 검사
        }
    }

    // 🔥 점수 증가
    score += linesCleared * 100;

    // 레벨업 (1000점마다 1레벨 상승)
    int newLevel = score / 1000;
    if (newLevel > level)
    {
        level = newLevel;
    }

    // 게임 오버 검사
    for (int y = 0; y < BLOCK_SIZE; ++y)
    {
        for (int x = 0; x < BLOCK_SIZE; ++x)
        {
            if (currentBlock[y][x] == 1)
            {
                int boardX = currentX + x;
                int boardY = currentY + y;

                if (boardY >= 0 && board[boardY][boardX] != 0)
                {
                    isGameOver = true;

                    // ✅ 최고 점수 갱신 및 저장
                    if (score > highScore)
                    {
                        highScore = score;
                        SaveHighScore();
                    }

                    StopBGM();  // ✅ 게임 오버 시 음악 정지
                    return;
                }
            }
        }
    }

    // ✅ 게임 오버가 아니더라도 최고 점수 갱신 필요
    if (score > highScore)
    {
        highScore = score;
        SaveHighScore();
    }
}



void CTetrisMFCDoc::SaveHighScore()
{
    CStdioFile file;
    if (file.Open(_T("highscore.txt"), CFile::modeCreate | CFile::modeWrite | CFile::typeText))
    {
        CString str;
        str.Format(_T("%d"), highScore);
        file.WriteString(str);
        file.Close();
    }
}

void CTetrisMFCDoc::LoadHighScore()
{
    CStdioFile file;
    if (file.Open(_T("highscore.txt"), CFile::modeRead | CFile::typeText))
    {
        CString str;
        if (file.ReadString(str))
        {
            highScore = _ttoi(str);
        }
        file.Close();
    }
    else
    {
        highScore = 0; // 파일이 없으면 0점부터 시작
    }
}



void CTetrisMFCDoc::RotateBlock()
{
    int rotated[BLOCK_SIZE][BLOCK_SIZE] = { 0 };

    // 1. 회전 시뮬레이션
    for (int y = 0; y < BLOCK_SIZE; ++y)
    {
        for (int x = 0; x < BLOCK_SIZE; ++x)
        {
            rotated[x][BLOCK_SIZE - 1 - y] = currentBlock[y][x];
        }
    }

    // 2. 회전된 블록이 보드 안에 들어가는지 검사
    for (int y = 0; y < BLOCK_SIZE; ++y)
    {
        for (int x = 0; x < BLOCK_SIZE; ++x)
        {
            if (rotated[y][x] == 0)
                continue;

            int boardX = currentX + x;
            int boardY = currentY + y;

            if (boardX < 0 || boardX >= BOARD_WIDTH || boardY >= BOARD_HEIGHT)
                return; // 회전하면 나가니까 취소

            if (boardY >= 0 && board[boardY][boardX] != 0)
                return; // 회전하면 충돌나니까 취소
        }
    }

    // 3. 회전 결과 적용
    for (int y = 0; y < BLOCK_SIZE; ++y)
    {
        for (int x = 0; x < BLOCK_SIZE; ++x)
        {
            currentBlock[y][x] = rotated[y][x];
        }
    }
}


bool CTetrisMFCDoc::CanMove(int dx, int dy)
{
    for (int y = 0; y < BLOCK_SIZE; ++y)
    {
        for (int x = 0; x < BLOCK_SIZE; ++x)
        {
            if (currentBlock[y][x] == 0)
                continue;

            int newX = currentX + x + dx;
            int newY = currentY + y + dy;

            if (newX < 0 || newX >= BOARD_WIDTH || newY >= BOARD_HEIGHT)
                return false;

            if (newY >= 0 && board[newY][newX] != 0)
                return false;
        }
    }
    return true;
}


bool CTetrisMFCDoc::IsBlockColliding()
{
    for (int y = 0; y < BLOCK_SIZE; ++y)
    {
        for (int x = 0; x < BLOCK_SIZE; ++x)
        {
            if (currentBlock[y][x] == 0)
                continue;

            int boardX = currentX + x;
            int boardY = currentY + y + 1;  // ⬅️ 한 칸 아래 검사

            // 1. 보드 바깥이면 충돌
            if (boardY >= BOARD_HEIGHT)
                return true;

            // 2. 이미 고정된 블록이 있다면 충돌
            if (board[boardY][boardX] != 0)
                return true;
        }
    }
    return false;
}



void CTetrisMFCDoc::FixCurrentBlock()
{
    for (int y = 0; y < BLOCK_SIZE; ++y)
    {
        for (int x = 0; x < BLOCK_SIZE; ++x)
        {
            if (currentBlock[y][x] == 1)
            {
                int bx = currentX + x;
                int by = currentY + y;
                if (by >= 0 && by < BOARD_HEIGHT && bx >= 0 && bx < BOARD_WIDTH)
                    board[by][bx] = currentBlockType + 1; // 색 구분 위해 1부터 저장
            }
        }
    }

    // 블록 고정 후 새 블록 생성
    GenerateRandomBlock();
}

void CTetrisMFCDoc::PlayBGM()
{
    PlaySound(_T("Background2.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
}

void CTetrisMFCDoc::StopBGM()
{
    PlaySound(NULL, NULL, 0);
}


CTetrisMFCDoc::~CTetrisMFCDoc()
{
}

BOOL CTetrisMFCDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CTetrisMFCDoc serialization

void CTetrisMFCDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}

#ifdef SHARED_HANDLERS

// 축소판 그림을 지원합니다.
void CTetrisMFCDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 문서의 데이터를 그리려면 이 코드를 수정하십시오.
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 검색 처리기를 지원합니다.
void CTetrisMFCDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 문서의 데이터에서 검색 콘텐츠를 설정합니다.
	// 콘텐츠 부분은 ";"로 구분되어야 합니다.

	// 예: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CTetrisMFCDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CTetrisMFCDoc 진단

#ifdef _DEBUG
void CTetrisMFCDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTetrisMFCDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CTetrisMFCDoc 명령
