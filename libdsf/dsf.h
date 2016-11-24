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

    typedef std::vector<MemChunk> Chunks;
    typedef std::map<Atom_ID, Chunks> Atoms;

    bool header_ok(const void* buffer, size_t size);
    bool md5sum_ok(const void* buffer, size_t size);
    void collect_atoms(const void* buffer, size_t size, Atoms& atoms);

    template<typename Fn>
    bool get_properties(const Atoms& atoms, Fn output)
    {
        std::map<Atom_ID, Chunks>::const_iterator aiter;
        std::vector<MemChunk>::const_iterator citer;
        aiter = atoms.find(atom_id_PROP);
        if (aiter == atoms.end())
            return false;
        for (citer = aiter->second.begin(); citer != aiter->second.end(); ++citer)
        {
            typedef std::pair<std::string, std::string> StrPair;
            StringTable::Cookie cookie;
            StringTable string_table(*citer);
            bool is_end;
            for (StrPair sp = string_table.get_first_pair(&is_end, cookie);
                !is_end;
                sp = string_table.get_next_pair(&is_end, cookie))
            {
                output(sp);
            }
        }
        return true;
    }

    template<typename Fn>
    bool get_strings(Atom_ID id, const Atoms& atoms, Fn output)
    {
        std::map<Atom_ID, Chunks>::const_iterator aiter;
        std::vector<MemChunk>::const_iterator citer;
        aiter = atoms.find(id);
        if (aiter == atoms.end())
            return false;
        for (citer = aiter->second.begin(); citer != aiter->second.end(); ++citer)
        {
            StringTable::Cookie cookie;
            StringTable string_table(*citer);
            bool is_end;
            for (std::string s = string_table.get_first(&is_end, cookie);
                !is_end;
                s = string_table.get_next(&is_end, cookie))
            {
                output(s);
            }
        }
        return true;
    }

    template<typename Fn>
    bool get_terrain_def(const Atoms& atoms, Fn fn) { return get_strings(atom_id_TERT, atoms, fn); }
    template<typename Fn>
    bool get_object_def(const Atoms& atoms, Fn fn) { return get_strings(atom_id_OBJT, atoms, fn); }
    template<typename Fn>
    bool get_polygon_def(const Atoms& atoms, Fn fn) { return get_strings(atom_id_POLY, atoms, fn); }
    template<typename Fn>
    bool get_network_def(const Atoms& atoms, Fn fn) { return get_strings(atom_id_NETW, atoms, fn); }
    template<typename Fn>
    bool get_raster_def(const Atoms& atoms, Fn fn) { return get_strings(atom_id_DEMN, atoms, fn); }

    bool get_plane_16(const Atoms& atoms, std::vector<std::vector<double>>& data, std::vector<unsigned int>& depth);
    bool get_plane_32(const Atoms& atoms, std::vector<std::vector<double>>& data, std::vector<unsigned int>& depth);

    enum Primitive_type
    {
        primitive_Tri,
        primitive_TriStrip,
        primitive_TriFan
    };

    class ICommand
    {
    public:
        virtual ~ICommand() {}

        virtual void begin(const Atoms& atoms) = 0;
        virtual void end(const Atoms& atoms) = 0;

        virtual void polygon_begin(unsigned int definition, unsigned int param, unsigned int pool) = 0;
        virtual void polygon_winding_begin() = 0;
        virtual void polygon_points(unsigned int first, unsigned int end) = 0;
        virtual void polygon_winding_end() = 0;
        virtual void polygon_end() = 0;

        virtual void patch_begin(
            unsigned int definition,
            unsigned int patch_flags, double patch_LOD_nea, double patch_LOD_far,
            unsigned int pool) = 0;
        virtual void patch_primitive_begin(Primitive_type type) = 0;
        virtual void patch_primitive_vertex(unsigned int pool, unsigned int index) = 0;
        virtual void patch_primitive_end() = 0;
        virtual void patch_end() = 0;

        virtual void segment(
            unsigned int definition,
            unsigned int road_subtype,
            unsigned int pool, unsigned int begin, unsigned int end) = 0;
    };

    bool exec(const Atoms& atoms, ICommand* cmd);
}
#endif
