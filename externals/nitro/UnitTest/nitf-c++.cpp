#include "pch.h"
#include "CppUnitTest.h"

#include <nitf/coda-oss.hpp>
#include <io/ReadUtils.h>
#include <io/TempFile.h>
#include <io/FileOutputStream.h>

#include <import/nitf.hpp>
#include <nitf/UnitTests.hpp>
#include <nitf/TREField.hpp>
#include <nitf/TestingTest.hpp>
#include <nitf/J2KReader.hpp>
#include <nitf/J2KWriter.hpp>
#include <nitf/J2KCompressor.hpp>
#include <nitf/J2KCompressionParameters.hpp>
#include <nitf/CompressedByteProvider.hpp>
#include <nitf/ImageWriter.hpp>
#include <nitf/WriteHandler.hpp>
#include <nitf/IOHandle.hpp>
#include <nitf/IOStreamWriter.hpp>
#include <nitf/Reader.hpp>
#include <nitf/Utils.hpp>
#include <nitf/Writer.hpp>

namespace nitf_cpp
{

TEST_CLASS(test_create_nitf__){ public:
#include "nitf/unittests/test_create_nitf++.cpp"
};

TEST_CLASS(test_field__){ public:
#include "nitf/unittests/test_field++.cpp"
};

TEST_CLASS(test_hash_table_1__){ public:
#include "nitf/unittests/test_hash_table_1++.cpp"
};

TEST_CLASS(test_image_blocker){ public:
#include "nitf/unittests/test_image_blocker.cpp"
};

TEST_CLASS(test_image_loading__){ public:
#include "nitf/unittests/test_image_loading++.cpp"
};

TEST_CLASS(test_image_segment_blank_nm_compression){ public:
#include "nitf/unittests/test_image_segment_blank_nm_compression.cpp"
};

TEST_CLASS(test_image_segment_computer){ public:
#include "nitf/unittests/test_image_segment_computer.cpp"
};

TEST_CLASS(test_image_writer){ public:
#include "nitf/unittests/test_image_writer.cpp"
};

TEST_CLASS(test_j2k_compress_tile){ public:
#include "nitf/unittests/test_j2k_compress_tile.cpp"
};

TEST_CLASS(test_j2k_compressed_byte_provider){ public:
#include "nitf/unittests/test_j2k_compressed_byte_provider.cpp"
};

TEST_CLASS(test_j2k_loading__){ public:
#include "nitf/unittests/test_j2k_loading++.cpp"
};

TEST_CLASS(test_j2k_read_tile){ public:
#include "nitf/unittests/test_j2k_read_tile.cpp"
};

TEST_CLASS(test_load_plugins){ public:
#include "nitf/unittests/test_load_plugins.cpp"
};

TEST_CLASS(test_nitf_buffer_list){ public:
#include "nitf/unittests/test_nitf_buffer_list.cpp"
};

TEST_CLASS(test_tre_create__){ public:
#include "nitf/unittests/test_tre_create++.cpp"
};

TEST_CLASS(test_tre_mods){ public:
#include "nitf/unittests/test_tre_mods.cpp"
};

TEST_CLASS(test_tre_mods__){ public:
#include "nitf/unittests/test_tre_mods++.cpp"
};

TEST_CLASS(test_tre_read){ public:
#include "nitf/unittests/test_tre_read.cpp"
};

TEST_CLASS(test_writer_3__){ public:
#include "nitf/unittests/test_writer_3++.cpp"
};

}