// Main D3DX framework from www.directxtutorial.com (free section) 
#include <assert.h>
#include <stdio.h>

// include the basic windows header files and the Direct3D header file
#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9.h>

// define the screen resolution
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// include the Direct3D Library files
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

// global declarations
LPDIRECT3D9 d3d;
LPDIRECT3DDEVICE9 d3ddev;
LPD3DXMESH mesh = NULL;    // define the mesh pointer

// function prototypes
void initD3D(HWND hWnd);
void render_frame(void);
void cleanD3D(void);
void init_graphics(void);

struct vertex_data 
{
    D3DXVECTOR3 position;
    DWORD color;
};
#define FVF_VERTEX_DATA (D3DFVF_XYZ | D3DFVF_DIFFUSE)

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    HWND hWnd;
    WNDCLASSEX wc;

    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = "WindowClass";

    RegisterClassEx(&wc);

    hWnd = CreateWindowEx(NULL, "WindowClass", "Our Direct3D Program",
                          WS_OVERLAPPEDWINDOW, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
                          NULL, NULL, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);

    initD3D(hWnd);

    MSG msg;

    while(TRUE)
    {
        while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if(msg.message == WM_QUIT)
            break;

        render_frame();
    }

    cleanD3D();

    return msg.wParam;
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc (hWnd, message, wParam, lParam);
}

// this function initializes and prepares Direct3D for use
void initD3D(HWND hWnd)
{
    d3d = Direct3DCreate9(D3D_SDK_VERSION);

    D3DPRESENT_PARAMETERS d3dpp;

    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = hWnd;
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferWidth = SCREEN_WIDTH;
    d3dpp.BackBufferHeight = SCREEN_HEIGHT;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    d3d->CreateDevice(D3DADAPTER_DEFAULT,
                      D3DDEVTYPE_HAL,
                      hWnd,
                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                      &d3dpp,
                      &d3ddev);

    init_graphics();

    d3ddev->SetRenderState(D3DRS_LIGHTING, FALSE);    // turn off the 3D lighting
    d3ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);    // turn off culling
    d3ddev->SetRenderState(D3DRS_ZENABLE, TRUE);    // turn on the z-buffer
}

// this is the function used to render a single frame
void render_frame(void)
{
    d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
    d3ddev->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

    d3ddev->BeginScene();

    d3ddev->SetFVF(FVF_VERTEX_DATA);

    // set the view transform
    D3DXMATRIX matView;    // the view transform matrix
    D3DXMatrixLookAtLH(&matView,
    &D3DXVECTOR3 (0.0f, 8.0f, 25.0f),    // the camera position
    &D3DXVECTOR3 (0.0f, 0.0f, 0.0f),      // the look-at position
    &D3DXVECTOR3 (0.0f, 1.0f, 0.0f));    // the up direction
    d3ddev->SetTransform(D3DTS_VIEW, &matView);    // set the view transform to matView

    // set the projection transform
    D3DXMATRIX matProjection;    // the projection transform matrix
    D3DXMatrixPerspectiveFovLH(&matProjection,
                               D3DXToRadian(45),    // the horizontal field of view
                               (FLOAT)SCREEN_WIDTH / (FLOAT)SCREEN_HEIGHT, // aspect ratio
                               1.0f,   // the near view-plane
                               100.0f);    // the far view-plane
    d3ddev->SetTransform(D3DTS_PROJECTION, &matProjection); // set the projection

    // set the world transform
    static float index = 0.0f; index+=0.03f; // an ever-increasing float value
    D3DXMATRIX matRotateY;    // a matrix to store the rotation for each triangle
    D3DXMatrixRotationY(&matRotateY, index);    // the rotation matrix
    d3ddev->SetTransform(D3DTS_WORLD, &(matRotateY));    // set the world transform

    if (mesh) mesh->DrawSubset(0);     

    d3ddev->EndScene(); 

    d3ddev->Present(NULL, NULL, NULL, NULL);
}

// this is the function that cleans up Direct3D and COM
void cleanD3D(void)
{
    if (mesh) mesh->Release();
    d3ddev->Release();
    d3d->Release();
}

#define MAX_PATCHES 1000
#define POINTS_PER_PATCH 16
int num_patches = -1;
int patches[MAX_PATCHES][POINTS_PER_PATCH];

void B_patch(int ii, int a, int b, int c, int d, int e, int f, int g, int h, int i, int j, int k, int l, int m, int n, int o, int p)
{
    assert(ii < MAX_PATCHES);
    patches[ii][0] = a-1;
    patches[ii][1] = b-1;
    patches[ii][2] = c-1;
    patches[ii][3] = d-1;
    patches[ii][4] = e-1;
    patches[ii][5] = f-1;
    patches[ii][6] = g-1;
    patches[ii][7] = h-1;
    patches[ii][8] = i-1;
    patches[ii][9] = j-1;
    patches[ii][10] = k-1;
    patches[ii][11] = l-1;
    patches[ii][12] = m-1;
    patches[ii][13] = n-1;
    patches[ii][14] = o-1;
    patches[ii][15] = p-1;
    assert(POINTS_PER_PATCH==16);
}

#define MAX_POINTS 1000
int num_points = -1;
D3DXVECTOR3 points[MAX_POINTS];

void B_point(int ii, double x, double y, double z)
{
    ii--;
    assert(ii < MAX_POINTS);
    points[ii].x = x;
    /*** Y AND Z FLIPPED ***/
    points[ii].y = z;
    points[ii].z = y;
}

// BEGIN http://www.sjbaker.org/teapot/teaset.tgz

