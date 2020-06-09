//INCLUDES
#include<GL/glut.h>
#include<stdlib.h>

#include<iostream>
#include<ctime>
#include<Windows.h>//sleep


//GLOBS

//mesh globals
bool faces = false;

const int segment = 128;

float originOffset = 2.0;
float heightOffset = 1.0;

float segmentCordX = -originOffset;
float segmentCordY = -originOffset;

float segmentShift = (2 * originOffset) / segment;

//perlin noise globals

int yOffset = 0;

int oct = 1;
float eps = 2.0;

float seedMap[segment * segment];
float heightMap[segment * segment];

float minHeightMap = 1.0;
float maxHeightMap = 0.0;

//PROTOS
void mapColor(float height);

void heightMapMinMax();
void mapHeight();

void perlinNoise1D(int size, float* seed, int octave, float eps, float* output);
void perlinNoise2D(int size, float* seed, int octave, float eps, float* output);

void squareMesh();

//FUNCTIONS

//COLOR MAPPING FUNCTION
void mapColor(float height) {

	float range = maxHeightMap - minHeightMap;

		if(height > (range / 8.0) * 0 + minHeightMap) glColor3f(0, 0.039, 0.501);//water
		if(height > (range / 8.0) * 1 + minHeightMap) glColor3f(0.141, 0.443, 1);
		if(height > (range / 8.0) * 2 + minHeightMap) glColor3f(0.929, 0.921, 0.709);//sand
		if(height > (range / 8.0) * 3 + minHeightMap) glColor3f(0.564, 0.796, 0.572);//grass
		if(height > (range / 8.0) * 4 + minHeightMap) glColor3f(0.223, 0.576, 0.235);
		if(height > (range / 8.0) * 5 + minHeightMap) glColor3f(0.415, 0.603, 0.423);
		if(height > (range / 8.0) * 6 + minHeightMap) glColor3f(0.615, 0.666, 0.615);//mountain
		if(height > (range / 8.0) * 7 + minHeightMap) glColor3f(0.949, 0.949, 0.949);//snow
}

//find min and max of height
void heightMapMinMax() {

	//for height map calculation
	minHeightMap = 1.0;
	maxHeightMap = 0.0;

	for (int x = 0; x < sizeof(heightMap) / sizeof(heightMap[0]); x++) {

		if (heightMap[x] > maxHeightMap) {

			maxHeightMap = heightMap[x];
		}
		if (heightMap[x] < minHeightMap) {

			minHeightMap = heightMap[x];
		}

	}
}

//procedure for generating sucessfull height map
void mapHeight() {

	for (int x = 0; x < segment; x++) {

		for (int y = 0; y < segment; y++) {

			seedMap[y * segment + x] = ((float)rand() / (float)RAND_MAX);
		}
	}

	perlinNoise2D(segment, seedMap, oct, eps, heightMap);

	heightMapMinMax();

	//update camera heights
	glLoadIdentity();
	gluPerspective(80.0, 1.0, 0.1, 4.0);
	gluLookAt(0.0, 1.0, -1.0 + maxHeightMap + 0.05, 0.0, -1.0, -1.0, 0.0, 0.0, 1.0);
}

//PERLIN NOISE APROXIMATION
void perlinNoise2D(int size, float* seed, int octave, float eps, float* output) {

	for (int x = 0; x < size; x++) {

		for (int y = 0; y < size; y++) {


			float noise = 0.0;
			float scaleAcc = 0.0; 
			float scale = 1.0;

			for (int oct = 0; oct < octave; oct++) {

				int pitch = size >> oct;

				int sampX1 = (x / pitch) * pitch;
				int sampY1 = (y / pitch) * pitch;

				int sampX2 = (sampX1 + pitch) % size;
				int sampY2 = (sampY1 + pitch) % size;

				float blendX = (float)(x - sampX1) / (float)pitch;
				float blendY = (float)(y - sampY1) / (float)pitch;

				float lerp1 = (1.0 - blendX) * seed[sampY1 * size + sampX1] + blendX * seed[sampY1 * size + sampX2];
				float lerp2 = (1.0 - blendX) * seed[sampY2 * size + sampX1] + blendX * seed[sampY2 * size + sampX2];

				scaleAcc += scale; 
				noise += (blendY * (lerp2 - lerp1) + lerp1) * scale;
				scale = scale / eps;
			}

			output[(y * size + x + yOffset*(segment))%((segment*segment) - 1)] = noise / scaleAcc;
		}
	}
}

void perlinNoise1D(int size, float *seed, int octave, float eps, float* output) {

	for (int x = 0; x < size; x++) {

		float noise = 0.0;
		float scaleAcc = 0.0; 
		float scale = 1.0;
		

		for (int oct = 0; oct < octave; oct++) {

			int pitch = size >> oct;
			int samp1 = (x / pitch) * pitch;
			int samp2 = (samp1 + pitch)%size;

			float blend = (float)(x - samp1) / (float)pitch;
			float lerp = (1.0 - blend) * seed[samp1] + blend * seed[samp2];

			scaleAcc += scale;
			noise += lerp * scale;
			scale = scale / eps;


		}

		output[x] = noise/scaleAcc;
	}
}

