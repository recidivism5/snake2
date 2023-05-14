#define WIN32_LEAN_AND_MEAN
#undef UNICODE
int _fltused;
#include <windows.h>
#include <stdio.h>
#include <math.h>
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define COUNT(arr) (sizeof(arr)/sizeof(*arr))
#define FOR(var,count) for(i32 var = 0; var < (count); var++)
void *memset(void *dest, int c, size_t count){
	u8 *b = dest;
	while (count--) *b++ = c;
	return dest;
}
void *memcpy(void *dest, const void *src, size_t count){
	u8 *d = dest, *s = src;
	while (count--) *d++ = *s++;
	return dest;
}
u32 seed;
u32 rand(){
	u32 r = 0;
	FOR(i,31) r |= ((seed += (seed*seed) | 5) & (1<<31)) >> i;
	return r;
}
LARGE_INTEGER freq,before,now;
void limitFPS(i32 fps){
	//Sleep(1) can take up to 1/64 of a second to return
	double sec = 1.0/(double)fps;
	while (sec > 1.0/58.0){
		QueryPerformanceCounter(&before);
		Sleep(1);
		QueryPerformanceCounter(&now);
		sec -= (double)(now.QuadPart-before.QuadPart) / (double)freq.QuadPart;
	}
	QueryPerformanceCounter(&before);
	i64 remaining = sec * freq.QuadPart;
	while (QueryPerformanceCounter(&now),now.QuadPart-before.QuadPart < remaining);
}
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#define FB_WIDTH 32
#define FB_HEIGHT 32
u32 fb[FB_WIDTH*FB_HEIGHT];
#define FBAT(x,y) ((x) + (y)*FB_WIDTH)
#define SCALE 16
i32 hx,hy,tx,ty,pd,ax,ay,alive = 1;
HWND wnd;
u8 font[]={95,0,0,0,0,0,0,0,4,3,4,3,0,0,0,0,20,127,20,127,20,0,0,0,36,42,127,42,18,0,0,0,67,48,8,6,97,0,0,0,48,74,93,50,72,0,0,0,4,3,0,0,0,0,0,0,28,34,65,0,0,0,0,0,65,34,28,0,0,0,0,0,20,8,20,0,0,0,0,0,8,8,62,8,8,0,0,0,128,96,0,0,0,0,0,0,8,8,8,8,8,0,0,0,96,0,0,0,0,0,0,0,64,48,8,6,1,0,0,0,62,81,73,69,62,0,0,0,64,66,127,64,64,0,0,0,98,81,73,73,102,0,0,0,34,65,73,73,54,0,0,0,24,20,18,17,127,0,0,0,47,73,69,69,57,0,0,0,60,82,73,73,48,0,0,0,3,1,113,9,7,0,0,0,54,73,73,73,54,0,0,0,6,73,73,41,30,0,0,0,102,0,0,0,0,0,0,0,64,38,0,0,0,0,0,0,8,20,34,65,0,0,0,0,36,36,36,36,36,0,0,0,65,34,20,8,0,0,0,0,2,1,81,9,6,0,0,0,62,65,93,81,94,0,0,0,120,22,17,22,120,0,0,0,127,69,69,69,58,0,0,0,62,65,65,65,34,0,0,0,127,65,65,65,62,0,0,0,127,73,73,73,65,0,0,0,127,5,5,5,1,0,0,0,62,65,65,73,58,0,0,0,127,8,8,8,127,0,0,0,65,127,65,0,0,0,0,0,32,64,65,65,63,0,0,0,127,8,20,34,65,0,0,0,127,64,64,64,64,0,0,0,127,2,4,2,127,0,0,0,127,3,28,96,127,0,0,0,62,65,65,65,62,0,0,0,127,9,9,9,6,0,0,0,62,65,81,33,94,0,0,0,127,9,9,9,118,0,0,0,38,73,73,73,50,0,0,0,1,1,127,1,1,0,0,0,63,64,64,64,63,0,0,0,7,56,64,56,7,0,0,0,127,32,16,32,127,0,0,0,99,20,8,20,99,0,0,0,3,4,120,4,3,0,0,0,97,81,73,69,67,0,0,0,127,65,65,0,0,0,0,0,1,6,8,48,64,0,0,0,65,65,127,0,0,0,0,0,4,2,1,2,4,0,0,0,128,128,128,128,128,0,0,0,1,2,4,0,0,0,0,0,32,84,84,84,120,0,0,0,127,80,72,72,48,0,0,0,56,68,68,68,0,0,0,0,48,72,72,80,127,0,0,0,56,84,84,84,88,0,0,0,16,124,18,4,0,0,0,0,152,164,164,248,0,0,0,0,127,16,8,8,112,0,0,0,121,0,0,0,0,0,0,0,64,128,128,125,0,0,0,0,127,16,40,68,0,0,0,0,63,64,0,0,0,0,0,0,124,4,24,4,120,0,0,0,124,4,4,120,0,0,0,0,56,68,68,68,56,0,0,0,248,36,36,24,0,0,0,0,24,36,36,248,128,0,0,0,124,8,4,4,8,0,0,0,8,84,84,84,32,0,0,0,8,62,72,0,0,0,0,0,60,64,64,60,64,0,0,0,12,48,64,48,12,0,0,0,60,64,48,64,60,0,0,0,68,40,16,40,68,0,0,0,76,144,144,124,0,0,0,0,68,100,84,76,68,0,0,0,8,54,65,0,0,0,0,0,127,0,0,0,0,0,0,0,65,54,8,0,0,0,0,0,16,8,8,16,16,8,0,0,};
void drawString(i32 x, i32 y, u8 *str, u32 color){
	while (*str){
		if (*str == ' ') x+=5;
		else {
			for (i32 i = 0; i < 8; i++){
				u8 col = font[8*(*str-'!') + i];
				if (col){
					for (i32 j = 0; j < 8; j++) if (col & (1<<j)) fb[FBAT(x,y-j)] = color;
					x++;
				} else {
					x++;
					break;
				}
			}
		}
		str++;
	}
}
void move(i32 *x, i32 *y, i32 d){
	switch (d){
		case VK_LEFT: *x = *x ? *x-1 : FB_WIDTH-1; break;
		case VK_RIGHT: *x = *x == FB_WIDTH-1 ? 0 : *x+1; break;
		case VK_DOWN: *y = *y ? *y-1 : FB_HEIGHT-1; break;
		case VK_UP: *y = *y == FB_HEIGHT-1 ? 0 : *y+1; break;
	}
}
struct BMI {
	BITMAPINFOHEADER header;
	RGBQUAD colors[3];
} bmi = {sizeof(struct BMI),FB_WIDTH,FB_HEIGHT,1,32,BI_BITFIELDS};
void draw(){
	HDC dc = GetDC(wnd);
	StretchDIBits(dc,0,0,FB_WIDTH*SCALE,FB_HEIGHT*SCALE,0,0,FB_WIDTH,FB_HEIGHT,fb,&bmi,DIB_RGB_COLORS,SRCCOPY);
	ReleaseDC(wnd,dc);
}
void placeApple(){
	ax = rand() % FB_WIDTH;
	ay = rand() % FB_HEIGHT;
}
void start(){
	hx = FB_WIDTH/2;
	hy = FB_HEIGHT/2;
	tx = hx;
	ty = hy;
	pd = VK_RIGHT;
	placeApple();
	alive = 1;
	FOR(i,COUNT(fb)) fb[i] = 0;
	fb[FBAT(ax,ay)] |= 0xff0000;
	fb[FBAT(hx,hy)] = 0xff00;
	draw();
}
i32 canTurn = 1;
LONG WINAPI WindowProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam){
	switch (msg){
	case WM_PAINT:{
		PAINTSTRUCT ps;
		HDC dc = BeginPaint(wnd,&ps);
		draw(dc);
		EndPaint(wnd,&ps);
		return 0;
	}
	case WM_CREATE:{
		RECT wr;
		GetWindowRect(wnd,&wr);
		SetWindowPos(wnd,0,wr.left,wr.top,wr.right-wr.left,wr.bottom-wr.top,SWP_FRAMECHANGED|SWP_NOMOVE|SWP_NOSIZE);//this prevents the single white frame when the window first appears
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		switch (wparam){
			case 'A': wparam = VK_LEFT; break;
			case 'D': wparam = VK_RIGHT; break;
			case 'S': wparam = VK_DOWN; break;
			case 'W': wparam = VK_UP; break;
		}
		switch (wparam){
			case VK_LEFT:case VK_RIGHT: if (canTurn && pd != VK_LEFT && pd != VK_RIGHT){pd = wparam; canTurn = 0;} break;
			case VK_DOWN:case VK_UP: if (canTurn && pd != VK_DOWN && pd != VK_UP){pd = wparam; canTurn = 0;} break;
			case 'R': start(); break;
		}
	}
	return DefWindowProcA(wnd, msg, wparam, lparam);
}
WNDCLASSA wc = {CS_HREDRAW|CS_VREDRAW,WindowProc,0,0,0,0,0,0,0,"Snake"};
int WINAPI WinMain(HINSTANCE instance, HINSTANCE previnstance, char *cmdline, int cmdshow){
	bmi.colors[0].rgbRed = 0xff;
	bmi.colors[1].rgbGreen = 0xff;
	bmi.colors[2].rgbBlue = 0xff;
	wc.hInstance = instance;
	wc.hIcon = LoadIconA(0,IDI_APPLICATION);
	wc.hCursor = LoadCursorA(0,IDC_ARROW);
	RegisterClassA(&wc);
	RECT wr = {0,0,FB_WIDTH*SCALE,FB_HEIGHT*SCALE};
	AdjustWindowRect(&wr,WS_OVERLAPPEDWINDOW,FALSE);
	int wndWidth = wr.right-wr.left;
	int wndHeight = wr.bottom-wr.top;
	wnd = CreateWindowExA(WS_EX_APPWINDOW,wc.lpszClassName,wc.lpszClassName,WS_OVERLAPPEDWINDOW^WS_THICKFRAME^WS_MAXIMIZEBOX,GetSystemMetrics(SM_CXSCREEN)/2-wndWidth/2,GetSystemMetrics(SM_CYSCREEN)/2-wndHeight/2,wndWidth,wndHeight,0,0,wc.hInstance,0);
	ShowWindow(wnd,SW_SHOWDEFAULT);
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&before);
	seed = before.LowPart;
	rand();
	start();
	MSG msg;
	do {
		if (PeekMessageA(&msg,0,0,0,PM_REMOVE)){
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		} else if (alive){
			canTurn = 1;
			fb[FBAT(hx,hy)] |= pd<<(24);
			move(&hx,&hy,pd);
			if (fb[FBAT(hx,hy)] & 0xff000000){
				alive = 0;
				drawString(0,FB_HEIGHT-1,"you're",0xffff);
				drawString(0,FB_HEIGHT-9,"dead,",0xffff);
				drawString(0,FB_HEIGHT-16,"press",0xffff);
				drawString(5,FB_HEIGHT-25,"R",0xffff);
			} else {
				if (hx == ax && hy == ay){
					placeApple();
				} else {
					i32 d = fb[FBAT(tx,ty)] >> 24;
					fb[FBAT(tx,ty)] = 0;
					move(&tx,&ty,d);
				}
				fb[FBAT(ax,ay)] |= 0xff0000;
				fb[FBAT(hx,hy)] = 0xff00;
			}
			draw();
			limitFPS(24);
		} else Sleep(1);
	} while (msg.message != WM_QUIT);
	return msg.wParam;
}
void __stdcall WinMainCRTStartup(){
	ExitProcess(WinMain(GetModuleHandle(0),0,0,0));
}