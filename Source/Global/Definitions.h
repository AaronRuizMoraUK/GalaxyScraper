#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include "VertexTypes.h"
#include "Mesh/CoreMesh/CoreMesh.h"

/**
* Several global necessary definitions.
*/

typedef CoreMesh<CUSTOMVERTEX_TEXTURE> CoreMeshTextured;
typedef CoreMesh<CUSTOMVERTEX_NORMAL_TEXTURE> CoreMeshNormalTextured;
typedef CoreMesh<CUSTOMVERTEX_NORMAL_TAN_TEXTURE> CoreMeshNormalTanTextured;
typedef CoreMesh<CUSTOMVERTEX_COLOR> CoreMeshColored;
typedef CoreMesh<CUSTOMVERTEX_TEXTURE_LIGHTMAP> CoreMeshTexturedLightmap;
typedef CoreMesh<CUSTOMVERTEX_CAL3D_NORMAL_TAN_TEXTURE> CoreMeshCal3DNormalTextured;

const enum ActionType {CYCLE, ACTION};

#endif //DEFINITIONS_H