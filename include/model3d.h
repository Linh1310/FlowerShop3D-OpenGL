#ifndef MODEL3D_H
#define MODEL3D_H

// ===== Cau truc du lieu theo slide CG.4 (trang 20-26) =====
const int MODEL_MAX_V  = 128;
const int MODEL_MAX_E  = 256;
const int MODEL_MAX_F  = 64;
const int MODEL_MAX_FV = 8;

struct Point3D { float x, y, z; };            // slide: 3DPoint
struct EdgeType { int beginP, endP; };

struct WireFrame {                            // MO HINH KHUNG KET NOI
    int numVertex, numEdge;
    Point3D  vertex[MODEL_MAX_V];
    EdgeType edge[MODEL_MAX_E];
};

struct FaceType {                             // MOT MAT DA GIAC
    int   nFace;                              // so dinh cua mat
    int   indexFace[MODEL_MAX_FV];            // chi so dinh, nguoc chieu kim dong ho nhin tu ngoai vao
    float color[3];                           // them vao de to mau mat
};

struct FaceModel {                            // MO HINH MAT DA GIAC
    int numVertex, numFace;
    Point3D  vertex[MODEL_MAX_V];
    FaceType face[MODEL_MAX_F];
};

// ===== API =====
void inspectBuild(int flowerType);            // 0 = tulip, 1 = cuc
void inspectCycleMode();                      // Tat -> Khung -> Mat -> Mat+Khung -> Tat
void inspectToggleLabels();                   // bat/tat so thu tu dinh (che do Khung)
void inspectOff();
bool inspectActive();
const char* inspectModeName();
int  inspectNumVertex();
int  inspectNumEdge();
int  inspectNumFace();
void drawInspect(int flowerType, float angleY);

void inspectToggleSource();     // hoa that (Linh) <-> mo hinh danh sach
bool inspectUsingReal();

#endif