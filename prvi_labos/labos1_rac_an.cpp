#include <stdio.h>
#include <iostream>
#include <math.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <vector>
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

GLuint width = 600, height = 600;
int kut = 0;

typedef struct _Ociste {
    GLdouble	x;
    GLdouble	y;
    GLdouble	z;
} Ociste;

typedef struct {
    float x;
    float y;
    float z;
} vrh;


typedef struct {
    int vrh1;
    int vrh2;
    int vrh3;
} poligon;

vector<vrh> tocke_kontrolnog;
vector<vrh> tocke_krivulje;
vector<vrh> tocke_tangente;
vector<vrh> vrhovi_objekta;
vector<poligon> poligoni_objekta;
vector<vrh> drugaDer;

int brojKontrolnih = 0;
int brojVrhova = 0;
int brojPoligona = 0;

bool kontrolni = false;
bool tangente = false;

vec3 s = vec3(0, 0, 1);
Ociste	ociste = { 0.0f, 0.0f, 2.0f };

void myDisplay();
void myReshape(int width, int height);
void myDrawSquare();
void myRenderScene();
void idle();
void myKeyboard(unsigned char theKey, int mouseX, int mouseY);
void loadCurve(const string& datoteka);
void myLoadObject(const string& datoteka);
void myDrawCurve();


int main(int argc, char** argv)
{
    loadCurve("C:/Users/Marina/Desktop/4.godina/ljetni/racGrafika/IRG/IRGPrimjeri/IRGPrimjeri/labos1_rac_an/kontrolni.txt");
    myLoadObject("C:/Users/Marina/Desktop/4.godina/ljetni/racGrafika/IRG/IRGPrimjeri/IRGPrimjeri/labos1_rac_an/f16.obj");
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Animacija kretanja po putanji");
    glutDisplayFunc(myDisplay);
    glutReshapeFunc(myReshape);
    glutIdleFunc(idle);
    glutKeyboardFunc(myKeyboard);
    printf("Tipka: a/d - pomicanje ocista po x os +/-\n");
    printf("Tipka: w/s - pomicanje ocista po y os +/-\n");
    printf("Tipka: r - pocetno stanje\n");
    printf("esc: izlaz iz programa\n");

    glutMainLoop();
    return 0;
}

void myDisplay()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    myRenderScene();
    glutSwapBuffers();
}

void myReshape(int w, int h)
{
    width = w; height = h;
    //glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, 1, 200);
    //glFrustum(-1, 1, -1, 1, 1, 5);
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(ociste.x, ociste.y, ociste.z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);	// ociste x,y,z; glediste x,y,z; up vektor x,y,z
}


void loadCurve(const string& datoteka) {
    ifstream file(datoteka);
    if (!file.is_open()) {
        printf("Datoteka nije učitana");
    }
    else {
        string redak;
        float minX = FLT_MAX;
        float minY = FLT_MAX;
        float minZ = FLT_MAX;
        float maxX = -FLT_MAX;
        float maxY = -FLT_MAX;
        float maxZ = -FLT_MAX;
        while (getline(file, redak))
        {
            if (redak.empty()) continue;
            char oznaka = redak[0];
            string tekst;
            if (oznaka == 'c') {
                istringstream iss(redak.substr(2));
                vrh kontrolni;
                iss >> kontrolni.x >> kontrolni.y >> kontrolni.z;
                tocke_kontrolnog.push_back(kontrolni);
                brojKontrolnih++;
                minX = std::min(minX, kontrolni.x);
                minY = std::min(minY, kontrolni.y);
                minZ = std::min(minZ, kontrolni.z);
                maxX = std::max(maxX, kontrolni.x);
                maxY = std::max(maxY, kontrolni.y);
                maxZ = std::max(maxZ, kontrolni.z);
                //printf(" vrhovi %f %f %f \n", kontrolni.x, kontrolni.y, kontrolni.z);
            }
        }
        float x_c = (minX + maxX) / 2;  
        float y_c = (minY + maxY) / 2;
        float z_c = (minZ + maxZ) / 2;

        for (auto& v : tocke_kontrolnog) {
            v.x -= x_c;
            v.y -= y_c;
            v.z -= z_c;
        }

        float d_x = maxX - minX;
        float d_y = maxY - minY;
        float d_z = maxZ - minZ;

        float maximum = std::max({ d_x, d_y, d_z });
        float scale_f = 2 / maximum;

        for (auto& v : tocke_kontrolnog) {
            v.x = v.x * scale_f;
            v.y = v.y * scale_f;
            v.z = v.z * scale_f;
        }
    }
    

    file.close();
    printf("broj kontrolnih tocaka %d\n", brojKontrolnih);
}

