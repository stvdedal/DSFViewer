#include "dsf2text.h"
#include "dsf_file.h"
#include <cstring>

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage:\ndfs2text <file.dsf> [-v]" << std::endl;
        return EXIT_FAILURE;
    }

    const char* name = argv[1];
    Dsf2Text cmd;
    dsf::File f;

    for (int i = 1; i < argc; ++i)
        if (strcmp(argv[i], "-v") == 0)
        {
            cmd.verbose = true;
            break;
        }


    std::cout << "I" << std::endl;
    std::cout << "800" << std::endl;
    std::cout << "DSF2TEXT" << std::endl;
    std::cout << std::endl;
    std::cout << "# file: " << name << std::endl;
    std::cout << std::endl;

    bool result = false;
    try
    {
        if (f.open(name) &&
            f.header_ok() &&
            f.md5sum_ok())
        {
            f.prepare();
            result = f.exec(&cmd);
        }
    }
    catch (...)
    {
        result = false;
    }

    if (result)
    {
        std::cout << "# Success" << std::endl;
        std::cerr << "SUCCESS" << std::endl;
    }
    else
    {
        std::cout << "# Fail" << std::endl;
        std::cerr << "FAIL" << std::endl;
    }
    getchar();
    return EXIT_SUCCESS;
}
