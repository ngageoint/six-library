#include <import/sys.h>
#include <import/nitf.hpp>


class RecordThread : public sys::Thread
{
public:
    RecordThread() {}
    virtual ~RecordThread() {}
    //static sys::Mutex m;
    
    virtual void run()
    {
        nitf::Record record(NITF_VER_21);
        nitf::Writer writer;
        nitf::FileHeader header = record.getHeader();
        header.getFileHeader().set("NITF");
        header.getComplianceLevel().set("09");
        header.getSystemType().set("BF01");
        header.getOriginStationID().set("Bckyd");
        header.getFileTitle().set("FTITLE");
        header.getClassification().set("U");
        header.getMessageCopyNum().set("00000");
        header.getMessageNumCopies().set("00000");
        header.getEncrypted().set("0");
        header.getBackgroundColor().setRawData((char*)"000", 3);
        header.getOriginatorName().set("");
        header.getOriginatorPhone().set("");
        const std::string name = "ACFTB";
        //m.lock();
        nitf::TRE* acftb = new nitf::TRE(name, name);

        std::string file = str::toString<long>(sys::getThreadID()) + ".ntf";

        nitf::IOHandle output(file, NITF_ACCESS_WRITEONLY, NITF_CREATE);
        writer.prepare(output, record);

        writer.write();
    }

};


//sys::Mutex RecordThread::m;

const int NTHR = 2;
int main(int argc, char** argv)
{
    try
    {
        sys::Thread** thrs = new sys::Thread*[NTHR];

        for (unsigned int i = 0; i < NTHR; ++i)
        {
            thrs[i] = new RecordThread();
            thrs[i]->start();
        }
        

        for (unsigned int i = 0; i < NTHR; ++i)
        {
            
            thrs[i]->join();
            delete thrs[i];

        }

        delete [] thrs;

    }
    catch (except::Exception& ex)
    {
        std::cout << "Exception: " << ex.getMessage() << std::endl;
        std::cout << ex.getTrace() << std::endl;
    }
    return 0;
}
