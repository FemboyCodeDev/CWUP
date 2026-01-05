#include <stdio.h>
#include <stdlib.h>
#include <math.h>


// TODO: Add vec2 structs - Done
// TODO: Add vec3 structs - Done
// TODO: Add vec3 -> vec2 projection functions - Done
// TODO: Add rotation functions

// TODO: Add filled triangle renderer - Done
// TODO: Add filled quad renderer - Done

// TODO: Add texture tools - Done in a different module
// TODO: Add texture mapping tools - Done
// TODO: Add textured quad renderer




typedef struct {
	float x;
	float y;
} vec2;

typedef struct  {
	float x;
	float y;
	float z;
} vec3;

typedef struct {
	vec3 position;
	vec3 rotation;
} transform3;

typedef struct {
	float sensorDistance;
	transform3 transform;
} camera;



vec2 project_3d(vec3 point, camera Cam){
	vec3 camPos = Cam.transform.position;
	vec3 d_position = {point.x-camPos.x, point.y - camPos.y, point.z - camPos.z};
	vec2 out = {0.0, 0.0};
	if (d_position.z < 0){
		out.x = (d_position.x/d_position.z)*Cam.sensorDistance;
		out.y = (d_position.y/d_position.z)*Cam.sensorDistance;
	}
	return out;
}



bool triangle2d(vec2 a, vec2 b, vec2 c, vec2 p){
	float v1 = ((b.x-a.x)*(p.y-a.y)) - ((b.y-a.y)*(p.x-a.x)); // A and B
	float v2 = ((c.x-b.x)*(p.y-b.y)) - ((c.y-b.y)*(p.x-b.x)); // B and C
	float v3 = ((a.x-c.x)*(p.y-c.y)) - ((a.y-c.y)*(p.x-c.x)); // C and A
	if (v1 > 0 && v2 > 0 && v3 > 0){return true;}
	if (v1 < 0 && v2 < 0 && v3 < 0){return true;}
	if (v1 == 0 && v2 == 0 && v3 == 0) {return true;}
	return false;
}


vec2 center_quad;
int compareCCW(const void *a, const void *b) {
    vec2 *p1 = (vec2 *)a;
    vec2 *p2 = (vec2 *)b;

    // Calculate angles relative to the center
    float angle1 = atan2f(p1->y - center_quad.y, p1->x - center_quad.x);
    float angle2 = atan2f(p2->y - center_quad.y, p2->x - center_quad.x);

    if (angle1 < angle2) return -1;
    if (angle1 > angle2) return 1;
    return 0;
}

bool quad2d(vec2 a, vec2 b, vec2 c, vec2 d, vec2 p){
	vec2 center = { ( a.x + b.x + c.x + d.x ) / 4, (a.y + b.y + c.y + d.y ) / 4 };
	vec2 quad[4] = {a, b, c, d};
	center_quad = center;
	qsort(quad, 4, sizeof(vec2), compareCCW);
	//printf("Sorted CCW Points:\n");
    	//for (int i = 0; i < 4; i++) {
        //	printf("Point %d: (%.1f, %.1f)\n", i, quad[i].x, quad[i].y);
    	//}
	bool tri1 = triangle2d(quad[0], quad[1], quad[2], p);
	bool tri2 = triangle2d(quad[0], quad[2], quad[3], p);
	return tri1 || tri2;

}


// Function to map a point from triangle (A1, B1, C1) to triangle (A2, B2, C2)
vec2 distort_point(vec2 P, vec2 A1, vec2 B1, vec2 C1, vec2 A2, vec2 B2, vec2 C2) {
    // Calculate the denominator for barycentric coordinates
    double det = (B1.y - C1.y) * (A1.x - C1.x) + (C1.x - B1.x) * (A1.y - C1.y);

    // Calculate Barycentric Weights (wA, wB, wC) for the point in the first triangle
    double wA = ((B1.y - C1.y) * (P.x - C1.x) + (C1.x - B1.x) * (P.y - C1.y)) / det;
    double wB = ((C1.y - A1.y) * (P.x - C1.x) + (A1.x - C1.x) * (P.y - C1.y)) / det;
    double wC = 1.0 - wA - wB;

    // Apply those weights to the second triangle's vertices
    vec2 P_new;
    P_new.x = wA * A2.x + wB * B2.x + wC * C2.x;
    P_new.y = wA * A2.y + wB * B2.y + wC * C2.y;

    return P_new;
}
