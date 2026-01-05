// TODO: Add vec2 structs - Done
// TODO: Add vec3 structs - Done
// TODO: Add vec3 -> vec2 projection functions - done
// TODO: Add rotation functions

// TODO: Add filled triangle renderer
// TODO: Add filled quad renderer

// TODO: Add texture tools
// TODO: Add texture mapping tools
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
	if (d_position.z > 0){
		out.x = (d_position.x/d_position.z)*Cam.sensorDistance;
		out.y = (d_position.y/d_position.z)*Cam.sensorDistance;
	}
	return out;
}



bool triangle2d(vec2 p1, vec2 p2, vec2 p3){
	

}
