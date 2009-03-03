#ifndef __NITF_TRE_DESCRIPTION_H__
#define __NITF_TRE_DESCRIPTION_H__


/*!
 * The TREDescription structure encapsulates the metadata that describes one
 * field description in a TRE.
 */
typedef struct _nitf_TREDescription
{
    int data_type;                  /*!< the type of field */
    int data_count;                 /*!< the size of the field */
    char *label;                    /*!< description */
    char *tag;                      /*!< unique tag */
    char *special;                  /*!< special field, reserved for special cases */
} nitf_TREDescription;


#define NITF_TRE_DESC_NO_LENGTH      -1

/*!
 * Information about one TREDescription object
 */
typedef struct _nitf_TREDescriptionInfo
{
    char *name; /*! The name to associate with the Description */
    nitf_TREDescription *description;   /*! The TREDescription */
    int lengthMatch;    /*! The length to match against TREs with; used to choose TREs */
} nitf_TREDescriptionInfo;

/*!
 * Contains a set, or array, of TREDescriptionInfos*
 */
typedef struct _nitf_TREDescriptionSet
{
    int defaultIndex;   /*! Set to the index of the default description */
    nitf_TREDescriptionInfo* descriptions;
} nitf_TREDescriptionSet;


#endif