void myDrawPolygon() // crtanje 
{
    glColor3f(0.0f, 0.7f, 0.5f);
    glBegin(GL_LINE_STRIP);
    for (auto& p : tocke_kontrolnog)
    {
        glVertex3f(p.x,p.y,p.z);
    }
    glEnd();
}

void myDrawCurve() {
    tocke_krivulje.clear();
    tocke_tangente.clear();
    mat4 B = mat4(vec4(-1, 3, -3, 1), vec4(3, -6, 0, 4), vec4(-3, 3, 3, 1), vec4(1, 0, 0, 0));

    for (int k = 0; k < brojKontrolnih - 3; k++) {
        vec3 r1 = vec3(tocke_kontrolnog[k].x, tocke_kontrolnog[k].y, tocke_kontrolnog[k].z);
        vec3 r2 = vec3(tocke_kontrolnog[k+1].x, tocke_kontrolnog[k+1].y, tocke_kontrolnog[k+1].z);
        vec3 r3 = vec3(tocke_kontrolnog[k+2].x, tocke_kontrolnog[k+2].y, tocke_kontrolnog[k+2].z);
        vec3 r4 = vec3(tocke_kontrolnog[k+3].x, tocke_kontrolnog[k+3].y, tocke_kontrolnog[k+3].z);

        for (float t = 0; t < 1; t += 0.01) {
            vec4 T = vec4(t * t * t, t * t, t, 1);
            T = (1 / 6.0f) * T;
            mat3x4 R = mat3x4(r1.x, r2.x, r3.x, r4.x,
                              r1.y, r2.y, r3.y, r4.y,
                              r1.z, r2.z, r3.z, r4.z);
            vec4 rez1 = T * B;
            vec3 tocka = rez1 * R;
            //printf("tocke x y z: %f, %f, %f \n", tocka.x, tocka.y, tocka.z);
            vrh tocka1;
            tocka1.x = tocka.x;
            tocka1.y = tocka.y;
            tocka1.z = tocka.z;

            tocke_krivulje.push_back(tocka1);

            vec4 Ttan = vec4(3 * t * t, 2 * t, 1, 0);
            Ttan = (1 / 6.0f) * Ttan;
            vec4 rez2 = Ttan * B;
            vec3 tockaT = rez2 * R;

            tockaT = normalize(tockaT);
            vrh tocka2;
            tocka2.x = tockaT.x;
            tocka2.y = tockaT.y;
            tocka2.z = tockaT.z;

            tocke_tangente.push_back(tocka2);

            vec4 Tdd = vec4(6 * t, 2, 0, 0);
            Tdd = (1 / 6.0f) * Tdd;
            vec4 rez3 = Tdd * B;
            vec3 tockaD = rez3 * R;

            tockaD = normalize(tockaD);
            vrh tocka3;
            tocka3.x = tockaD.x;
            tocka3.y = tockaD.y;
            tocka3.z = tockaD.z;

            drugaDer.push_back(tocka3);

        }


    } 
    glColor3f(0, 0, 0);
    glBegin(GL_LINE_STRIP);
    for (auto& p : tocke_krivulje)
    {
        glVertex3f(p.x,p.y,p.z);
    }
    glEnd();

    if (tangente) {
        glColor3f(1,0.6f, 0.0f);
        for (int i = 0; i < tocke_krivulje.size(); i += 30) {
            glBegin(GL_LINES);
            glVertex3f(tocke_krivulje[i].x, tocke_krivulje[i].y, tocke_krivulje[i].z);
            glVertex3f(tocke_krivulje[i].x + tocke_tangente[i].x / 5, tocke_krivulje[i].y + tocke_tangente[i].y / 5, tocke_krivulje[i].z + tocke_tangente[i].z / 5);
            glEnd();
        }
    }
}

