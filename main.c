#include "cwup.c"
#include "cwup3d.c"

int main(int argc, char *argv[]){

        initialise();
	xfile_auto_flush(false);
        XFile *display_file = xfile_open(dpy, win, pen, width, height);
        for (int i = 0; i < 40; i++){
                xfile_put_pixel(display_file, i, 20, WhitePixel(dpy, screen_num));

        }
        for (int x = 0; x < 512; x++){
		//printf("%d\n",x);
                for (int y = 0; y < 512;y++){
                        float u = (float)x / 512;
                        float v = (float)y / 512;
			vec2 p = {u,v};
			vec2 a = {0.1,0.1};
			vec2 b = {0.1,0.5};
			vec2 c = {0.5,0.5};
			vec2 d = {0.5, 0.1};
			bool in_quad = quad2d(a,b,c,d,p);
			if (in_quad){
                        	xfile_put_pixel(display_file, x, y, _RGB((int)(u*255),(int)(v*255),0));
        		}
	}
        }
        while (1){
                displayFrame(display_file);
        }


        return 0;
}
