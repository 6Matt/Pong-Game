// included files
#include "stdafx.h"
#include "Pong Game.h"

#define MAX_LOADSTRING 100	// code included by Visual C++

using namespace std;

// code included by Visual C++
HINSTANCE hInst;
TCHAR szTitle[MAX_LOADSTRING];
TCHAR szWindowClass[MAX_LOADSTRING];

// definition of global variables and functions
HANDLE m_timerHandle;
void CALLBACK TimerProc();
void beginGame();
void calculateIntervals();
void ballevent(char collisiontype);

#define PI 3.14159265

int ballx = 290, bally = 244, ppaddley = 195, cpaddley = 195;

double velocity = 0;
int direction = 0, xintervals = 0, yintervals = 0, ticker = 1;

bool movingup = false, movingright = false, cmovingup = false, ppaddleinmotion = false, gamestarted = false, gameended = false;

int compscore = 0, playerscore = 0, finalscore = 0;

// code included by Visual C++
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

// class for timer
class timerclass{
	public:

	void start()
	{
		// creates the timer
		BOOL success = ::CreateTimerQueueTimer(
			&m_timerHandle,
			NULL,
			(WAITORTIMERCALLBACK)TimerProc,
			this,
			2000,
			1,
			WT_EXECUTEINTIMERTHREAD);
	}

	// code that runs each time the timer is executed
	void QueueTimerHandler()
	{
		if(gameended == false){	// stops the timer when the game ends
			// increments variable that holds the number of timer executions
			if(ticker == 100000){ticker -= 100000;}
			ticker++;

			// code that runs at specified interval and moves ball in x-direction
			if(xintervals != 0 && ticker % xintervals == 0){
				if(movingright == true){ballx++;}
				else{ballx--;}
				SetWindowPos(BLL0, HWND_TOP, ballx, bally, 0, 0, SWP_NOSIZE);
			}

			// code that runs at specified interval and moves ball in y-direction
			if(yintervals != 0 && ticker % yintervals == 0){
				if(movingup == true){bally--;}
				else{bally++;}
				SetWindowPos(BLL0, HWND_TOP, ballx, bally, 0, 0, SWP_NOSIZE);
			}

			// controls movement of computer's paddle
			if(ticker % 6 == 0){
				// reverses direction when the paddle is a specified distance from the ball
				if(cmovingup == true && (cpaddley <= 3 || (bally + 6) - (cpaddley + 30) > 58 ) ){cmovingup = false;}
				else if(cmovingup == false && (cpaddley >= 381 || (cpaddley + 30) - (bally + 6) > 58 ) ){cmovingup = true;}
				// changes the y-coordinate
				if(cmovingup == true){cpaddley -= 2;}
				else{cpaddley += 2;}
				// moves the paddle to the new coordinates
				SetWindowPos(PDL0, HWND_TOP, 20, cpaddley, 0, 0, SWP_NOSIZE);
			}
			// changes variable that holds whether the player's paddle has recently moved
			if(ticker % 500 == 0){ppaddleinmotion = false;}
			// executes function when the ball reaches a wall or paddle
			if( (ballx == 32 && movingright == false) || (ballx == 546 && movingright == true) ){ballevent('a');}
			else if( (bally == 1 && movingup == true) || (bally == 436 && movingup == false) ){ballevent('b');}
			else if( (ballx == -4 && movingright == false) || (ballx == 584 && movingright == true) ){ballevent('c');}
		}
	}

	// function from Microsoft documentation, used to call QueueTimerHandler
	static void CALLBACK TimerProc(void* lpParametar, BOOLEAN TimerOrWaitFired)
	{
		timerclass* obj = (timerclass*) lpParametar;
		obj->QueueTimerHandler();
	}
}myTimer;

void beginGame(){
	velocity = 500;

	// randomizes direction of the ball
	do{
		srand( (unsigned int) time(NULL));
		direction = rand() % 350 + 5;
	}while(direction > 85 && direction < 95);

	// calculates the intervals according to velocity and direction
	calculateIntervals();
	// starts the timer
	myTimer.start();
}

