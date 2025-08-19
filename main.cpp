#include <windows.h>
#include <GL/gl.h>
#include <math.h>

float rotX = 20.0f, rotY = -30.0f;
bool dragging = false;
POINT lastPt;

void drawCube() {
    glBegin(GL_QUADS);
    glColor3f(0.5f, 0.5f, 0.5f);
    glVertex3f(-1, -1,  1);
    glVertex3f( 1, -1,  1);
    glVertex3f( 1,  1,  1);
    glVertex3f(-1,  1,  1);
    glVertex3f(-1, -1, -1);
    glVertex3f(-1,  1, -1);
    glVertex3f( 1,  1, -1);
    glVertex3f( 1, -1, -1);
    glVertex3f(-1, -1, -1);
    glVertex3f(-1, -1,  1);
    glVertex3f(-1,  1,  1);
    glVertex3f(-1,  1, -1);
    glVertex3f( 1, -1, -1);
    glVertex3f( 1,  1, -1);
    glVertex3f( 1,  1,  1);
    glVertex3f( 1, -1,  1);
    glVertex3f(-1,  1, -1);
    glVertex3f(-1,  1,  1);
    glVertex3f( 1,  1,  1);
    glVertex3f( 1,  1, -1);
    glVertex3f(-1, -1, -1);
    glVertex3f( 1, -1, -1);
    glVertex3f( 1, -1,  1);
    glVertex3f(-1, -1,  1);
    glEnd();
}

void setupProjection(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = (float)w / (float)h;
    float fovY = 45.0f * 3.14159f / 180.0f;
    float f = 1.0f / tan(fovY / 2);
    float zNear = 0.1f, zFar = 100.0f;
    float proj[16] = {
        f/aspect, 0, 0, 0,
        0, f, 0, 0,
        0, 0, (zFar+zNear)/(zNear-zFar), -1,
        0, 0, (2*zFar*zNear)/(zNear-zFar), 0
    };
    glMultMatrixf(proj);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_SIZE:
        setupProjection(LOWORD(lParam), HIWORD(lParam));
        return 0;
    case WM_LBUTTONDOWN:
        dragging = true;
        lastPt.x = LOWORD(lParam);
        lastPt.y = HIWORD(lParam);
        return 0;
    case WM_LBUTTONUP:
        dragging = false;
        return 0;
    case WM_MOUSEMOVE:
        if (dragging) {
            int x = LOWORD(lParam), y = HIWORD(lParam);
            rotY += (x - lastPt.x) * 0.5f;
            rotX += (y - lastPt.y) * 0.5f;
            lastPt.x = x;
            lastPt.y = y;
        }
        return 0;
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) {
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = "CubeWinClass";
    RegisterClass(&wc);
    HWND hWnd = CreateWindow("CubeWinClass", "Gray Cube",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInst, NULL);
    HDC hDC = GetDC(hWnd);
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR), 1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA, 24,
        0,0,0,0,0,0,
        0,0,0,0,0,0,0,
        32,0,0,
        PFD_MAIN_PLANE,0,0,0,0
    };
    int pf = ChoosePixelFormat(hDC, &pfd);
    SetPixelFormat(hDC, pf, &pfd);
    HGLRC hRC = wglCreateContext(hDC);
    wglMakeCurrent(hDC, hRC);
    glEnable(GL_DEPTH_TEST);
    setupProjection(800, 600);
    MSG msg;
    while (true) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) goto end;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0, 0, -5);
        glRotatef(rotX, 1, 0, 0);
        glRotatef(rotY, 0, 1, 0);
        drawCube();
        SwapBuffers(hDC);
    }
end:
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hWnd, hDC);
    return 0;
}
