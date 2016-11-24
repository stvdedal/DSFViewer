#ifndef DSF2TEXT_H_
#define DSF2TEXT_H_

#include "dsf.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

class Dsf2Text : public dsf::ICommand
{
    std::vector<std::vector<double>> _data16;
    std::vector<unsigned int>        _depth16;
    std::vector<std::vector<double>> _data32;
    std::vector<unsigned int>        _depth32;

    unsigned int _pool_index;

    std::ios::fmtflags flags;
    void set_format(int precision = 9)
    {
        flags = std::cout.flags();
        std::cout << std::fixed << std::setprecision(precision) << std::setfill('0');
    }

    void restore_format()
    {
        std::cout.flags(flags);
    }

public:
    Dsf2Text()
        :
        _pool_index(0),
        verbose(false)
    {
    }

    bool verbose;

    /* dsf::ICommand */
    virtual void begin(const dsf::Atoms& atoms);
    virtual void end(const dsf::Atoms& atoms);

    virtual void polygon_begin(unsigned int definition, unsigned int param, unsigned int pool);
    virtual void polygon_winding_begin();
    virtual void polygon_points(unsigned int first, unsigned int end);
    virtual void polygon_winding_end();
    virtual void polygon_end();

    virtual void patch_begin(
        unsigned int definition,
        unsigned int patch_flags, double patch_LOD_nea, double patch_LOD_far,
        unsigned int pool);
    virtual void patch_primitive_begin(dsf::Primitive_type type);
    virtual void patch_primitive_vertex(unsigned int pool, unsigned int index);
    virtual void patch_primitive_end();
    virtual void patch_end();

    virtual void segment(
        unsigned int definition,
        unsigned int road_subtype,
        unsigned int pool, unsigned int begin, unsigned int end);

};

#endif
