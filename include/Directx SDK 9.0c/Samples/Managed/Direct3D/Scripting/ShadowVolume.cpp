//--------------------------------------------------------------------------------------
// File: ShadowVolume.cpp
//
// Starting point for new Direct3D applications
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma unmanaged
#include "dxstdafx.h"
#include "resource.h"

//#define DEBUG_VS   // Uncomment this line to debug vertex shaders 
//#define DEBUG_PS   // Uncomment this line to debug pixel shaders 

// Script functions
HRESULT InitializeScriptEngine();
HRESULT LoadScript(LPWSTR pScriptFileName);
HRESULT UpdatePlayerPosition(double appTime, float __nogc* pX, float __nogc* pY, float __nogc* pZ);
HRESULT UpdatePlayerRotationX(double appTime, D3DXMATRIX __nogc* pMatrix);
HRESULT UpdatePlayerRotationY(double appTime, D3DXMATRIX __nogc* pMatrix);
HRESULT UpdatePlayerRotationZ(double appTime, D3DXMATRIX __nogc* pMatrix);

#define DEFMESHFILENAME L"dwarf\\dwarf.x"
#define MAX_NUM_LIGHTS 6
#define ADJACENCY_EPSILON 0.0001f
#define EXTRUDE_EPSILON 0.1f
#define AMBIENT 0.10f

enum RENDER_TYPE { RENDERTYPE_SCENE, RENDERTYPE_SHADOWVOLUME, RENDERTYPE_COMPLEXITY };

D3DXVECTOR4 g_vShadowColor[MAX_NUM_LIGHTS] =
{
    D3DXVECTOR4( 0.0f, 1.0f, 0.0f, 0.2f ),
    D3DXVECTOR4( 1.0f, 1.0f, 0.0f, 0.2f ),
    D3DXVECTOR4( 1.0f, 0.0f, 0.0f, 0.2f ),
    D3DXVECTOR4( 0.0f, 0.0f, 1.0f, 0.2f ),
    D3DXVECTOR4( 1.0f, 0.0f, 1.0f, 0.2f ),
    D3DXVECTOR4( 0.0f, 1.0f, 1.0f, 0.2f )
};


struct LIGHTINITDATA
{
    D3DXVECTOR3 Position;
    D3DXVECTOR4 Color;
public:
    LIGHTINITDATA() { }
    LIGHTINITDATA( D3DXVECTOR3 P, D3DXVECTOR4 C )
    {
        Position = P;
        Color = C;
    }
};
LIGHTINITDATA g_LightInit[MAX_NUM_LIGHTS] =
{
    LIGHTINITDATA( D3DXVECTOR3( -2.0f, 3.0f, -3.0f ), D3DXVECTOR4( 11.0f, 11.0f, 11.0f, 1.0f ) ),
#if MAX_NUM_LIGHTS > 1
    LIGHTINITDATA(  D3DXVECTOR3( 2.0f, 3.0f, -3.0f ), D3DXVECTOR4( 11.0f, 11.0f, 11.0f, 1.0f ) ),
#endif
#if MAX_NUM_LIGHTS > 2
    LIGHTINITDATA(  D3DXVECTOR3( -2.0f, 3.0f, 3.0f ), D3DXVECTOR4( 11.0f, 11.0f, 11.0f, 1.0f ) ),
#endif
#if MAX_NUM_LIGHTS > 3
    LIGHTINITDATA(  D3DXVECTOR3( 2.0f, 3.0f, 3.0f ), D3DXVECTOR4( 11.0f, 11.0f, 11.0f, 1.0f ) ),
#endif
#if MAX_NUM_LIGHTS > 4
    LIGHTINITDATA(  D3DXVECTOR3( -2.0f, 3.0f, 0.0f ), D3DXVECTOR4( 15.0f, 0.0f, 0.0f, 1.0f ) ),
#endif
#if MAX_NUM_LIGHTS > 5
    LIGHTINITDATA(  D3DXVECTOR3( 2.0f, 3.0f, 0.0f ), D3DXVECTOR4( 0.0f, 0.0f, 15.0f, 1.0f ) )
#endif
};

struct POSTPROCVERT
{
    float x, y, z;
    float rhw;

    const static D3DVERTEXELEMENT9 Decl[2];
};

const D3DVERTEXELEMENT9 POSTPROCVERT::Decl[2] =
{
    { 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0 },
    D3DDECL_END()
};


struct SHADOWVERT
{
    D3DXVECTOR3 Position;
    D3DXVECTOR3 Normal;

    const static D3DVERTEXELEMENT9 Decl[3];
};

const D3DVERTEXELEMENT9 SHADOWVERT::Decl[3] =
{
    { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
    { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
    D3DDECL_END()
};


struct MESHVERT
{
    D3DXVECTOR3 Position;
    D3DXVECTOR3 Normal;
    D3DXVECTOR2 Tex;

    const static D3DVERTEXELEMENT9 Decl[4];
};

const D3DVERTEXELEMENT9 MESHVERT::Decl[4] =
{
    { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
    { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0 },
    { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
    D3DDECL_END()
};


struct CEdgeMapping
{
    int m_anOldEdge[2];  // vertex index of the original edge
    int m_aanNewEdge[2][2]; // vertex indexes of the new edge
                            // First subscript = index of the new edge
                            // Second subscript = index of the vertex for the edge

public:
    CEdgeMapping()
    {
        FillMemory( m_anOldEdge, sizeof(m_anOldEdge), -1 );
        FillMemory( m_aanNewEdge, sizeof(m_aanNewEdge), -1 );
    }
};


struct CLight
{
    D3DXVECTOR3 m_Position;  // Light position
    D3DXVECTOR4 m_Color;     // Light color
    D3DXMATRIXA16 m_mWorld;  // World transform
};


//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
ID3DXFont*              g_pFont = NULL;         // Font for drawing text
ID3DXSprite*            g_pTextSprite = NULL;   // Sprite for batching draw text calls
ID3DXEffect*            g_pEffect = NULL;       // D3DX effect interface
IDirect3DTexture9*      g_pDefaultTex = NULL;   // Default texture for faces without one
IDirect3DVertexDeclaration9* g_pMeshDecl = NULL; // Vertex declaration for the meshes
IDirect3DVertexDeclaration9* g_pShadowDecl = NULL;// Vertex declaration for the shadow volume
IDirect3DVertexDeclaration9* g_pPProcDecl = NULL;// Vertex declaration for post-process quad rendering
CFirstPersonCamera      g_Camera;               // Viewer's camera
CModelViewerCamera      g_MCamera;              // Camera for mesh control
CModelViewerCamera      g_LCamera;              // Camera for easy light movement control
D3DXHANDLE              g_hRenderShadow;        // Technique handle for rendering shadow
D3DXHANDLE              g_hShowShadow;          // Technique handle for showing shadow volume
D3DXHANDLE              g_hRenderScene;         // Technique handle for rendering scene
D3DXMATRIXA16           g_mWorldScaling;        // Scaling to apply to mesh
bool                    g_bShowHelp = true;     // If true, it renders the UI control text
bool                    g_bShowShadowVolume = false;  // Whether the shadow volume is visibly shown.
RENDER_TYPE             g_RenderType = RENDERTYPE_SCENE;  // Type of rendering to perform
bool                    g_bModelMovementMode = false;  // If true, camera movement keys move the model instead of the camera.
int                     g_nNumLights = 2;       // Number of active lights
CDXUTMesh               g_Background[2];        // Background meshes
D3DXMATRIXA16           g_mWorldBack[2];        // Additional scaling and translation for background meshes
int                     g_nCurrBackground = 0;  // Current background mesh
CDXUTMesh               g_LightMesh;            // Mesh to represent the light source
CDXUTMesh               g_Mesh;                 // The mesh object
ID3DXMesh*              g_pShadowMesh = NULL;   // The shadow volume mesh
CDXUTDialog             g_HUD;                  // dialog for standard controls
CDXUTDialog             g_SampleUI;             // dialog for sample specific controls
D3DXMATRIXA16           g_mProjection;
CLight                  g_aLights[MAX_NUM_LIGHTS];  // Light objects
bool                    g_bLeftButtonDown = false;
bool                    g_bRightButtonDown = false;
bool                    g_bMiddleButtonDown = false;

//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           3
#define IDC_CHANGEDEVICE        4
#define IDC_CHANGESCRIPT        5


//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
bool    CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed );
void    CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps );
void    CALLBACK ModifySettingsDlg( CDXUTDialog* pSettingsDialog );
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc );
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc );
void    CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime );
void    CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime );
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing );
void    CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown  );
void    CALLBACK MouseProc( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta, int xPos, int yPos );
void    CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl );
void    CALLBACK OnLostDevice();
void    CALLBACK OnDestroyDevice();

void    InitApp();
HRESULT LoadMesh( IDirect3DDevice9* pd3dDevice, WCHAR* strFileName, ID3DXMesh** ppMesh );
void    RenderText();



