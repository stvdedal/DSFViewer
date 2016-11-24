#ifndef DSF_CHUNK_H_
#define DSF_CHUNK_H_

#include "byteorder.h"
#include <cstddef>
#include <vector>
#include <limits>
#include <string>
#include <utility>

namespace dsf {

    /* === Exceptions === */

    struct Exception{};
    struct BadFormatException {};
    struct MemChunkOverrunException : public BadFormatException {};
    struct AtomBadLengthException : public BadFormatException {};
    struct RLEBadLengthException : public BadFormatException {};
    struct BadStringTableException : public BadFormatException {};


    /* === MemChunk === */

    class MemChunk
    {
    protected:
        const void* _begin;
        const void* _end;

        template<class T>
        static const T* cast_ptr(const void* p)
        {
            return reinterpret_cast<const T*>(p);
        }

    public:
        MemChunk(const void* buffer, size_t size)
            :_begin(buffer), _end(cast_ptr<char>(buffer) + size)
        {
        }

        MemChunk(const void* begin = nullptr, const void* end = nullptr)
            : _begin(begin), _end(end)
        {
        }

        template<class T>
        const T* begin() const
        {
            return cast_ptr<T>(_begin);
        }

        template<class T>
        const T* end() const
        {
            return cast_ptr<T>(_end);
        }

        // how many objects of type T
        template<class T>
        std::size_t size() const
        {
            return end<T>() - begin<T>();
        }

        bool empty() const
        {
            return size<char>() == 0;
        }

        template<class T>
        T get(std::size_t index) const
        {
            if (index >= size<T>())
            {
                throw MemChunkOverrunException();
            }
            return lit2host(cast_ptr<T>(_begin)[index]);
        }

        template<class T>
        T get_at(std::size_t pos) const
        {
            if (pos + sizeof(T) > size<char>())
            {
                throw MemChunkOverrunException();
            }
            return lit2host(*cast_ptr<T>(cast_ptr<char>(_begin) + pos));
        }
    };

    template<class T>
    T read(const MemChunk& ch, std::size_t& n)
    {
        T v = ch.get_at<T>(n);
        n += sizeof(T);
        return v;
    }

    /* === Atom === */

    class Atom : public MemChunk
    {
    public:
        Atom(const void* begin, const void* end);
        uint32_t id() const;
        uint32_t length() const;
        MemChunk data() const;
    };


    /* === Atoms === */


    class AtomChunks : private MemChunk
    {
    public:
        AtomChunks(const void* begin, const void* end);
        AtomChunks(const MemChunk& chunk);
        Atom get_first() const;
        Atom get_next(const Atom& atom) const;
    };


    /* === StringTable === */


    class StringTable
    {
        MemChunk _chunk;
    public:
        StringTable(const MemChunk& chunk) :_chunk(chunk) {}
        typedef std::size_t Cookie;
        std::string get_first(bool* is_end, Cookie& cookie) const;
        std::string get_next(bool* is_end, Cookie& cookie) const;
        std::pair<std::string, std::string> get_first_pair(bool* is_end, Cookie& cookie) const;
        std::pair<std::string, std::string> get_next_pair(bool* is_end, Cookie& cookie) const;
    };

    /* === PlanarNumeric === */


    class PlanarNumeric : public MemChunk
    {
    public:
        PlanarNumeric(const MemChunk& chunk) : MemChunk(chunk) {}
        // raw = 0, differenced = 1, RLE = 2, RLE differenced = 3
        int         encoded_mode() const { return get_at<uint8_t>(0); }
        MemChunk    encoded_data() const { return MemChunk(begin<uint8_t>() + 1, end<uint8_t>());  }
    };


    /* === PlanarNumericTable === */


