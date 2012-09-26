/**********************************************
  Copyright Mentor Graphics Corporation 2008

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
**********************************************/

#ifndef _FILE_GARBLER_H_
#define _FILE_GARBLER_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>


namespace fileGarbler
{
    // base class used for encoding and decoding of character buffers
    class cEncodeDecode
    {
    public:
        cEncodeDecode() : m_encodingChars(0), m_encodingNumChars(0), m_encodingIndex(0) { }
       ~cEncodeDecode() { delete m_encodingChars; }
        // encrypt or decrypt a buffer of characters
        void Hash(int numChars, char *buffer)
        {
            // is object encrypted?
            if(IsEncrypted()) {
                // XOR the buffer with the encoding string - works for encryption or decryption
                while(numChars-- > 0) {
                    *buffer++ ^= m_encodingChars[m_encodingIndex];
                    if(++m_encodingIndex >= m_encodingNumChars)
                        m_encodingIndex = 0;
                }
            }
        }
    protected:
        bool IsEncrypted() { return m_encodingNumChars > 0; }
        void SetNoEncryption() { m_encodingNumChars = 0; }
        char *m_encodingChars;      // the encoding characters XOR'ed against the incoming data
        int m_encodingNumChars;     // number of encoding characters
        int m_encodingIndex;        // index into the cyclical buffer of encoding characters
    };


    // class to encrypt a file
    class cEncrypt : public cEncodeDecode
    {
    public:
        // constructor to initiate encryption to file - we write encryption marker to the
        // file as well as the encoding data. The file needs to be opened as binary.
        // After the constructor is called then the Hash() method is used to encode a 
        // buffer before writing the contents of the encrypted buffer to the file.
        cEncrypt(FILE *fp)
        {
            // generate random encoding data
            GenerateRandomHeader();
            // and write it and the header out to the file
            ::fprintf(fp, "%c%c%c%c", 5,6,19,m_encodingNumChars);
            for(int i=0; i<m_encodingNumChars; i++)  // encoding data
                ::fputc(m_encodingChars[i], fp);
        }
        // replacement for fprintf - will encrypt the formatted string 
        // as its written to the file.
        int fprintf(FILE *stream, const char *format, ...)
        {
            // get var args list
            va_list args;
            va_start(args, format);
            // format into a buffer
            char buff[2048]; int buffSize=2048; char *pbuff=buff; 
#ifndef UNIX
            int nChars = _vsnprintf(pbuff, buffSize, format, args);
#else
            int nChars = vsnprintf(pbuff, buffSize, format, args);
#endif
            while((nChars < 0) || (nChars > buffSize))
            {
                // cleanup 
                if (pbuff != buff)
                    delete pbuff;
                // reallocate buffer
                buffSize *= 2;
                pbuff = new char[buffSize];
                // format into the reallocated buffer
#ifndef UNIX
                nChars = _vsnprintf(pbuff, buffSize, format, args);
#else
                nChars = vsnprintf(pbuff, buffSize, format, args);
#endif
            }
            va_end(args);
#ifndef UNIX
			// translate newline characters
			nChars = TranslateNewlineChars(&pbuff, buffSize, pbuff != buff);
#endif
            // encrypt the string - overwrite the buffer
            Hash(nChars, pbuff);
            // and write to file
            nChars = (int)fwrite(pbuff, 1, nChars, stream);
            // and cleanup
            if (pbuff != buff)
                delete pbuff;
            return nChars;
        }
        // replacement for fputc - will encrypt the character as its written to the file
        int fputc(char c, FILE *stream)
        {
            // encrypt
            Hash(1, &c);
            // write
            return ::fputc(c, stream);
        }
    private:
        // generate random header with which to encrypt data
        void GenerateRandomHeader()
        {
            // init random number generator
            srand((unsigned)time(NULL));
            // generate random encoding byte count (30 max (say))
            int num = rand() % 25;
            num += 5;                   // ensure there are at least five chars
            m_encodingChars = new char[num];
            for(int i=0; i<num; i++)
                m_encodingChars[i] = (char)(rand() % 255);
            m_encodingNumChars = num;
        }
		// translate newline characters into carriage-return/newline pairs
		int TranslateNewlineChars(char **buffer, int bufferSize, bool deleteBuffer)
		{
			char *tmpBuffer = new char[bufferSize];
			char lastChar = '\0';
			char thisChar;
			int  len = (int)strlen(*buffer);
			int  tmpBufferSize = bufferSize;
			int  i, j;

			for (i=0, j=0; i<len; i++)
			{
				if (j+2 >= tmpBufferSize)
				{
					// need to allocate more memory.
					char *cptr = new char[tmpBufferSize+256];
					memcpy(cptr, tmpBuffer, tmpBufferSize);
					tmpBufferSize += 256;
					delete tmpBuffer;
					tmpBuffer = cptr;
				}
				thisChar = (*buffer)[i];
				if (thisChar == '\n') 
				{
					if (lastChar != '\r')
						tmpBuffer[j++] = '\r';
					tmpBuffer[j++] = thisChar;
				}
				else if ( thisChar == '\r' )
				{
					if ( lastChar != '\r' )
						tmpBuffer[j++] = '\r';
				}
				else
					tmpBuffer[j++] = thisChar;
				lastChar = thisChar;
			}
			tmpBuffer[j] = '\0';
			if (tmpBufferSize == bufferSize)
			{
				strcpy(*buffer, tmpBuffer);
				delete tmpBuffer;
			}
			else
			{
				if (deleteBuffer) delete *buffer;
				*buffer = tmpBuffer;
			}
			return j;
		}
    };


    // class to decrypt a file
    class cDecrypt : public cEncodeDecode
    {
    public:
        cDecrypt() { }
        cDecrypt(FILE *fp)
        {
            // do what we need to do to set this file up for decryption. If the file
            // is encrypted we will position the file pointer after the encryption 
            // header.
            fseek(fp, 0L, SEEK_SET);
            // read 128 bytes from start of file
            char buff[128];
            int nRead = (int)fread(buff, 1, 128, fp);
            int nHdr = InitializeFromBuffer(nRead, buff);
            // seek to end of encryption header
            fseek(fp, nHdr, SEEK_SET);
            // file pointer positioned after the encryption header (or at
            // start of file if no encryption)
        }
        // init for decryption using this buffer read from start of file. Return
        // value is the number of bytes in the encryption header (or zero if no
        // encryption).
        int InitializeFromBuffer(int headerSize, const char *header)
        {
            // test for presence of encrypted marker
            if(headerSize>4 && *header==5 && *(header+1)==6 && *(header+2)==19) {
                // get encoding string count
                header += 3;
                m_encodingNumChars = *header++;
                // get encoding string
                m_encodingChars = new char[m_encodingNumChars];
                memcpy(m_encodingChars, header, m_encodingNumChars);
                m_encodingIndex = 0;
                // return count of bytes in encryption header
                return 3 + 1 + m_encodingNumChars;
            }
            // there is no encryption on passed buffer
            SetNoEncryption();
            return 0;
        }
    };
};

#endif