//--------------------------------------------------------------------------------------
// Takes an array of CEdgeMapping objects, then returns an index for the edge in the
// table if such entry exists, or returns an index at which a new entry for the edge
// can be written.
// nV1 and nV2 are the vertex indexes for the old edge.
// nCount is the number of elements in the array.
// The function returns -1 if an available entry cannot be found.  In reality,
// this should never happens as we should have allocated enough memory.
int FindEdgeInMappingTable( int nV1, int nV2, CEdgeMapping *pMapping, int nCount )
{
    for( int i = 0; i < nCount; ++i )
    {
        // If both vertex indexes of the old edge in mapping entry are -1, then
        // we have searched every valid entry without finding a match.  Return
        // this index as a newly created entry.
        if( ( pMapping[i].m_anOldEdge[0] == -1 && pMapping[i].m_anOldEdge[1] == -1 ) ||

            // Or if we find a match, return the index.
            ( pMapping[i].m_anOldEdge[1] == nV1 && pMapping[i].m_anOldEdge[0] == nV2 ) )
        {
            return i;
        }
    }

    return -1;  // We should never reach this line
}


//--------------------------------------------------------------------------------------
// Takes a mesh and generate a new mesh from it that contains the degenerate invisible
// quads for shadow volume extrusion.
HRESULT GenerateShadowMesh( IDirect3DDevice9 *pd3dDevice, ID3DXMesh *pMesh, ID3DXMesh **ppOutMesh )
{
    HRESULT hr = S_OK;
    ID3DXMesh *pInputMesh;

    if( !ppOutMesh )
        return E_INVALIDARG;
    *ppOutMesh = NULL;

    // Convert the input mesh to a format same as the output mesh using 32-bit index.
    hr = pMesh->CloneMesh( D3DXMESH_32BIT, SHADOWVERT::Decl, pd3dDevice, &pInputMesh );
    if( FAILED( hr ) )
        return hr;

    DXUTTRACE( L"Input mesh has %u vertices, %u faces\n", pInputMesh->GetNumVertices(), pInputMesh->GetNumFaces() );

    // Generate adjacency information
    DWORD *pdwAdj = new DWORD[3 * pInputMesh->GetNumFaces()];
    DWORD *pdwPtRep = new DWORD[pInputMesh->GetNumVertices()];
    if( !pdwAdj || !pdwPtRep )
    {
        delete[] pdwAdj; delete[] pdwPtRep;
        pInputMesh->Release();
        return E_OUTOFMEMORY;
    }

    hr = pInputMesh->GenerateAdjacency( ADJACENCY_EPSILON, pdwAdj );
    if( FAILED( hr ) )
    {
        delete[] pdwAdj; delete[] pdwPtRep;
        pInputMesh->Release();
        return hr;
    }

    pInputMesh->ConvertAdjacencyToPointReps( pdwAdj, pdwPtRep );
    delete[] pdwAdj;

    SHADOWVERT *pVBData = NULL;
    DWORD *pdwIBData = NULL;

    pInputMesh->LockVertexBuffer( 0, (LPVOID*)&pVBData );
    pInputMesh->LockIndexBuffer( 0, (LPVOID*)&pdwIBData );

    if( pVBData && pdwIBData )
    {
        // Maximum number of unique edges = Number of faces * 3
        DWORD dwNumEdges = pInputMesh->GetNumFaces() * 3;
        CEdgeMapping *pMapping = new CEdgeMapping[dwNumEdges];
        if( pMapping )
        {
            int nNumMaps = 0;  // Number of entries that exist in pMapping

            // Create a new mesh
            ID3DXMesh *pNewMesh;
            hr = D3DXCreateMesh( pInputMesh->GetNumFaces() + dwNumEdges * 2,
                                 pInputMesh->GetNumFaces() * 3,
                                 D3DXMESH_32BIT,
                                 SHADOWVERT::Decl,
                                 pd3dDevice,
                                 &pNewMesh );
            if( SUCCEEDED( hr ) )
            {
                SHADOWVERT *pNewVBData = NULL;
                DWORD *pdwNewIBData = NULL;

                pNewMesh->LockVertexBuffer( 0, (LPVOID*)&pNewVBData );
                pNewMesh->LockIndexBuffer( 0, (LPVOID*)&pdwNewIBData );

                // nNextIndex is the array index in IB that the next vertex index value
                // will be store at.
                int nNextIndex = 0;

                if( pNewVBData && pdwNewIBData )
                {
                    ZeroMemory( pNewVBData, pNewMesh->GetNumVertices() * pNewMesh->GetNumBytesPerVertex() );
                    ZeroMemory( pdwNewIBData, sizeof(DWORD) * pNewMesh->GetNumFaces() * 3 );

                    // pNextOutVertex is the location to write the next
                    // vertex to.
                    SHADOWVERT *pNextOutVertex = pNewVBData;

                    // Iterate through the faces.  For each face, output new
                    // vertices and face in the new mesh, and write its edges
                    // to the mapping table.

                    for( UINT f = 0; f < pInputMesh->GetNumFaces(); ++f )
                    {
                        // Copy the vertex data for all 3 vertices
                        CopyMemory( pNextOutVertex, pVBData + pdwIBData[f * 3], sizeof(SHADOWVERT) );
                        CopyMemory( pNextOutVertex + 1, pVBData + pdwIBData[f * 3 + 1], sizeof(SHADOWVERT) );
                        CopyMemory( pNextOutVertex + 2, pVBData + pdwIBData[f * 3 + 2], sizeof(SHADOWVERT) );

                        // Write out the face
                        pdwNewIBData[nNextIndex++] = f * 3;
                        pdwNewIBData[nNextIndex++] = f * 3 + 1;
                        pdwNewIBData[nNextIndex++] = f * 3 + 2;

                        // Compute the face normal and assign it to
                        // the normals of the vertices.
                        D3DXVECTOR3 v1, v2;  // v1 and v2 are the edge vectors of the face
                        D3DXVECTOR3 vNormal;
                        v1 = *(D3DXVECTOR3*)(pNextOutVertex + 1) - *(D3DXVECTOR3*)pNextOutVertex;
                        v2 = *(D3DXVECTOR3*)(pNextOutVertex + 2) - *(D3DXVECTOR3*)(pNextOutVertex + 1);
                        D3DXVec3Cross( &vNormal, &v1, &v2 );
                        D3DXVec3Normalize( &vNormal, &vNormal );

                        pNextOutVertex->Normal = vNormal;
                        (pNextOutVertex + 1)->Normal = vNormal;
                        (pNextOutVertex + 2)->Normal = vNormal;

                        pNextOutVertex += 3;

                        // Add the face's edges to the edge mapping table

                        // Edge 1
                        int nIndex;
                        int nVertIndex[3] = { pdwPtRep[pdwIBData[f * 3]],
                                              pdwPtRep[pdwIBData[f * 3 + 1]],
                                              pdwPtRep[pdwIBData[f * 3 + 2]] };
                        nIndex = FindEdgeInMappingTable( nVertIndex[0], nVertIndex[1], pMapping, dwNumEdges );

                        // If error, we are not able to proceed, so abort.
                        if( -1 == nIndex )
                        {
                            hr = E_INVALIDARG;
                            goto cleanup;
                        }

                        if( pMapping[nIndex].m_anOldEdge[0] == -1 && pMapping[nIndex].m_anOldEdge[1] == -1 )
                        {
                            // No entry for this edge yet.  Initialize one.
                            pMapping[nIndex].m_anOldEdge[0] = nVertIndex[0];
                            pMapping[nIndex].m_anOldEdge[1] = nVertIndex[1];
                            pMapping[nIndex].m_aanNewEdge[0][0] = f * 3;
                            pMapping[nIndex].m_aanNewEdge[0][1] = f * 3 + 1;

                            ++nNumMaps;
                        } else
                        {
                            // An entry is found for this edge.  Create
                            // a quad and output it.
                            assert( nNumMaps > 0 );

                            pMapping[nIndex].m_aanNewEdge[1][0] = f * 3;      // For clarity
                            pMapping[nIndex].m_aanNewEdge[1][1] = f * 3 + 1;

                            // First triangle
                            pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][1];
                            pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][0];
                            pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][0];

                            // Second triangle
                            pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][1];
                            pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][0];
                            pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][0];

                            // pMapping[nIndex] is no longer needed. Copy the last map entry
                            // over and decrement the map count.

                            pMapping[nIndex] = pMapping[nNumMaps-1];
                            FillMemory( &pMapping[nNumMaps-1], sizeof( pMapping[nNumMaps-1] ), 0xFF );
                            --nNumMaps;
                        }

                        // Edge 2
                        nIndex = FindEdgeInMappingTable( nVertIndex[1], nVertIndex[2], pMapping, dwNumEdges );

                        // If error, we are not able to proceed, so abort.
                        if( -1 == nIndex )
                        {
                            hr = E_INVALIDARG;
                            goto cleanup;
                        }

                        if( pMapping[nIndex].m_anOldEdge[0] == -1 && pMapping[nIndex].m_anOldEdge[1] == -1 )
                        {
                            pMapping[nIndex].m_anOldEdge[0] = nVertIndex[1];
                            pMapping[nIndex].m_anOldEdge[1] = nVertIndex[2];
                            pMapping[nIndex].m_aanNewEdge[0][0] = f * 3 + 1;
                            pMapping[nIndex].m_aanNewEdge[0][1] = f * 3 + 2;

                            ++nNumMaps;
                        } else
                        {
                            // An entry is found for this edge.  Create
                            // a quad and output it.
                            assert( nNumMaps > 0 );

                            pMapping[nIndex].m_aanNewEdge[1][0] = f * 3 + 1;
                            pMapping[nIndex].m_aanNewEdge[1][1] = f * 3 + 2;

                            // First triangle
                            pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][1];
                            pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][0];
                            pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][0];

                            // Second triangle
                            pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][1];
                            pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][0];
                            pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][0];

                            // pMapping[nIndex] is no longer needed. Copy the last map entry
                            // over and decrement the map count.

                            pMapping[nIndex] = pMapping[nNumMaps-1];
                            FillMemory( &pMapping[nNumMaps-1], sizeof( pMapping[nNumMaps-1] ), 0xFF );
                            --nNumMaps;
                        }

                        // Edge 3
                        nIndex = FindEdgeInMappingTable( nVertIndex[2], nVertIndex[0], pMapping, dwNumEdges );

                        // If error, we are not able to proceed, so abort.
                        if( -1 == nIndex )
                        {
                            hr = E_INVALIDARG;
                            goto cleanup;
                        }

                        if( pMapping[nIndex].m_anOldEdge[0] == -1 && pMapping[nIndex].m_anOldEdge[1] == -1 )
                        {
                            pMapping[nIndex].m_anOldEdge[0] = nVertIndex[2];
                            pMapping[nIndex].m_anOldEdge[1] = nVertIndex[0];
                            pMapping[nIndex].m_aanNewEdge[0][0] = f * 3 + 2;
                            pMapping[nIndex].m_aanNewEdge[0][1] = f * 3;

                            ++nNumMaps;
                        } else
                        {
                            // An entry is found for this edge.  Create
                            // a quad and output it.
                            assert( nNumMaps > 0 );

                            pMapping[nIndex].m_aanNewEdge[1][0] = f * 3 + 2;
                            pMapping[nIndex].m_aanNewEdge[1][1] = f * 3;

                            // First triangle
                            pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][1];
                            pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][0];
                            pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][0];

                            // Second triangle
                            pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][1];
                            pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][0];
                            pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][0];

                            // pMapping[nIndex] is no longer needed. Copy the last map entry
                            // over and decrement the map count.

                            pMapping[nIndex] = pMapping[nNumMaps-1];
                            FillMemory( &pMapping[nNumMaps-1], sizeof( pMapping[nNumMaps-1] ), 0xFF );
                            --nNumMaps;
                        }
                    }

                    // Now the entries in the edge mapping table represent
                    // non-shared edges.  What they mean is that the original
                    // mesh has openings (holes), so we attempt to patch them.
                    // First we need to recreate our mesh with a larger vertex
                    // and index buffers so the patching geometry could fit.

                    DXUTTRACE( L"Faces to patch: %d\n", nNumMaps );

                    // Create a mesh with large enough vertex and
                    // index buffers.

                    SHADOWVERT *pPatchVBData = NULL;
                    DWORD *pdwPatchIBData = NULL;

                    ID3DXMesh *pPatchMesh = NULL;
                    // Make enough room in IB for the face and up to 3 quads for each patching face
                    hr = D3DXCreateMesh( nNextIndex / 3 + nNumMaps * 7,
                                         ( pInputMesh->GetNumFaces() + nNumMaps ) * 3,
                                         D3DXMESH_32BIT,
                                         SHADOWVERT::Decl,
                                         pd3dDevice,
                                         &pPatchMesh );

                    if( FAILED( hr ) )
                        goto cleanup;

                    hr = pPatchMesh->LockVertexBuffer( 0, (LPVOID*)&pPatchVBData );
                    if( SUCCEEDED( hr ) )
                        hr = pPatchMesh->LockIndexBuffer( 0, (LPVOID*)&pdwPatchIBData );

                    if( pPatchVBData && pdwPatchIBData )
                    {
                        ZeroMemory( pPatchVBData, sizeof(SHADOWVERT) * ( pInputMesh->GetNumFaces() + nNumMaps ) * 3 );
                        ZeroMemory( pdwPatchIBData, sizeof(DWORD) * ( nNextIndex + 3 * nNumMaps * 7 ) );

                        // Copy the data from one mesh to the other

                        CopyMemory( pPatchVBData, pNewVBData, sizeof(SHADOWVERT) * pInputMesh->GetNumFaces() * 3 );
                        CopyMemory( pdwPatchIBData, pdwNewIBData, sizeof(DWORD) * nNextIndex );
                    } else
                    {
                        // Some serious error is preventing us from locking.
                        // Abort and return error.

                        pPatchMesh->Release();
                        goto cleanup;
                    }

                    // Replace pNewMesh with the updated one.  Then the code
                    // can continue working with the pNewMesh pointer.

                    pNewMesh->UnlockVertexBuffer();
                    pNewMesh->UnlockIndexBuffer();
                    pNewVBData = pPatchVBData;
                    pdwNewIBData = pdwPatchIBData;
                    pNewMesh->Release();
                    pNewMesh = pPatchMesh;

                    // Now, we iterate through the edge mapping table and
                    // for each shared edge, we generate a quad.
                    // For each non-shared edge, we patch the opening
                    // with new faces.

                    // nNextVertex is the index of the next vertex.
                    int nNextVertex = pInputMesh->GetNumFaces() * 3;

                    for( int i = 0; i < nNumMaps; ++i )
                    {
                        if( pMapping[i].m_anOldEdge[0] != -1 &&
                            pMapping[i].m_anOldEdge[1] != -1 )
                        {
                            // If the 2nd new edge indexes is -1,
                            // this edge is a non-shared one.
                            // We patch the opening by creating new
                            // faces.
                            if( pMapping[i].m_aanNewEdge[1][0] == -1 ||  // must have only one new edge
                                pMapping[i].m_aanNewEdge[1][1] == -1 )
                            {
                                // Find another non-shared edge that
                                // shares a vertex with the current edge.
                                for( int i2 = i + 1; i2 < nNumMaps; ++i2 )
                                {
                                    if( pMapping[i2].m_anOldEdge[0] != -1 &&       // must have a valid old edge
                                        pMapping[i2].m_anOldEdge[1] != -1 &&
                                        ( pMapping[i2].m_aanNewEdge[1][0] == -1 || // must have only one new edge
                                        pMapping[i2].m_aanNewEdge[1][1] == -1 ) )
                                    {
                                        int nVertShared = 0;
                                        if( pMapping[i2].m_anOldEdge[0] == pMapping[i].m_anOldEdge[1] )
                                            ++nVertShared;
                                        if( pMapping[i2].m_anOldEdge[1] == pMapping[i].m_anOldEdge[0] )
                                            ++nVertShared;

                                        if( 2 == nVertShared )
                                        {
                                            // These are the last two edges of this particular
                                            // opening. Mark this edge as shared so that a degenerate
                                            // quad can be created for it.

                                            pMapping[i2].m_aanNewEdge[1][0] = pMapping[i].m_aanNewEdge[0][0];
                                            pMapping[i2].m_aanNewEdge[1][1] = pMapping[i].m_aanNewEdge[0][1];
                                            break;
                                        }
                                        else
                                        if( 1 == nVertShared )
                                        {
                                            // nBefore and nAfter tell us which edge comes before the other.
                                            int nBefore, nAfter;
                                            if( pMapping[i2].m_anOldEdge[0] == pMapping[i].m_anOldEdge[1] )
                                            {
                                                nBefore = i;
                                                nAfter = i2;
                                            } else
                                            {
                                                nBefore = i2;
                                                nAfter = i;
                                            }

                                            // Found such an edge. Now create a face along with two
                                            // degenerate quads from these two edges.

                                            pNewVBData[nNextVertex] = pNewVBData[pMapping[nAfter].m_aanNewEdge[0][1]];
                                            pNewVBData[nNextVertex+1] = pNewVBData[pMapping[nBefore].m_aanNewEdge[0][1]];
                                            pNewVBData[nNextVertex+2] = pNewVBData[pMapping[nBefore].m_aanNewEdge[0][0]];
                                            // Recompute the normal
                                            D3DXVECTOR3 v1 = pNewVBData[nNextVertex+1].Position - pNewVBData[nNextVertex].Position;
                                            D3DXVECTOR3 v2 = pNewVBData[nNextVertex+2].Position - pNewVBData[nNextVertex+1].Position;
                                            D3DXVec3Normalize( &v1, &v1 );
                                            D3DXVec3Normalize( &v2, &v2 );
                                            D3DXVec3Cross( &pNewVBData[nNextVertex].Normal, &v1, &v2 );
                                            pNewVBData[nNextVertex+1].Normal = pNewVBData[nNextVertex+2].Normal = pNewVBData[nNextVertex].Normal;

                                            pdwNewIBData[nNextIndex] = nNextVertex;
                                            pdwNewIBData[nNextIndex+1] = nNextVertex + 1;
                                            pdwNewIBData[nNextIndex+2] = nNextVertex + 2;

                                            // 1st quad

                                            pdwNewIBData[nNextIndex+3] = pMapping[nBefore].m_aanNewEdge[0][1];
                                            pdwNewIBData[nNextIndex+4] = pMapping[nBefore].m_aanNewEdge[0][0];
                                            pdwNewIBData[nNextIndex+5] = nNextVertex + 1;

                                            pdwNewIBData[nNextIndex+6] = nNextVertex + 2;
                                            pdwNewIBData[nNextIndex+7] = nNextVertex + 1;
                                            pdwNewIBData[nNextIndex+8] = pMapping[nBefore].m_aanNewEdge[0][0];

                                            // 2nd quad

                                            pdwNewIBData[nNextIndex+9] = pMapping[nAfter].m_aanNewEdge[0][1];
                                            pdwNewIBData[nNextIndex+10] = pMapping[nAfter].m_aanNewEdge[0][0];
                                            pdwNewIBData[nNextIndex+11] = nNextVertex;

                                            pdwNewIBData[nNextIndex+12] = nNextVertex + 1;
                                            pdwNewIBData[nNextIndex+13] = nNextVertex;
                                            pdwNewIBData[nNextIndex+14] = pMapping[nAfter].m_aanNewEdge[0][0];

                                            // Modify mapping entry i2 to reflect the third edge
                                            // of the newly added face.

                                            if( pMapping[i2].m_anOldEdge[0] == pMapping[i].m_anOldEdge[1] )
                                            {
                                                pMapping[i2].m_anOldEdge[0] = pMapping[i].m_anOldEdge[0];
                                            } else
                                            {
                                                pMapping[i2].m_anOldEdge[1] = pMapping[i].m_anOldEdge[1];
                                            }
                                            pMapping[i2].m_aanNewEdge[0][0] = nNextVertex + 2;
                                            pMapping[i2].m_aanNewEdge[0][1] = nNextVertex;

                                            // Update next vertex/index positions

                                            nNextVertex += 3;
                                            nNextIndex += 15;

                                            break;
                                        }
                                    }
                                }
                            } else
                            {
                                // This is a shared edge.  Create the degenerate quad.

                                // First triangle
                                pdwNewIBData[nNextIndex++] = pMapping[i].m_aanNewEdge[0][1];
                                pdwNewIBData[nNextIndex++] = pMapping[i].m_aanNewEdge[0][0];
                                pdwNewIBData[nNextIndex++] = pMapping[i].m_aanNewEdge[1][0];

                                // Second triangle
                                pdwNewIBData[nNextIndex++] = pMapping[i].m_aanNewEdge[1][1];
                                pdwNewIBData[nNextIndex++] = pMapping[i].m_aanNewEdge[1][0];
                                pdwNewIBData[nNextIndex++] = pMapping[i].m_aanNewEdge[0][0];
                            }
                        }
                    }
                }

