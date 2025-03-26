
// TetrisMFCDoc.h: CTetrisMFCDoc 클래스의 인터페이스
//


#pragma once


class CTetrisMFCDoc : public CDocument
{
protected: // serialization에서만 만들어집니다.
	CTetrisMFCDoc() noexcept;
	DECLARE_DYNCREATE(CTetrisMFCDoc)

// 특성입니다.s
public:
	static const int BOARD_WIDTH = 10;
	static const int BOARD_HEIGHT = 20;
	static const int CELL_SIZE = 30; // 셀 하나 크기 (30x30 픽셀)
	static const int BOARD_OFFSET_X = 100;
	static const int BOARD_OFFSET_Y = 50;

	static const int NUM_BLOCKS = 7;
	static const int BLOCK_SIZE = 4;

	static const int BLOCK_SHAPES[NUM_BLOCKS][BLOCK_SIZE][BLOCK_SIZE];
	static const COLORREF BLOCK_COLORS[NUM_BLOCKS];

	void GenerateRandomBlock();
	void RotateBlock();
	void ClearFullLines();
	bool IsBlockColliding();         // 충돌 여부 확인
	void FixCurrentBlock();          // 블록 고정 후 다음 블록 생성
	bool CanMove(int dx, int dy);
	int score; // 현재 점수
	int highScore;
	void LoadHighScore();
	void SaveHighScore();
	void PlayBGM();   // 배경음악 재생
	void StopBGM();   // 배경음악 정지
	int level; // 현재 레벨
	int nextBlockType;
	int nextBlock[BLOCK_SIZE][BLOCK_SIZE];


	const int PREVIEW_OFFSET_X = BOARD_WIDTH * CELL_SIZE + 40; // 오른쪽 여백
	const int PREVIEW_OFFSET_Y = 100;

	bool isGameOver;
	bool isPaused;

	bool isStarted; // 메인화면 상태 여부







// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 구현입니다.
public:
	virtual ~CTetrisMFCDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	int board[BOARD_HEIGHT][BOARD_WIDTH]; // 보드 배열

	int currentBlock[BLOCK_SIZE][BLOCK_SIZE]; // 현재 블록 모양
	int currentBlockType;                     // 0~6 블록 타입
	int currentX, currentY;                   // 현재 블록 좌표

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 검색 처리기에 대한 검색 콘텐츠를 설정하는 도우미 함수
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