void calculateIntervals(){
	// gets acute angle for calculations
	double acute = (double) direction;
	while(acute >= 90){acute -= 90;}

	double tempa = (cos (acute*PI/180)) * velocity;	// calculates x-component of velocity
	if(tempa != 0){
		tempa = 1000 / tempa;						// changes velocity to interval
		// rounds to nearest whole number
		tempa *= 10;
		int remainder = ( (int) tempa ) % 10;
		if(remainder >= 1 && remainder <= 4){tempa -= (double) remainder;}
		else if(remainder >= 5 && remainder <= 9){tempa += (10 - (double) remainder);}
		tempa = tempa / 10;
		// if interval is rounded to zero it is made one
		if(tempa == 0){tempa = 1;}
	}

	double tempb = (sin (acute*PI/180)) * velocity;	// calculates y-component of velocity
	if(tempb != 0){
		tempb = 1000 / tempb;						// changes velocity to interval
		// rounds to nearest whole number
		tempb *= 10;
		int remainder2 = ( (int) tempb ) % 10;
		if(remainder2 >= 1 && remainder2 <= 4){tempb -= (double) remainder2;}
		else if(remainder2 >= 5 && remainder2 <= 9){tempb += (10 - (double) remainder2);}
		tempb = tempb / 10;
		// if interval is rounded to zero it is made one
		if(tempb == 0){tempb = 1;}
	}

	if( (direction >= 0 && direction < 90) || (direction >= 180 && direction < 270) ){
		// assigns intervals for acute angle to appropriate variable
		xintervals = (int) tempb;
		yintervals = (int) tempa;
	}
	else{
		// assigns intervals for acute angle to appropriate variable
		xintervals = (int) tempa;
		yintervals = (int) tempb;
	}
	// determines whether the ball is moving up or down
	if( (direction >= 0 && direction <= 90) || (direction >= 270 && direction <= 360) ){movingup = true;}
	else{movingup = false;}
	// determines whether the ball is moving left or right
	if( direction >= 0 && direction <= 180 ){movingright = true;}
	else{movingright = false;}
}