//MESH GENERATOR
void squareMesh() {

	for (int x = 0; x < segment - 2; x++){

		for (int y = 0; y < segment - 1; y++){

			if (faces == false) {

				glBegin(GL_LINES);

				mapColor(heightMap[y * segment + x]);
				glVertex3f(segmentCordX, segmentCordY, heightMap[y * segment + x] - heightOffset);
				mapColor(heightMap[(y + 1) * segment + x]);
				glVertex3f(segmentCordX, segmentCordY + segmentShift, heightMap[(y + 1) * segment + x] - heightOffset);

				mapColor(heightMap[y * segment + x]);
				glVertex3f(segmentCordX, segmentCordY, heightMap[y * segment + x] - heightOffset);
				mapColor(heightMap[(y + 1) * segment + (x + 1)]);
				glVertex3f(segmentCordX + segmentShift, segmentCordY + segmentShift, heightMap[(y + 1) * segment + (x + 1)] - heightOffset);

				mapColor(heightMap[y * segment + x]);
				glVertex3f(segmentCordX, segmentCordY, heightMap[y * segment + x] - heightOffset);
				mapColor(heightMap[y * segment + (x + 1)]);
				glVertex3f(segmentCordX + segmentShift, segmentCordY, heightMap[y * segment + (x + 1)] - heightOffset);

				glEnd();
			}
			else {

				glBegin(GL_POLYGON);

				mapColor(heightMap[y * segment + x]);
				glVertex3f(segmentCordX, segmentCordY, heightMap[y * segment + x] - heightOffset);
				mapColor(heightMap[(y + 1) * segment + x]);
				glVertex3f(segmentCordX, segmentCordY + segmentShift, heightMap[(y + 1) * segment + x] - heightOffset);
				mapColor(heightMap[y * segment + (x + 1)]);
				glVertex3f(segmentCordX + segmentShift, segmentCordY, heightMap[y * segment + (x + 1)] - heightOffset);

				glEnd();

				glBegin(GL_POLYGON);

				mapColor(heightMap[(y + 1) * segment + x]);
				glVertex3f(segmentCordX, segmentCordY + segmentShift, heightMap[(y + 1) * segment + x] - heightOffset);
				mapColor(heightMap[(y + 1) * segment + (x + 1)]);
				glVertex3f(segmentCordX + segmentShift, segmentCordY + segmentShift, heightMap[(y + 1) * segment + (x + 1)] - heightOffset);
				mapColor(heightMap[y * segment + (x + 1)]);
				glVertex3f(segmentCordX + segmentShift, segmentCordY, heightMap[y * segment + (x + 1)] - heightOffset);

				glEnd();
			}
			

			
			segmentCordY += segmentShift;
		}

		segmentCordX += segmentShift;
		segmentCordY = -originOffset;
	}

	//resenter for next redraw
	segmentCordX = -originOffset;
	segmentCordY = -originOffset;
}


//DRAW
void display() {

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glColor3f(1.0, 0.0, 0.0);

	//regen array, fake moving effect.
	yOffset += 1;
	perlinNoise2D(segment, seedMap, oct, eps, heightMap);
	heightMapMinMax();
	
	squareMesh();

	glFlush();
	glutPostRedisplay();

	Sleep(50);
}

//KEYBOARD CONTROL
void keyboard(int key, int x, int y) {


	if (key == GLUT_KEY_LEFT && oct > 0) {

		oct -= 1;
		std::cout << "Octave set to: " << oct << std::endl;
		perlinNoise2D(segment, seedMap, oct, eps, heightMap);
		heightMapMinMax();

	}
	else if (key == GLUT_KEY_RIGHT && segment >> oct > 0) {

		oct += 1;
		std::cout << "Octave set to: " << oct << std::endl;
		perlinNoise2D(segment, seedMap, oct, eps, heightMap);
		heightMapMinMax();

	}
	else if (key == GLUT_KEY_UP) {

		eps += 0.02;
		std::cout << "Epsilon set to: " << eps << std::endl;
		perlinNoise2D(segment, seedMap, oct, eps, heightMap);
		heightMapMinMax();
	}
	else if (key == GLUT_KEY_DOWN) {

		(eps > 0.02) ? eps -= 0.02 : eps = eps;
		std::cout << "Epsilon set to: " << eps << std::endl;
		perlinNoise2D(segment, seedMap, oct, eps, heightMap);
		heightMapMinMax();

	}else if(key == GLUT_KEY_HOME){

		faces = !faces;
	}else if (key == GLUT_KEY_INSERT) {

		mapHeight();
	}

	glutPostRedisplay();
}


void init() {
	//SEED
	srand(std::time(nullptr));

	//PERLIN NOISE
	mapHeight();

	glClearColor(0.4, 0.4, 0.4, 1.0);

	//CAMERA
	glLoadIdentity();
	gluPerspective(75.0, 1.0, 0.1, 4.0);
	gluLookAt(0.0, 1.0, -1.0 + maxHeightMap + 0.05, 0.0, -1.0, -1.0, 0.0, 0.0, 1.0);


}

//MAIN
int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(1000, 1000);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Perlin Noise - Mesh Generator");

	glEnable(GL_DEPTH_TEST);

	init();
	glutDisplayFunc(display);
	glutSpecialFunc(keyboard);



	glutMainLoop();
}