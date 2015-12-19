#ifndef __SIX_SIDD_ENUMS_H__
#define __SIX_SIDD_ENUMS_H__

namespace six
{
namespace sidd
{
struct KernelDatabaseName
{
    enum
    {
        BILINEAR = 0,
        CUBIC,
        LAGRANGE,
        NEAREST_NEIGHBOR,
        NOT_SET = six::NOT_SET_VALUE
    };

    KernelDatabaseName() :
        value(NOT_SET)
    {
    }

    int value;
};
}

struct KernelCustomType
{
    enum
    {
        GENERAL = 0,
        FILTER_BANK,
        NOT_SET = six::NOT_SET_VALUE
    };

    KernelCustomType() :
        value(NOT_SET)
    {
    }

    int value;
};

struct KernelOperation
{
    enum
    {
        CONVOLUTION = 0,
        CORRELATION,
        NOT_SET = six::NOT_SET_VALUE
    };

    KernelOperation() :
        value(NOT_SET)
    {
    }

    int value;
};

struct BandEqualizationAlgorithm
{
    enum
    {
        LUT_1D = 0,
        NOT_SET = six::NOT_SET_VALUE
    };

    BandEqualizationAlgorithm() :
        value(NOT_SET)
    {
    }

    int value;
};

struct DownsamplingMethod
{
    enum
    {
        DECIMATE = 0,
        MAX_PIXEL,
        AVERAGE,
        NOT_SET = six::NOT_SET_VALUE
    };

    DownsamplingMethod() :
        value(NOT_SET)
    {
    }

    int value;
};

struct DerivedOrientationType
{
    enum
    {
        UP = 0,
        SHADOWS_DOWN,
        NORTH,
        SOUTH,
        EAST,
        WEST,
        ANGLE,
        NOT_SET = six::NOT_SET_VALUE
    };

    DerivedOrientationType() :
        value(NOT_SET)
    {
    }

    int value;
};

struct RenderingIntent
{
    enum
    {
        PERCEPTUAL = 0,
        SATURATION,
        RELATIVE_INTENT,
        ABSOLUTE_INTENT,
        NOT_SET = six::NOT_SET_VALUE
    };

    RenderingIntent() :
        value(NOT_SET)
    {
    }

    int value;
};

struct DRAType
{
    enum
    {
        AUTO = 0,
        MANUAL,
        NONE,
        NOT_SET = six::NOT_SET_VALUE
    };

    DRAType() :
        value(NOT_SET)
    {
    }

    int value;
};
}

#endif