// function executed when ball collides with wall or paddle
void ballevent(char collisiontype){
	if (collisiontype == 'a'){
		// changes direction of ball when ball collides with paddle, according to where ball strikes
		if(ballx < 296){
			int difference = (bally + 6) - cpaddley;
			if(difference >= 0 && difference <= 60){direction = 30 + (2 * difference);}
		}
		else{
			int difference = (bally + 6) - ppaddley;
			if(difference >= 0 && difference <= 60){direction = 330 - (2 * difference);}
		}
		// increases velocity when ball strikes moving paddle
		if( (ppaddleinmotion == true || ballx < 296) && velocity < 1000){velocity += 100;}

		calculateIntervals();
	}
	else if(collisiontype == 'b'){
		// changes direction when ball strikes wall such that angle is mirrored
		if( (direction > 0 && direction < 90) || (direction > 180 && direction < 270) ){
			int change = direction;
			if(change > 90){change -= 180;}
			change = (90 - change) * 2;
			direction += change;
		}
		else{
			int change = direction;
			if(change > 180){change -= 180;}
			change = 180 - change;
			change = (90 - change) * 2;
			direction -= change;
		}
		// decreases velocity when ball strikes wall
		if(velocity > 500){velocity -= 100;}

		calculateIntervals();
	}
	else if (collisiontype == 'c'){
		if(ballx < 296){
			// increases player score when ball gets past computer paddle
			playerscore++;
			// displays score
			stringstream sstream;
			sstream << playerscore;
			string output = sstream.str();
			std::wstring wtemp(output.begin(), output.end());
			SetWindowText( TXT1, wtemp.c_str());
			// randomizes new direction
			srand( (unsigned int) time(NULL));
			direction = rand() % 131 + 25;
		}
		else{
			// increases computer score when ball gets past player paddle
			compscore++;
			// displays score
			stringstream sstream;
			sstream << compscore;
			string output = sstream.str();
			std::wstring wtemp(output.begin(), output.end());
			SetWindowText( TXT0, wtemp.c_str());
			// randomizes new direction
			srand( (unsigned int) time(NULL));
			direction = rand() % 131 + 205;
		}

		// run when either player reaches a score of five
		if(compscore == 5 || playerscore == 5){
			gameended = true;
			// displays controls for high-scores
			ShowWindow(BTN1, SW_SHOW);
			ShowWindow(TXT3, SW_SHOW);
			ShowWindow(TXT4, SW_SHOW);
			ShowWindow(TXT5, SW_SHOW);
			ShowWindow(TXT6, SW_SHOW);
			ShowWindow(TXT7, SW_SHOW);
			ShowWindow(TXT8, SW_SHOW);
			ShowWindow(TXT9, SW_SHOW);
			ShowWindow(TXT10, SW_SHOW);
			ShowWindow(TXT11, SW_SHOW);
			ShowWindow(TXT12, SW_SHOW);
			ShowWindow(TXT13, SW_SHOW);
			ShowWindow(TXT14, SW_SHOW);
			
			if(playerscore > compscore){
				//calculates score if player wins
				finalscore = (playerscore - compscore)*10;

				// retrieves lowest score from text file
				int lowestscore = 0;
				ifstream scorefile("scores.txt");
				string temp;
				for(int a = 0; a <= 9; a++){getline(scorefile, temp);}
				stringstream(temp) >> lowestscore;
				scorefile.close();

				if(finalscore > lowestscore){
					// asks user for their name if they achieved a high-score
					stringstream sstream;
					sstream << "You won! Your score was " << finalscore << ".";
					string scoreOutput = sstream.str();
					std::wstring wtemp(scoreOutput.begin(), scoreOutput.end());
					SetWindowText( TXT3, wtemp.c_str());

					SetWindowText( TXT4, L"Enter your name for the high-scores.");
				}
				else{
					// disables objects if user did not achieve a high-score
					stringstream sstream;
					sstream << "You won! Your score was " << finalscore << ".";
					string scoreOutput = sstream.str();
					std::wstring wtemp(scoreOutput.begin(), scoreOutput.end());
					SetWindowText( TXT3, wtemp.c_str());

					SetWindowText( TXT4, L"You did not achieve a high-score.");

					EnableWindow( TXT4, FALSE);
					EnableWindow( BTN1, FALSE);
				}
			}
			else{
				// informs user that they lost the game and disables high-score controls
				SetWindowText( TXT3, L"You lost the game!");
				SetWindowText( TXT4, L"You did not achieve a high-score.");

				EnableWindow( TXT4, FALSE);
				EnableWindow( BTN1, FALSE);
			}
			
			ifstream scorefile("scores.txt");	// opens high-score file
			
			// reads names and scores from file and displays them
			string name = "";
			getline(scorefile, name);
			std::wstring wtemp0(name.begin(), name.end());
			SetWindowText( TXT5, wtemp0.c_str());

			string score = "";
			getline(scorefile, score);
			std::wstring wtemp1(score.begin(), score.end());
			SetWindowText( TXT6, wtemp1.c_str());

			name = "";
			getline(scorefile, name);
			std::wstring wtemp2(name.begin(), name.end());
			SetWindowText( TXT7, wtemp2.c_str());

			score = "";
			getline(scorefile, score);
			std::wstring wtemp3(score.begin(), score.end());
			SetWindowText( TXT8, wtemp3.c_str());

			name = "";
			getline(scorefile, name);
			std::wstring wtemp4(name.begin(), name.end());
			SetWindowText( TXT9, wtemp4.c_str());

			score = "";
			getline(scorefile, score);
			std::wstring wtemp5(score.begin(), score.end());
			SetWindowText( TXT10, wtemp5.c_str());

			name = "";
			getline(scorefile, name);
			std::wstring wtemp6(name.begin(), name.end());
			SetWindowText( TXT11, wtemp6.c_str());

			score = "";
			getline(scorefile, score);
			std::wstring wtemp7(score.begin(), score.end());
			SetWindowText( TXT12, wtemp7.c_str());

			name = "";
			getline(scorefile, name);
			std::wstring wtemp8(name.begin(), name.end());
			SetWindowText( TXT13, wtemp8.c_str());

			score = "";
			getline(scorefile, score);
			std::wstring wtemp9(score.begin(), score.end());
			SetWindowText( TXT14, wtemp9.c_str());

			scorefile.close();	// closes file stream
		}
		else{
			// resets the ball if the game is not over
			velocity = 500;
			ballx = 290;
			bally = 244;

			calculateIntervals();
		}
	}
}


// This function is included by Visual C++
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_PONGGAME, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PONGGAME));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



// this function is included by Visual C++
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PONGGAME));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_PONGGAME);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

// this function is included by Visual C++
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   // creates window with specified dimensions
   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 600, 500, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

