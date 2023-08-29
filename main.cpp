#include <GL/glut.h>
#include <GL/freeglut.h>
#include <cmath>
#include <iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>

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

GLfloat movimentoCaminhao = 20.f;
GLfloat pXMove = 10.f;
GLfloat pZMove = 0.f;
GLfloat pYMove = 2.f;
GLfloat jumpHeight = 0.0f, jumpVelocity = 0.0f, cYPos = 0.0f;

bool isJumping = false;
int x_ini, y_ini, mouse_button;

GLuint id, idSol;

void JumpAnimation(int value) {
	if (jumpHeight > 0.0) {
		pYMove = cYPos + jumpHeight;
		jumpVelocity -= 0.005; // Ajuste a velocidade do pulo aqui
		jumpHeight += jumpVelocity;

		glutTimerFunc(10, JumpAnimation, 0);
	} else {
		pYMove = cYPos;
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
	GLfloat luzEspecular[4] = {1, 0, 0, 1.0}; // "brilho"
	GLfloat posicaoLuz[4] = {10, 2, 0, 0};
	GLint   spot_luz1         = 30;

	GLfloat especularidade[4] = {1.0, 1.0, 1.0, 1.0};
	GLint especMaterial = 60;

	glMaterialfv(GL_FRONT, GL_SPECULAR, especularidade);
	glMateriali(GL_FRONT, GL_SHININESS, especMaterial);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);
	glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa);
	glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular);
	glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz);
	glLightf (GL_LIGHT1, GL_SPOT_CUTOFF, spot_luz1);
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
	gluPerspective(angle, fAspect, 1, 700);
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

void GerenciaMovimentoCarro(int x, int y) {
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

	glPushMatrix(); // Ground with Texture
	glColor3f(0.0f, 1.0f, 0.0f);
    glTranslatef(0, -10, 0);
	glEnable(GL_TEXTURE_2D);
    // Enable texture mapping for the ground
    glBindTexture(GL_TEXTURE_2D, id);
        glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-200.0f, 0.0f, -200.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(200.0f, 0.0f, -200.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(200.0f, 0.0f, 200.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-200.0f, 0.0f, 200.0f);
		glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix(); // Fim Grama

	glPushMatrix(); // Sol
		glColor3d(1, 1, 0.2);
		glTranslatef(-40, 50, -100);
		glutSolidSphere(10, 100, 100);
	glPopMatrix(); // Fim Sol

	//carro
	glPushMatrix();
    polarView();

	/// Corpo do carro
    glColor3d(0.0, 0.0, 1.0); 
    glTranslatef(movimentoCaminhao, 0, 0);

    // Caroceria
    glPushMatrix();
    glScalef(4.0, 0.6, 1.5);
    glutSolidCube(15);
    glPopMatrix();

    // Teto
    glPushMatrix();
    glColor3d(0.7, 0.7, 1.0); 
    glTranslatef(-5, 12, 0);
    glScalef(5, 0.2, 3); 
    glutSolidCube(7.5);
    glPopMatrix();

    // Janelas
    glColor3d(0.7, 0.7, 1.0);

    // Janela da frente
    glPushMatrix();
    glTranslatef(14, 7, 0);
    glRotatef(90, 0, 45, 0);
    glScalef(1.2,0.7, 0.02);
    glutSolidCube(15);
    glPopMatrix();

    // Janela da direita
    glPushMatrix();
    glTranslatef(-8, 7.5, 10);
    glRotatef(0, 0, 1, 0);
    glScalef(3, 0.5, 0.02);
    glutSolidCube(15);
    glPopMatrix();

	// Janela da esquerda
    glPushMatrix();
    glTranslatef(-8, 7.5, -10);
    glRotatef(0, 0, 1, 0);
    glScalef(3, 0.5, 0.02);
    glutSolidCube(15);
    glPopMatrix();

    // Rodas
    glColor3d(0, 0, 0);

    // Roda direita traseira
    glPushMatrix();
    glTranslatef(-17, -6, 7.5);
    glutSolidTorus(2, 4, 100, 100);
    glPopMatrix();

    // Roda esquerda traseira
    glPushMatrix();
    glTranslatef(-17, -6, -7.5);
    glutSolidTorus(2, 4, 100, 100);
    glPopMatrix();

    // Roda direita diantera
    glPushMatrix();
    glTranslatef(17, -6, 7.5);
    glutSolidTorus(2, 4, 100, 100);
    glPopMatrix();

    // Roda esquerda diantera
    glPushMatrix();
    glTranslatef(17, -6, -7.5);
    glutSolidTorus(2, 4, 100, 100);
    glPopMatrix();

    glPopMatrix(); // End Car

	glPushMatrix(); // Lixo
		glTranslatef(40, -10, 20);
		glRotatef(90, -1, 0, 0);
		glColor3d(0.5, 0.5, 0.5);
		glutSolidCylinder(5, 10, 100, 100);
		glutSolidCylinder(1, 10.5, 100, 100);

		glPushMatrix();
			glTranslatef(0, -6, 10.2);
			glRotatef(90, -1, 0, 0);
			glColor3d(0.5, 0.5, 0.5);
			glutSolidCylinder(6, 12, 2, 2);
		glPopMatrix();
	glPopMatrix(); // Fim do Lixo

	glPushMatrix(); // Pessoa
		
		// Cabeca
		glTranslatef(pXMove, pYMove, pZMove);
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
		movimentoCaminhao -= 5.f;
		break;
	case GLUT_KEY_RIGHT:
		movimentoCaminhao += 5.f;
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
		pXMove += 5.f;
		break;
	case 'a':
		pXMove -= 5.f;
		break;
	case 'w':
		pZMove += 5.f;
		break;
	case 's':
		pZMove -= 5.f;
		break;
	case 'f':
		if (jumpHeight == 0.0) {
			cYPos = pYMove;
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

// função detecta movimentoCaminhaoimento do mouse e salva coordenadas
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
	// glShadeModel(GL_FLAT);

    SDL_Surface* image = IMG_Load("Grama.jpg");

	// Enable texture mapping
    glEnable(GL_TEXTURE_2D);
    // Generate a texture ID
    glGenTextures(1, &id);
    // Load and bind the grass texture
    glBindTexture(GL_TEXTURE_2D, id);


    glGenTextures(1, &id);

	glBindTexture(GL_TEXTURE_2D, id);
	//Filtro
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//Descobrimos o formato a partir da imagem
	GLint format = image->format->BytesPerPixel == 3 ? GL_RGB : GL_RGBA;

	//Carregamos a imagem do disco
	glTexImage2D(GL_TEXTURE_2D, 0, format, image->w, image->h, 0, format, GL_UNSIGNED_BYTE, image->pixels);

	SDL_FreeSurface(image);


	angle = 90;
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
	glutMotionFunc(GerenciaMovimentoCarro);
	init();
	glutMainLoop();

	return 0;
}