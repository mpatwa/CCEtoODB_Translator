/**********************************************
Copyright Mentor Graphics Corporation 2011

All Rights Reserved.

THIS WORK CONTAINS TRADE SECRET
AND PROPRIETARY INFORMATION WHICH IS THE
PROPERTY OF MENTOR GRAPHICS
CORPORATION OR ITS LICENSORS AND IS
SUBJECT TO LICENSE TERMS. 
**********************************************/


#include "StdAfx.h"
#include "ZipUtility.h"
#include "zlib.h"

const int s_nChunkSize  = 16384;
int s_nCompressionLevel = 6;

unsigned int s_sixBitsMask      = 0x3F;
unsigned int s_eightBitsMask    = 0xFF;

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif

int usecsInflateSStream = 0;
int usecsDeflateSStream = 0;

//  Standard Base64 map - source http://en.wikipedia.org/wiki/Base64#MIME
std::string s_base64map = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

//  A random Base64 map - more obfuscation
//std::string s_base64map = "Ab0CdZy1XWEF2GHVUT3SIJKL4RQPOMN5aBcD6zYxwef7ghvut8sijk9lrqpomn$@";

/*
    Deflates source stringstream, encodes compressed bytes into Base64
    and then puts the Base64 encoded data into destination stringstream
*/
int DeflateSStream( std::stringstream &ssSrc, std::stringstream &ssDst )
{
    int ret = Z_OK;

    int flush = true;

    //  Allocate deflate state
    z_stream c_stream; // Compression stream
    c_stream.zalloc = Z_NULL;
    c_stream.zfree  = Z_NULL;
    c_stream.opaque = Z_NULL;

    //  Initialize for deflation
    ret = deflateInit( &c_stream, s_nCompressionLevel );
    if ( ret != Z_OK )
        return ret;

    std::string strSrc( ssSrc.str() );

    //  Free up the source stringstream
    ssSrc.str("");

    int nInputSize  = strSrc.size();
    int nOutputSize = 0;

#ifndef USE_CHUNKS
    //  Allocate buffer for deflate to put data into
    char* out = new char[ nInputSize ];

    c_stream.avail_in   = nInputSize;
    c_stream.avail_out  = nInputSize;
    c_stream.next_in    = ( Bytef* ) strSrc.c_str();
    c_stream.next_out   = ( Bytef* ) out;

    //  Compress
    ret = deflate( &c_stream, Z_FULL_FLUSH /*Z_PARTIAL_FLUSH*/ );
    if ( ret != Z_OK )
        return ret;

    std::string base64Str   = ssDst.str();

    //  How big is the compressed result
    nOutputSize = nInputSize - c_stream.avail_out;

    //  Encode the compressed bytes to Base64
    StringToBase64( out, nOutputSize, base64Str );
    delete[] out;

    //  Put compressed Base64 data into the destination stringstream
    int nBase64 = base64Str.size();
    ssDst.write( base64Str.c_str(), nBase64 );

#else // USE_CHUNKS
    int nRemaining  = nInputSize;
    while ( nRemaining > 0 )
    {
        char out[ s_nChunkSize ];
        unsigned nHave = 0;

        strm.avail_in   = ( nRemaining > s_nChunkSize ) ? s_nChunkSize : nRemaining;
        strm.avail_out  = s_nChunkSize;
        strm.next_in    = ( Bytef* ) strSrc.c_str() + ( nInputSize - nRemaining );
        strm.next_out   = ( Bytef* ) out;

        nRemaining -= strm.avail_in;

        ret = deflate( &strm, flush );
        if ( ret != Z_OK )
            return ret;

        //  How big is the compressed result
        nHave = s_nChunkSize - strm.avail_out;

        ssDst.write( out, nOutputSize );

        nOutputSize += nHave;
    }
#endif // USE_CHUNKS

    //  Free all allocated state information referenced by stream
    ret = deflateEnd( &c_stream );

    return ret;
}

