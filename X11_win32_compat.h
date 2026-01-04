#ifndef X11_WIN32_COMPAT_H
#define X11_WIN32_COMPAT_H

#include <windows.h>
#include <stdlib.h>

/* --- Missing Constants & Enums --- */
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
typedef HDC  Drawable; 
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

/* --- Internal Helper --- */
// Converts a Window (HWND) to a Drawable (HDC) if needed
inline HDC GetDrawableDC(Drawable d) {
    // If it's a window handle, GetDC will work. If it's already a MemDC, it returns itself.
    HDC hdc = GetDC((HWND)d);
    return hdc ? hdc : (HDC)d; 
}

/* --- Display & Screen --- */
inline Display* XOpenDisplay(const char* d) {
    Display* dpy = (Display*)malloc(sizeof(Display));
    if (dpy) dpy->default_screen = 0;
    return dpy;
}
inline void XFlush(Display* dpy) { GdiFlush(); }
inline int DefaultScreen(Display* d) { return 0; }
inline Window DefaultRootWindow(Display* d) { return GetDesktopWindow(); }
inline int DefaultDepth(Display* d, int s) { return 32; }
inline unsigned long BlackPixel(Display* d, int s) { return RGB(0,0,0); }
inline unsigned long WhitePixel(Display* d, int s) { return RGB(255,255,255); }

/* --- Window & GC --- */
inline Window XCreateSimpleWindow(Display* d, Window p, int x, int y, unsigned int w, unsigned int h, unsigned int bw, unsigned long brd, unsigned long bg) {
    HWND hwnd = CreateWindowA("STATIC", "X11", WS_OVERLAPPEDWINDOW, x, y, w, h, p, NULL, NULL, NULL);
    SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)CreateSolidBrush(bg));
    return hwnd;
}

inline GC XCreateGC(Display* dpy, Drawable d, unsigned long mask, XGCValues* v) {
    return CreateSolidBrush(v ? v->foreground : RGB(0,0,0));
}

inline Pixmap XCreatePixmap(Display* dpy, Drawable d, unsigned int w, unsigned int h, unsigned int depth) {
    HDC hdc = GetDC(NULL);
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP hbm = CreateCompatibleBitmap(hdc, w, h);
    SelectObject(memDC, hbm);
    ReleaseDC(NULL, hdc);
    return memDC;
}

/* --- Drawing & Events --- */
inline void XFillRectangle(Display* dpy, Drawable d, GC gc, int x, int y, unsigned int w, unsigned int h) {
    HDC hdc = GetDrawableDC(d);
    RECT r = {x, y, (int)(x+w), (int)(y+h)};
    FillRect(hdc, &r, gc);
    if ((HWND)d != GetDesktopWindow()) ReleaseDC((HWND)d, hdc);
}

inline void XCopyArea(Display* dpy, Drawable src, Drawable dest, GC gc, int sx, int sy, unsigned int w, unsigned int h, int dx, int dy) {
    HDC hDest = GetDrawableDC(dest);
    BitBlt(hDest, dx, dy, w, h, (HDC)src, sx, sy, SRCCOPY);
    if ((HWND)dest != GetDesktopWindow()) ReleaseDC((HWND)dest, hDest);
}

inline void XSelectInput(Display* d, Window w, long m) {}
inline void XMapWindow(Display* d, Window w) { ShowWindow(w, SW_SHOW); }

inline void XNextEvent(Display* dpy, XEvent* ev) {
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