#ifndef X11_WIN32_COMPAT_H
#define X11_WIN32_COMPAT_H

#include <windows.h>
#include <stdlib.h>

/* --- X11 Constants --- */
#define ExposureMask          (1L<<15)
#define ButtonPressMask       (1L<<2)
#define StructureNotifyMask   (1L<<17)
#define GCForeground          (1L<<2)
#define GCLineWidth           (1L<<4)
#define GCLineStyle           (1L<<5)
#define LineSolid             0
#define Expose                12
#define ConfigureNotify       22

/* --- X11 Types --- */
typedef HWND Window;
typedef HDC  Pixmap;
typedef void* Drawable; 
typedef HBRUSH GC;

typedef struct {
    unsigned long foreground;
    int line_width;
    int line_style;
} XGCValues;

typedef struct {
    int type;
    Window window;
    struct {
        int width, height;
    } xconfigure;
} XEvent;

typedef struct {
    int default_screen;
} Display;

/* --- Internal Helpers --- */
static inline HDC InternalGetDC(Drawable d) {
    if (IsWindow((HWND)d)) return GetDC((HWND)d);
    return (HDC)d; 
}

static inline void InternalReleaseDC(Drawable d, HDC hdc) {
    if (IsWindow((HWND)d)) ReleaseDC((HWND)d, hdc);
}

/* --- Display & Connection --- */
static inline Display* XOpenDisplay(const char* name) {
    Display* dpy = (Display*)malloc(sizeof(Display));
    if (dpy) dpy->default_screen = 0;
    return dpy;
}
static inline void XFlush(Display* dpy) { GdiFlush(); }
static inline int DefaultScreen(Display* dpy) { return 0; }
static inline Window DefaultRootWindow(Display* dpy) { return GetDesktopWindow(); }
static inline int DefaultDepth(Display* dpy, int scr) { return 32; }
static inline unsigned long BlackPixel(Display* d, int s) { return RGB(0,0,0); }
static inline unsigned long WhitePixel(Display* d, int s) { return RGB(255,255,255); }

/* --- Window & GC Control --- */
static inline Window XCreateSimpleWindow(Display* d, Window p, int x, int y, unsigned int w, unsigned int h, unsigned int bw, unsigned long brd, unsigned long bg) {
    WNDCLASSA wc = {0};
    wc.lpfnWndProc = DefWindowProcA;
    wc.lpszClassName = "X11Shim";
    wc.hbrBackground = CreateSolidBrush(bg);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassA(&wc);
    return CreateWindowA("X11Shim", "X11 Window", WS_OVERLAPPEDWINDOW, x, y, w, h, p, NULL, NULL, NULL);
}

static inline GC XCreateGC(Display* d, Drawable dr, unsigned long mask, XGCValues* v) {
    return CreateSolidBrush(v ? v->foreground : RGB(0,0,0));
}

static inline void XSetForeground(Display* d, GC gc, unsigned long color) {
    // Basic shim: colors in GDI brushes are static, but we provide the symbol
}

static inline Pixmap XCreatePixmap(Display* d, Drawable dr, unsigned int w, unsigned int h, unsigned int depth) {
    HDC ref = GetDC(NULL);
    HDC memDC = CreateCompatibleDC(ref);
    HBITMAP hbm = CreateCompatibleBitmap(ref, w, h);
    SelectObject(memDC, hbm);
    ReleaseDC(NULL, ref);
    return memDC;
}

/* --- Drawing Commands --- */
static inline void XFillRectangle(Display* dpy, Drawable d, GC gc, int x, int y, unsigned int w, unsigned int h) {
    HDC hdc = InternalGetDC(d);
    RECT r = { x, y, (int)(x + w), (int)(y + h) };
    FillRect(hdc, &r, gc);
    InternalReleaseDC(d, hdc);
}

static inline void XDrawPoint(Display* dpy, Drawable d, GC gc, int x, int y) {
    HDC hdc = InternalGetDC(d);
    LOGBRUSH lb;
    GetObject(gc, sizeof(lb), &lb);
    SetPixel(hdc, x, y, lb.lbColor);
    InternalReleaseDC(d, hdc);
}

static inline void XDrawLine(Display* d, Drawable dr, GC gc, int x1, int y1, int x2, int y2) {
    HDC hdc = InternalGetDC(dr);
    LOGBRUSH lb;
    GetObject(gc, sizeof(lb), &lb);
    HPEN pen = CreatePen(PS_SOLID, 1, lb.lbColor);
    HPEN old = (HPEN)SelectObject(hdc, pen);
    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x2, y2);
    SelectObject(hdc, old);
    DeleteObject(pen);
    InternalReleaseDC(dr, hdc);
}

static inline void XCopyArea(Display* dpy, Drawable src, Drawable dest, GC gc, int sx, int sy, unsigned int w, unsigned int h, int dx, int dy) {
    HDC hSrc = InternalGetDC(src);
    HDC hDest = InternalGetDC(dest);
    BitBlt(hDest, dx, dy, w, h, hSrc, sx, sy, SRCCOPY);
    InternalReleaseDC(src, hSrc);
    InternalReleaseDC(dest, hDest);
}

/* --- Event Handling --- */
static inline void XSelectInput(Display* d, Window w, long m) {}
static inline void XMapWindow(Display* d, Window w) { ShowWindow(w, SW_SHOW); }

static inline void XNextEvent(Display* dpy, XEvent* ev) {
    MSG msg;
    if (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        ev->window = msg.hwnd;
        if (msg.message == WM_PAINT) ev->type = Expose;
        if (msg.message == WM_SIZE) {
            ev->type = ConfigureNotify;
            ev->xconfigure.width = LOWORD(msg.lParam);
            ev->xconfigure.height = HIWORD(msg.lParam);
        }
    }
}

#endif
