//
// Graphic Global Include
// Upgrade DX11 
//
#pragma once


#pragma warning(push)
#pragma warning (disable: 4005) //warning C4005: 'DXGI_ERROR_REMOTE_OUTOFMEMORY': macro redefinition
#include <d3d11.h>
#include <dxgitype.h>
#pragma warning(pop)

#include <d3dcompiler.h>


//--------------------------------------------------------------------------------------------------------------
// External

// DirectXTK Desktop VS2015
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXCollision.h>

#include "../../DirectXTK/Inc/SimpleMath.h"
#include "../../DirectXTK/Inc/DDSTextureLoader.h"
#include "../../DirectXTK/Inc/WICTextureLoader.h"
#include "../../DirectXTK/Inc/CommonStates.h"
#include "../../DirectXTK/Inc/Effects.h"
#include "../../DirectXTK/Inc/Model.h"
#include "../../DirectXTK/Inc/GeometricPrimitive.h"
#include "../../DirectXTK/Inc/SpriteFont.h"
#include "../../DirectXTK/Inc/SpriteBatch.h"
#include "../../DirectXTK/Inc/SimpleMath.h"
#include "../../DirectXTK/Inc/ScreenGrab.h"
#include <Wincodec.h>   // for ScreenGrab

using namespace DirectX;

// FW1FontWrapper
#include "../External/FW1FontWrapper/FW1FontWrapper.h"
//--------------------------------------------------------------------------------------------------------------



#include <objidl.h>
#include <gdiplus.h> 

#include "../Common/common.h"
using namespace common;

//#include "../wxMemMonitorLib/wxMemMonitor.h" // debug library

static const char* g_defaultTexture = "white.dds";

namespace graphic {
	class cRenderer;
}


#include "base\d3dx11effect.h"

#include "../ai/ai.h"

#include "utility\utility.h"

#pragma warning(push)
#pragma warning (disable: 4244) //warning C4244:  conversion from 'float' to 'const int', possible loss of data
	#include "textdesigner\PngOutlineText.h"
#pragma warning(pop)

#include "base/graphicdef.h"
#include "model/rawmesh.h"
#include "model/rawani.h"
#include "collision/boundingbox.h"
#include "collision/boundingsphere.h"
//
//#include "interface/renderable.h"
//#include "interface/shaderrenderer.h"
//#include "interface/shadowrenderer.h"
//
#include "model/node2.h"
//


#include "base/color.h"
#include "base/viewport.h"
#include "base/material.h"
#include "base/light.h"
#include "base/vertexlayout.h"
#include "base/vertexbuffer.h"
#include "base/indexbuffer.h"
#include "base/temporalbuffer.h"
#include "base/meshbuffer.h"
#include "base/samplerstate.h"
#include "base/texture.h"
#include "base/rendertarget.h"
//#include "base/texturecube.h"
#include "base/vertexformat.h"

#include "shape\shape.h"
#include "shape\cubeshape.h"
#include "shape\sphereshape.h"
#include "shape\pyramidshape.h"
#include "shape\circleshape.h"
#include "shape\quadshape.h"
#include "shape\torusshape.h"


#include "base/grid.h"
#include "base/pyramid.h"
#include "base/line.h"
//#include "base/line2d.h"
#include "base/cube.h"
#include "base/shader11.h"
#include "base/constantbuffer.h"
#include "base/camera.h"
#include "base/skybox.h"
#include "base/sphere.h"
#include "base/circle.h"
//#include "base/circleline.h"
#include "base/quad.h"
#include "base/quad2d.h"
#include "base/torus.h"
#include "base/billboard.h"
#include "base/text.h"
#include "base/text3d3.h"
//
//
//#include "collision/collision.h"
//#include "collision/collisionmanager.h"
#include "collision/frustum.h"
//
//
#include "dbg/dbgbox.h"
#include "dbg/dbgsphere.h"
#include "dbg/dbgline.h"
#include "dbg/dbglinelist.h"
#include "dbg/dbgarrow.h"
#include "dbg/dbgfrustum.h"
#include "dbg/dbgquad.h"
//#include "dbg/dbgquad2.h"
#include "dbg/dbgaxis.h"

#include "base/gizmo.h"



//#include "particle/particles.h"
//#include "particle/snow.h"
//
//
//#include "model/track.h"
//#include "model/blendtrack.h"
//#include "model/mesh.h"
//#include "model/rigidmesh.h"
//#include "model/skinnedmesh.h"
//#include "model/model.h"
//#include "model/bonemgr.h"
//#include "model/bonenode.h"
//
#include "model_assimp/skeleton.h"
#include "model_assimp/animationnode.h"
#include "model_assimp/animation.h"
#include "model_assimp/mesh2.h"
#include "model_assimp/model_assimp.h"
#include "model_assimp/assimploader.h"

#include "model_new/model2.h"

#include "terrain/cascadedshadowmap.h"
#include "terrain/water.h"
#include "terrain/ocean.h"
#include "terrain/terrain2.h"
#include "terrain/tile.h"
#include "terrain/terraincursor.h"
//#include "terrain/terraineditor.h"
//#include "terrain/terrainimporter.h"
//#include "terrain/terrainexporter.h"
#include "terrain/terrainloader.h"


namespace graphic
{
	struct sRenderObj
	{
		int opt;
		Vector3 normal;
		Matrix44 tm;
		cNode2 *p;
	};

	struct sAlphaBlendSpace
	{
		cBoundingBox bbox;
		std::vector<sRenderObj> renders;
	};
}


#include "manager/textmanager.h"
#include "manager/fontmanager.h"
#include "manager/shadermanager.h"
#include "manager/pickmanager.h"
#include "manager/Renderer.h"
#include "manager/resourcemanager.h"
#include "manager/maincamera.h"
#include "manager/lightmanager.h"

#include "importer/parallelloader.h"
#include "importer/modelimporter.h"


#pragma comment( lib, "gdiplus.lib" ) 
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

#pragma comment (lib, "FW1FontWrapper/Win32/FW1FontWrapper.lib")

#ifdef _DEBUG
	#pragma comment(lib, "Effects11/effects11d.lib")
	#pragma comment( lib, "assimp-vc140-mtd.lib" ) 
#else
	#pragma comment(lib, "Effects11/effects11.lib")
	#pragma comment( lib, "assimp-vc140-mt.lib" ) 
#endif
