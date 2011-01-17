#ifndef __J2K_CONTAINER_H__
#define __J2K_CONTAINER_H__

#include <import/nrt.h>

NRT_CXX_GUARD

typedef NRT_DATA J2K_USER_DATA;

typedef nrt_Uint32(*J2K_ICONTAINER_GET_TILESX)(J2K_USER_DATA*, nrt_Error*);
typedef nrt_Uint32(*J2K_ICONTAINER_GET_TILESY)(J2K_USER_DATA*, nrt_Error*);
typedef nrt_Uint32(*J2K_ICONTAINER_GET_TILE_WIDTH)(J2K_USER_DATA*, nrt_Error*);
typedef nrt_Uint32(*J2K_ICONTAINER_GET_TILE_HEIGHT)(J2K_USER_DATA*, nrt_Error*);
typedef nrt_Uint32(*J2K_ICONTAINER_GET_WIDTH)(J2K_USER_DATA*, nrt_Error*);
typedef nrt_Uint32(*J2K_ICONTAINER_GET_HEIGHT)(J2K_USER_DATA*, nrt_Error*);
typedef nrt_Uint32(*J2K_ICONTAINER_GET_NUM_COMPONENTS)(J2K_USER_DATA*, nrt_Error*);
typedef nrt_Uint32(*J2K_ICONTAINER_GET_COMPONENT_BYTES)(J2K_USER_DATA*, nrt_Error*);
typedef nrt_Uint32(*J2K_ICONTAINER_READ_TILE)(J2K_USER_DATA*, nrt_Uint32 tileX,
        nrt_Uint32 tileY, nrt_Uint8 **buf, nrt_Error*);
typedef nrt_Uint64(*J2K_ICONTAINER_READ_REGION)(J2K_USER_DATA*, nrt_Uint32 x0,
        nrt_Uint32 y0, nrt_Uint32 x1, nrt_Uint32 y1, nrt_Uint8 **buf,
        nrt_Error*);
typedef void (*J2K_ICONTAINER_DESTRUCT)(J2K_USER_DATA *);

typedef struct _j2k_IContainer
{
    J2K_ICONTAINER_GET_TILESX getTilesX;
    J2K_ICONTAINER_GET_TILESY getTilesY;
    J2K_ICONTAINER_GET_TILE_WIDTH getTileWidth;
    J2K_ICONTAINER_GET_TILE_HEIGHT getTileHeight;
    J2K_ICONTAINER_GET_WIDTH getWidth;
    J2K_ICONTAINER_GET_HEIGHT getHeight;
    J2K_ICONTAINER_GET_NUM_COMPONENTS getNumComponents;
    J2K_ICONTAINER_GET_COMPONENT_BYTES getComponentBytes;
    J2K_ICONTAINER_READ_TILE readTile;
    J2K_ICONTAINER_READ_REGION readRegion;
    J2K_ICONTAINER_DESTRUCT destruct;
} j2k_IContainer;

typedef struct _j2k_Container
{
    j2k_IContainer *iface;
    J2K_USER_DATA *data;
} j2k_Container;

/**
 * Opens a J2K Container from an IOInterface
 */
NRTAPI(j2k_Container*) j2k_Container_openIO(nrt_IOInterface*, nrt_Error*);

/**
 * Opens a J2K Container from a file on disk.
 */
NRTAPI(j2k_Container*) j2k_Container_open(const char*, nrt_Error*);

NRTAPI(nrt_Uint32) j2k_Container_getTilesX(j2k_Container*, nrt_Error*);
NRTAPI(nrt_Uint32) j2k_Container_getTilesY(j2k_Container*, nrt_Error*);
NRTAPI(nrt_Uint32) j2k_Container_getTileWidth(j2k_Container*, nrt_Error*);
NRTAPI(nrt_Uint32) j2k_Container_getTileHeight(j2k_Container*, nrt_Error*);
NRTAPI(nrt_Uint32) j2k_Container_getWidth(j2k_Container*, nrt_Error*);
NRTAPI(nrt_Uint32) j2k_Container_getHeight(j2k_Container*, nrt_Error*);
NRTAPI(nrt_Uint32) j2k_Container_getNumComponents(j2k_Container*, nrt_Error*);
NRTAPI(nrt_Uint32) j2k_Container_getComponentBytes(j2k_Container*, nrt_Error*);
NRTAPI(nrt_Uint32) j2k_Container_readTile(j2k_Container*, nrt_Uint32 tileX,
                                          nrt_Uint32 tileY, nrt_Uint8 **buf,
                                          nrt_Error*);
NRTAPI(nrt_Uint64) j2k_Container_readRegion(j2k_Container*, nrt_Uint32 x0,
                                          nrt_Uint32 y0, nrt_Uint32 x1,
                                          nrt_Uint32 y1, nrt_Uint8 **buf,
                                          nrt_Error*);
NRTAPI(void) j2k_Container_destruct(j2k_Container**);

NRT_CXX_ENDGUARD

#endif
