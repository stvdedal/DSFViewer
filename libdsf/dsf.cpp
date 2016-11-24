#include "dsf.h"
#include <iostream>

namespace dsf
{
    bool header_ok(const void* buffer, size_t size)
    {
        bool retval = false;
        MemChunk entire(buffer, size);

        try
        {
            if (entire.get_at<char>(0) == 'X' &&
                entire.get_at<char>(1) == 'P' &&
                entire.get_at<char>(2) == 'L' &&
                entire.get_at<char>(3) == 'N' &&
                entire.get_at<char>(4) == 'E' &&
                entire.get_at<char>(5) == 'D' &&
                entire.get_at<char>(6) == 'S' &&
                entire.get_at<char>(7) == 'F' &&
                entire.get_at<uint32_t>(8) == 1)
            {
                retval = true;
            }
        }
        catch (...)
        {
            retval = false;
        }
        return retval;
    }

    bool md5sum_ok(const void* buffer, size_t size)
    {
        // TODO:
        return true;
    }

    void collect_atoms(const AtomChunks& atom_area, Atoms& atoms)
    {
        for (Atom atom = atom_area.get_first(); !atom.empty(); atom = atom_area.get_next(atom)) {
            uint32_t id = atom.id();
            MemChunk data = atom.data();
            if (id == atom_id_HEAD ||
                id == atom_id_DEMS ||
                id == atom_id_DEFN ||
                id == atom_id_GEOD)
            {
                collect_atoms(data, atoms);
            }
            else
            {
                atoms[Atom_ID(id)].push_back(data);
            }
        }
    }

    void collect_atoms(const void* buffer, size_t size, Atoms& atoms)
    {
        const uint8_t* p = static_cast<const uint8_t*>(buffer);
        collect_atoms(MemChunk(p + 12, p + size - 16), atoms);
    }

    bool get_properties(const Atoms& atoms, std::map<std::string, std::string>& properties)
    {
        return get_properties(atoms, [&properties](auto& v) { properties.insert(v); });
    }

    bool get_properties(const Atoms& atoms, std::vector<std::pair<std::string, std::string>>& properties)
    {
        return get_properties(atoms, [&properties](auto& v) { properties.push_back(v); });
    }

    bool get_strings(Atom_ID id, const Atoms& atoms, std::vector<std::string>& def)
    {
        return get_strings(id, atoms, [&def](auto& v) { def.push_back(v); });
    }

    bool get_terrain_def(const Atoms& atoms, std::vector<std::string>& def)
    {
        return get_strings(atom_id_TERT, atoms, def);
    }

    bool get_object_def(const Atoms& atoms, std::vector<std::string>& def)
    {
        return get_strings(atom_id_OBJT, atoms, def);
    }

    bool get_polygon_def(const Atoms& atoms, std::vector<std::string>& def)
    {
        return get_strings(atom_id_POLY, atoms, def);
    }

    bool get_network_def(const Atoms& atoms, std::vector<std::string>& def)
    {
        return get_strings(atom_id_NETW, atoms, def);
    }

    bool get_raster_def(const Atoms& atoms, std::vector<std::string>& def)
    {
        return get_strings(atom_id_DEMN, atoms, def);
    }

    template<class T>
    void get_plane(
        const Chunks& scal_chunks,
        const Chunks& pool_chunks,
        std::vector<std::vector<double>>& data,
        std::vector<unsigned int>& depth)
    {
        std::vector<std::vector<double>> scales;
        std::vector<std::vector<double>> offsets;

        if (scal_chunks.size() != pool_chunks.size())
            throw BadFormatException();

        std::size_t atom_count = scal_chunks.size();

        for (std::size_t i = 0; i < atom_count; ++i)
        {
            const MemChunk& chunk = scal_chunks[i];

            scales.push_back(std::vector<double>());
            offsets.push_back(std::vector<double>());

            if (chunk.size<float>() % 2 != 0)
                throw BadFormatException();
            for (std::size_t j = 0; j < chunk.size<float>(); j += 2)
            {
                scales.back().push_back(chunk.get<float>(j + 0));
                offsets.back().push_back(chunk.get<float>(j + 1));
            }
        }

        for (std::size_t i = 0; i < atom_count; ++i)
        {
            PlanarNumericTable pool_numeric_table(pool_chunks[i]);
            std::size_t array_size = pool_numeric_table.array_size();
            std::size_t plane_count = pool_numeric_table.plane_count();

            data.push_back(std::vector<double>());
            data.back().resize(array_size * plane_count);
            depth.push_back(static_cast<unsigned int>(plane_count));

            decode_PlanarNumericTable_InterleavedScaled<T, double>(
                pool_numeric_table,
                scales[i],
                offsets[i],
                data[i]);
        }
    }