/*
 *  The file input.c  -- Juhana Kouhia, jk87377@cs.tut.fi, Oct. 25, 1991
 *
 *  Load_patch(filename, patches, vertices);
 *    char *filename; int *patches, *vertices;  
 *    A sample program to read Bezier patches in.
 *    Returns count of patches and vertices.
 *  User defined subroutines:
 *    B_patch(ii, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p);
 *      int ii, a, b, ..., p;
 *      Defines one Bezier patch with index number ii,
 *      indexes to points are in a, b, c, ..., p.
 *    B_point(ii, x, y, z);
 *      int ii; double x, y, z;
 *      Defines one point with index number ii.
 */

#include <stdio.h>

// Modified to work with g++
void Load_patch(char *filename, int *patches, int *vertices)
{
  int ii;
  float x,y,z;
  int a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p;

  FILE *fp;

  if (!(fp = fopen(filename,"r"))) {
    fprintf(stderr,"Load_patch: Can't open %s\n",filename);
    exit(1);
  }

  (void)fscanf(fp,"%i\n",patches);
  for (ii = 0; ii < *patches; ii++) {
    (void)fscanf(fp,"%i, %i, %i, %i,",&a,&b,&c,&d);
    (void)fscanf(fp,"%i, %i, %i, %i,",&e,&f,&g,&h);
    (void)fscanf(fp,"%i, %i, %i, %i,",&i,&j,&k,&l);
    (void)fscanf(fp,"%i, %i, %i, %i\n",&m,&n,&o,&p);
    B_patch(ii, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p);
  }
  (void)fscanf(fp,"%i\n",vertices);
  for (ii = 1; ii <= *vertices; ii++) {
    (void)fscanf(fp,"%f, %f, %f\n",&x,&y,&z);
    B_point(ii, (double)x,(double)y,(double)z);
  }
}

// END http://www.sjbaker.org/teapot/teaset.tgz

// this is the function that puts the 3D models into video RAM
void init_graphics(void)
{
    // load patch
    char filename[255];
    sprintf(filename,"teapot");
    Load_patch(filename,&num_patches,&num_points);
    printf("Loaded patch %s with %d patches and %d vertices.\n",
           filename,num_patches,num_points);

    // create declarator from FVF
    D3DVERTEXELEMENT9 inDecl[MAX_FVF_DECL_SIZE];
    if (!SUCCEEDED(D3DXDeclaratorFromFVF(FVF_VERTEX_DATA,inDecl)))
        assert(FALSE);    

    // create patch mesh
    LPD3DXPATCHMESH p_mesh;
    D3DXPATCHINFO info;
    info.PatchType = D3DXPATCHMESH_RECT;
    info.Degree = D3DDEGREE_CUBIC;
    info.Basis = D3DBASIS_BEZIER;

    if (!SUCCEEDED(D3DXCreatePatchMesh(&info,num_patches,num_points,0,inDecl,d3ddev,&p_mesh)))
        assert(FALSE);
    assert(p_mesh->GetControlVerticesPerPatch()==POINTS_PER_PATCH);

    // copy vertices
    LPDIRECT3DVERTEXBUFFER9 v_buffer = NULL;
    if (!SUCCEEDED(p_mesh->GetVertexBuffer(&v_buffer))) 
        assert(FALSE);
    struct vertex_data* vertex_data = NULL;
    DWORD number_of_vertices=p_mesh->GetNumVertices();
    assert(number_of_vertices==num_points);
    if (!SUCCEEDED(v_buffer->Lock(0,number_of_vertices*sizeof(struct vertex_data),(void **)&vertex_data,D3DLOCK_DISCARD)))
        assert(FALSE);
    for (int i=0; i<num_points; i++)
    {
        vertex_data[i].position.x = points[i].x;
        vertex_data[i].position.y = points[i].y;
        vertex_data[i].position.z = points[i].z;
        vertex_data[i].color = D3DCOLOR_XRGB(255,0,0);
    }
    v_buffer->Unlock();
    v_buffer->Release();    

    // copy indices
    LPDIRECT3DINDEXBUFFER9 i_buffer = NULL;
    if (!SUCCEEDED(p_mesh->GetIndexBuffer(&i_buffer))) 
        assert(FALSE);
    D3DINDEXBUFFER_DESC i_buffer_desc;
    if (!SUCCEEDED(i_buffer->GetDesc(&i_buffer_desc)))
        assert(FALSE);
    assert(i_buffer_desc.Size==num_patches*POINTS_PER_PATCH*sizeof(WORD));
    WORD* index_data = NULL;
    if (!SUCCEEDED(i_buffer->Lock(0,0,(void **)&index_data,D3DLOCK_DISCARD)))
        assert(FALSE);
    int idx=0;
    for (int i=0; i<num_patches; i++)
    {
        for (int j=0; j<POINTS_PER_PATCH; j++)
        {
            index_data[idx] = patches[i][j];
            idx++;
        }
    }
    i_buffer->Unlock();
    i_buffer->Release();    

    // create mesh for tesselation
    FLOAT fTessLevel=1.0f;
    DWORD Adaptive=FALSE;
    DWORD NumTriangles,NumVertices;
    if (!SUCCEEDED(p_mesh->GetTessSize(fTessLevel,Adaptive,&NumTriangles,&NumVertices)))
        assert(FALSE);
    if (!SUCCEEDED(D3DXCreateMeshFVF(NumTriangles,NumVertices,D3DXMESH_MANAGED,FVF_VERTEX_DATA,d3ddev,&mesh)))
        assert(FALSE);

    // tesselate
    assert(Adaptive==FALSE);
    if (!SUCCEEDED(p_mesh->Tessellate(fTessLevel,mesh)))
        assert(FALSE);
    printf("Generated tesselated mesh with %d triangles, %d vertices\n",NumTriangles,NumVertices);

    p_mesh->Release();
}