cleanup:;
                if( pNewVBData )
                {
                    pNewMesh->UnlockVertexBuffer();
                    pNewVBData = NULL;
                }
                if( pdwNewIBData )
                {
                    pNewMesh->UnlockIndexBuffer();
                    pdwNewIBData = NULL;
                }

                if( SUCCEEDED( hr ) )
                {
                    // At this time, the output mesh may have an index buffer
                    // bigger than what is actually needed, so we create yet
                    // another mesh with the exact IB size that we need and
                    // output it.  This mesh also uses 16-bit index if
                    // 32-bit is not necessary.

                    DXUTTRACE( L"Shadow volume has %u vertices, %u faces.\n", ( pInputMesh->GetNumFaces() + nNumMaps ) * 3, nNextIndex / 3 );

                    bool bNeed32Bit = ( pInputMesh->GetNumFaces() + nNumMaps ) * 3 > 65535;
                    ID3DXMesh *pFinalMesh;
                    hr = D3DXCreateMesh( nNextIndex / 3,  // Exact number of faces
                                         ( pInputMesh->GetNumFaces() + nNumMaps ) * 3,
                                         D3DXMESH_WRITEONLY | ( bNeed32Bit ? D3DXMESH_32BIT : 0 ),
                                         SHADOWVERT::Decl,
                                         pd3dDevice,
                                         &pFinalMesh );
                    if( SUCCEEDED( hr ) )
                    {
                        pNewMesh->LockVertexBuffer( 0, (LPVOID*)&pNewVBData );
                        pNewMesh->LockIndexBuffer( 0, (LPVOID*)&pdwNewIBData );

                        SHADOWVERT *pFinalVBData = NULL;
                        WORD *pwFinalIBData = NULL;

                        pFinalMesh->LockVertexBuffer( 0, (LPVOID*)&pFinalVBData );
                        pFinalMesh->LockIndexBuffer( 0, (LPVOID*)&pwFinalIBData );

                        if( pNewVBData && pdwNewIBData && pFinalVBData && pwFinalIBData )
                        {
                            CopyMemory( pFinalVBData, pNewVBData, sizeof(SHADOWVERT) * ( pInputMesh->GetNumFaces() + nNumMaps ) * 3 );

                            if( bNeed32Bit )
                                CopyMemory( pwFinalIBData, pdwNewIBData, sizeof(DWORD) * nNextIndex );
                            else
                            {
                                for( int i = 0; i < nNextIndex; ++i )
                                    pwFinalIBData[i] = (WORD)pdwNewIBData[i];
                            }
                        }

                        if( pNewVBData )
                            pNewMesh->UnlockVertexBuffer();
                        if( pdwNewIBData )
                            pNewMesh->UnlockIndexBuffer();
                        if( pFinalVBData )
                            pFinalMesh->UnlockVertexBuffer();
                        if( pwFinalIBData )
                            pFinalMesh->UnlockIndexBuffer();

                        // Release the old
                        pNewMesh->Release();
                        pNewMesh = pFinalMesh;
                    }

                    *ppOutMesh = pNewMesh;
                }
                else
                    pNewMesh->Release();
            }
            delete[] pMapping;
        } else
            hr = E_OUTOFMEMORY;
    } else
        hr = E_FAIL;

    if( pVBData )
        pInputMesh->UnlockVertexBuffer();

    if( pdwIBData )
        pInputMesh->UnlockIndexBuffer();

    delete[] pdwPtRep;
    pInputMesh->Release();

    return hr;
}