// function that handles messages sent to the program
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// included by Visual C++
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	// handles messges
	switch (message)
	{
	case WM_CREATE:	// code executing when creating the window
	{
		// creates fonts that are used in the window
		HFONT titleFont = CreateFont(72,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,DEFAULT_PITCH,TEXT("Arial"));

		HFONT scoreFont = CreateFont(32,0,0,0,FW_REGULAR,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,DEFAULT_PITCH,TEXT("Impact"));

		HFONT endScreenFont = CreateFont(14,0,0,0,FW_REGULAR,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,DEFAULT_PITCH,TEXT("Arial"));
		
		// creates text control for score
		TXT0 = CreateWindow(TEXT("STATIC"), TEXT("0"),
			WS_VISIBLE | WS_CHILD,
			100, 60, 30, 30,
			hWnd, (HMENU) ID_TXT0, NULL, NULL
		);
		SendMessage(TXT0, WM_SETFONT, WPARAM(scoreFont), TRUE);
		// creates text control for score
		TXT1 = CreateWindow(TEXT("STATIC"), TEXT("0"),
			WS_VISIBLE | WS_CHILD,
			460, 60, 30, 30,
			hWnd, (HMENU) ID_TXT1, NULL, NULL
		);
		SendMessage(TXT1, WM_SETFONT, WPARAM(scoreFont), TRUE);
		// creates control for paddle
		PDL0 = CreateWindow(TEXT("STATIC"), TEXT(""),
			WS_VISIBLE | WS_CHILD,
			20, ppaddley, 12, 60,
			hWnd, (HMENU) ID_PDL0, NULL, NULL
		);
		// creates control for paddle
		PDL1 = CreateWindow(TEXT("STATIC"), TEXT(""),
			WS_VISIBLE | WS_CHILD,
			558, cpaddley, 12, 60,
			hWnd, (HMENU) ID_PDL1, NULL, NULL
		);
		// creates control for ball
		BLL0 = CreateWindow(TEXT("STATIC"), TEXT(""),
			WS_VISIBLE | WS_CHILD,
			ballx, bally, 12, 12,
			hWnd, (HMENU) ID_BLL0, NULL, NULL
		);

		// creates text control for welcome screen
		TXT2 = CreateWindow(TEXT("STATIC"), TEXT("PONG"),
			WS_VISIBLE | WS_CHILD,
			270, 125, 65, 32,
			hWnd, (HMENU) ID_TXT2, NULL, NULL
		);
		SendMessage(TXT2, WM_SETFONT, WPARAM(scoreFont), TRUE);
		// creates button control for welcome screen
		BTN0 = CreateWindow(TEXT("BUTTON"), TEXT("Start"),
			WS_VISIBLE | WS_CHILD | WS_BORDER | BS_FLAT,
			200, 340, 195, 40,
			hWnd, (HMENU) ID_BTN0, NULL, NULL
		);

		// creates text control for displaying score
		TXT3 = CreateWindow(TEXT("STATIC"), TEXT("You won! Your score was 123."),
			WS_CHILD | WS_BORDER,
			195, 68, 200, 32,
			hWnd, (HMENU) ID_TXT3, NULL, NULL
		);
		SendMessage(TXT3, WM_SETFONT, WPARAM(endScreenFont), TRUE);
		// creates text control for user to enter name
		TXT4 = CreateWindow(TEXT("EDIT"), TEXT("Enter your name for high-scores."),
			WS_CHILD | WS_BORDER,
			150, 125, 200, 30,
			hWnd, (HMENU) ID_TXT4, NULL, NULL
		);
		SendMessage(TXT4, WM_SETFONT, WPARAM(endScreenFont), TRUE);
		// creates button control for end screen
		BTN1 = CreateWindow(TEXT("BUTTON"), TEXT("Enter"),
			WS_CHILD | WS_BORDER | BS_FLAT,
			360, 125, 65, 30,
			hWnd, (HMENU) ID_BTN1, NULL, NULL
		);

		// creates text controls for displaying high-scores

		TXT5 = CreateWindow(TEXT("STATIC"), TEXT(""),
			WS_CHILD,
			200, 180, 140, 25,
			hWnd, (HMENU) ID_TXT5, NULL, NULL
		);
		SendMessage(TXT5, WM_SETFONT, WPARAM(endScreenFont), TRUE);
		
		TXT6 = CreateWindow(TEXT("STATIC"), TEXT(""),
			WS_CHILD,
			350, 180, 40, 25,
			hWnd, (HMENU) ID_TXT6, NULL, NULL
		);
		SendMessage(TXT6, WM_SETFONT, WPARAM(endScreenFont), TRUE);

		TXT7 = CreateWindow(TEXT("STATIC"), TEXT(""),
			WS_CHILD,
			200, 220, 140, 25,
			hWnd, (HMENU) ID_TXT7, NULL, NULL
		);
		SendMessage(TXT7, WM_SETFONT, WPARAM(endScreenFont), TRUE);

		TXT8 = CreateWindow(TEXT("STATIC"), TEXT(""),
			WS_CHILD,
			350, 220, 40, 25,
			hWnd, (HMENU) ID_TXT8, NULL, NULL
		);
		SendMessage(TXT8, WM_SETFONT, WPARAM(endScreenFont), TRUE);

		TXT9 = CreateWindow(TEXT("STATIC"), TEXT(""),
			WS_CHILD,
			200, 260, 140, 25,
			hWnd, (HMENU) ID_TXT9, NULL, NULL
		);
		SendMessage(TXT9, WM_SETFONT, WPARAM(endScreenFont), TRUE);

		TXT10 = CreateWindow(TEXT("STATIC"), TEXT(""),
			WS_CHILD,
			350, 260, 40, 25,
			hWnd, (HMENU) ID_TXT10, NULL, NULL
		);
		SendMessage(TXT10, WM_SETFONT, WPARAM(endScreenFont), TRUE);

		TXT11 = CreateWindow(TEXT("STATIC"), TEXT(""),
			WS_CHILD,
			200, 300, 140, 25,
			hWnd, (HMENU) ID_TXT11, NULL, NULL
		);
		SendMessage(TXT11, WM_SETFONT, WPARAM(endScreenFont), TRUE);

		TXT12 = CreateWindow(TEXT("STATIC"), TEXT(""),
			WS_CHILD,
			350, 300, 40, 25,
			hWnd, (HMENU) ID_TXT12, NULL, NULL
		);
		SendMessage(TXT12, WM_SETFONT, WPARAM(endScreenFont), TRUE);

		TXT13 = CreateWindow(TEXT("STATIC"), TEXT(""),
			WS_CHILD,
			200, 340, 140, 25,
			hWnd, (HMENU) ID_TXT13, NULL, NULL
		);
		SendMessage(TXT13, WM_SETFONT, WPARAM(endScreenFont), TRUE);

		TXT14 = CreateWindow(TEXT("STATIC"), TEXT(""),
			WS_CHILD,
			350, 340, 40, 25,
			hWnd, (HMENU) ID_TXT14, NULL, NULL
		);
		SendMessage(TXT14, WM_SETFONT, WPARAM(endScreenFont), TRUE);

		break;
	}
	case WM_CTLCOLORSTATIC:
	{
		// retrieves ID for control that is being created
		DWORD controlID = GetDlgCtrlID( (HWND) lParam );
		HDC hdcStatic = (HDC) wParam;
		
		// changes text and background colours for controls
		if(controlID == ID_TXT0 || controlID == ID_TXT1){
			SetTextColor( hdcStatic, RGB(255,255,255) );
			SetBkColor( hdcStatic, RGB(0,0,0) );
			return (INT_PTR) CreateSolidBrush(RGB(0,0,0));
		}
		else if(controlID == ID_PDL0 || controlID == ID_PDL1 || controlID == ID_BLL0){
			SetTextColor( hdcStatic, RGB(0,0,0) );
			SetBkColor( hdcStatic, RGB(255,255,255) );
			return (INT_PTR) CreateSolidBrush(RGB(255,255,255));
		}
		else if(controlID == ID_TXT2 || controlID == ID_TXT3){
			SetTextColor( hdcStatic, RGB(50,222,46) );
			SetBkColor( hdcStatic, RGB(54,54,54) );
			return (INT_PTR) CreateSolidBrush(RGB(54,54,54));
		}
	}
	case WM_KEYDOWN:	//handles keyboard events
	{
		if(wParam == VK_UP && ppaddley != 5 && gamestarted == true && gameended == false){
			// moves player's paddle in appropriate direction
			ppaddleinmotion = true;
			ppaddley -= 5;
			SetWindowPos(PDL1, HWND_TOP, 558, ppaddley, 0, 0, SWP_NOSIZE);
		}
		else if(wParam == VK_DOWN && ppaddley != 385 && gamestarted == true && gameended == false){
			// moves player's paddle in appropriate direction
			ppaddleinmotion = true;
			ppaddley += 5;
			SetWindowPos(PDL1, HWND_TOP, 558, ppaddley, 0, 0, SWP_NOSIZE);
		}
	}
	case WM_COMMAND:
		wmId    = LOWORD(wParam);	// included by Visual C++
		wmEvent = HIWORD(wParam);	// included by Visual C++
		
		switch (wmId)
		{
		case ID_BTN0:
		{
			if(gamestarted == false)
			{
				// begins game when user presses button
				ShowWindow(BTN0, SW_HIDE);
				ShowWindow(TXT2, SW_HIDE);
				gamestarted = true;
				beginGame();
			}
			break;
		}
		case ID_BTN1:
		{
			if(gameended == true)
			{
				EnableWindow( BTN1, FALSE);	// disables button

				// creates and initializes variables for high-scores
				string names[5];
				int scores[5];
				for(int i = 0; i <= 4; i++){names[i] = ""; scores[i] = 0;}

				// reads names and scores from file and places them in arrays
				ifstream readscores("scores.txt");
				for(int y = 0; y <= 3; y++){
					getline(readscores, names[y]);
					string temp;
					getline(readscores, temp);
					stringstream(temp) >> scores[y];
				}
				readscores.close();

				// retreives user's name from text control and places it in array
				stringstream sstream;
				char input[100];
				for(int x = 0; x <= 99; x++){input[x] = '\0';}
				GetWindowText(TXT4, (LPWSTR) input, 100);
				for(int y = 0; y <= 99; y++){if(input[y] != '\0'){sstream << input[y];}}
				names[4] = sstream.str();
				// places score in array
				scores[4] = finalscore;

				// arranges names in decreasing order of score
				bool swapperformed;
				do{
					swapperformed = false;
					// swaps names and scores when lower element is higher
					for(int a = 3, b = 4; a >= 0; a--, b--){
						if(scores[a] < scores[b])
						{
							string tname = names[a];
							int tscore = scores[a];
							names[a] = names[b];
							scores[a] = scores[b];
							names[b] = tname;
							scores[b] = tscore;

							swapperformed = true;
						}
					}
				}while(swapperformed == true);

				// writes scores and names to file
				ofstream output;
				output.open("scores.txt", ios::trunc);
				for(int z = 0; z <= 4; z++){
					output << names[z];
					output << '\n';
					output << scores[z];
					output << '\n';
				}
				output.close();

				// reads scores and names from file and displays them
				ifstream scorefile("scores.txt");

				string name = "";
				getline(scorefile, name);
				std::wstring wtemp0(name.begin(), name.end());
				SetWindowText( TXT5, wtemp0.c_str());

				string score = "";
				getline(scorefile, score);
				std::wstring wtemp1(score.begin(), score.end());
				SetWindowText( TXT6, wtemp1.c_str());

				name = "";
				getline(scorefile, name);
				std::wstring wtemp2(name.begin(), name.end());
				SetWindowText( TXT7, wtemp2.c_str());

				score = "";
				getline(scorefile, score);
				std::wstring wtemp3(score.begin(), score.end());
				SetWindowText( TXT8, wtemp3.c_str());

				name = "";
				getline(scorefile, name);
				std::wstring wtemp4(name.begin(), name.end());
				SetWindowText( TXT9, wtemp4.c_str());

				score = "";
				getline(scorefile, score);
				std::wstring wtemp5(score.begin(), score.end());
				SetWindowText( TXT10, wtemp5.c_str());

				name = "";
				getline(scorefile, name);
				std::wstring wtemp6(name.begin(), name.end());
				SetWindowText( TXT11, wtemp6.c_str());

				score = "";
				getline(scorefile, score);
				std::wstring wtemp7(score.begin(), score.end());
				SetWindowText( TXT12, wtemp7.c_str());

				name = "";
				getline(scorefile, name);
				std::wstring wtemp8(name.begin(), name.end());
				SetWindowText( TXT13, wtemp8.c_str());

				score = "";
				getline(scorefile, score);
				std::wstring wtemp9(score.begin(), score.end());
				SetWindowText( TXT14, wtemp9.c_str());

				scorefile.close();
			}
			break;
		}
		case IDM_ABOUT:	// included by Visual C++
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:	// included by Visual C++
			DestroyWindow(hWnd);
			break;
		default:		// included by Visual C++
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:		// included by Visual C++
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:	// included by Visual C++
		PostQuitMessage(0);
		break;
	default:			// included by Visual C++
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// function included by Visual C++
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