    bool get_plane_16(const Atoms& atoms, std::vector<std::vector<double>>& data, std::vector<unsigned int>& depth)
    {
        std::map<Atom_ID, Chunks>::const_iterator aiter;
        aiter = atoms.find(atom_id_SCAL);
        if (aiter == atoms.end())
            return false;
        const std::vector<MemChunk>& scal_chunks = aiter->second;
        aiter = atoms.find(atom_id_POOL);
        if (aiter == atoms.end())
            return false;
        const std::vector<MemChunk>& pool_chunks = aiter->second;

        get_plane<uint16_t>(scal_chunks, pool_chunks, data, depth);
        return true;
    }

    bool get_plane_32(const Atoms& atoms, std::vector<std::vector<double>>& data, std::vector<unsigned int>& depth)
    {
        std::map<Atom_ID, Chunks>::const_iterator aiter;
        aiter = atoms.find(atom_id_SC32);
        if (aiter == atoms.end())
            return false;
        const std::vector<MemChunk>& scal_chunks = aiter->second;
        aiter = atoms.find(atom_id_PO32);
        if (aiter == atoms.end())
            return false;
        const std::vector<MemChunk>& pool_chunks = aiter->second;

        get_plane<uint32_t>(scal_chunks, pool_chunks, data, depth);
        return true;
    }

#define CMD_NOT_IMPL                 \
do {    \
    std::cerr << "Command " << (int)cmd_id << " NOT IMPLEMENTED" << std::endl; \
    return false;   \
} while (0)


