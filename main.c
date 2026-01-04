#include "cwup.c"


int main(int argc, char *argv[]){

        initialise();
        XFile *display_file = xfile_open(dpy, win, pen, width, height);
        for (int i = 0; i < 40; i++){
                xfile_put_pixel(display_file, i, 20, WhitePixel(dpy, screen_num));

        }
        for (int x = 0; x < 512; x++){
                for (int y = 0; y < 512;y++){
                        float u = (float)x / 512;
                        float v = (float)y / 512;
                        xfile_put_pixel(display_file, x, y, _RGB((int)(u*255),(int)(v*255),0));
        }
        }
        while (1){
                displayFrame(display_file);
        }


        return 0;
}