//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
{
    // Set the callback functions. These functions allow the sample framework to notify
    // the application about device changes, user input, and windows messages.  The 
    // callbacks are optional so you need only set callbacks for events you're interested 
    // in. However, if you don't handle the device reset/lost callbacks then the sample 
    // framework won't be able to reset your device since the application must first 
    // release all device resources before resetting.  Likewise, if you don't handle the 
    // device created/destroyed callbacks then the sample framework won't be able to 
    // recreate your device resources.
    DXUTSetCallbackDeviceCreated( OnCreateDevice );
    DXUTSetCallbackDeviceReset( OnResetDevice );
    DXUTSetCallbackDeviceLost( OnLostDevice );
    DXUTSetCallbackDeviceDestroyed( OnDestroyDevice );
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackKeyboard( KeyboardProc );
    DXUTSetCallbackMouse( MouseProc );
    DXUTSetCallbackFrameRender( OnFrameRender );
    DXUTSetCallbackFrameMove( OnFrameMove );

    // This sample requires a stencil buffer. See the callback function for details.
    CGrowableArray<D3DFORMAT>* pDSFormats = DXUTGetEnumeration()->GetPossibleDepthStencilFormatList();
    pDSFormats->RemoveAll();
    pDSFormats->Add( D3DFMT_D24S8 );
    pDSFormats->Add( D3DFMT_D24X4S4 );
    pDSFormats->Add( D3DFMT_D15S1 );
    pDSFormats->Add( D3DFMT_D24FS8 );

    // Show the cursor and clip it when in full screen
    DXUTSetCursorSettings( true, true );

    InitApp();

    // Initialize the scripting engines application domain
    InitializeScriptEngine();

    // Initialize the scaling and translation for the background meshes
    // Hardcode the matrices since we only have two.
    D3DXMATRIXA16 m;
    D3DXMatrixTranslation( &g_mWorldBack[0], 0.0f, 2.0f, 0.0f );
    D3DXMatrixScaling( &g_mWorldBack[1], 0.3f, 0.3f, 0.3f );
    D3DXMatrixTranslation( &m, 0.0f, 1.5f, 0.0f );
    D3DXMatrixMultiply( &g_mWorldBack[1], &g_mWorldBack[1], &m );

    // Initialize the sample framework and create the desired Win32 window and Direct3D 
    // device for the application. Calling each of these functions is optional, but they
    // allow you to set several options which control the behavior of the framework.
    DXUTInit( true, true, true ); // Parse the command line, handle the default hotkeys, and show msgboxes
    DXUTCreateWindow( L"Scripting ShadowVolume" );
    DXUTCreateDevice( D3DADAPTER_DEFAULT, true, 640, 480, IsDeviceAcceptable, ModifyDeviceSettings );

    // Pass control to the sample framework for handling the message pump and 
    // dispatching render calls. The sample framework will call your FrameMove 
    // and FrameRender callback when there is idle time between handling window messages.
    DXUTMainLoop();

    // Perform any application-level cleanup here. Direct3D device resources are released within the
    // appropriate callback functions and therefore don't require any cleanup code here.

    return DXUTGetExitCode();
}