/*
    Source stringstream is in Base64, decoded from Base64 to get compressed bytes
    which is uncompressed and then put into the destination stringstream
*/
int InflateSStream( std::stringstream &ssSrc, std::stringstream &ssDst )
{
    int ret = Z_OK;

    int flush = true;

    //  Allocate inflate state
    z_stream d_stream; //decompression stream
    d_stream.zalloc     = Z_NULL;
    d_stream.zfree      = Z_NULL;
    d_stream.opaque     = Z_NULL;
    d_stream.avail_in   = Z_NULL;
    d_stream.next_in    = Z_NULL;

    //  Initialize inflate
    ret = inflateInit( &d_stream );
    if ( ret != Z_OK )
        return ret;

    std::string strBase64( ssSrc.str() );

    //  Free up the source stringstream
    ssSrc.str("");

    //  Decode from Base64
    std::string asciiStr;
    Base64ToString( strBase64.c_str(), strBase64.size(), asciiStr );

    //  Free up the temporary Base64 data
    strBase64 = "";

    uLong nInputSize  = asciiStr.size();
    int nRemaining = 0;

#ifndef OBSOLETE
    while ( d_stream.total_in < nInputSize )
    {
        char out[ s_nChunkSize ];

        d_stream.avail_in   = nInputSize - d_stream.total_in;
        d_stream.avail_out  = s_nChunkSize;
        d_stream.next_in    = ( Bytef* ) asciiStr.c_str() + d_stream.total_in;
        d_stream.next_out   = ( Bytef* ) out;

        //  Uncompress
        ret = inflate( &d_stream, Z_NO_FLUSH );

        //  Check for possible error conditions with ret
        switch ( ret )
        {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     /* and fall through */
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                inflateEnd( &d_stream );
                return ret;
        }

        int nHave = s_nChunkSize - d_stream.avail_out;

        //  Copy the uncompressed data into the stringstream
        ssDst.write ( out, nHave );

        if ( ret == Z_STREAM_END ) break;
    }
#else
    do {
        d_stream.avail_in   = nInputSize;
        d_stream.next_in    = ( Bytef* ) asciiStr.c_str() + nRemaining;

        do {
            d_stream.avail_out  = s_nChunkSize;
            d_stream.next_out   = ( Bytef* ) out;
            ret = inflate( &d_stream, Z_NO_FLUSH );
            if ( ret != Z_OK )
                return ret;

            //  How big is the inflate result
            int nHave = s_nChunkSize - d_stream.avail_out;

            ssDst.write ( out, nHave );

        } while ( d_stream.avail_out == 0 );
        ;//  Update nRemaining here
    } while ( ret != Z_STREAM_END );
#endif // OBSOLETE

    //  Free all allocated state information referenced by stream
    ret = inflateEnd( &d_stream );

return ret;
}

void StringToBase64( const char* pSrc, int inputLen, std::string &base64Str )
{
    // Conversion: Place 3 8-bit bytes (chars) from asciiStr into an unsigned integer buffer,
    // first char in highest order position, second in middle, third in lowest.
    // That is 24 bits total.

    // Access the bits 6 at a time, from left to right (highest order to lowest).
    // Use the integer value of the 6 bits as index into the base64 char map.
    // This makes 4 encoded chars for 3 input chars.

    // Ensure output is initially empty
    base64Str.clear();

    for (int i = 0; i < inputLen; i += 3)
    {
        unsigned int leftChar = pSrc[i]; // will always be the 1st char, may not be 2nd and 3rd
        unsigned int middleChar = ((i+1) < inputLen) ? (unsigned char)pSrc[i+1] : ' ';
        unsigned int rightChar =  ((i+2) < inputLen) ? (unsigned char)pSrc[i+2] : ' ';

        unsigned int packedChars = ( (leftChar << 16) | (middleChar << 8) | (rightChar) );

        unsigned int indx1 = (packedChars >> 18) & s_sixBitsMask;
        unsigned int indx2 = (packedChars >> 12) & s_sixBitsMask;
        unsigned int indx3 = (packedChars >>  6) & s_sixBitsMask;
        unsigned int indx4 = (packedChars      ) & s_sixBitsMask;

        unsigned char encodedChar1 = s_base64map.at(indx1);
        unsigned char encodedChar2 = s_base64map.at(indx2);
        unsigned char encodedChar3 = s_base64map.at(indx3);
        unsigned char encodedChar4 = s_base64map.at(indx4);

        base64Str += encodedChar1;  //*rcf Once this is all debugged and working, we can get rid of temp vars
        base64Str += encodedChar2;  // encodedChar1,2,3,4 and put directly from map into output string.
        base64Str += encodedChar3;  // Every little bit of speed will help.
        base64Str += encodedChar4;
    }
}

void Base64ToString( const char* pSrc, int inputLen, std::string &asciiStr )
{
    // Conversion: Opposite of StringToBase64, but same basic scheme.
    // Instead of 3 in and 4 out it will be 4 in and 3 out.

    // Ensure output is initially empty
    asciiStr.clear();

    for (int i = 0; i < inputLen; i += 4)
    {
        unsigned char encodedChar1 = pSrc[i]; // will always be the 1st char, may not be 2nd and 3rd
        unsigned char encodedChar2 = ((i+1) < inputLen) ? pSrc[i+1] : ' ';
        unsigned char encodedChar3 = ((i+2) < inputLen) ? pSrc[i+2] : ' ';
        unsigned char encodedChar4 = ((i+3) < inputLen) ? pSrc[i+3] : ' ';

        unsigned int indx1 = s_base64map.find(encodedChar1);
        unsigned int indx2 = s_base64map.find(encodedChar2);
        unsigned int indx3 = s_base64map.find(encodedChar3);
        unsigned int indx4 = s_base64map.find(encodedChar4);

        unsigned int bits1 = (indx1 & s_sixBitsMask) << 18;
        unsigned int bits2 = (indx2 & s_sixBitsMask) << 12;
        unsigned int bits3 = (indx3 & s_sixBitsMask) <<  6;
        unsigned int bits4 = (indx4 & s_sixBitsMask) ;//<<  0));

        unsigned int packedChars = ( bits1 | bits2 | bits3 | bits4 );

        unsigned char originalChar1 = (packedChars >> 16) & s_eightBitsMask;
        unsigned char originalChar2 = (packedChars >>  8) & s_eightBitsMask;
        unsigned char originalChar3 = (packedChars      ) & s_eightBitsMask;

        asciiStr += originalChar1;
        asciiStr += originalChar2;
        asciiStr += originalChar3;
    }
}
