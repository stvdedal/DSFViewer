#include "dsf2text.h"

void Dsf2Text::begin(const dsf::Atoms& atoms)
{
    get_plane_16(atoms, _data16, _depth16);
    get_plane_32(atoms, _data32, _depth32);

    get_properties(atoms, [](auto& v)
    {
        std::cout << "PROPERTY\t" << v.first << '\t' << v.second << std::endl;
    });
    get_terrain_def(atoms, [](auto& s)
    {
        std::cout << "TERRAIN_DEF\t" << s << std::endl;
    });
    get_object_def(atoms, [](auto& s)
    {
        std::cout << "OBJECT_DEF\t" << s << std::endl;
    });
    get_polygon_def(atoms, [](auto& s)
    {
        std::cout << "POLYGON_DEF\t" << s << std::endl;
    });
    get_network_def(atoms, [](auto& s)
    {
        std::cout << "NETWORK_DEF\t" << s << std::endl;
    });
    get_raster_def(atoms, [](auto& s)
    {
        std::cout << "RASTER_DEF\t" << s << std::endl;
    });
}

void Dsf2Text::end(const dsf::Atoms& atoms)
{
}

void Dsf2Text::polygon_begin(unsigned int definition, unsigned int param, unsigned int pool)
{
    _pool_index = pool;
    std::cout << "BEGIN_POLYGON\t" << definition << '\t' << param << '\t' << _depth16.at(_pool_index) << std::endl;
}

void Dsf2Text::polygon_winding_begin()
{
    std::cout << "BEGIN_WINDING" << std::endl;
}

void Dsf2Text::polygon_points(unsigned int first, unsigned int end)
{
    const double* p = _data16.at(_pool_index).data();
    unsigned int d = _depth16.at(_pool_index);

    for (unsigned int index = first; index < end; ++index)
    {
        std::cout << "POLYGON_POINT\t";
        set_format();
        for (unsigned int i = 0; i < d; ++i)
            std::cout << p[index * d + i] << '\t';
        if (verbose)
            std::cout << "# pool=" << _pool_index << " point=" << index << " depth=" << d;
        std::cout << std::endl;
        restore_format();
    }
}

void Dsf2Text::polygon_winding_end()
{
    std::cout << "END_WINDING" << std::endl;
}

void Dsf2Text::polygon_end()
{
    std::cout << "END_POLYGON" << std::endl;
}

void Dsf2Text::patch_begin(
    unsigned int definition,
    unsigned int patch_flags, double patch_LOD_nea, double patch_LOD_far,
    unsigned int pool)
{
    _pool_index = pool;

    std::cout << "BEGIN_PATCH\t";

    set_format(6);
    std::cout
        << definition << '\t' << patch_LOD_nea << '\t' << patch_LOD_far << '\t'
        << std::hex << patch_flags << std::dec << '\t'
        << _depth16.at(_pool_index) << std::endl;
    restore_format();
}

void Dsf2Text::patch_primitive_begin(dsf::Primitive_type type)
{
    std::cout << "BEGIN_PRIMITIVE\t" << type << std::endl;
}

void Dsf2Text::patch_primitive_vertex(unsigned int pool, unsigned int index)
{
    const double* p = _data16.at(pool).data();
    unsigned int d = _depth16.at(pool);

    std::cout << "PATCH_VERTEX\t";
    set_format();
    for (unsigned int i = 0; i < d; ++i)
        std::cout << p[index * d + i] << "\t";
    if (verbose)
        std::cout << "# pool=" << pool << " point=" << index << " depth=" << d;
    restore_format();
    std::cout << std::endl;
}

void Dsf2Text::patch_primitive_end()
{
    std::cout << "END_PRIMITIVE" << std::endl;
}

void Dsf2Text::patch_end()
{
    std::cout << "END_PATCH" << std::endl;
}

void Dsf2Text::segment(
    unsigned int definition,
    unsigned int road_subtype,
    unsigned int pool, unsigned int begin, unsigned int end)
{
    _pool_index = pool;

    const double* p = _data32.at(_pool_index).data();
    unsigned int d = _depth32.at(_pool_index);
    bool curved = d >= 7;

    for (unsigned int index = begin; index < end; ++index)
    {
        // longitude latitude elevation, start node ID, shape longitude latitude elevation
        // [0]       [1]      [2]        [3]                  [4]       [5]      [6]

        const double* coord = &p[index * d];

        if (coord[3] > 0.000000000001)
        {
            if (index != begin)
            {
                std::cout << "END_SEGMENT\t";

                // start node ID
                std::cout << coord[3] << '\t';

                set_format();
                // lon lat el
                for (unsigned int i = 0; i < 3; ++i)
                    std::cout << coord[i] << '\t';

                // shape
                // lon lat el
                if (curved)
                    for (unsigned int i = 4; i < 7; ++i)
                        std::cout << coord[i] << '\t';
                restore_format();
                std::cout << std::endl;
            }
            if (index != (end - 1))
            {
                std::cout << "BEGIN_SEGMENT\t" << definition << '\t' << road_subtype << '\t';

                // start node ID
                std::cout << coord[3] << '\t';

                set_format();
                // lon lat el
                for (unsigned int i = 0; i < 3; ++i)
                    std::cout << coord[i] << '\t';

                // shape
                // lon lat el
                if (curved)
                    for (unsigned int i = 4; i < 7; ++i)
                        std::cout << coord[i] << '\t';
                restore_format();
                std::cout << std::endl;
            }
        }
        else
        {
            std::cout << "SHAPE_POINT\t";
            set_format();
            for (unsigned int i = 0; i < (curved ? 6U : 3U); ++i)
                std::cout << coord[i] << '\t';
            restore_format();
            std::cout << std::endl;
        }
    }
}
