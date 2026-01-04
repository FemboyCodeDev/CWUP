
#ifdef _WIN32
	#include "X11_win32_compat.h"
#else
	#include <X11/Xlib.h>
	#include <X11/Xutil.h>
	#include <X11/Xresource.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct {
	Display *dpy;
	Window win;
	Pixmap buffer;
	GC pen;
	int width;
	int height;
} XFile;


XFile* xfile_open(Display *d, Window w, GC p, int w_width, int w_height) {
	XFile *xf = malloc(sizeof(XFile));
	xf->dpy = d;
	xf->win = w;
	xf->pen = p;

	int screen = DefaultScreen(d);
	xf->buffer = XCreatePixmap(d, w, w_width, w_height, DefaultDepth(d, screen));
	
	XSetForeground(d, p, BlackPixel(d, screen));
	XFillRectangle(d, xf->buffer, p, 0, 0, w_width, w_height);
	return xf;

}

void xfile_put_pixel(XFile *xf, int x, int y, unsigned long color){
	XSetForeground(xf->dpy, xf->pen, color);
	XDrawPoint(xf->dpy, xf->buffer,xf->pen,x,y);

	XCopyArea(xf->dpy, xf->buffer, xf->win, xf->pen, x, y, 1, 1, x, y);
	XFlush(xf->dpy);

}



int screen_num, width, height;
unsigned long background, border;
Window win;
XEvent ev;
Display *dpy;
GC pen;
XGCValues values;
int initialise(){
	//Connect to the server

	dpy = XOpenDisplay(NULL);

	if (!dpy) {
		fprintf(stderr, "unable to connect to display");
		return 7;
 	}

	// These pull useful data out of the display object
	// They are used enough to benfit from them being in there own variables

	screen_num = DefaultScreen(dpy);
	background = BlackPixel(dpy,screen_num);
	border = WhitePixel(dpy, screen_num);

	width = 40;
	height = 40;

	win = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0, width, height, 2, border, background);

	values.foreground = WhitePixel(dpy, screen_num);
	values.line_width = 1;
	values.line_style = LineSolid;
	pen = XCreateGC(dpy, win, GCForeground|GCLineWidth|GCLineStyle,&values);


	XSelectInput(dpy, win, ButtonPressMask|StructureNotifyMask|ExposureMask); // Tell the display server what kind of events we want to see

	XMapWindow(dpy,win); // Diplay the window on the screen please

	//XFile *display_file = xfile_open(dpy, win, pen, width, height);

	//for (int i = 0; i< 40; i++){
	//	xfile_put_pixel(display_file, i, 20, WhitePixel(dpy, screen_num));
	//}
	return 0;
}
int displayFrame(XFile *display_file){
	XNextEvent(dpy, &ev);
	switch(ev.type){
		case Expose:
			XCopyArea(dpy, display_file->buffer, win, pen,
					0, 0, width, height, // Source area
					0, 0); // Destination Coordinates
			//XDrawLine(dpy, win, pen, 0, 0, width, height);
			//XDrawLine(dpy, win, pen, width, 0, 0, height);
			break;
		case ConfigureNotify:
			if (width != ev.xconfigure.width || height != ev.xconfigure.height) {
				width = ev.xconfigure.width;
				height = ev.xconfigure.height;
				//printf("Size changed to: %d by %d", width, height);
			}
			break;
		//case ButtonPress:
			//XCloseDisplay(dpy);
			//return 0;
	}
	return 0;
	}
	//return 0;


int main(int argc, char *argv[]){

	initialise();
	XFile *display_file = xfile_open(dpy, win, pen, width, height);
	for (int i = 0; i < 40; i++){
		xfile_put_pixel(display_file, i, 20, WhitePixel(dpy, screen_num));

	}
	while (1){
		displayFrame(display_file);
	}


	return 0;
}


