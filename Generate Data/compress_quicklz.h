/*
 * Copyright (c) 2016 Zhao DAI <daidodo@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or any
 * later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see accompanying file LICENSE.txt
 * or <http://www.gnu.org/licenses/>.
 */

/**
 * @file
 * @brief [QuickLZ](http://www.quicklz.com/index.php) compression algorithm.
 * @author Zhao DAI
 */

#ifndef DOZERG_COMPRESSOR_QUICKLZ_H_20090222
#define DOZERG_COMPRESSOR_QUICKLZ_H_20090222

#include <vector>
#include <string>

#ifndef QLZ_COMPRESSION_LEVEL
//#   define QLZ_COMPRESSION_LEVEL 1
//#   define QLZ_COMPRESSION_LEVEL 2
#   define QLZ_COMPRESSION_LEVEL 3
#endif

#ifndef QLZ_STREAMING_BUFFER
#   define QLZ_STREAMING_BUFFER 0
//#   define QLZ_STREAMING_BUFFER 100000
//#   define QLZ_STREAMING_BUFFER 1000000
#endif

//#define QLZ_MEMORY_SAFE

#include "quicklz.c.h"

/**
 * @brief A convenient interface for [QuickLZ](http://www.quicklz.com/index.php) compression
 * algorithm.
 * @note CCompressorQuickLZ object can @em NOT be copied.
 *
 * There are 3 macros that can impact the behavior of @c CCompressorQuickLZ.
 * @par @c QLZ_COMPRESSION_LEVEL
 * Compression level, default 3.
 * @n Valid values are @c 1, @c 2 or @c 3.
 * @n @c 1 gives fastest compression speed. @c 3 gives fastest decompression speed and best
 * compression ratio.
 * @n Example code:
 * @code #define QLZ_COMPRESSION_LEVEL 1 @endcode
 *
 * @par @c QLZ_STREAMING_BUFFER
 * Streaming compression buffer size, default 0. If it is:
 * @li @c 0: Streaming compression is disabled;
 * @li A positive number: Bytes size of scratch buffer for streaming compression.
 *
 * The suggested buffer sizes are @c 100000 or @c 1000000. If it's too small, streaming compression
 * may @em NOT function properly.
 * @n Example code:
 * @code #define QLZ_STREAMING_BUFFER 100000 @endcode
 *
 * @par @c QLZ_MEMORY_SAFE
 * Whether to enable memory access checks, undefined by default.
 * @n If @c QLZ_MEMORY_SAFE is defined, it is guaranteed that decompression of corrupted data cannot
 * crash. But this could decrease decompression speed by 10-20%. Compression speed is not affected.
 * @n Example code:
 * @code #define QLZ_MEMORY_SAFE @endcode
 */
class CCompressorQuickLZ
{
    typedef ::qlz_state_compress __CompBuf;
    typedef ::qlz_state_decompress __DecompBuf;
public:
    /**
     * @brief Default constructor.
     */
    CCompressorQuickLZ(){}
    /**
     * @name Compression
     * @{
     * @brief Compress given data.
     * If @c output is not empty, the result will append to @c output.
     * @param[in] input Data to be compressed
     * @param[out] output A buffer to receive the result
     * @return @c true if succeeded; otherwise @c false
     */
    bool compress(const std::vector<char> & input, std::vector<char> & output) const{
        return compressTemplate(input, output);
    }
    bool compress(const std::vector<unsigned char> & input, std::vector<unsigned char> & output) const{
        return compressTemplate(input, output);
    }
    bool compress(const std::vector<signed char> & input, std::vector<signed char> & output) const{
        return compressTemplate(input, output);
    }
    bool compress(const std::string & input, std::string & output) const{
        return compressTemplate(input, output);
    }
    /**  @} */
    /**
     * @name Decompression
     * @{
     * @brief Decompress given data.
     * If @c output is not empty, the result will append to @c output.
     * @param[in] input Data to be decompressed
     * @param[out] output A buffer to receive the result
     * @return @c true if succeeded; otherwise @c false
     */
    bool decompress(const std::vector<char> & input, std::vector<char> & output) const{
        return decompressTemplate(input, output);
    }
    bool decompress(const std::vector<unsigned char> & input, std::vector<unsigned char> & output) const{
        return decompressTemplate(input, output);
    }
    bool decompress(const std::vector<signed char> & input, std::vector<signed char> & output) const{
        return decompressTemplate(input, output);
    }
    bool decompress(const std::string & input, std::string & output) const{
        return decompressTemplate(input, output);
    }
    /**  @} */
private:
    template<class Buffer>
    bool compressTemplate(const Buffer & input, Buffer & output) const{
        output.resize(input.size() + 400);
        if(compBuf_.size() < sizeof(__CompBuf))
            compBuf_.resize(sizeof(__CompBuf));
        const size_t out_len = ::qlz_compress(
                &input[0],
                reinterpret_cast<char *>(&output[0]),
                input.size(),
                reinterpret_cast<__CompBuf *>(&compBuf_[0]));
        output.resize(out_len);
        return true;
    }
    template<class Buffer>
    bool decompressTemplate(const Buffer & input, Buffer & output) const{
        if(input.size() < 9)
            return false;
        const char * const buf = reinterpret_cast<const char *>(&input[0]);
        size_t out_len = ::qlz_size_decompressed(buf);
        output.resize(out_len);
        if(decompBuf_.size() < sizeof(__DecompBuf))
            decompBuf_.resize(sizeof(__DecompBuf));
        out_len = ::qlz_decompress(buf, &output[0], reinterpret_cast<__DecompBuf *>(&decompBuf_[0]));
        output.resize(out_len);
        return true;
    }
    CCompressorQuickLZ(const CCompressorQuickLZ &) = delete;
    CCompressorQuickLZ & operator =(const CCompressorQuickLZ &) = delete;
    //members
    mutable std::vector<char> compBuf_;
    mutable std::vector<char> decompBuf_;
};

#endif
