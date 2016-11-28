#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <iostream>

#include <extract.h>
#include "dsf_file.h"

#ifdef _WIN32
#define FILE_SEPARATOR   '\\'
#else
#define FILE_SEPARATOR   '/'
#endif

namespace dsf
{
    std::string File::tmpDirectory;

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

    void File::setTmpDirectory(const std::string& tmpDir)
    {
        tmpDirectory = tmpDir;
    }

    File::File()
    {
    }

    bool File::open(const char* fullname)
    {
        unsigned char header[16];
        
        FILE* f = fopen(fullname, "rb");
        if (!f)
            return false;
        if (fread(header, 1, sizeof(header), f) != sizeof(header))
        {
            fclose(f);
            return false;
        }
        fclose(f);

        if (dsf::header_ok(header, sizeof(header)))
        {
            // dsf file is already unpacked
            return read(fullname, _buffer);
        }

        const char *dsfFile = strrchr(fullname, FILE_SEPARATOR);
        if (!dsfFile)
            dsfFile = fullname;
        
        if (tmpDirectory.empty())
        {
            std::cerr << "[Dsf] ERROR: temp path not set. Use \"File::setTmpDirectory(<some dir>)\"" << std::endl;
            return false;
        }

        char extractedFile[64];
        snprintf(extractedFile, sizeof(extractedFile), "%s\\%s", tmpDirectory.c_str(), dsfFile);

        if (!extract(fullname, tmpDirectory.c_str()))
        {
            std::cerr << "[Dsf] Error: file \"" << fullname << "\" not extracted to " << tmpDirectory << std::endl;
            std::remove(extractedFile);
            return false;
        }

        bool result = read(extractedFile, _buffer);
        std::remove(extractedFile);
        
        return result;
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
