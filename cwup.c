#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[]){

	int screen_num, width, height;
	unsigned long background, border;
	Window win;
	XEvent ev;
	Display *dpy;

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


	XSelectInput(dpy, win, ButtonPressMask|StructureNotifyMask); // Tell the display server what kind of events we want to see

	XMapWindow(dpy,win); // Diplay the window on the screen please

	while (1) {
		XNextEvent(dpy, &ev);
		switch(ev.type){
		case ConfigureNotify:
			if (width != ev.xconfigure.width || height != ev.xconfigure.height) {
				width = ev.xconfigure.width;
				height = ev.xconfigure.height;
				printf("Size changed to: %d by %d", width, height);
			}
			break;
		case ButtonPress:
			XCloseDisplay(dpy);
			return 0;
		}
	}
	//return 0;
}


