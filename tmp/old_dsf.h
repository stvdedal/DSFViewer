#ifndef DSF_PARSER_H_
#define DSF_PARSER_H_

#include <vector>
#include <map>

#include "dsf_chunk.h"

namespace dsf
{
    enum Atom_ID
    {
        atom_id_HEAD = 'HEAD',   // Atom of Atoms

        // The properties atom is a string table atom with an even number of strings;
        // each consecutive pair of strings represents a property name and a property value.
        atom_id_PROP = 'PROP',

        atom_id_DEFN = 'DEFN',   // Atom of Atoms
        atom_id_TERT = 'TERT',
        atom_id_OBJT = 'OBJT',
        atom_id_POLY = 'POLY',
        atom_id_NETW = 'NETW',

        // Raster Definition Atom
        atom_id_DEMN = 'DEMN',

        // Raster Data Atom
        // The raster data atom contains:
        //    one raster layer information ('DEMI')
        //    one raster layer data atom ('DEMD') for each raster layer
        // The order of information and data atoms must match with the raster definition atoms.
        // This is how names, meta data, and raw data are matched in the DSF
        atom_id_DEMS = 'DEMS',   // Atom of Atoms
        atom_id_DEMI = 'DEMI',
        atom_id_DEMD = 'DEMD',


        atom_id_GEOD = 'GEOD',   // Atom of Atoms
        atom_id_POOL = 'POOL',
        atom_id_SCAL = 'SCAL',
        atom_id_PO32 = 'PO32',
        atom_id_SC32 = 'SC32',

        atom_id_CMDS = 'CMDS',
    };

    enum Cmd_ID
    {
        cmd_id_Reserved = 0,

        cmd_id_PoolSelect = 1,
        cmd_id_JunctionOffsetSelect = 2,
        cmd_id_SetDefinition8 = 3,
        cmd_id_SetDefinition16 = 4,
        cmd_id_SetDefinition32 = 5,
        cmd_id_SetRoadSubtype8 = 6,

        cmd_id_Object = 7,
        cmd_id_ObjectRange = 8,

        cmd_id_NetworkChain = 9,
        cmd_id_NetworkChainRange = 10,
        cmd_id_NetworkChain32 = 11,
        cmd_id_Polygon = 12,
        cmd_id_PolygonRange = 13,
        cmd_id_NestedPolygon = 14,
        cmd_id_NestedPolygonRange = 15,

        cmd_id_TerrainPatch = 16,
        cmd_id_TerrainPatchFlags = 17,
        cmd_id_TerrainPatchFlagsLOD = 18,

        cmd_id_Triangle = 23,
        cmd_id_TriangleCrossPool = 24,
        cmd_id_TriangleRange = 25,
        cmd_id_TriangleStrip = 26,
        cmd_id_TriangleStripCrossPool = 27,
        cmd_id_TriangleStripRange = 28,
        cmd_id_TriangleFan = 29,
        cmd_id_TriangleFanCrossPool = 30,
        cmd_id_TriangleFanRange = 31,

        cmd_id_Comment8 = 32,
        cmd_id_Comment16 = 33,
        cmd_id_Comment32 = 34,
    };

    enum Primitive_type
    {
        primitive_Tri,
        primitive_TriStrip,
        primitive_TriFan
    };

    // high level commands
    class ICommand
    {
    public:
        virtual ~ICommand() {}

        virtual void accept_property(const std::string& name, const std::string& value) {}
        virtual void accept_terrain_def(const std::string& def) {}
        virtual void accept_object_def(const std::string& def) {}
        virtual void accept_polygon_def(const std::string& def) {}
        virtual void accept_network_def(const std::string& def) {}
        virtual void accept_raster_def(const std::string& def) {}

        virtual void begin_polygon(unsigned int type, unsigned int param, unsigned int coord_depth) {}
        virtual void begin_polygon_winding() {}
        virtual void add_polygon_point(const double* coord) {}
        virtual void end_polygon_winding() {}
        virtual void end_polygon() {}

        virtual void begin_patch(
            unsigned int type,
            double near_LOD, double far_LOD,
            unsigned int flags,
            unsigned int coord_depth) {}
        virtual void end_patch() {}

        virtual void begin_primitive(Primitive_type type) {}
        virtual void add_patch_vertex(const double* coord) {}
        virtual void end_primitive() {}

        virtual void begin_segment(unsigned int type, unsigned int subtype, const double* coord, bool curved) {}
        virtual void add_segment_shape_point(const double* coord, bool curved) {}
        virtual void end_segment(const double* coord, bool curved) {}
    };

    // low level commands



    typedef std::vector<MemChunk> Chunks;
    typedef std::map<Atom_ID, Chunks> Atoms;

    bool header_ok(const void* buffer, size_t size);
    bool md5sum_ok(const void* buffer, size_t size);
    void collect_atoms(const void* buffer, size_t size, Atoms& atoms);
    bool get_plane_16(const Atoms& atoms, std::vector<std::vector<double>>& data, std::vector<unsigned int>& depth);
    bool get_plane_32(const Atoms& atoms, std::vector<std::vector<double>>& data, std::vector<unsigned int>& depth);

    bool exec(
        const Atoms& atoms,
        const std::vector<std::vector<double>>& data16, const std::vector<unsigned int>& depth16,
        const std::vector<std::vector<double>>& data32, const std::vector<unsigned int>& depth32,
        ICommand* cmd);

    bool exec(
        const void* buffer, size_t size,
        ICommand* cmd);
}
#endif
