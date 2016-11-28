#ifndef DSF_FILE_H_
#define DSF_FILE_H_

#include "dsf.h"
#include <vector>
#include <string>

namespace dsf
{
    class File
    {
        std::vector<char> _buffer;
        Atoms             _atoms;

        static bool read(const char* name, std::vector<char>& buffer);

        File(const File&) = delete;
        File& operator=(const File&) = delete;

        static std::string tmpDirectory;

    public:
        static void setTmpDirectory(const std::string& tmpDir);

        File();
        bool open(const char* fullname);
        bool header_ok() const;
        bool md5sum_ok() const;
        void prepare();
        bool exec(ICommand* cmd) const;
    };
}

#endif
