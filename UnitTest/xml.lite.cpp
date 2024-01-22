#include "pch.h"
#include "CppUnitTest.h"

#include <xml/lite/Document.h>
#include <xml/lite/Element.h>
#include <xml/lite/QName.h>
#include <xml/lite/MinidomParser.h>
#include <xml/lite/Validator.h>

namespace xml_lite
{

TEST_CLASS(test_soapelements){ public:
#include "xml.lite/unittests/test_soapelements.cpp"
};

TEST_CLASS(test_xmlattribute){ public:
#include "xml.lite/unittests/test_xmlattribute.cpp"
};

TEST_CLASS(test_xmlcreate){ public:
#include "xml.lite/unittests/test_xmlcreate.cpp"
};

TEST_CLASS(test_xmlelement){ public:
#include "xml.lite/unittests/test_xmlelement.cpp"
};

TEST_CLASS(test_xmlparser){ public:
#include "xml.lite/unittests/test_xmlparser.cpp"
};

}