//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
void InitApp()
{
    // Initialize dialogs
    g_HUD.SetCallback( OnGUIEvent ); int iY = 10; 
    g_HUD.AddButton( IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 35, iY, 125, 22 );
    g_HUD.AddButton( IDC_TOGGLEREF, L"Toggle REF (F3)", 35, iY += 24, 125, 22 );
    g_HUD.AddButton( IDC_CHANGEDEVICE, L"Change device (F2)", 35, iY += 24, 125, 22 );

    g_SampleUI.SetCallback( OnGUIEvent ); iY = 10;
    g_SampleUI.AddComboBox( IDC_CHANGESCRIPT, 0, iY += 24, 190, 22, 0, false );
    g_SampleUI.GetComboBox( IDC_CHANGESCRIPT )->SetDropHeight( 80 );
    g_SampleUI.GetComboBox( IDC_CHANGESCRIPT )->AddItem( L"Not Using Scripts", NULL );
    g_SampleUI.GetComboBox( IDC_CHANGESCRIPT )->AddItem( L"Script #1", L"script1.cs" );
    g_SampleUI.GetComboBox( IDC_CHANGESCRIPT )->AddItem( L"Script #2", L"script2.cs" );
    g_SampleUI.GetComboBox( IDC_CHANGESCRIPT )->AddItem( L"Bad Guy Hacker Script", L"unsecure.cs" );

    // Initialize cameras
    g_Camera.SetRotateButtons( true, false, false );
    g_MCamera.SetButtonMasks( MOUSE_RIGHT_BUTTON, 0, 0 );
    g_LCamera.SetButtonMasks( MOUSE_MIDDLE_BUTTON, 0, 0 );

    // Initialize the lights
    for( int L = 0; L < MAX_NUM_LIGHTS; ++L )
    {
        D3DXMATRIXA16 m;
        D3DXVECTOR4 v;
        D3DXMatrixScaling( &g_aLights[L].m_mWorld, 0.1f, 0.1f, 0.1f );
        D3DXMatrixTranslation( &m, g_LightInit[L].Position.x,
                                   g_LightInit[L].Position.y,
                                   g_LightInit[L].Position.z );
        D3DXMatrixMultiply( &g_aLights[L].m_mWorld, &g_aLights[L].m_mWorld, &m );
        g_aLights[L].m_Position = g_LightInit[L].Position;
        g_aLights[L].m_Color = g_LightInit[L].Color;
    }
}


//--------------------------------------------------------------------------------------
// Returns true if a particular depth-stencil format can be created and used with
// an adapter format and backbuffer format combination.
BOOL IsDepthFormatOk( D3DFORMAT DepthFormat,
                      D3DFORMAT AdapterFormat,
                      D3DFORMAT BackBufferFormat )
{
    // Verify that the depth format exists
    HRESULT hr = DXUTGetD3DObject()->CheckDeviceFormat( D3DADAPTER_DEFAULT,
                                                        D3DDEVTYPE_HAL,
                                                        AdapterFormat,
                                                        D3DUSAGE_DEPTHSTENCIL,
                                                        D3DRTYPE_SURFACE,
                                                        DepthFormat );
    if( FAILED( hr ) ) return FALSE;

    // Verify that the backbuffer format is valid
    hr = DXUTGetD3DObject()->CheckDeviceFormat( D3DADAPTER_DEFAULT,
                                                D3DDEVTYPE_HAL,
                                                AdapterFormat,
                                                D3DUSAGE_RENDERTARGET,
                                                D3DRTYPE_SURFACE,
                                                BackBufferFormat );
    if( FAILED( hr ) ) return FALSE;

    // Verify that the depth format is compatible
    hr = DXUTGetD3DObject()->CheckDepthStencilMatch( D3DADAPTER_DEFAULT,
                                                     D3DDEVTYPE_HAL,
                                                     AdapterFormat,
                                                     BackBufferFormat,
                                                     DepthFormat );

    return SUCCEEDED(hr);
}


//--------------------------------------------------------------------------------------
// Called during device initialization, this code checks the device for some 
// minimum set of capabilities, and rejects those that don't pass by returning false.
//--------------------------------------------------------------------------------------
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, 
                                  D3DFORMAT BackBufferFormat, bool bWindowed )
{
    // Skip backbuffer formats that don't support alpha blending
    IDirect3D9* pD3D = DXUTGetD3DObject(); 
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                    AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
                    D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
        return false;

    // Must support stencil buffer
    if( !IsDepthFormatOk( D3DFMT_D24S8,
                          AdapterFormat,
                          BackBufferFormat ) &&
        !IsDepthFormatOk( D3DFMT_D24X4S4,
                          AdapterFormat,
                          BackBufferFormat ) &&
        !IsDepthFormatOk( D3DFMT_D15S1,
                          AdapterFormat,
                          BackBufferFormat ) &&
        !IsDepthFormatOk( D3DFMT_D24FS8,
                          AdapterFormat,
                          BackBufferFormat ) )
        return false;

    return true;
}