    class PlanarNumericTable
    {
        MemChunk _chunk;
    public:
        PlanarNumericTable(const MemChunk& chunk) :_chunk(chunk) { }
        // A 32-bit item count for the number of items in the array
        std::size_t  array_size() const { return _chunk.get_at<uint32_t>(0); }
        // An 8-bit plane count for the number of planes in the atom
        std::size_t plane_count() const { return _chunk.get_at<uint8_t>(4); }
        // Variable length encoded data depending on the compression type and numeric format
        PlanarNumeric      data() const { return MemChunk(_chunk.begin<uint8_t>() + 5, _chunk.end<uint8_t>()); }
    };

    /* === Decoders === */

    template<class T>
    class FlatDecoder
    {
        MemChunk _chunk;
        std::size_t _pos;
    public:
        FlatDecoder(const MemChunk& chunk) :_chunk(chunk), _pos(0) { }
        T fetch()
        {
            T val = _chunk.get_at<T>(_pos);
            _pos += sizeof(T);
            return val;
        }
        MemChunk rest() const
        {
            return MemChunk(_chunk.begin<char>() + _pos, _chunk.end<char>());
        }
    };
    
    template<class T>
    class RLEDecoder
    {
        MemChunk _chunk;
        std::size_t _pos;
        enum { INIT, FWD, STOP } _mode;
        std::size_t _len;
    public:
        RLEDecoder(const MemChunk& chunk) :_chunk(chunk), _pos(0), _mode(INIT), _len(0) {}

        T fetch()
        {
            if (_mode == FWD)
            {
                T retval = _chunk.get_at<T>(_pos);
                _pos += sizeof(T);
                if (--_len == 0)
                    _mode = INIT;
                return retval;
            }
            else if (_mode == STOP)
            {
                T retval = _chunk.get_at<T>(_pos);
                if (--_len == 0)
                {
                    _pos += sizeof(T);
                    _mode = INIT;
                }
                return retval;
            }
            else
            {
                uint8_t code = _chunk.get_at<uint8_t>(_pos);
                _mode = code & 0x80 ? STOP : FWD;
                _len = code & 0x7F;
                if (_len == 0)
                {
                    throw RLEBadLengthException();
                }
                ++_pos;
                return fetch();
            }
        }
        MemChunk rest() const
        {
            return MemChunk(_chunk.begin<char>() + _pos, _chunk.end<char>());
        }
    };

    template<class T, class F>
    void decode_PlanarNumericTable_InterleavedScaled(
        const PlanarNumericTable& numTable,
        const std::vector<F>& scales,
        const std::vector<F>& offsets,
        std::vector<F>& result)
    {
        const F reduce = F(1) / F(std::numeric_limits<T>::max());

        if (scales.size() != numTable.plane_count() || offsets.size() != numTable.plane_count())
            throw BadFormatException();

        std::size_t plane_count = numTable.plane_count();
        std::size_t array_size = numTable.array_size();

        PlanarNumeric nums = numTable.data();

        for (std::size_t plane = 0; plane < plane_count; ++plane)
        {
            F sc = scales[plane];
            F of = offsets[plane];
            bool no_sc = sc < F(0.0000000000000001);

            switch (nums.encoded_mode()) {
            case 0:
                printf("encoded %d\n", 0);
                break;
            case 1:
                printf("encoded %d\n", 1);
                break;
            case 2:
                printf("encoded %d\n", 2);
                break;
            case 3:
            {
                RLEDecoder<T> decoder(nums.encoded_data());
                T last = T(0);
                for (std::size_t i = 0; i < array_size; ++i)
                {
                    T val = last + decoder.fetch();
                    if (!no_sc)
                    {
                        result[i * plane_count + plane] = F(val) * sc * reduce + of;
                    }
                    else
                    {
                        result[i * plane_count + plane] = F(val);
                    }
                    last = val;

                }
                nums = decoder.rest();
            }
                break;
            default:
                fprintf(stderr, "ERROR: unknow encode_mode %d\n", (int)nums.encoded_mode());
                throw BadFormatException();
                break;
            }
        }
    }
}
#endif
