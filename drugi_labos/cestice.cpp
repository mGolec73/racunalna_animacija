#define _USE_MATH_DEFINES
#include <GL/glut.h>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <ctime>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

GLuint width = 800, height = 600;

struct Cestica {
    float x, y, z;     // trenutna pozicija
    float px, py, pz;  // prethodna pozcija 
    float vx, vy, vz;
    float r, g, b, alfa;
    float trajanje;
};

struct Ociste {
    float x;
    float y;
    float z;
    float kut;
};

std::vector<Cestica> cestice;

Ociste ociste = { 0.0f,5.0f,20.0f,0.0f };


void myDisplay();
void myReshape();
void myKeyboard(unsigned char theKey, int mouseX, int mouseY);
void myRenderScene();
void myGenerateFirework(float x, float y, float z);
void myUpdateParticles(float delta_t);
void myDrawParticle();
void timer(int value);

int main(int argc, char** argv) {
    srand(time(0));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Vatromet");

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 800.0 / 600.0, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);

    glutDisplayFunc(myDisplay);
    glutKeyboardFunc(myKeyboard);
    glutTimerFunc(40, timer, 0);
    glutMainLoop();
    return 0;
}


void myGenerateFirework(float x, float y, float z) {
    for (int i = 0; i < 200; i++) {

        Cestica p;
        p.x = x;
        p.y = y;
        p.z = z;

        p.px = x; 
        p.py = y;
        p.pz = z; 

        float kutSmjera = ((float)rand() / RAND_MAX) * 2.0f * M_PI;
        float brzina = ((float)rand() / RAND_MAX) * 3.0f + 1.0f;

        p.vx = cos(kutSmjera) * brzina;
        p.vy = sin(kutSmjera) * brzina;
        p.vz = ((float)rand() / RAND_MAX) * 10.0f - 5.0f;

        p.r = ((float)rand() / RAND_MAX);
        p.g = ((float)rand() / RAND_MAX);
        p.b = ((float)rand() / RAND_MAX);
        p.alfa = 1.0f;

        p.trajanje = 1.0f;
        cestice.push_back(p);
    }
}

void myUpdateParticles(float delta_t) {
    for (auto it = cestice.begin(); it != cestice.end();) {
 
        it->px = it->x;
        it->py = it->y;
        it->pz = it->z;

        it->x += it->vx * delta_t;
        it->y += it->vy * delta_t;
        it->z += it->vz * delta_t;

        it->vy -= 9.8f * delta_t; // gravitacijska sila
        it->trajanje -= delta_t;
        it->alfa -= delta_t; // povećavanje prozirnosti 

        if (it->trajanje <= 0.0f) {
            it = cestice.erase(it);
        }
        else {
            ++it;
        }
    }
}

void myKeyboard(unsigned char key, int x, int y) {
    const float pomakKamere = 1.0f;
    const float kutPomaka = 5.0f;

    switch (key) {
    case 'w': // naprijed
        ociste.z -= pomakKamere * cos(ociste.kut * M_PI / 180.0f);
        ociste.x -= pomakKamere * sin(ociste.kut * M_PI / 180.0f);
        break;
    case 's': // natrag
        ociste.z += pomakKamere * cos(ociste.kut * M_PI / 180.0f);
        ociste.x += pomakKamere * sin(ociste.kut * M_PI / 180.0f);
        break;
    case 'a': // lijevo
        ociste.x -= pomakKamere * cos(ociste.kut * M_PI / 180.0f);
        ociste.z += pomakKamere * sin(ociste.kut * M_PI / 180.0f);
        break;
    case 'd': // desno
        ociste.x += pomakKamere * cos(ociste.kut * M_PI / 180.0f);
        ociste.z -= pomakKamere * sin(ociste.kut * M_PI / 180.0f);
        break;
    case 'q': // rotacija lijevo
        ociste.kut -= kutPomaka;
        break;
    case 'e': // rotacija desno
        ociste.kut += kutPomaka;
        break;
    }
}

void myDrawParticle() {
    glLineWidth(2.0f);
    glBegin(GL_LINES);

    for (const auto& p : cestice) {
        glColor4f(p.r, p.g, p.b, p.alfa); 
        glVertex3f(p.px, p.py, p.pz);
        glVertex3f(p.x, p.y, p.z);
    }
    glEnd();
}

void myRenderScene() {

    float lookX = ociste.x + sin(ociste.kut * M_PI / 180.0f);
    float lookZ = ociste.z - cos(ociste.kut * M_PI / 180.0f);
    gluLookAt(ociste.x, ociste.y, ociste.z, lookX, ociste.y, lookZ, 0.0f, 1.0f, 0.0f);
    myDrawParticle();
}

void myDisplay() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.2f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    myRenderScene();
    glutSwapBuffers();
}

int brojac = 0;

void timer(int value) {

    myUpdateParticles(0.016f); 

    float x = ((float)rand() / RAND_MAX) * 20.0f - 10.0f;
    float y = ((float)rand() / RAND_MAX) * 10.0f + 5.0f;
    float z = ((float)rand() / RAND_MAX) * 20.0f - 10.0f;

    if (brojac == 6) {
        myGenerateFirework(x, y, z);
        brojac = 0;
        //printf("cestice vektor: %d", cestice.size());
    }
    brojac++;
    glutPostRedisplay();
    glutTimerFunc(40, timer, 0);
}