//--------------------------------------------------------------------------------------
// This callback function is called immediately before a device is created to allow the 
// application to modify the device settings. The supplied pDeviceSettings parameter 
// contains the settings that the framework has selected for the new device, and the 
// application can make any desired changes directly to this structure.  Note however that 
// the sample framework will not correct invalid device settings so care must be taken 
// to return valid device settings, otherwise IDirect3D9::CreateDevice() will fail.  
//--------------------------------------------------------------------------------------
void CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps )
{
    // If device doesn't support HW T&L or doesn't support 1.1 vertex shaders in HW 
    // then switch to SWVP.
    if( (pCaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
         pCaps->VertexShaderVersion < D3DVS_VERSION(1,1) )
    {
        pDeviceSettings->BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }
    else
    {
        pDeviceSettings->BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    }

    // Debugging vertex shaders requires either REF or software vertex processing 
    // and debugging pixel shaders requires REF.  
#ifdef DEBUG_VS
    if( pDeviceSettings->DeviceType != D3DDEVTYPE_REF )
    {
        pDeviceSettings->BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
        pDeviceSettings->BehaviorFlags &= ~D3DCREATE_PUREDEVICE;                            
        pDeviceSettings->BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }
#endif
#ifdef DEBUG_PS
    pDeviceSettings->DeviceType = D3DDEVTYPE_REF;
#endif

    // This sample requires a stencil buffer.
    if( IsDepthFormatOk( D3DFMT_D24S8,
                         pDeviceSettings->AdapterFormat,
                         pDeviceSettings->pp.BackBufferFormat ) )
        pDeviceSettings->pp.AutoDepthStencilFormat = D3DFMT_D24S8;
    else
    if( IsDepthFormatOk( D3DFMT_D24X4S4,
                         pDeviceSettings->AdapterFormat,
                         pDeviceSettings->pp.BackBufferFormat ) )
        pDeviceSettings->pp.AutoDepthStencilFormat = D3DFMT_D24X4S4;
    else
    if( IsDepthFormatOk( D3DFMT_D24FS8,
                         pDeviceSettings->AdapterFormat,
                         pDeviceSettings->pp.BackBufferFormat ) )
        pDeviceSettings->pp.AutoDepthStencilFormat = D3DFMT_D24FS8;
    else
    if( IsDepthFormatOk( D3DFMT_D15S1,
                         pDeviceSettings->AdapterFormat,
                         pDeviceSettings->pp.BackBufferFormat ) )
        pDeviceSettings->pp.AutoDepthStencilFormat = D3DFMT_D15S1;
}


//--------------------------------------------------------------------------------------
// This function is called after the D3D Settings Dialog is refreshed, giving the
// application a chance to modify the dialog contents before being displayed to the
// user.
//--------------------------------------------------------------------------------------
void CALLBACK ModifySettingsDlg( CDXUTDialog* pSettingsDialog )
{
    CDXUTComboBox* pComboBox = NULL;

    pComboBox = pSettingsDialog->GetComboBox( DXUTSETTINGSDLG_DEPTH_STENCIL );
    for( int i = pComboBox->GetNumItems() - 1; i >= 0; --i )
    {
        DXUTComboBoxItem* pItem = pComboBox->GetItem( i );
        if( pItem->pData != (void*)D3DFMT_D24S8 &&
            pItem->pData != (void*)D3DFMT_D24X4S4 &&
            pItem->pData != (void*)D3DFMT_D15S1 &&
            pItem->pData != (void*)D3DFMT_D24FS8 )
            pComboBox->RemoveItem( i );
    }
}


// Compute a matrix that scales Mesh to a specified size and centers around origin
void ComputeMeshScaling( CDXUTMesh &Mesh, D3DXMATRIX *pmScalingCenter )
{
    LPVOID pVerts = NULL;
    D3DXMatrixIdentity( pmScalingCenter );
    if( SUCCEEDED( Mesh.GetSysMemMesh()->LockVertexBuffer( 0, &pVerts ) ) )
    {
        D3DXVECTOR3 vCtr;
        float fRadius;
        if( SUCCEEDED( D3DXComputeBoundingSphere( (const D3DXVECTOR3*)pVerts,
                                                  Mesh.GetSysMemMesh()->GetNumVertices(),
                                                  Mesh.GetSysMemMesh()->GetNumBytesPerVertex(),
                                                  &vCtr, &fRadius ) ) )
        {
            D3DXMatrixTranslation( pmScalingCenter, -vCtr.x, -vCtr.y, -vCtr.z );
            D3DXMATRIXA16 m;
            D3DXMatrixScaling( &m, 1.0f / fRadius,
                                   1.0f / fRadius,
                                   1.0f / fRadius );
            D3DXMatrixMultiply( pmScalingCenter, pmScalingCenter, &m );
        }
        Mesh.GetSysMemMesh()->UnlockVertexBuffer();
    }
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been 
// created, which will happen during application initialization and windowed/full screen 
// toggles. This is the best location to create D3DPOOL_MANAGED resources since these 
// resources need to be reloaded whenever the device is destroyed. Resources created  
// here should be released in the OnDestroyDevice callback. 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc )
{
    HRESULT hr;

    // Initialize the vertex declaration
    V_RETURN( pd3dDevice->CreateVertexDeclaration( MESHVERT::Decl, &g_pMeshDecl ) );
    V_RETURN( pd3dDevice->CreateVertexDeclaration( SHADOWVERT::Decl, &g_pShadowDecl ) );
    V_RETURN( pd3dDevice->CreateVertexDeclaration( POSTPROCVERT::Decl, &g_pPProcDecl ) );

    // Initialize the font
    V_RETURN( D3DXCreateFont( pd3dDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, 
                         OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
                         L"Arial", &g_pFont ) );

    // Define DEBUG_VS and/or DEBUG_PS to debug vertex and/or pixel shaders with the 
    // shader debugger. Debugging vertex shaders requires either REF or software vertex 
    // processing, and debugging pixel shaders requires REF.  The 
    // D3DXSHADER_FORCE_*_SOFTWARE_NOOPT flag improves the debug experience in the 
    // shader debugger.  It enables source level debugging, prevents instruction 
    // reordering, prevents dead code elimination, and forces the compiler to compile 
    // against the next higher available software target, which ensures that the 
    // unoptimized shaders do not exceed the shader model limitations.  Setting these 
    // flags will cause slower rendering since the shaders will be unoptimized and 
    // forced into software.  See the DirectX documentation for more information about 
    // using the shader debugger.
    DWORD dwShaderFlags = 0;
    #ifdef DEBUG_VS
        dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
    #endif
    #ifdef DEBUG_PS
        dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
    #endif

    // Read the D3DX effect file
    WCHAR str[MAX_PATH];
    V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"ShadowVolume.fx" ) );

    // If this fails, there should be debug output as to 
    // they the .fx file failed to compile
    V_RETURN( D3DXCreateEffectFromFile( pd3dDevice, str, NULL, NULL, dwShaderFlags, 
                                        NULL, &g_pEffect, NULL ) );

    // Determine the rendering techniques to use based on device caps
    D3DCAPS9 Caps;
    V_RETURN( pd3dDevice->GetDeviceCaps( &Caps ) );

    if( Caps.PixelShaderVersion >= D3DPS_VERSION( 2, 0 ) )
        g_hRenderScene = g_pEffect->GetTechniqueByName( "RenderScene" );
    else
        g_hRenderScene = g_pEffect->GetTechniqueByName( "RenderScene1x" );

    // If 2-sided stencil is supported, use it.
    if( Caps.StencilCaps & D3DSTENCILCAPS_TWOSIDED )
    {
        g_hRenderShadow = g_pEffect->GetTechniqueByName( "RenderShadowVolume2Sided" );
        g_hShowShadow = g_pEffect->GetTechniqueByName( "ShowShadowVolume2Sided" );
    }
    else
    {
        g_hRenderShadow = g_pEffect->GetTechniqueByName( "RenderShadowVolume" );
        g_hShowShadow = g_pEffect->GetTechniqueByName( "ShowShadowVolume" );
    }

    // Load the meshes
    V_RETURN( g_Background[0].Create( pd3dDevice, L"misc\\cell.x" ) );
    g_Background[0].SetVertexDecl( pd3dDevice, MESHVERT::Decl );
    V_RETURN( g_Background[1].Create( pd3dDevice, L"misc\\seafloor.x" ) );
    g_Background[1].SetVertexDecl( pd3dDevice, MESHVERT::Decl );
    V_RETURN( g_LightMesh.Create( pd3dDevice, L"misc\\sphere0.x" ) );
    g_LightMesh.SetVertexDecl( pd3dDevice, MESHVERT::Decl );
    V_RETURN( g_Mesh.Create( pd3dDevice, DEFMESHFILENAME ) );
    g_Mesh.SetVertexDecl( pd3dDevice, MESHVERT::Decl );

    // Compute the scaling matrix for the mesh so that the size of the mesh
    // that shows on the screen is consistent.
    ComputeMeshScaling( g_Mesh, &g_mWorldScaling );

    // Setup the camera's view parameters
    D3DXVECTOR3 vecEye(0.0f, 0.0f, -5.0f);
    D3DXVECTOR3 vecAt (0.0f, 0.0f, -0.0f);
    g_Camera.SetViewParams( &vecEye, &vecAt );
    g_LCamera.SetViewParams( &vecEye, &vecAt );
    g_MCamera.SetViewParams( &vecEye, &vecAt );

    // Create the 1x1 white default texture
    V_RETURN( pd3dDevice->CreateTexture( 1, 1, 1, 0, D3DFMT_A8R8G8B8,
                                         D3DPOOL_MANAGED, &g_pDefaultTex, NULL ) );
    D3DLOCKED_RECT lr;
    V_RETURN( g_pDefaultTex->LockRect( 0, &lr, NULL, 0 ) );
    *(LPDWORD)lr.pBits = D3DCOLOR_RGBA( 255, 255, 255, 255 );
    V_RETURN( g_pDefaultTex->UnlockRect( 0 ) );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been 
// reset, which will happen after a lost device scenario. This is the best location to 
// create D3DPOOL_DEFAULT resources since these resources need to be reloaded whenever 
// the device is lost. Resources created here should be released in the OnLostDevice 
// callback. 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, 
                                const D3DSURFACE_DESC* pBackBufferSurfaceDesc )
{
    HRESULT hr;

    if( g_pFont )
        V_RETURN( g_pFont->OnResetDevice() );
    if( g_pEffect )
        V_RETURN( g_pEffect->OnResetDevice() );

    g_Background[0].RestoreDeviceObjects( pd3dDevice );
    g_Background[1].RestoreDeviceObjects( pd3dDevice );
    g_LightMesh.RestoreDeviceObjects( pd3dDevice );
    g_Mesh.RestoreDeviceObjects( pd3dDevice );

    // Create a sprite to help batch calls when drawing many lines of text
    V_RETURN( D3DXCreateSprite( pd3dDevice, &g_pTextSprite ) );

    // Setup the camera's projection parameters
    float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
    g_Camera.SetProjParams( D3DX_PI/4, fAspectRatio, 0.1f, 20.0f );
    g_MCamera.SetWindow( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height );
    g_LCamera.SetWindow( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height );
    g_pEffect->SetFloat( "g_fFarClip", 20.0f - EXTRUDE_EPSILON );
    V( g_pEffect->SetMatrix( "g_mProj", g_Camera.GetProjMatrix() ) );

    g_HUD.SetLocation( pBackBufferSurfaceDesc->Width-170, 0 );
    g_HUD.SetSize( 170, 170 );
    g_SampleUI.SetLocation( 0, pBackBufferSurfaceDesc->Height-200 );
    g_SampleUI.SetSize( pBackBufferSurfaceDesc->Width, 150 );

    int iY = 10;
    g_SampleUI.GetControl( IDC_CHANGESCRIPT )->SetLocation( pBackBufferSurfaceDesc->Width - 200, iY += 24 );

    // Generate the shadow volume mesh
    GenerateShadowMesh( pd3dDevice, g_Mesh.GetSysMemMesh(), &g_pShadowMesh );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// This callback function will be called once at the beginning of every frame. This is the
// best location for your application to handle updates to the scene, but is not 
// intended to contain actual rendering calls, which should instead be placed in the 
// OnFrameRender callback.  
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime )
{
    // Update the camera's position based on user input 
    g_Camera.FrameMove( fElapsedTime );
    g_MCamera.FrameMove( fElapsedTime );
    g_LCamera.FrameMove( fElapsedTime );

    // Let script engine handle this
    D3DXMATRIX* pMatrix = g_MCamera.GetWorldMatrix();
    UpdatePlayerRotationX(fTime, pMatrix);
    UpdatePlayerRotationY(fTime, pMatrix);
    UpdatePlayerRotationZ(fTime, pMatrix);
    UpdatePlayerPosition(fTime, (float*)&pMatrix->_41, (float*)&pMatrix->_42, (float*)&pMatrix->_43);
}


