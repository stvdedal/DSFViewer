#include "dsf_chunk.h"

namespace dsf
{
    /* === Atom === */

    Atom::Atom(const void* begin, const void* end)
        : MemChunk(begin, end)
    {
        if (!empty()) {
            if (length() > size<char>())
                throw AtomBadLengthException();
            _end = cast_ptr<char>(_begin) + length();
        }
    }

    uint32_t Atom::id() const
    {
        return get<uint32_t>(0);
    }

    uint32_t Atom::length() const
    {
        return get<uint32_t>(1);
    }

    MemChunk Atom::data() const
    {
        return MemChunk(
            cast_ptr<char>(_begin) + 8,
            cast_ptr<char>(_begin) + length());
    }


    /* === Atoms === */

    AtomChunks::AtomChunks(const void* begin, const void* end)
        : MemChunk(begin, end)
    {
    }

    AtomChunks::AtomChunks(const MemChunk& chunk)
        : MemChunk(chunk)
    {
    }

    Atom AtomChunks::get_first() const
    {
        return Atom(_begin, _end);
    }

    Atom AtomChunks::get_next(const Atom& atom) const
    {
        return Atom(atom.begin<char>() + atom.length(), _end);
    }


    /* === StringTable === */

    std::string StringTable::get_first(bool* is_end, Cookie& cookie) const
    {
        std::string str;
        std::size_t size = _chunk.size<char>();
        *is_end = true;
        if (size == 0)
            return str;
        if (_chunk.get<char>(size - 1) != '\0')
            throw BadStringTableException();
        str.assign(_chunk.begin<char>());
        cookie = str.length() + 1;
        *is_end = false;
        return str;
    }

    std::string StringTable::get_next(bool* is_end, Cookie& cookie) const
    {
        std::string str;
        *is_end = true;
        if (cookie >= _chunk.size<char>())
            return str;
        str.assign(_chunk.begin<char>() + cookie);
        cookie += str.length() + 1;
        *is_end = false;
        return str;
    }

    std::pair<std::string, std::string> StringTable::get_first_pair(bool* is_end, Cookie& cookie) const
    {
        std::pair<std::string, std::string> sp;
        sp.first = get_first(is_end, cookie);
        if (*is_end)
            return sp;
        sp.second = get_next(is_end, cookie);
        return sp;
    }

    std::pair<std::string, std::string> StringTable::get_next_pair(bool* is_end, Cookie& cookie) const
    {
        std::pair<std::string, std::string> sp;
        sp.first = get_next(is_end, cookie);
        if (*is_end)
            return sp;
        sp.second = get_next(is_end, cookie);
        return sp;
    }
}