void myLoadObject(const string& datoteka) {
    ifstream file(datoteka);
    if (!file.is_open()) {
        printf("Datoteka nije ucitana");
    }
    else {
        float minX = FLT_MAX;
        float minY = FLT_MAX;
        float minZ = FLT_MAX;
        float maxX = -FLT_MAX;
        float maxY = -FLT_MAX;
        float maxZ = -FLT_MAX;
        string redak;
        while (getline(file, redak))
        {
            if (redak.empty()) continue;
            char oznaka = redak[0];
            string tekst;
            if (oznaka == 'v') {
                istringstream iss(redak.substr(2));
                vrh v;
                iss >> tekst;
                v.x = stof(tekst);
                iss >> tekst;
                v.y = stof(tekst);
                iss >> tekst;
                v.z = stof(tekst);
                vrhovi_objekta.push_back(v);
                minX = std::min(minX, v.x);
                minY = std::min(minY, v.y);
                minZ = std::min(minZ, v.z);
                maxX = std::max(maxX, v.x);
                maxY = std::max(maxY, v.y);
                maxZ = std::max(maxZ, v.z);
                brojVrhova++;
                //printf(" vrhovi %f %f %f \n", v.x, v.y, v.z);
            }
            else if (oznaka == 'f') {
                istringstream iss(redak.substr(2));
                poligon p;
                iss >> p.vrh1 >> p.vrh2 >> p.vrh3;
                poligoni_objekta.push_back(p);
                brojPoligona++;
                //printf(" poligoni %d %d %d \n", p.vrh1, p.vrh2, p.vrh3);

            }
        }
        float x_c = (minX + maxX) / 2;
        float y_c = (minY + maxY) / 2;
        float z_c = (minZ + maxZ) / 2;
        //printf(" srediste koordinate: %f %f %f", x_c, y_c, z_c);

        for (auto& v : vrhovi_objekta) {
            v.x -= x_c;
            v.y -= y_c;
            v.z -= z_c;
        }
        float d_x = maxX - minX;
        float d_y = maxY - minY;
        float d_z = maxZ - minZ;

        float maximum = std::max({ d_x, d_y, d_z });
        float scale_f = 2 / maximum;


        for (auto& v : vrhovi_objekta) {
            v.x = v.x * scale_f;
            v.y = v.y * scale_f;
            v.z = v.z * scale_f;
        }
    }
    file.close();
    //printf("broj vrhova: %d, broj poligona %d\n", brojVrhova, brojPoligona);
}

