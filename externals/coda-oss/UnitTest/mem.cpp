#include "pch.h"
#include "CppUnitTest.h"

#include <import/mem.h>
#include <mem/ScratchMemory.h>
#include <mem/SharedPtr.h>
#include <mem/ComplexView.h>

namespace mem
{

TEST_CLASS(test_scoped_cloneable_ptr){ public:
#include "mem/unittests/test_scoped_cloneable_ptr.cpp"
};

TEST_CLASS(test_scoped_copyable_ptr){ public:
#include "mem/unittests/test_scoped_copyable_ptr.cpp"
};

TEST_CLASS(test_scratch_memory){ public:
#include "mem/unittests/test_scratch_memory.cpp"
};

TEST_CLASS(test_shared_ptr){ public:
#include "mem/unittests/test_shared_ptr.cpp"
};

TEST_CLASS(test_Span){ public:
#include "mem/unittests/test_Span.cpp"
};

TEST_CLASS(test_unique_ptr){ public:
#include "mem/unittests/test_unique_ptr.cpp"
};

TEST_CLASS(test_vector_pointers){ public:
#include "mem/unittests/test_vector_pointers.cpp"
};

}