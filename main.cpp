#include <GL/glut.h>
#include <GL/freeglut.h>
#include <cmath>
#include <iostream>

#define SENS_ROT 5.0
#define SENS_OBS 10.0
#define SENS_TRANSL 10.0

GLfloat distance = 50;
GLfloat azimuth = 0;
GLfloat incidence = 0;
GLfloat twist = 0;
GLfloat angle, fAspect;

GLfloat rotX, rotY, rotX_ini, rotY_ini;

GLfloat obsX, obsY, obsZ, obsX_ini, obsY_ini, obsZ_ini;

GLfloat truckMove = 20.f;
GLfloat cXMove = 10.f;
GLfloat cZMove = 0.f;
GLfloat cYMove = 2.f;
GLfloat jumpHeight = 0.0f, jumpVelocity = 0.0f, cYPos = 0.0f;

bool isJumping = false;
int x_ini, y_ini, mouse_button;

// Defina as propriedades dos materiais para a grama
GLfloat matGrassDiffuse[] = { 0.0, 1.0, 0.0, 1.0 }; // Cor difusa
GLfloat matGrassSpecular[] = { 0.1, 0.1, 0.1, 1.0 }; // Cor especular
GLfloat matGrassShininess = 10.0; // Brilho


void JumpAnimation(int value) {
	if (jumpHeight > 0.0) {
		cYMove = cYPos + jumpHeight;
		jumpVelocity -= 0.005; // Ajuste a velocidade do pulo aqui
		jumpHeight += jumpVelocity;

		glutTimerFunc(10, JumpAnimation, 0);
	} else {
		cYMove = cYPos;
}

glutPostRedisplay();
}

void polarView() {
	glTranslatef(0.0, 0.0, -distance);
	glRotatef(-twist, 0.0, 0.0, 1.0);
	glRotatef(-incidence, 1.0, 0.0, 0.0);
	glRotatef(-azimuth, 0.0, 0.0, 1.0);
}

void DefineIluminacao() {
	GLfloat luzAmbiente[4] = {0.2, 0.2, 0.2, 1.0};
	GLfloat luzDifusa[4] = {0.7, 0.7, 0.7, 1.0}; // "cor"
	GLfloat luzEspecular[4] = {1.0, 1.0, 1.0, 1.0}; // "brilho"
	GLfloat posicaoLuz[4] = {20, 100, 110, 1.0};

	GLfloat especularidade[4] = {1.0, 1.0, 1.0, 1.0};
	GLint especMaterial = 60;

	glMaterialfv(GL_FRONT, GL_SPECULAR, especularidade);
	glMateriali(GL_FRONT, GL_SHININESS, especMaterial);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);
	glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa);
	glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular);
	glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz);
}

void PosicionaObservador() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	DefineIluminacao();
	glTranslatef(-obsX, -obsY, -obsZ);
	glRotatef(rotX, 1, 0, 0);
	glRotatef(rotY, 0, 1, 0);
}

void EspecificaParametrosVisualizacao() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(angle, fAspect, 0.5, 500);
	PosicionaObservador();
}

void GerenciaMouse(int button, int state, int x, int y) {
	if (state == GLUT_DOWN) {
		x_ini = x;
		y_ini = y;
		obsX_ini = obsX;
		obsY_ini = obsY;
		obsZ_ini = obsZ;
		rotX_ini = rotX;
		rotY_ini = rotY;
		mouse_button = button;
	}

	else mouse_button = -1;
}

void GerenciaMovimentoCaminhao(int x, int y) {
	if (mouse_button == GLUT_LEFT_BUTTON) {
		int deltax = x_ini - x;
		int deltay = y_ini - y;
		rotY = rotY_ini - deltax / SENS_ROT;
		rotX = rotX_ini - deltay / SENS_ROT;
	} else if (mouse_button == GLUT_RIGHT_BUTTON) {
		int deltaz = y_ini - y;
		obsZ = obsZ_ini + deltaz / SENS_OBS;
	} else if (mouse_button == GLUT_MIDDLE_BUTTON) {
		int deltax = x_ini - x;
		int deltay = y_ini - y;
		obsX = obsX_ini + deltax / SENS_TRANSL;
		obsY = obsY_ini - deltay / SENS_TRANSL;
	}

	PosicionaObservador();
	glutPostRedisplay();
}

