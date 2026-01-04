#ifndef X11_WIN32_COMPAT_H
#define X11_WIN32_COMPAT_H

#include <windows.h>
#include <stdint.h>

/* --- X11 Types & Macros --- */
typedef HWND   Window;
typedef HDC    Pixmap;
typedef HDC    Drawable; // In this layer, we treat Drawables as DCs
typedef HBRUSH GC;

typedef struct {
    int default_screen;
} Display;

typedef struct {
    int type;
    Window window;
    int x, y;
    unsigned int width, height;
} XEvent;

#define ExposureMask (1L<<15)
#define Expose       12

/* --- Connection Management --- */

inline Display* XOpenDisplay(const char* name) {
    Display* dpy = (Display*)malloc(sizeof(Display));
    if (dpy) dpy->default_screen = 0;
    return dpy;
}

inline void XFlush(Display* dpy) {
    GdiFlush();
}

// inline void XCloseDisplay(Display* dpy) { ... }

/* --- Screen Info --- */

inline int DefaultScreen(Display* dpy) { return 0; }
inline Window DefaultRootWindow(Display* dpy) { return GetDesktopWindow(); }
inline int DefaultDepth(Display* dpy, int scr) {
    HDC hdc = GetDC(NULL);
    int d = GetDeviceCaps(hdc, BITSPIXEL);
    ReleaseDC(NULL, hdc);
    return d;
}

inline unsigned long BlackPixel(Display* dpy, int scr) { return RGB(0, 0, 0); }
inline unsigned long WhitePixel(Display* dpy, int scr) { return RGB(255, 255, 255); }

/* --- Window Control --- */

inline Window XCreateSimpleWindow(Display* dpy, Window parent, int x, int y, 
                                 unsigned int w, unsigned int h, 
                                 unsigned int bw, unsigned long brd, unsigned long bg) {
    // Note: Uses a pre-registered system class for simplicity
    HWND hwnd = CreateWindowA("STATIC", "", WS_OVERLAPPEDWINDOW, 
                              x, y, w, h, parent, NULL, NULL, NULL);
    SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)CreateSolidBrush(bg));
    return hwnd;
}

inline void XSelectInput(Display* dpy, Window w, long mask) { /* Windows sends all by default */ }
inline void XMapWindow(Display* dpy, Window w) { ShowWindow(w, SW_SHOW); }

/* --- Graphics Context & Pixmaps --- */

inline GC XCreateGC(Display* dpy, Drawable d, unsigned long mask, void* val) {
    return CreateSolidBrush(RGB(0, 0, 0)); 
}

inline void XSetForeground(Display* dpy, GC gc, unsigned long color) {
    // In GDI, we effectively replace the brush. 
    // This is a simplified shim; real X11 GCs are more complex!
}

inline Pixmap XCreatePixmap(Display* dpy, Drawable d, unsigned int w, unsigned int h, unsigned int depth) {
    HDC screenDC = GetDC(NULL);
    HDC memDC = CreateCompatibleDC(screenDC);
    HBITMAP hbm = CreateCompatibleBitmap(screenDC, w, h);
    SelectObject(memDC, hbm);
    ReleaseDC(NULL, screenDC);
    return memDC; 
}

/* --- Drawing --- */

inline void XFillRectangle(Display* dpy, Drawable d, GC gc, int x, int y, unsigned int w, unsigned int h) {
    RECT r = { x, y, (int)(x + w), (int)(y + h) };
    FillRect(d, &r, gc);
}

inline void XDrawPoint(Display* dpy, Drawable d, GC gc, int x, int y) {
    LOGBRUSH lb;
    GetObject(gc, sizeof(lb), &lb);
    SetPixel(d, x, y, lb.lbColor);
}

inline void XCopyArea(Display* dpy, Drawable src, Drawable dest, GC gc, 
                      int sx, int sy, unsigned int w, unsigned int h, int dx, int dy) {
    BitBlt(dest, dx, dy, w, h, src, sx, sy, SRCCOPY);
}

// inline void XDrawLine(...) { ... }

/* --- Events --- */

inline void XNextEvent(Display* dpy, XEvent* ev) {
    MSG msg;
    if (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        
        // Map common messages to X11 events
        ev->window = msg.hwnd;
        if (msg.message == WM_PAINT) ev->type = Expose;
    }
}

#endif
