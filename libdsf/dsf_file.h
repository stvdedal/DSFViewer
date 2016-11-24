#ifndef DSF_FILE_H_
#define DSF_FILE_H_

#include "dsf.h"
#include <vector>

namespace dsf
{
    class File
    {
        std::vector<char> _buffer;
        Atoms             _atoms;

        static bool read(const char* name, std::vector<char>& buffer);
        static bool extract(const std::vector<char>& src, std::vector<char>& dst);

        File(const File&) = delete;
        File& operator=(const File&) = delete;

    public:
        File();
        bool open(const char* name);
        bool header_ok() const;
        bool md5sum_ok() const;
        void prepare();
        bool exec(ICommand* cmd) const;
    };
}

#endif
