#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <iostream>
#include "dsf_file.h"

namespace dsf
{
    bool File::read(const char* name, std::vector<char>& buffer)
    {
        FILE* f = fopen(name, "rb");
        if (!f)
            return false;

        if (fseek(f, 0, SEEK_END) != 0)
        {
            fclose(f);
            return false;
        }
        long fsize = ftell(f);
        fseek(f, 0, SEEK_SET);
        if (fsize == 0)
        {
            fclose(f);
            return false;
        }

        buffer.resize(fsize);

        if (fread(buffer.data(), 1, fsize, f) != fsize)
        {
            fclose(f);
            return false;
        }
        fclose(f);
        return true;
    }

    bool File::extract(const std::vector<char>& src, std::vector<char>& dst)
    {
        // TODO: 7z extraction

        dst = src;
        return true;
    }

    File::File()
    {
    }

    bool File::open(const char* name)
    {
        std::vector<char> tmpbuffer;
        if (!read(name, tmpbuffer) || !extract(tmpbuffer, _buffer))
            return false;
        return true;
    }

    bool File::header_ok() const
    {
        return ::dsf::header_ok(_buffer.data(), _buffer.size());
    }

    bool File::md5sum_ok() const
    {
        return ::dsf::md5sum_ok(_buffer.data(), _buffer.size());
    }

    void File::prepare()
    {
        ::dsf::collect_atoms(_buffer.data(), _buffer.size(), _atoms);
    }

    bool File::exec(ICommand* cmd) const
    {
        return ::dsf::exec(_atoms, cmd);
    }
}
