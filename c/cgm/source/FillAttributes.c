#include "cgm/FillAttributes.h"


NITFAPI(cgm_FillAttributes*) 
    cgm_FillAttributes_construct(nitf_Error* error)
{
    cgm_FillAttributes* atts = 
        (cgm_FillAttributes*)NITF_MALLOC(sizeof(cgm_FillAttributes));
    if (!atts)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                        NITF_ERR_MEMORY);
        return NULL;
    }

    atts->fillColor = (short*)NITF_MALLOC(sizeof(short) * CGM_RGB);
    if (!atts->fillColor)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                        NITF_ERR_MEMORY);
        cgm_FillAttributes_destruct(&atts);
        return NULL;
    }
    
    atts->edgeColor = (short*)NITF_MALLOC(sizeof(short) * CGM_RGB);
    if (!atts->edgeColor)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                        NITF_ERR_MEMORY);
        cgm_FillAttributes_destruct(&atts);
        return NULL;
    }
    
    atts->fillColor[CGM_R] = -1;
    atts->fillColor[CGM_G] = -1;
    atts->fillColor[CGM_B] = -1;

    atts->interiorStyle = CGM_IS_NOT_SET;
    atts->edgeVisibility = -1;
    atts->edgeWidth = -1;
    atts->edgeType = CGM_TYPE_NOT_SET;

    atts->edgeColor[CGM_R] = -1;
    atts->edgeColor[CGM_G] = -1;
    atts->edgeColor[CGM_B] = -1;

    return atts;
}

NITFAPI(void) cgm_FillAttributes_destruct(cgm_FillAttributes** atts)
{
    if (*atts)
    {
        if ((*atts)->fillColor)
        {
            NITF_FREE((*atts)->fillColor);
        }
        if ((*atts)->edgeColor)
        {
            NITF_FREE((*atts)->edgeColor);
        }
        NITF_FREE( *atts );
        *atts = NULL;
    }
}

NITFAPI(void) cgm_FillAttributes_print(cgm_FillAttributes* atts)
{
    printf("\tFill Color: (%d, %d, %d)\n", 
	   atts->fillColor[CGM_R],
	   atts->fillColor[CGM_G],
	   atts->fillColor[CGM_B]);
    printf("\tInterior Style [%d]\n", atts->interiorStyle);
    printf("\tEdge Visibility [%d]\n", atts->edgeVisibility);
    printf("\tEdge Width [%d]\n", atts->edgeWidth);
    printf("\tEdge Type [%d]\n", atts->edgeType);
    printf("\tEdge Color: (%d, %d, %d)\n", 
	   atts->edgeColor[CGM_R],
	   atts->edgeColor[CGM_G],
	   atts->edgeColor[CGM_B]);

}
