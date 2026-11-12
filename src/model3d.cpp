#include <GL/freeglut.h>
#include <cmath>
#include <cstdio>
#include "model3d.h"
#include "flowers.h"

static FaceModel fm;                // danh sach dinh + danh sach mat
static WireFrame wf;                // danh sach dinh + danh sach canh
static int  mode = 0;               // 0 tat | 1 khung | 2 mat | 3 mat + khung
static bool labels = true;
static bool useReal = true;     // true = ve hoa that cua Linh
static int  builtType = -99;
static const float PI = 3.14159265f;

// ---------- Them dinh / them mat ----------
static int addVertex(float x, float y, float z) {
    fm.vertex[fm.numVertex].x = x;
    fm.vertex[fm.numVertex].y = y;
    fm.vertex[fm.numVertex].z = z;
    return fm.numVertex++;
}

static void addFace(int n, const int* idx, float r, float g, float b) {
    FaceType& f = fm.face[fm.numFace++];
    f.nFace = n;
    for (int i = 0; i < n; i++) f.indexFace[i] = idx[i];
    f.color[0] = r; f.color[1] = g; f.color[2] = b;
}

// ---------- Chau hoa: hinh non cut 8 canh ----------
static void buildPot() {
    const int N = 8;
    const float rB = 0.11f, rT = 0.16f, H = 0.22f;
    int b[N], t[N];
    for (int i = 0; i < N; i++) {
        float a = 2.0f * PI * i / N;
        b[i] = addVertex(rB * cosf(a), 0.0f, rB * sinf(a));
        t[i] = addVertex(rT * cosf(a), H,    rT * sinf(a));
    }
    for (int i = 0; i < N; i++) {                       // 8 mat ben
        int j = (i + 1) % N;
        int q[4] = { b[i], t[i], t[j], b[j] };
        float s = (i % 2) ? 0.88f : 1.0f;
        addFace(4, q, 0.80f * s, 0.45f * s, 0.30f * s);
    }
    int top[N], bot[N];
    for (int i = 0; i < N; i++) { top[i] = t[N - 1 - i]; bot[i] = b[i]; }
    addFace(N, top, 0.35f, 0.22f, 0.12f);               // mat dat
    addFace(N, bot, 0.55f, 0.30f, 0.20f);               // day chau
}

// ---------- Than: lang tru 4 mat ----------
static float buildStem() {
    const float r = 0.02f, y0 = 0.22f, y1 = 0.67f;
    int s0[4], s1[4];
    for (int i = 0; i < 4; i++) {
        float a = 2.0f * PI * i / 4;
        s0[i] = addVertex(r * cosf(a), y0, r * sinf(a));
        s1[i] = addVertex(r * cosf(a), y1, r * sinf(a));
    }
    for (int i = 0; i < 4; i++) {
        int j = (i + 1) % 4;
        int q[4] = { s0[i], s1[i], s1[j], s0[j] };
        addFace(4, q, 0.20f, 0.60f, 0.25f);
    }
    return y1;
}

// ---------- Tulip: coc 6 canh, dinh nhon ----------
static void buildTulip(float y) {
    const int N = 6;
    int base[N], mid[N];
    for (int i = 0; i < N; i++) {
        float a = 2.0f * PI * i / N;
        base[i] = addVertex(0.030f * cosf(a), y,         0.030f * sinf(a));
        mid[i]  = addVertex(0.075f * cosf(a), y + 0.08f, 0.075f * sinf(a));
    }
    int apex = addVertex(0.0f, y + 0.17f, 0.0f);
    for (int i = 0; i < N; i++) {
        int j = (i + 1) % N;
        int q[4]   = { base[i], mid[i], mid[j], base[j] };
        addFace(4, q, 0.95f, 0.45f, 0.60f);
        int tri[3] = { mid[i], apex, mid[j] };
        addFace(3, tri, 0.90f, 0.30f, 0.50f);
    }
}

// ---------- Cuc: nhuy 8 tam giac + 8 canh tam giac ----------
static void buildDaisy(float y) {
    const int N = 8;
    int c = addVertex(0.0f, y + 0.01f, 0.0f);
    int inner[N], tip[N];
    for (int i = 0; i < N; i++) {
        float a  = 2.0f * PI * i / N;
        float a2 = 2.0f * PI * (i + 0.5f) / N;
        inner[i] = addVertex(0.035f * cosf(a),  y,          0.035f * sinf(a));
        tip[i]   = addVertex(0.130f * cosf(a2), y - 0.015f, 0.130f * sinf(a2));
    }
    for (int i = 0; i < N; i++) {
        int j = (i + 1) % N;
        int d[3] = { c, inner[j], inner[i] };
        addFace(3, d, 1.00f, 0.80f, 0.10f);             // nhuy
        int p[3] = { inner[i], inner[j], tip[i] };
        addFace(3, p, 0.98f, 0.95f, 0.85f);             // canh
    }
}

// ---------- Sinh danh sach canh tu danh sach mat ----------
// Moi canh chung cua hai mat chi luu mot lan.
static void buildWireFrame() {
    wf.numVertex = fm.numVertex;
    for (int i = 0; i < fm.numVertex; i++) wf.vertex[i] = fm.vertex[i];
    wf.numEdge = 0;
    for (int f = 0; f < fm.numFace; f++) {
        const FaceType& F = fm.face[f];
        for (int k = 0; k < F.nFace; k++) {
            int a = F.indexFace[k], b = F.indexFace[(k + 1) % F.nFace];
            bool dup = false;
            for (int e = 0; e < wf.numEdge && !dup; e++) {
                if ((wf.edge[e].beginP == a && wf.edge[e].endP == b) ||
                    (wf.edge[e].beginP == b && wf.edge[e].endP == a)) dup = true;
            }
            if (!dup && wf.numEdge < MODEL_MAX_E) {
                wf.edge[wf.numEdge].beginP = a;
                wf.edge[wf.numEdge].endP   = b;
                wf.numEdge++;
            }
        }
    }
}

