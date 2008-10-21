#include "cgm/LineAttributes.h"


NITFAPI(cgm_LineAttributes*) 
    cgm_LineAttributes_construct(nitf_Error* error)
{
    cgm_LineAttributes* atts = 
        (cgm_LineAttributes*)NITF_MALLOC(sizeof(cgm_LineAttributes));
    if (!atts)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                        NITF_ERR_MEMORY);
        return NULL;
    }

    atts->lineWidth = -1;
    atts->lineType = CGM_TYPE_NOT_SET;
    
    atts->lineColor = (short*)NITF_MALLOC(sizeof(short) * CGM_RGB);
    if (!atts->lineColor)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                        NITF_ERR_MEMORY);
        cgm_LineAttributes_destruct(&atts);
        return NULL;
    }
    atts->lineColor[CGM_R] = -1;
    atts->lineColor[CGM_G] = -1;
    atts->lineColor[CGM_B] = -1;
    
    return atts;
}

NITFAPI(void) cgm_LineAttributes_destruct(cgm_LineAttributes** atts)
{
    if (*atts)
    {
        if ((*atts)->lineColor)
        {
            NITF_FREE((*atts)->lineColor);
        }
        NITF_FREE( *atts );
        *atts = NULL;
    }
}

NITFAPI(void) cgm_LineAttributes_print(cgm_LineAttributes* atts)
{
    printf("\tLine Color: (%d, %d, %d)\n", 
	   atts->lineColor[CGM_R],
	   atts->lineColor[CGM_G],
	   atts->lineColor[CGM_B]);
    printf("\tLine Width [%d]\n", atts->lineWidth);
    printf("\tLine Type: [%d]\n", atts->lineType);

}
