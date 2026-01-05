#include "cwup.c"
#include "cwup3d.c"
#include "openbmp.c"
#include "openModel.h"


int main(int argc, char *argv[]){

	

	openImage *texture = loadBMP("blackbuck.bmp");

	if (texture != NULL && texture-> pixels != NULL){

		printf("Texture loaded succsessfully ^^ \n");
		int pixelCount = texture->pixelCount;
		openPixel *imagePixels = texture->pixels;
	
	} else {
		printf("Failed to load texture! :c \n");
		return 0;
	}

	openModel model = loadModel("test.openModel");
	camera Cam;
	Cam.sensorDistance = 0.1;

        initialise();
	//xfile_auto_flush(false);
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
			for (int i = 0; i < model.triCount; i++){
				Tri3 current_tri = model.triangles[i];
				vec3 a3d = {current_tri.a.x,current_tri.a.y,current_tri.a.z};
				vec3 b3d = {current_tri.b.x,current_tri.b.y,current_tri.b.z};
				vec3 c3d = {current_tri.c.x,current_tri.c.y,current_tri.c.z};
				vec2 a = project_3d(a3d,Cam);
				vec2 b = project_3d(b3d,Cam);
				vec2 c = project_3d(c3d,Cam);
				//vec2 d = {0.5, 0.1};
				vec2 a1 = {current_tri.uv.a.x,current_tri.uv.a.y};
				vec2 b1 = {current_tri.uv.b.x,current_tri.uv.b.y};
				vec2 c1 = {current_tri.uv.c.x,current_tri.uv.c.y};
				bool in_tri = triangle2d(a,b,c,p);
				if (in_tri){
					vec2 texCoord = distort_point(p,a,b,c,a1,b1,c1);
					//printf("LoadingPixel");
					openPixel *pix = getPixel(texture, texCoord.x*(texture->width), texCoord.y*(texture->height));
					//printf("PixelLoaded");
					//xfile_put_pixel(display_file, x, y, _RGB(pix->r,pix->g,pix->b));
                        		xfile_put_pixel(display_file, x, y, _RGB((int)(texCoord.x*255),(int)(texCoord.y*255),0));
        			}
			}
	}
        }
        while (1){
                displayFrame(display_file);
        }


        return 0;
}