void myDisplayObject() {

    for (int i = 0; i < brojPoligona; i++) {
        poligon p = poligoni_objekta[i];

        int v_ind1 = p.vrh1 - 1;
        int v_ind2 = p.vrh2 - 1;
        int v_ind3 = p.vrh3 - 1;

        vrh v1 = vrhovi_objekta[v_ind1];
        vrh v2 = vrhovi_objekta[v_ind2];
        vrh v3 = vrhovi_objekta[v_ind3];

        glColor3f(0.7f, 0.2f, 0.4f);
        glBegin(GL_TRIANGLES);
        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
        glVertex3f(v3.x, v3.y, v3.z);
        glEnd();

    }
}
int tt = 0;
void myAnimateObject() {
    vec3 e = vec3(tocke_tangente[tt].x, tocke_tangente[tt].y, tocke_tangente[tt].z);
    vec3 os = cross(s, e);

    float skal = dot(s, e);
    float kut = acos(skal) * (180 / 3.14159);

    glPushMatrix();
    glTranslatef(tocke_krivulje[tt].x, tocke_krivulje[tt].y, tocke_krivulje[tt].z);
    glScalef(1 / 5.0f, 1 / 5.0f, 1 / 5.0f);
    glRotatef(kut, os.x, os.y, os.z);
    myDisplayObject();
    glPopMatrix();
    tt++;

    if (tt == 100 * (brojKontrolnih - 3)) tt = 0;

}

int tt2 = 0;
void myAnimateObject2() {

    vec3 tangenta = vec3(tocke_tangente[tt2].x, tocke_tangente[tt2].y, tocke_tangente[tt2].z);
    vec3 normala = cross(vec3(tocke_tangente[tt2].x, tocke_tangente[tt2].y, tocke_tangente[tt2].z), vec3(drugaDer[tt2].x, drugaDer[tt2].y, drugaDer[tt2].z));
    vec3 binormala = cross(vec3(tocke_tangente[tt2].x, tocke_tangente[tt2].y, tocke_tangente[tt2].z), normala);

    normala = normalize(normala);
    binormala = normalize(binormala);

    mat3 DCM = inverse(mat3(binormala, normala, tangenta));


    GLfloat* matrica = new GLfloat[16];

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            matrica[i * 4 + j] = DCM[j][i];
        }
    }
    matrica[3] = 0.0f;
    matrica[7] = 0.0f;
    matrica[11] = 0.0f;
    matrica[12] = 0.0f;
    matrica[13] = 0.0f;
    matrica[14] = 0.0f;
    matrica[15] = 1.0f;

    glPushMatrix();
    //glLoadIdentity();
    //gluLookAt(ociste.x, ociste.y, ociste.z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    glTranslatef(tocke_krivulje[tt2].x, tocke_krivulje[tt2].y, tocke_krivulje[tt2].z);
    glScalef(1 / 5.0f, 1 / 5.0f, 1 / 5.0f);  
    glMultMatrixf(matrica);
    if (tt2 == 0) {
        glRotatef(90, 1, 0, 0);
    }
    myDisplayObject();
    glPopMatrix();
    tt2++;

    if (tt2 == 100 * (brojKontrolnih - 3)) tt2 = 0;

    delete[] matrica;
}


void myRenderScene()
{
    glColor3f(1.0f, 0.0f, 0.0f);
    myDrawCurve();
    myAnimateObject2();

    if (kontrolni) {
        myDrawPolygon();
    }
}

void myKeyboard(unsigned char theKey, int mouseX, int mouseY)
{
    switch (theKey)
    {
    case 'a': ociste.x = ociste.x + 0.2f;
        break;
    case 'd': ociste.x = ociste.x - 0.2f;
        break;
    case 'w': ociste.y = ociste.y + 0.2f;
        break;
    case 's': ociste.y = ociste.y - 0.2f;
        break;
    case 'r': ociste.x = 0.0; ociste.y = 0.0;
        break;
    case 'k':
        if (kontrolni) {
            kontrolni = false;
        }
        else {
            kontrolni = true;
        }
        break;
    case 't':
        if (tangente) {
            tangente = false;
        }
        else {
            tangente = true;
        }
        break;
    case 27:  exit(0);
        break;
    }
    myReshape(width, height);
    glutPostRedisplay();
}


int trenutak1 = 0;
int trenutak2 = 0;
void idle() {
    trenutak1 = glutGet(GLUT_ELAPSED_TIME);
    if ((trenutak1 - trenutak2) > 10) {
        myDisplay();
        trenutak2 = trenutak1;
    }
}