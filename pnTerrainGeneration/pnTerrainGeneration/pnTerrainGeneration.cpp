//INCLUDES
#include<GL/glut.h>
#include<stdlib.h>


//GLOBS
const int segment = 32;

float originOffset = 0.75;

float segmentCordX = -originOffset;
float segmentCordY = -originOffset;

float segmentShift = (2 * originOffset) / segment;

float heightRange = 0.000002;
float heightMap[segment][segment];


//Perlin Noise Mesh
void perlinNoiseMesh() {

	for (int x = 0; x < segment - 2; x++){

		for (int y = 0; y < segment - 1; y++){

			glVertex3f(segmentCordX, segmentCordY, heightMap[x][y]);
			glVertex3f(segmentCordX, segmentCordY + segmentShift, heightMap[x][y + 1]);

			glVertex3f(segmentCordX, segmentCordY, heightMap[x][y]);
			glVertex3f(segmentCordX + segmentShift, segmentCordY + segmentShift, heightMap[x + 1][y + 1]);

			glVertex3f(segmentCordX, segmentCordY, heightMap[x][y]);
			glVertex3f(segmentCordX + segmentShift, segmentCordY, heightMap[x + 1][y]);

			
			segmentCordY += segmentShift;
		}

		segmentCordX += segmentShift;
		segmentCordY = -originOffset;


	}


}



void display() {
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glColor3f(1.0, 0.0, 0.0);

	glBegin(GL_LINES);
	
		perlinNoiseMesh();

	//glVertex3f(0.0, 0.0, 0.0);
	//glVertex3f(1.0, 0.0, 0.0);
	//glVertex3f(1.0, 1.0, 0.0);
	//glVertex3f(0.0, 1.0, 0.0);

	glEnd();

	glFlush();
}

void init() {

	//GENERATE HEIGH MAP
	for (int x = 0; x < segment; x++) {

		for (int y = 0; y < segment; y++) {

			heightMap[x][y] = (rand() * heightRange);
		}
	}

	glClearColor(1.0, 1.0, 1.0, 1.0);
	
	gluPerspective(100.0, 1.0, 0.1
		, -1.0);
	gluLookAt(0.0, -0.75, 0.3, 0.0, 1.0, -1.0, 0.0, 0.0, 1.0);


}

void main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(800, 800);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Points");

	glEnable(GL_DEPTH_TEST);

	init();
	glutDisplayFunc(display);



	glutMainLoop();
}