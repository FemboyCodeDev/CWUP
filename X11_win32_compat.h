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
typedef void* Drawable; // Changed to void* to allow HWND or HDC without casting errors
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
// Logic to determine if the passed handle is a Window or a Device Context
inline HDC InternalGetDC(Drawable d) {
    // If it's a window handle, we get its DC. If it's already a DC, it remains valid.
    if (IsWindow((HWND)d)) return GetDC((HWND)d);
    return (HDC)d; 
}

inline void InternalReleaseDC(Drawable d, HDC hdc) {
    if (IsWindow((HWND)d)) ReleaseDC((HWND)d, hdc);
}

/* --- Display & Connection --- */
inline Display* XOpenDisplay(const char* name) {
    Display* dpy = (Display*)malloc(sizeof(Display));
    if (dpy) dpy->default_screen = 0;
    return dpy;
}
inline void XFlush(Display* dpy) { GdiFlush(); }
inline int DefaultScreen(Display* dpy) { return 0; }
inline Window DefaultRootWindow(Display* dpy) { return GetDesktopWindow(); }
inline int DefaultDepth(Display* dpy, int scr) { return 32; }
inline unsigned long BlackPixel(Display* d, int s) { return RGB(0,0,0); }
inline unsigned long WhitePixel(Display* d, int s) { return RGB(255,255,255); }

/* --- Window & GC Control --- */
inline Window XCreateSimpleWindow(Display* d, Window p, int x, int y, unsigned int w, unsigned int h, unsigned int bw, unsigned long brd, unsigned long bg) {
    // Registering a basic class for the window
    WNDCLASSA wc = {0};
    wc.lpfnWndProc = DefWindowProcA;
    wc.lpszClassName = "X11Shim";
    wc.hbrBackground = CreateSolidBrush(bg);
    RegisterClassA(&wc);

    return CreateWindowA("X11Shim", "X11 Window", WS_OVERLAPPEDWINDOW, x, y, w, h, p, NULL, NULL, NULL);
}

inline GC XCreateGC(Display* d, Drawable dr, unsigned long mask, XGCValues* v) {
    return CreateSolidBrush(v ? v->foreground : RGB(0,0,0));
}

inline void XSetForeground(Display* d, GC gc, unsigned long color) {
    // Note: In GDI, you usually recreate the brush, but for this shim, 
    // we assume the color doesn't change after creation or is handled by the caller.
}

inline Pixmap XCreatePixmap(Display* d, Drawable dr, unsigned int w, unsigned int h, unsigned int depth) {
    HDC ref = GetDC(NULL);
    HDC memDC = CreateCompatibleDC(ref);
    HBITMAP hbm = CreateCompatibleBitmap(ref, w, h);
    SelectObject(memDC, hbm);
    ReleaseDC(NULL, ref);
    return memDC;
}

/* --- Drawing Commands --- */
inline void XFillRectangle(Display* dpy, Drawable d, GC gc, int x, int y, unsigned int w, unsigned int h) {
    HDC hdc = InternalGetDC(d);
    RECT r = { x, y, (int)(x + w), (int)(y + h) };
    FillRect(hdc, &r, gc);
    InternalReleaseDC(d, hdc);
}

inline void XDrawPoint(Display* dpy, Drawable d, GC gc, int x, int y) {
    HDC hdc = InternalGetDC(d);
    LOGBRUSH lb;
    GetObject(gc, sizeof(lb), &lb);
    SetPixel(hdc, x, y, lb.lbColor);
    InternalReleaseDC(d, hdc);
}

inline void XCopyArea(Display* dpy, Drawable src, Drawable dest, GC gc, int sx, int sy, unsigned int w, unsigned int h, int dx, int dy) {
    HDC hSrc = InternalGetDC(src);
    HDC hDest = InternalGetDC(dest);
    BitBlt(hDest, dx, dy, w, h, hSrc, sx, sy, SRCCOPY);
    InternalReleaseDC(src, hSrc);
    InternalReleaseDC(dest, hDest);
}

/* --- Event Handling --- */
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
