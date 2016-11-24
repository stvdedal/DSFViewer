#include <iostream>
#include <iomanip>
#include <string>

#include "dsf_file.h"

class DsfCommand : public dsf::ICommand
{
    unsigned int _depth;
public:
    DsfCommand()
        :_depth(0)
    {
    }

    virtual void accept_property(const std::string& name, const std::string& value)
    {
        std::cout << "PROPERTY\t" << name << '\t' << value << std::endl;
    }

    virtual void accept_terrain_def(const std::string& def)
    {
        std::cout << "TERRAIN_DEF\t" << def << std::endl;
    }

    virtual void accept_object_def(const std::string& def)
    {
        std::cout << "OBJECT: " << def << std::endl;
    }

    virtual void accept_polygon_def(const std::string& def)
    {
        std::cout << "POLYGON_DEF\t" << def << std::endl;
    }

    virtual void accept_network_def(const std::string& def)
    {
        std::cout << "NETWORK_DEF\t" << def << std::endl;
    }

    virtual void accept_raster_def(const std::string& def)
    {
        std::cout << "RASTER_DEF\t" << def << std::endl;
    }

    virtual void begin_polygon(unsigned int type, unsigned int param, unsigned int coord_depth)
    {
        std::cout << "BEGIN_POLYGON\t" << type << '\t' << param << '\t' << coord_depth << std::endl;
        _depth = coord_depth;
    };

    virtual void begin_polygon_winding()
    {
        std::cout << "BEGIN_WINDING" << std::endl;
    };

    virtual void add_polygon_point(const double* coord)
    {
        std::ios::fmtflags f(std::cout.flags());
        std::cout << "POLYGON_POINT\t";
        std::cout << std::fixed << std::setprecision(9) << std::setfill('0');
        for (unsigned int i = 0; i < _depth; ++i)
            std::cout << coord[i] << '\t';
        std::cout << std::endl;
        std::cout.flags(f);
    };

    virtual void end_polygon_winding()
    {
        std::cout << "END_WINDING" << std::endl;
    };

    virtual void end_polygon()
    {
        std::cout << "END_POLYGON" << std::endl;
    };

    virtual void begin_patch(
        unsigned int type,
        double near_LOD, double far_LOD,
        unsigned int flags,
        unsigned int coord_depth)
    {
        std::ios::fmtflags f(std::cout.flags());
        std::cout << std::fixed << std::setprecision(6) << std::setfill('0');
        std::cout << "BEGIN_PATCH\t"
            << type << '\t' << near_LOD << '\t' << far_LOD << '\t'
            << std::hex << flags << std::dec << '\t'
            << coord_depth << std::endl;
        std::cout.flags(f);

        _depth = coord_depth;
    }

    virtual void end_patch()
    {
        std::cout << "END_PATCH" << std::endl;
    }

    virtual void begin_primitive(dsf::Primitive_type type)
    {
        std::cout << "BEGIN_PRIMITIVE\t" << type << std::endl;
    }

    virtual void add_patch_vertex(const double* coord)
    {
        std::ios::fmtflags f(std::cout.flags());
        std::cout << "PATCH_VERTEX\t";
        std::cout << std::fixed << std::setprecision(9) << std::setfill('0');
        for (unsigned int i = 0; i < _depth; ++i)
            std::cout << coord[i] << "\t";
        std::cout << std::endl;
        std::cout.flags(f);
    }

    virtual void end_primitive()
    {
        std::cout << "END_PRIMITIVE" << std::endl;
    }

    virtual void begin_segment(unsigned int type, unsigned int subtype, const double* coord, bool curved)
    {
        std::ios::fmtflags f(std::cout.flags());
        std::cout << "BEGIN_SEGMENT\t" << type << '\t' << subtype << '\t';

        // longitude latitude elevation, start node ID, shape longitude latitude elevation
        // [0]       [1]      [2]        [3]                  [4]       [5]      [6]

        // start node ID
        std::cout << coord[3] << '\t';

        std::cout << std::fixed << std::setprecision(9) << std::setfill('0');

        // lon lat el
        for (unsigned int i = 0; i < 3; ++i)
            std::cout << coord[i] << '\t';

        // shape
        // lon lat el
        if (curved)
            for (unsigned int i = 4; i < 7; ++i)
                std::cout << coord[i] << '\t';

        std::cout << std::endl;
        std::cout.flags(f);
    }

    virtual void add_segment_shape_point(const double* coord, bool curved)
    {
        std::ios::fmtflags f(std::cout.flags());
        std::cout << "SHAPE_POINT\t";
        std::cout << std::fixed << std::setprecision(9) << std::setfill('0');
        for (unsigned int i = 0; i < (curved ? 6U : 3U); ++i)
            std::cout << coord[i] << '\t';
        std::cout << std::endl;
        std::cout.flags(f);
    }

    virtual void end_segment(const double* coord, bool curved)
    {
        std::ios::fmtflags f(std::cout.flags());
        std::cout << "END_SEGMENT\t";

        // longitude latitude elevation, start node ID, shape longitude latitude elevation
        // [0]       [1]      [2]        [3]                  [4]       [5]      [6]

        // start node ID
        std::cout << coord[3] << '\t';

        std::cout << std::fixed << std::setprecision(9) << std::setfill('0');

        // lon lat el
        for (unsigned int i = 0; i < 3; ++i)
            std::cout << coord[i] << '\t';

        // shape
        // lon lat el
        if (curved)
            for (unsigned int i = 4; i < 7; ++i)
                std::cout << coord[i] << '\t';

        std::cout << std::endl;
        std::cout.flags(f);
    }

};



int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage:\ndfs2text <file.dsf>" << std::endl;
        return EXIT_FAILURE;
    }

    const char* name = argv[1];
    DsfCommand cmd;

    std::cout << "I" << std::endl;
    std::cout << "800" << std::endl;
    std::cout << "DSF2TEXT" << std::endl;
    std::cout << std::endl;
    std::cout << "# file: " << name << std::endl;
    std::cout << std::endl;

    dsf::File f;
    if (f.open(name) && f.exec(&cmd))
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