void Reshape(GLsizei w, GLsizei h) {
	if (h == 0)
		h = 1;

	glViewport(0, 0, w, h);
	fAspect = (GLfloat)w / (GLfloat)h;
	EspecificaParametrosVisualizacao();
}

void display() {
	/* Limpa todos os pixels da tela */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DefineIluminacao();

	glPushMatrix(); // Inicio Grama
		glTranslatef(0, -10, 0);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, matGrassDiffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, matGrassSpecular);
		glMaterialf(GL_FRONT, GL_SHININESS, matGrassShininess);
        // Renderizar a grama ao redor do asfalto (um quadrado verde)
        glColor3f(0.0f, 1.0f, 0.0f);

        glBegin(GL_QUADS);
			glVertex3f(-200.0f, 0.0f, -200.0f);
			glVertex3f(200.0f, 0.0f, -200.0f);
			glVertex3f(200.0f, 0.0f, 200.0f);
			glVertex3f(-200.0f, 0.0f, 200.0f);
        glEnd();
	glPopMatrix(); // Fim Grama

	glPushMatrix(); // Sol
		glTranslatef(0, 100, 100);
		glColor3d(1, 1, 0.2);
		glutSolidSphere(25, 100, 100);
	glPopMatrix(); // Fim Sol

	glPushMatrix(); // Caminhon
		polarView();

		glColor3d(1, 0, 0); // Traseira Caminhao
		glTranslatef(truckMove, 0, 0);
		glutSolidCube(15);

		glColor3d(1, 0, 0);
		glTranslatef(15, 0, 0);
		glutSolidCube(15); // Fim Traseira Caminhao

		glColor3d(1, 1, 1); // Frente Caminhao
		glTranslatef(13.2, -2, 0);
		glutSolidCube(10); // Fim da Frente Caminhao

		glTranslatef(0, -5, 5); // Roda direita da frente
		glColor3d(0, 0, 0);
		glutSolidTorus(1, 1, 100, 100); // Fim da Roda direita da frente

		glTranslatef(0, 0, -10); // Roda esquerda da frente
		glColor3d(0, 0, 0);
		glutSolidTorus(1, 1, 100, 100); // Fim da Roda esquerda da frente

		glTranslatef(-28, 0, 12); // Roda1 da direita traseira
		glColor3d(0, 0, 0);
		glutSolidTorus(1, 1, 100, 100); // Fim da Roda1 da direita traseira

		glTranslatef(-4, 0, 0); // Roda2 da direita traseira
		glColor3d(0, 0, 0);
		glutSolidTorus(1, 1, 100, 100); // Fim da Roda2 da direita traseira


		glTranslatef(4, 0, -14); // Roda1 traseira esquerda
		glColor3d(0, 0, 0);
		glutSolidTorus(1, 1, 100, 100); // Fim da Roda1 traseira esquerda

		glTranslatef(-4, 0, 0); // Roda2 traseira esquerda
		glColor3d(0, 0, 0);
		glutSolidTorus(1, 1, 100, 100); // Fim da Roda2 traseira esquerda
	glPopMatrix(); // Fim do caminhon

	glPushMatrix(); // Lixo
		glTranslatef(40, -10, 20);
		glRotatef(90, -1, 0, 0);
		glColor3d(0.5, 0.5, 0.5);
		glutSolidCylinder(5, 10, 100, 100);
		glutSolidCylinder(1, 10.5, 100, 100);

		glPushMatrix();
			glTranslatef(0, -6, 10.1);
			glRotatef(90, -1, 0, 0);
			glColor3d(0.5, 0.5, 0.5);
			glutSolidCylinder(6, 12, 2, 2);
		glPopMatrix();
	glPopMatrix(); // Fim do Lixo

	glPushMatrix(); // Pessoa
		
		// Cabeca
		glTranslatef(cXMove, cYMove, cZMove);
		glColor3d(0, 0, 1);
		glutSolidSphere(2, 100, 100);

		// Corpito
		glPushMatrix();
			glTranslatef(0, -2, 0);
			glRotatef(90, 50, 0, 0);
			glColor3d(0, 0, 1);
			glutSolidCylinder(2, 5, 40, 40);
		glPopMatrix();


		// Pernas
		glPushMatrix();
			glTranslatef(-1, -7, 0);
			glRotatef(90, 50, 0, 0);
			glColor3d(0, 0, 1);
			glutSolidCylinder(0.8, 5, 40, 40);
		glPopMatrix();

		glPushMatrix();
			glTranslatef(1, -7, 0);
			glRotatef(90, 50, 0, 0);
			glColor3d(0, 0, 1);
			glutSolidCylinder(0.8, 5, 40, 40);
		glPopMatrix();

		// Braco Direita
		glPushMatrix();
			glTranslatef(-1, -2, 0);
			glRotatef(45, 0, 0, 0);
			glColor3d(0, 0, 1);
			glutSolidCylinder(0.5, 5, 40, 40);
		glPopMatrix();

		// Braco Esquerda
		glPushMatrix();
			glTranslatef(1, -2, 0);
			glRotatef(45, 0, 0, 0);
			glColor3d(0, 0, 1);
			glutSolidCylinder(0.5, 5, 40, 40);
		glPopMatrix();

	glPopMatrix(); // Fim da character

	glutSwapBuffers();
}