//--------------------------------------------------------------------------------------
// Simply renders the entire scene without any shadow handling.
void RenderScene( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, bool bRenderLight )
{
    HRESULT hr;
    D3DXMATRIXA16 mWorldView;
    D3DXMATRIXA16 mViewProj;
    D3DXMATRIXA16 mWorldViewProjection;

    // Get the projection & view matrix from the camera class
    D3DXMatrixMultiply( &mViewProj, g_Camera.GetViewMatrix(), g_Camera.GetProjMatrix() );

    // Render the lights if requested
    if( bRenderLight )
    {
        D3DXHANDLE hCurrTech;
        hCurrTech = g_pEffect->GetCurrentTechnique();  // Save the current technique
        V( g_pEffect->SetTechnique( "RenderSceneAmbient" ) );
        V( g_pEffect->SetTexture( "g_txScene", g_pDefaultTex ) );
        D3DXVECTOR4 vLightMat( 1.0f, 1.0f, 1.0f, 1.0f );
        V( g_pEffect->SetVector( "g_vMatColor", &vLightMat ) );
        UINT cPasses;
        ID3DXMesh *pMesh = g_LightMesh.GetLocalMesh();
        V( g_pEffect->Begin( &cPasses, 0 ) );
        for( UINT i = 0; i < cPasses; ++i )
        {
            V( g_pEffect->BeginPass( i ) );
            for( int i = 0; i < g_nNumLights; ++i )
            {
                for( UINT m = 0; m < g_LightMesh.m_dwNumMaterials; ++m )
                {
                    mWorldView = g_aLights[i].m_mWorld * *g_LCamera.GetWorldMatrix() * *g_Camera.GetViewMatrix();
                    mWorldViewProjection = mWorldView * *g_Camera.GetProjMatrix();
                    V( g_pEffect->SetMatrix( "g_mWorldView", &mWorldView ) );
                    V( g_pEffect->SetMatrix( "g_mWorldViewProjection", &mWorldViewProjection ) );
                    V( g_pEffect->SetVector( "g_vAmbient", &g_aLights[i].m_Color ) );

                    // The effect interface queues up the changes and performs them 
                    // with the CommitChanges call. You do not need to call CommitChanges if 
                    // you are not setting any parameters between the BeginPass and EndPass.
                    V( g_pEffect->CommitChanges() );

                    V( pMesh->DrawSubset( m ) );
                }
            }
            V( g_pEffect->EndPass() );
        }
        V( g_pEffect->End() );
        V( g_pEffect->SetTechnique( hCurrTech ) ); // Restore the old technique
        D3DXVECTOR4 vAmb( AMBIENT, AMBIENT, AMBIENT, 1.0f );
        V( g_pEffect->SetVector( "g_vAmbient", &vAmb ) );
    }

    // Render the background mesh
    V( pd3dDevice->SetVertexDeclaration( g_pMeshDecl ) );
    mWorldView = g_mWorldBack[g_nCurrBackground] * *g_Camera.GetViewMatrix();
    mWorldViewProjection = g_mWorldBack[g_nCurrBackground] * mViewProj;
    V( g_pEffect->SetMatrix( "g_mWorldViewProjection", &mWorldViewProjection ) );
    V( g_pEffect->SetMatrix( "g_mWorldView", &mWorldView ) );
    UINT cPasses;
    V( g_pEffect->Begin( &cPasses, 0 ) );
    for( UINT i = 0; i < cPasses; ++i )
    {
        V( g_pEffect->BeginPass( i ) );
        ID3DXMesh *pMesh = g_Background[g_nCurrBackground].GetLocalMesh();
        for( UINT i = 0; i < g_Background[g_nCurrBackground].m_dwNumMaterials; ++i )
        {
            V( g_pEffect->SetVector( "g_vMatColor", (D3DXVECTOR4*)&g_Background[g_nCurrBackground].m_pMaterials[i].Diffuse ) );
            if( g_Background[g_nCurrBackground].m_pTextures[i] )
                V( g_pEffect->SetTexture( "g_txScene", g_Background[g_nCurrBackground].m_pTextures[i] ) )
            else
                V( g_pEffect->SetTexture( "g_txScene", g_pDefaultTex ) );
            // The effect interface queues up the changes and performs them 
            // with the CommitChanges call. You do not need to call CommitChanges if 
            // you are not setting any parameters between the BeginPass and EndPass.
            V( g_pEffect->CommitChanges() );
            V( pMesh->DrawSubset( i ) );
        }
        V( g_pEffect->EndPass() );
    }
    V( g_pEffect->End() );

    // Render the mesh
    V( pd3dDevice->SetVertexDeclaration( g_pMeshDecl ) );
    mWorldView = g_mWorldScaling * *g_MCamera.GetWorldMatrix() * *g_Camera.GetViewMatrix();
    mWorldViewProjection = mWorldView * *g_Camera.GetProjMatrix();
    V( g_pEffect->SetMatrix( "g_mWorldViewProjection", &mWorldViewProjection ) );
    V( g_pEffect->SetMatrix( "g_mWorldView", &mWorldView ) );
    V( g_pEffect->Begin( &cPasses, 0 ) );
    for( UINT i = 0; i < cPasses; ++i )
    {
        V( g_pEffect->BeginPass( i ) );
        ID3DXMesh *pMesh = g_Mesh.GetLocalMesh();
        for( UINT i = 0; i < g_Mesh.m_dwNumMaterials; ++i )
        {
            V( g_pEffect->SetVector( "g_vMatColor", (D3DXVECTOR4*)&g_Mesh.m_pMaterials[i].Diffuse ) );
            if( g_Mesh.m_pTextures[i] )
                V( g_pEffect->SetTexture( "g_txScene", g_Mesh.m_pTextures[i] ) )
            else
                V( g_pEffect->SetTexture( "g_txScene", g_pDefaultTex ) );
            // The effect interface queues up the changes and performs them 
            // with the CommitChanges call. You do not need to call CommitChanges if 
            // you are not setting any parameters between the BeginPass and EndPass.
            V( g_pEffect->CommitChanges() );
            V( pMesh->DrawSubset( i ) );
        }
        V( g_pEffect->EndPass() );
    }
    V( g_pEffect->End() );
}


//--------------------------------------------------------------------------------------
// This callback function will be called at the end of every frame to perform all the 
// rendering calls for the scene, and it will also be called if the window needs to be 
// repainted. After this function has returned, the sample framework will call 
// IDirect3DDevice9::Present to display the contents of the next buffer in the swap chain
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime )
{
    HRESULT hr;

    // Clear the render target and the zbuffer
    V( pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 66, 75, 121), 1.0f, 0 ) );

    // Render the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
        // First, render the scene with only ambient lighting
        g_pEffect->SetTechnique( "RenderSceneAmbient" );
        D3DXVECTOR4 vAmb( AMBIENT, AMBIENT, AMBIENT, 1.0f );
        V( g_pEffect->SetVector( "g_vAmbient", &vAmb ) );
        RenderScene( pd3dDevice, fTime, fElapsedTime, true );

        // Now process the lights.  For each light in the scene,
        // render the shadow volume, then render the scene with
        // stencil enabled.

        for( int L = 0; L < g_nNumLights; ++L )
        {
            // Clear the stencil buffer
            if( g_RenderType != RENDERTYPE_COMPLEXITY )
                V( pd3dDevice->Clear(0, NULL, D3DCLEAR_STENCIL, D3DCOLOR_ARGB(0, 170, 170, 170), 1.0f, 0 ) );

            D3DXVECTOR4 vLight( g_aLights[L].m_Position.x, g_aLights[L].m_Position.y, g_aLights[L].m_Position.z, 1.0f );
            D3DXVec4Transform( &vLight, &vLight, g_LCamera.GetWorldMatrix() );
            D3DXVec4Transform( &vLight, &vLight, g_Camera.GetViewMatrix() );
            V( g_pEffect->SetVector( "g_vLightView", &vLight ) );
            V( g_pEffect->SetVector( "g_vLightColor", &g_aLights[L].m_Color ) );

            // Render the shadow volume
            switch( g_RenderType )
            {
                case RENDERTYPE_COMPLEXITY:
                    g_pEffect->SetTechnique( "RenderShadowVolumeComplexity" );
                    break;
                case RENDERTYPE_SHADOWVOLUME:
                    g_pEffect->SetTechnique( g_hShowShadow );
                    break;
                default:
                    g_pEffect->SetTechnique( g_hRenderShadow );
            }

            g_pEffect->SetVector( "g_vShadowColor", &g_vShadowColor[L] );

            // If there was an error generating the shadow volume,
            // skip rendering the shadow mesh.  The scene will show
            // up without shadow.
            if( g_pShadowMesh )
            {
                V( pd3dDevice->SetVertexDeclaration( g_pShadowDecl ) );
                UINT cPasses;
                V( g_pEffect->Begin( &cPasses, 0 ) );
                for( UINT i = 0; i < cPasses; ++i )
                {
                    V( g_pEffect->BeginPass( i ) );
                    V( g_pEffect->CommitChanges() );
                    V( g_pShadowMesh->DrawSubset( 0 ) );
                    V( g_pEffect->EndPass() );
                }
                V( g_pEffect->End() );
            }

            //
            // Render the scene with stencil and lighting enabled.
            //
            if( g_RenderType != RENDERTYPE_COMPLEXITY )
            {
                g_pEffect->SetTechnique( g_hRenderScene );
                RenderScene( pd3dDevice, fTime, fElapsedTime, false );
            }
        }

        if( g_RenderType == RENDERTYPE_COMPLEXITY )
        {
            // Clear the render target
            V( pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0 ) );

            // Render scene complexity visualization
            const D3DSURFACE_DESC* pd3dsdBackBuffer = DXUTGetBackBufferSurfaceDesc();
            POSTPROCVERT quad[4] =
            {
                { -0.5f,                                                -0.5f, 0.5f, 1.0f },
                { pd3dsdBackBuffer->Width-0.5f,                         -0.5f, 0.5f, 1.0f },
                { -0.5f,                        pd3dsdBackBuffer->Height-0.5f, 0.5f, 1.0f },
                { pd3dsdBackBuffer->Width-0.5f, pd3dsdBackBuffer->Height-0.5f, 0.5f, 1.0f }
            };

            pd3dDevice->SetVertexDeclaration( g_pPProcDecl );
            g_pEffect->SetTechnique( "RenderComplexity" );
            UINT cPasses;
            g_pEffect->Begin( &cPasses, 0 );
            for( UINT p = 0; p < cPasses; ++p )
            {
                g_pEffect->BeginPass( p );
                pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, quad, sizeof(POSTPROCVERT) );
                g_pEffect->EndPass();
            }
            g_pEffect->End();
        }

        // Miscellaneous rendering
        RenderText();
        g_HUD.OnRender( fElapsedTime );
        g_SampleUI.OnRender( fElapsedTime );

        V( pd3dDevice->EndScene() );
    }
}


