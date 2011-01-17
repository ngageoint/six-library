#include "j2k/Container.h"

NRTAPI(nrt_Uint32) j2k_Container_getTilesX(j2k_Container *container, nrt_Error *error)
{
    return container->iface->getTilesX(container->data, error);
}

NRTAPI(nrt_Uint32) j2k_Container_getTilesY(j2k_Container *container, nrt_Error *error)
{
    return container->iface->getTilesY(container->data, error);
}

NRTAPI(nrt_Uint32) j2k_Container_getTileWidth(j2k_Container *container, nrt_Error *error)
{
    return container->iface->getTileWidth(container->data, error);
}

NRTAPI(nrt_Uint32) j2k_Container_getTileHeight(j2k_Container *container, nrt_Error *error)
{
    return container->iface->getTileHeight(container->data, error);
}

NRTAPI(nrt_Uint32) j2k_Container_getWidth(j2k_Container *container, nrt_Error *error)
{
    return container->iface->getWidth(container->data, error);
}

NRTAPI(nrt_Uint32) j2k_Container_getHeight(j2k_Container *container, nrt_Error *error)
{
    return container->iface->getHeight(container->data, error);
}

NRTAPI(nrt_Uint32) j2k_Container_getNumComponents(j2k_Container *container, nrt_Error *error)
{
    return container->iface->getNumComponents(container->data, error);
}

NRTAPI(nrt_Uint32) j2k_Container_getComponentBytes(j2k_Container *container, nrt_Error *error)
{
    return container->iface->getComponentBytes(container->data, error);
}

NRTAPI(nrt_Uint32) j2k_Container_readTile(j2k_Container *container,
        nrt_Uint32 tileX, nrt_Uint32 tileY,
        nrt_Uint8 **buf, nrt_Error *error)
{
    return container->iface->readTile(container->data, tileX, tileY, buf, error);
}

NRTAPI(nrt_Uint64) j2k_Container_readRegion(j2k_Container *container,
        nrt_Uint32 x0, nrt_Uint32 y0, nrt_Uint32 x1, nrt_Uint32 y1,
        nrt_Uint8 **buf, nrt_Error *error)
{
    return container->iface->readRegion(container->data, x0, y0, x1, y1, buf, error);
}

NRTAPI(void)
j2k_Container_destruct(j2k_Container **container)
{
    if (*container)
    {
        if ((*container)->iface)
            (*container)->iface->destruct((*container)->data);
        NRT_FREE(*container);
        *container = NULL;
    }
}

