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

    KernelDatabaseName(int inValue)
    {
        switch (inValue)
        {
        case 0:
            value = BILINEAR;
            break;
        case 1:
            value = CUBIC;
            break;
        case 2:
            value = LAGRANGE;
            break;
        case 3:
            value = NEAREST_NEIGHBOR;
            break;
        case six::NOT_SET_VALUE:
            value = NOT_SET;
            break;
        default:
            throw except::InvalidFormatException(Ctxt("Invalid enum value: " +
                    str::toString(inValue)));
        }
    }

    //! Returns string representation of the value
    std::string toString() const
    {
        switch (value)
        {
        case 0:
            return "BILINEAR";
        case 1:
            return "CUBIC";
        case 2:
            return "LAGRANGE";
        case 3:
            return "NEAREST_NEIGHBOR";
        case six::NOT_SET_VALUE:
            return "NOT_SET";
        default:
            throw except::InvalidFormatException(Ctxt("Invalid enum value: " +
                    str::toString(value)));
        }
    }

    int value;
};

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

    KernelOperation(int inValue)
    {
        switch (inValue)
        {
        case 0:
            value = CONVOLUTION;
            break;
        case 1:
            value = CORRELATION;
            break;
        case six::NOT_SET_VALUE:
            value = NOT_SET;
            break;
        default:
            throw except::InvalidFormatException(Ctxt("Invalid enum value: " +
                    str::toString(inValue)));
        }
    }

    //! Returns string representation of the value
    std::string toString() const
    {
        switch (value)
        {
        case 0:
            return "CONVOLUTION";
        case 1:
            return "CORRELATION";
        case six::NOT_SET_VALUE:
            return "NOT_SET";
        default:
            throw except::InvalidFormatException(Ctxt("Invalid enum value: " +
                    str::toString(value)));
        }
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

struct CoordinateSystemType
{
    enum
    {
        GCS = 0,
        UTM,
        NOT_SET = six::NOT_SET_VALUE
    };

    CoordinateSystemType() :
        value(NOT_SET)
    {
    }

    int value;
};
}
}

#endif
