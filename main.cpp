#include <windows.h>
#include <GL/gl.h>
#include <math.h>

struct Vec4 { float x,y,z,w; };
Vec4 cubeVerts[16];
int edges[32][2];
float rotXY=0, rotZW=0;
bool dragging=false;
POINT lastPt;

void initTesseract() {
    int idx=0;
    for(int i=0;i<2;i++)
    for(int j=0;j<2;j++)
    for(int k=0;k<2;k++)
    for(int l=0;l<2;l++)
        cubeVerts[idx++]={(float)(i*2-1),(float)(j*2-1),(float)(k*2-1),(float)(l*2-1)};
    int e=0;
    for(int i=0;i<16;i++)
        for(int j=i+1;j<16;j++) {
            int diff=0;
            for(int k=0;k<4;k++) {
                float a=((float*)&cubeVerts[i])[k];
                float b=((float*)&cubeVerts[j])[k];
                if(a!=b) diff++;
            }
            if(diff==1) {
                edges[e][0]=i;
                edges[e][1]=j;
                e++;
            }
        }
}

Vec4 rotate4D(Vec4 v,float a,float b) {
    float x=v.x,y=v.y,z=v.z,w=v.w;
    float ca=cos(a),sa=sin(a);
    float cb=cos(b),sb=sin(b);
    float x1=ca*x-sa*y, y1=sa*x+ca*y;
    float z1=cb*z-sb*w, w1=sb*z+cb*w;
    return {x1,y1,z1,w1};
}

void drawTesseract() {
    glColor3f(0.7f,0.7f,0.7f);
    glBegin(GL_LINES);
    for(int i=0;i<32;i++) {
        Vec4 v1=rotate4D(cubeVerts[edges[i][0]],rotXY*0.02f,rotZW*0.02f);
        Vec4 v2=rotate4D(cubeVerts[edges[i][1]],rotXY*0.02f,rotZW*0.02f);
        float d1=4/(4-v1.w), d2=4/(4-v2.w);
        glVertex3f(v1.x*d1,v1.y*d1,v1.z*d1);
        glVertex3f(v2.x*d2,v2.y*d2,v2.z*d2);
    }
    glEnd();
}

void setupProjection(int w,int h) {
    if(h==0) h=1;
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect=(float)w/(float)h;
    float fovY=60.0f*3.14159f/180.0f;
    float f=1.0f/tan(fovY/2);
    float zNear=0.1f,zFar=100.0f;
    float proj[16]={
        f/aspect,0,0,0,
        0,f,0,0,
        0,0,(zFar+zNear)/(zNear-zFar),-1,
        0,0,(2*zFar*zNear)/(zNear-zFar),0
    };
    glMultMatrixf(proj);
}

LRESULT CALLBACK WndProc(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp) {
    switch(msg) {
    case WM_SIZE:
        setupProjection(LOWORD(lp),HIWORD(lp));
        return 0;
    case WM_LBUTTONDOWN:
        dragging=true;
        lastPt.x=LOWORD(lp);
        lastPt.y=HIWORD(lp);
        return 0;
    case WM_LBUTTONUP:
        dragging=false;
        return 0;
    case WM_MOUSEMOVE:
        if(dragging) {
            int x=LOWORD(lp),y=HIWORD(lp);
            rotXY+=(x-lastPt.x);
            rotZW+=(y-lastPt.y);
            lastPt.x=x; lastPt.y=y;
        }
        return 0;
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd,msg,wp,lp);
}

int WINAPI WinMain(HINSTANCE hInst,HINSTANCE,LPSTR,int) {
    WNDCLASS wc={0};
    wc.lpfnWndProc=WndProc;
    wc.hInstance=hInst;
    wc.lpszClassName="TesseractWin";
    RegisterClass(&wc);
    HWND hWnd=CreateWindow("TesseractWin","4D Tesseract",WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                           CW_USEDEFAULT,CW_USEDEFAULT,800,600,NULL,NULL,hInst,NULL);
    HDC hDC=GetDC(hWnd);
    PIXELFORMATDESCRIPTOR pfd={sizeof(PIXELFORMATDESCRIPTOR),1,
        PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,24,0,0,0,0,0,0,0,0,0,0,0,0,32,0,0,
        PFD_MAIN_PLANE,0,0,0,0};
    int pf=ChoosePixelFormat(hDC,&pfd);
    SetPixelFormat(hDC,pf,&pfd);
    HGLRC hRC=wglCreateContext(hDC);
    wglMakeCurrent(hDC,hRC);
    glEnable(GL_DEPTH_TEST);
    setupProjection(800,600);
    initTesseract();
    MSG msg;
    while(true) {
        while(PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {
            if(msg.message==WM_QUIT) goto end;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0,0,-8);
        drawTesseract();
        SwapBuffers(hDC);
    }
end:
    wglMakeCurrent(NULL,NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hWnd,hDC);
    return 0;
}
