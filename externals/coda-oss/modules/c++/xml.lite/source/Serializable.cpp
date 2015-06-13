#include "xml/lite/Serializable.h"

void xml::lite::Serializable::serialize(io::OutputStream& os)
{
    xml::lite::Element *root = getDocument()->getRootElement();
    if (root != NULL)
        os.write("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
    root->print(os);
}

void xml::lite::Serializable::deserialize(io::InputStream& is)
{
    //EVAL(is.available());
    mParser.parse(is);
}