void myReshape(int winWidth, int winHeight) {
	int tam = winWidth < winHeight ? winWidth : winHeight;
	glViewport(0, 0, tam, tam); // reset the viewport
}

/*comandos especiais do teclado*/
void keySpecial(int key, int x, int y) {
	switch (key)
	{
	case GLUT_KEY_LEFT:
		truckMove -= 5.f;
		break;
	case GLUT_KEY_RIGHT:
		truckMove += 5.f;
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

void myKeyboard(unsigned char c, int x, int y) {
	switch (c) {
	case 27: // ESC em ASCII
		exit(0);
		break;
	case 'q':
		exit(0);
		break;
	case 'd':
		cXMove += 5.f;
		break;
	case 'a':
		cXMove -= 5.f;
		break;
	case 'w':
		cZMove += 5.f;
		break;
	case 's':
		cZMove -= 5.f;
		break;
	case 'f':
		if (jumpHeight == 0.0) {
			cYPos = cYMove;
			jumpHeight = 1.0; // Ajuste a altura máxima do pulo aqui
			jumpVelocity = 0.1; // Ajuste a velocidade inicial do pulo aqui
			JumpAnimation(0);
		}
		else{
			jumpHeight = 0.0;
			JumpAnimation(0);
		}
			break;
	default:
		break;
	}

	glutPostRedisplay();
}

// imprimir as coordenadas com o clique do mouse
void myMouse(int b, int s, int x, int y) {
	switch (b) {
	case GLUT_LEFT_BUTTON:
		printf("(%d, %d)\n", x, y);
	}
}

// função detecta truckMoveimento do mouse e salva coordenadas
void myMotion(int x, int y) {
	printf("Motion: (%d, %d)\n", x, y);
}

void init() {
	// background color
	glClearColor(0.53f, 0.81f, 0.92f, 1.0f);

	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_FLAT);

	angle = 45;
	rotX = 0;
	rotY = 10;
	obsX = 20;
	obsY = 0;
	obsZ = 100;
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(5, 5);
	glutInitWindowSize(450, 450);
	glutCreateWindow("Trabalho Carro OpenGl CGPDI");
	glutDisplayFunc(display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(myKeyboard);
	glutSpecialFunc(keySpecial);
	glutMouseFunc(GerenciaMouse);
	glutMotionFunc(GerenciaMovimentoCaminhao);
	init();
	glutMainLoop();

	return 0;
}