    bool exec(const Atoms& atoms, ICommand* cmd)
    {
        cmd->begin(atoms);

        std::map<Atom_ID, Chunks>::const_iterator aiter;
        aiter = atoms.find(atom_id_CMDS);
        if (aiter == atoms.end())
            return false;

        const MemChunk& chunk = aiter->second.back();
        std::size_t cmd_size = chunk.size<char>();
        std::size_t pos = 0;
        int ncmd = 0;

        /* States */
        unsigned int pool_index = 0;
        unsigned int junction_offset = 0;
        unsigned int definition = 0;
        unsigned int road_subtype = 0;
        unsigned int patch_flags = 0;
        double patch_LOD_nea = -1.0;
        double patch_LOD_far = -1.0;
        bool patch_open = false;

        while (pos < cmd_size)
        {
            ++ncmd;

            uint8_t cmd_id = read<uint8_t>(chunk, pos);
            switch (cmd_id)
            {
                // -----------------------------
                // State Selection Commands
                // -----------------------------
            case cmd_id_PoolSelect:
                pool_index = read<uint16_t>(chunk, pos);
                break;
            case cmd_id_JunctionOffsetSelect:
                junction_offset = read<uint32_t>(chunk, pos);
                break;
            case cmd_id_SetDefinition8:
                definition = read<uint8_t>(chunk, pos);
                break;
            case cmd_id_SetDefinition16:
                definition = read<uint16_t>(chunk, pos);
                break;
            case cmd_id_SetDefinition32:
                definition = read<uint32_t>(chunk, pos);
                break;
            case cmd_id_SetRoadSubtype8:
                road_subtype = read<uint8_t>(chunk, pos);
                break;

                // -----------------------------
                // OBJECT COMMANDS
                // -----------------------------
            case cmd_id_Object:
                CMD_NOT_IMPL;
                break;
            case cmd_id_ObjectRange:
                CMD_NOT_IMPL;
                break;

                // -----------------------------
                // NETWORK COMMANDS
                // -----------------------------
            case cmd_id_NetworkChain:
                CMD_NOT_IMPL;
                break;
            case cmd_id_NetworkChainRange:
            {
                unsigned int index1 = junction_offset + read<uint16_t>(chunk, pos);
                unsigned int index2 = junction_offset + read<uint16_t>(chunk, pos);
                cmd->segment(definition, road_subtype, pool_index, index1, index2);
            }
            break;

            case cmd_id_NetworkChain32:
                CMD_NOT_IMPL;
                break;

                // -----------------------------
                // POLYGON COMMANDS
                // -----------------------------
            case cmd_id_Polygon:
                CMD_NOT_IMPL;
                break;
            case cmd_id_PolygonRange:
            {
                unsigned int param = read<uint16_t>(chunk, pos);
                unsigned int index1 = read<uint16_t>(chunk, pos);
                unsigned int index2 = read<uint16_t>(chunk, pos);
                cmd->polygon_begin(definition, param, pool_index);
                cmd->polygon_winding_begin();
                cmd->polygon_points(index1, index2);
                cmd->polygon_winding_end();
                cmd->polygon_end();
            }
            break;

            case cmd_id_NestedPolygon:
                CMD_NOT_IMPL;
                break;
            case cmd_id_NestedPolygonRange:
            {
                unsigned int param = read<uint16_t>(chunk, pos);
                unsigned int count = read<uint8_t>(chunk, pos);
                unsigned int index1 = read<uint16_t>(chunk, pos);
                cmd->polygon_begin(definition, param, pool_index);
                while (count--)
                {
                    unsigned int index2 = read<uint16_t>(chunk, pos);
                    cmd->polygon_winding_begin();
                    cmd->polygon_points(index1, index2);
                    cmd->polygon_winding_end();
                    index1 = index2;
                }
                cmd->polygon_end();
            }
            break;

            // -----------------------------
            // TERRAIN COMMANDS
            // -----------------------------
            case cmd_id_TerrainPatch:
                if (patch_open)
                    cmd->patch_end();
                patch_open = true;
                cmd->patch_begin(definition, patch_flags, patch_LOD_nea, patch_LOD_far, pool_index);
                break;
            case cmd_id_TerrainPatchFlags:
                if (patch_open)
                    cmd->patch_end();
                patch_open = true;
                patch_flags = read<uint8_t>(chunk, pos);
                cmd->patch_begin(definition, patch_flags, patch_LOD_nea, patch_LOD_far, pool_index);
                break;
            case cmd_id_TerrainPatchFlagsLOD:
                if (patch_open)
                    cmd->patch_end();
                patch_open = true;
                patch_flags = read<uint8_t>(chunk, pos);
                patch_LOD_nea = read<float>(chunk, pos);
                patch_LOD_far = read<float>(chunk, pos);
                cmd->patch_begin(definition, patch_flags, patch_LOD_nea, patch_LOD_far, pool_index);
                break;

                // -----------------------------
                // TRIANGLE COMMANDS
                // -----------------------------
            case cmd_id_Triangle:
            {
                cmd->patch_primitive_begin(primitive_Tri);
                unsigned int count = read<uint8_t>(chunk, pos);
                for (unsigned int counter = 0; counter < count; ++counter)
                {
                    unsigned int index = read<uint16_t>(chunk, pos);
                    cmd->patch_primitive_vertex(pool_index, index);
                }
                cmd->patch_primitive_end();
            }
            break;

            case cmd_id_TriangleCrossPool:
            {
                cmd->patch_primitive_begin(primitive_Tri);
                unsigned int count = read<uint8_t>(chunk, pos);
                for (unsigned int counter = 0; counter < count; ++counter)
                {
                    unsigned int pool = read<uint16_t>(chunk, pos);
                    unsigned int index = read<uint16_t>(chunk, pos);
                    cmd->patch_primitive_vertex(pool, index);
                }
                cmd->patch_primitive_end();
            }
            break;

            case cmd_id_TriangleRange:
            {
                unsigned int index1 = read<uint16_t>(chunk, pos);
                unsigned int index2 = read<uint16_t>(chunk, pos);
                cmd->patch_primitive_begin(primitive_Tri);
                for (unsigned int index = index1; index < index2; ++index)
                    cmd->patch_primitive_vertex(pool_index, index);
                cmd->patch_primitive_end();
            }
            break;

            case cmd_id_TriangleStrip:
                CMD_NOT_IMPL;
                break;

            case cmd_id_TriangleStripCrossPool:
            {
                cmd->patch_primitive_begin(primitive_TriStrip);
                unsigned int count = read<uint8_t>(chunk, pos);
                for (unsigned int counter = 0; counter < count; ++counter)
                {
                    unsigned int pool = read<uint16_t>(chunk, pos);
                    unsigned int index = read<uint16_t>(chunk, pos);
                    cmd->patch_primitive_vertex(pool, index);
                }
                cmd->patch_primitive_end();
            }
            break;

            case cmd_id_TriangleStripRange:
            {
                unsigned int index1 = read<uint16_t>(chunk, pos);
                unsigned int index2 = read<uint16_t>(chunk, pos);
                cmd->patch_primitive_begin(primitive_TriStrip);
                for (unsigned int index = index1; index < index2; ++index)
                    cmd->patch_primitive_vertex(pool_index, index);
                cmd->patch_primitive_end();
            }
            break;

            case cmd_id_TriangleFan:
                CMD_NOT_IMPL;
                break;
            case cmd_id_TriangleFanCrossPool:
                CMD_NOT_IMPL;
                break;
            case cmd_id_TriangleFanRange:
                CMD_NOT_IMPL;
                break;

            case cmd_id_Comment8:
                CMD_NOT_IMPL;
                break;
            case cmd_id_Comment16:
                CMD_NOT_IMPL;
                break;
            case cmd_id_Comment32:
                CMD_NOT_IMPL;
                break;

            case cmd_id_Reserved:
                CMD_NOT_IMPL;
                break;

            default:
                fprintf(stderr, "ERROR: Command #%d. Unknown command id %hhu\n", ncmd, cmd_id);
                return false;
            } /* end: switch (cmd_id) */
        } /* while (n < cmd_size) */

        if (patch_open)
            cmd->patch_end();

        cmd->end(atoms);
        return true;
    }

} /* end: namespace dsf */