//--------------------------------------------------------------------------------------
// Render the help and statistics text. This function uses the ID3DXFont interface for 
// efficient text rendering.
//--------------------------------------------------------------------------------------
void RenderText()
{
    // The helper object simply helps keep track of text position, and color
    // and then it calls pFont->DrawText( m_pSprite, strMsg, -1, &rc, DT_NOCLIP, m_clr );
    // If NULL is passed in as the sprite object, then it will work however the 
    // pFont->DrawText() will not be batched together.  Batching calls will improves performance.
    CDXUTTextHelper txtHelper( g_pFont, g_pTextSprite, 15 );
    const D3DSURFACE_DESC* pd3dsdBackBuffer = DXUTGetBackBufferSurfaceDesc();

    // Output statistics
    txtHelper.Begin();
    txtHelper.SetInsertionPos( 5, 5 );
    txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
    txtHelper.DrawTextLine( DXUTGetFrameStats() );
    txtHelper.DrawTextLine( DXUTGetDeviceStats() );

    // Draw help
    if( g_bShowHelp )
    {
        txtHelper.SetInsertionPos( 10, pd3dsdBackBuffer->Height-80 );
        txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 0.75f, 0.0f, 1.0f ) );
        txtHelper.DrawTextLine( L"Controls (F1 to hide):" );

        txtHelper.SetInsertionPos( 20, pd3dsdBackBuffer->Height-65 );
        txtHelper.DrawTextLine( L"Camera control: Left mouse\n"
                                L"Mesh control: Right mouse\n"
                                L"Light control: Middle mouse\n"
                                L"Quit: ESC" );
    }
    else
    {
        txtHelper.SetInsertionPos( 10, pd3dsdBackBuffer->Height-25 );
        txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
        txtHelper.DrawTextLine( L"Press F1 for help" );
    }

    txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
    if( !(g_bLeftButtonDown || g_bMiddleButtonDown || g_bRightButtonDown) )
    {
        txtHelper.SetInsertionPos( pd3dsdBackBuffer->Width / 2 - 90, pd3dsdBackBuffer->Height - 40 );
        txtHelper.DrawTextLine( L"\nW/S/A/D/Q/E to move camera." );
    } else
    {
        txtHelper.SetInsertionPos( pd3dsdBackBuffer->Width / 2 - 70, pd3dsdBackBuffer->Height - 40 );
        if( g_bLeftButtonDown )
        {
            txtHelper.DrawTextLine( L"Camera Control Mode" );
        } else
        if( g_bMiddleButtonDown )
        {
            txtHelper.DrawTextLine( L"Light Control Mode" );
        }
        if( g_bRightButtonDown )
        {
            txtHelper.DrawTextLine( L"Model Control Mode" );
        }
        txtHelper.SetInsertionPos( pd3dsdBackBuffer->Width / 2 - 130, pd3dsdBackBuffer->Height - 25 );
        txtHelper.DrawTextLine( L"Move mouse to rotate. W/S/A/D/Q/E to move." );
    }

    if( g_RenderType == RENDERTYPE_COMPLEXITY )
    {
        txtHelper.SetInsertionPos( 5, 70 );
        txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
        txtHelper.DrawTextLine( L"Shadow volume complexity:" );
        txtHelper.SetInsertionPos( 5, 90 );
        txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 0.0f, 1.0f, 1.0f ) );
        txtHelper.DrawTextLine( L"1 to 5 fills\n" );
        txtHelper.SetForegroundColor( D3DXCOLOR( 0.0f, 0.0f, 1.0f, 1.0f ) );
        txtHelper.DrawTextLine( L"6 to 10 fills\n" );
        txtHelper.SetForegroundColor( D3DXCOLOR( 0.0f, 1.0f, 1.0f, 1.0f ) );
        txtHelper.DrawTextLine( L"11 to 20 fills\n" );
        txtHelper.SetForegroundColor( D3DXCOLOR( 0.0f, 1.0f, 0.0f, 1.0f ) );
        txtHelper.DrawTextLine( L"21 to 30 fills\n" );
        txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
        txtHelper.DrawTextLine( L"31 to 40 fills\n" );
        txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 0.5f, 0.0f, 1.0f ) );
        txtHelper.DrawTextLine( L"41 to 50 fills\n" );
        txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ) );
        txtHelper.DrawTextLine( L"51 to 70 fills\n" );
        txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
        txtHelper.DrawTextLine( L"71 or more fills\n" );
    }

    // Display an error message if unable to generate shadow mesh
    if( !g_pShadowMesh )
    {
        txtHelper.SetInsertionPos( 5, 35 );
        txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ) );
        txtHelper.DrawTextLine( L"Unable to generate closed shadow volume for this mesh\n" );
        txtHelper.DrawTextLine( L"No shadow will be rendered" );
    }

    txtHelper.End();
}


//--------------------------------------------------------------------------------------
// Before handling window messages, the sample framework passes incoming windows 
// messages to the application through this callback function. If the application sets 
// *pbNoFurtherProcessing to TRUE, then the sample framework will not process this message.
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing )
{
    // Give the dialogs a chance to handle the message first
    *pbNoFurtherProcessing = g_HUD.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;
    *pbNoFurtherProcessing = g_SampleUI.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    // Pass all remaining windows messages to camera so it can respond to user input
    g_Camera.HandleMessages( hWnd, uMsg, wParam, lParam );
    g_MCamera.HandleMessages( hWnd, uMsg, wParam, lParam );
    g_LCamera.HandleMessages( hWnd, uMsg, wParam, lParam );

    return 0;
}


//--------------------------------------------------------------------------------------
// As a convenience, the sample framework inspects the incoming windows messages for
// keystroke messages and decodes the message parameters to pass relevant keyboard
// messages to the application.  The framework does not remove the underlying keystroke 
// messages, which are still passed to the application's MsgProc callback.
//--------------------------------------------------------------------------------------
void CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown )
{
    if( bKeyDown )
    {
        switch( nChar )
        {
            case VK_F1: g_bShowHelp = !g_bShowHelp; break;
        }
    }
}


void CALLBACK MouseProc( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta, int xPos, int yPos )
{
    g_bLeftButtonDown = bLeftButtonDown;
    g_bMiddleButtonDown = bMiddleButtonDown;
    g_bRightButtonDown = bRightButtonDown;

    bool bOldModelMovementMode = g_bModelMovementMode;
    g_bModelMovementMode = g_bRightButtonDown;
    if( bOldModelMovementMode && !g_bModelMovementMode )
    {
        // Enable camera movement. Disable model movement
        g_Camera.SetEnablePositionMovement( true );
        g_MCamera.SetEnablePositionMovement( false );
    } else
    if( !bOldModelMovementMode && g_bModelMovementMode )
    {
        // Enable model movement. Disable camera movement
        g_Camera.SetEnablePositionMovement( false );
        g_MCamera.SetEnablePositionMovement( true );
    }
}


//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl )
{
    switch( nControlID )
    {
        case IDC_TOGGLEFULLSCREEN: DXUTToggleFullScreen(); break;
        case IDC_TOGGLEREF:        DXUTToggleREF(); break;
        case IDC_CHANGEDEVICE:     DXUTSetShowSettingsDialog( !DXUTGetShowSettingsDialog() ); break;
        case IDC_CHANGESCRIPT:
            if( g_SampleUI.GetComboBox( IDC_CHANGESCRIPT )->GetSelectedData() )
            {
                // Find the script file
                WCHAR strPath[MAX_PATH];
                DXUTFindDXSDKMediaFileCch( strPath, sizeof(strPath) / sizeof(WCHAR), 
                    (LPCWSTR)g_SampleUI.GetComboBox( IDC_CHANGESCRIPT )->GetSelectedData()) ;
                
                LoadScript(strPath);
            }
            else
            {
                // Load an 'empty' script file to turn off scripting
                LoadScript(NULL);
            }

            break;
    }
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has 
// entered a lost state and before IDirect3DDevice9::Reset is called. Resources created
// in the OnResetDevice callback should be released here, which generally includes all 
// D3DPOOL_DEFAULT resources. See the "Lost Devices" section of the documentation for 
// information about lost devices.
//--------------------------------------------------------------------------------------
void CALLBACK OnLostDevice()
{
    g_Background[0].InvalidateDeviceObjects();
    g_Background[1].InvalidateDeviceObjects();
    g_LightMesh.InvalidateDeviceObjects();
    g_Mesh.InvalidateDeviceObjects();
    SAFE_RELEASE( g_pShadowMesh );

    if( g_pFont )
        g_pFont->OnLostDevice();
    if( g_pEffect )
        g_pEffect->OnLostDevice();
    SAFE_RELEASE(g_pTextSprite);
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has 
// been destroyed, which generally happens as a result of application termination or 
// windowed/full screen toggles. Resources created in the OnCreateDevice callback 
// should be released here, which generally includes all D3DPOOL_MANAGED resources. 
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice()
{
    g_Background[0].Destroy();
    g_Background[1].Destroy();
    g_LightMesh.Destroy();
    g_Mesh.Destroy();

    SAFE_RELEASE( g_pDefaultTex );
    SAFE_RELEASE( g_pEffect );
    SAFE_RELEASE( g_pFont );
    SAFE_RELEASE( g_pMeshDecl );
    SAFE_RELEASE( g_pShadowDecl );
    SAFE_RELEASE( g_pPProcDecl );
}