void inspectBuild(int flowerType) {
    fm.numVertex = 0;
    fm.numFace = 0;
    buildPot();
    float y = buildStem();
    if (flowerType == 1) buildDaisy(y); else buildTulip(y);
    buildWireFrame();
    builtType = flowerType;
}

// ---------- Ve ----------
// Phap tuyen mat theo phuong phap Newell (dung cho ca mat khong phai tam giac)
static void faceNormal(const FaceType& F, float n[3]) {
    n[0] = n[1] = n[2] = 0.0f;
    for (int k = 0; k < F.nFace; k++) {
        const Point3D& a = fm.vertex[F.indexFace[k]];
        const Point3D& b = fm.vertex[F.indexFace[(k + 1) % F.nFace]];
        n[0] += (a.y - b.y) * (a.z + b.z);
        n[1] += (a.z - b.z) * (a.x + b.x);
        n[2] += (a.x - b.x) * (a.y + b.y);
    }
    float len = sqrtf(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
    if (len > 1e-6f) { n[0] /= len; n[1] /= len; n[2] /= len; }
}

static void drawFaces() {
    for (int f = 0; f < fm.numFace; f++) {
        const FaceType& F = fm.face[f];
        float n[3];
        faceNormal(F, n);
        glColor3fv(F.color);
        glNormal3fv(n);
        glBegin(GL_POLYGON);
        for (int k = 0; k < F.nFace; k++) {
            const Point3D& p = fm.vertex[F.indexFace[k]];
            glVertex3f(p.x, p.y, p.z);
        }
        glEnd();
    }
}

static void drawEdges() {
    glBegin(GL_LINES);
    for (int e = 0; e < wf.numEdge; e++) {
        const Point3D& a = wf.vertex[wf.edge[e].beginP];
        const Point3D& b = wf.vertex[wf.edge[e].endP];
        glVertex3f(a.x, a.y, a.z);
        glVertex3f(b.x, b.y, b.z);
    }
    glEnd();
}

static void drawLabels() {
    glColor3f(0.75f, 0.0f, 0.0f);
    char s[8];
    for (int i = 0; i < wf.numVertex; i++) {
        snprintf(s, sizeof s, "%d", i + 1);             // danh so tu 1 nhu trong slide
        glRasterPos3f(wf.vertex[i].x, wf.vertex[i].y, wf.vertex[i].z);
        glutBitmapString(GLUT_BITMAP_HELVETICA_10, (const unsigned char*)s);
    }
}

static void drawRealFlower(int flowerType) {
    FlowerType ft = (flowerType == 1) ? FlowerType::DAISY : FlowerType::TULIP;

    if (mode == 2) { drawFlower(ft); return; }              // Mat

    if (mode == 3) {                                         // Mat + Khung
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.0f, 1.0f);
        drawFlower(ft);
        glDisable(GL_POLYGON_OFFSET_FILL);
    }
    glDisable(GL_LIGHTING);                                  // Khung
    glLineWidth(1.5f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    drawFlower(ft);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void drawInspect(int flowerType, float angleY) {
    if (flowerType != builtType) inspectBuild(flowerType);
    if (mode == 0) return;

    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT |
                 GL_POLYGON_BIT | GL_LIGHTING_BIT);
    glPushMatrix();

    // Thu tu ap dung len moi dinh: TI LE -> QUAY QUANH TRUC Y -> TINH TIEN
    // Goc toa do cua mo hinh nam o tam day chau, nen day chau la tam cua phep ti le va phep quay.
    glTranslatef(0.0f, 0.15f, 0.0f);
    glRotatef(angleY, 0.0f, 1.0f, 0.0f);
    glScalef(2.5f, 2.5f, 2.5f);

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

        if (useReal) {
        drawRealFlower(flowerType);
    } else {
        if (mode == 2 || mode == 3) {
            if (mode == 3) {
                glEnable(GL_POLYGON_OFFSET_FILL);
                glPolygonOffset(1.0f, 1.0f);
            }
            drawFaces();
            glDisable(GL_POLYGON_OFFSET_FILL);
        }
        if (mode == 1 || mode == 3) {
            glDisable(GL_LIGHTING);
            glLineWidth(2.0f);
            glColor3f(0.05f, 0.05f, 0.15f);
            drawEdges();
            if (mode == 1 && labels) drawLabels();
        }
    }

    glPopMatrix();
    glPopAttrib();
}

// ---------- Dieu khien / thong tin ----------
void inspectCycleMode()    { mode = (mode + 1) % 4; }
void inspectToggleLabels() { labels = !labels; }
void inspectOff()          { mode = 0; }
bool inspectActive()       { return mode != 0; }

const char* inspectModeName() {
    static const char* names[] = {
        "Tat", "Khung ket noi (WireFrame)", "Mat da giac (Polygon Mesh)", "Mat + Khung"
    };
    return names[mode];
}
int inspectNumVertex() { return fm.numVertex; }
int inspectNumEdge()   { return wf.numEdge; }
int inspectNumFace()   { return fm.numFace; }
void inspectToggleSource() { useReal = !useReal; }
bool inspectUsingReal()    { return useReal; }