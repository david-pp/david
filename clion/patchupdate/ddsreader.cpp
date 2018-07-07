/*
 * DDS reader
 *
 * author: Juho Peltonen
 * license: GPL3, see license.txt
 */


/*
 * Most of the code is taken from FreeImage project PluginDDS.cpp
 *
 * Below is the original file header
 */


// ==========================================================
// DDS Loader
//
// Design and implementation by
// - Volker Gärtner (volkerg@gmx.at)
// - Sherman Wilcox
//
// This file is part of FreeImage 3
//
// COVERED CODE IS PROVIDED UNDER THIS LICENSE ON AN "AS IS" BASIS, WITHOUT WARRANTY
// OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, WITHOUT LIMITATION, WARRANTIES
// THAT THE COVERED CODE IS FREE OF DEFECTS, MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE
// OR NON-INFRINGING. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE COVERED
// CODE IS WITH YOU. SHOULD ANY COVERED CODE PROVE DEFECTIVE IN ANY RESPECT, YOU (NOT
// THE INITIAL DEVELOPER OR ANY OTHER CONTRIBUTOR) ASSUME THE COST OF ANY NECESSARY
// SERVICING, REPAIR OR CORRECTION. THIS DISCLAIMER OF WARRANTY CONSTITUTES AN ESSENTIAL
// PART OF THIS LICENSE. NO USE OF ANY COVERED CODE IS AUTHORIZED HEREUNDER EXCEPT UNDER
// THIS DISCLAIMER.
//
// Use at your own risk!
// ==========================================================

#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>

#include "ddsreader.hpp"


// ==========================================================

// Get the 4 possible colors for a block
//
static void 
GetBlockColors (const DXTColBlock &block, Color8888 colors[4], bool isDXT1) {
	int i;
	// expand from 565 to 888
	for (i = 0; i < 2; i++)	{
		colors[i].a = 0xff;
		/*
		colors[i].r = (BYTE)(block.colors[i].r * 0xff / 0x1f);
		colors[i].g = (BYTE)(block.colors[i].g * 0xff / 0x3f);
		colors[i].b = (BYTE)(block.colors[i].b * 0xff / 0x1f);
		*/
		colors[i].r = (BYTE)((block.colors[i].r << 3U) | (block.colors[i].r >> 2U));
		colors[i].g = (BYTE)((block.colors[i].g << 2U) | (block.colors[i].g >> 4U));
		colors[i].b = (BYTE)((block.colors[i].b << 3U) | (block.colors[i].b >> 2U));
	}

	WORD *wCol = (WORD *)block.colors;
	if (wCol[0] > wCol[1] || !isDXT1) {
		// 4 color block
		for (i = 0; i < 2; i++)	{
			colors[i + 2].a = 0xff;
			colors[i + 2].r = (BYTE)((WORD (colors[0].r) * (2 - i) + WORD (colors[1].r) * (1 + i)) / 3);
			colors[i + 2].g = (BYTE)((WORD (colors[0].g) * (2 - i) + WORD (colors[1].g) * (1 + i)) / 3);
			colors[i + 2].b = (BYTE)((WORD (colors[0].b) * (2 - i) + WORD (colors[1].b) * (1 + i)) / 3);
		}
	}
	else {
		// 3 color block, number 4 is transparent
		colors[2].a = 0xff;
		colors[2].r = (BYTE)((WORD (colors[0].r) + WORD (colors[1].r)) / 2);
		colors[2].g = (BYTE)((WORD (colors[0].g) + WORD (colors[1].g)) / 2);
		colors[2].b = (BYTE)((WORD (colors[0].b) + WORD (colors[1].b)) / 2);

		colors[3].a = 0x00;
		colors[3].g = 0x00;
		colors[3].b = 0x00;
		colors[3].r = 0x00;
	}
}

struct DXT_INFO_1 {
	typedef DXT1Block Block;
	enum {
		isDXT1 = 1,
		bytesPerBlock = 8
	};
};

struct DXT_INFO_3 {
	typedef DXT3Block Block;
	enum {
		isDXT1 = 1,
		bytesPerBlock = 16
	};
};

struct DXT_INFO_5 {
	typedef DXT5Block Block;
	enum
	{
		isDXT1 = 1,
		bytesPerBlock = 16
	};
};

template <class INFO> class DXT_BLOCKDECODER_BASE {
protected:
	Color8888 m_colors[4];
	const typename INFO::Block *m_pBlock;
	unsigned m_colorRow;

public:
	void Setup (const BYTE *pBlock) {
		m_pBlock = (const typename INFO::Block *)pBlock;
		GetBlockColors (m_pBlock->color, m_colors, INFO::isDXT1);
	}

	void SetY (int y) {
		m_colorRow = m_pBlock->color.row[y];
	}

	void GetColor (int x, int y, Color8888 &color) {
		unsigned bits = (m_colorRow >> (x * 2)) & 3;
		color = m_colors[bits];
	}
};

class DXT_BLOCKDECODER_1 : public DXT_BLOCKDECODER_BASE <DXT_INFO_1> {
public:
	typedef DXT_INFO_1 INFO;
};

class DXT_BLOCKDECODER_3 : public DXT_BLOCKDECODER_BASE <DXT_INFO_3> {
public:
	typedef DXT_BLOCKDECODER_BASE <DXT_INFO_3> base;
	typedef DXT_INFO_3 INFO;

protected:
	unsigned m_alphaRow;

public:
	void SetY (int y) {
		base::SetY (y);
		m_alphaRow = m_pBlock->alpha.row[y];
	}

	void GetColor (int x, int y, Color8888 &color) {
		base::GetColor (x, y, color);
		const unsigned bits = (m_alphaRow >> (x * 4)) & 0xF;
		color.a = (BYTE)((bits * 0xFF) / 0xF);
	}
};

class DXT_BLOCKDECODER_5 : public DXT_BLOCKDECODER_BASE <DXT_INFO_5> {
public:
	typedef DXT_BLOCKDECODER_BASE <DXT_INFO_5> base;
	typedef DXT_INFO_5 INFO;

protected:
	unsigned m_alphas[8];
	unsigned m_alphaBits;
	int m_offset;

public:
	void Setup (const BYTE *pBlock) {
		base::Setup (pBlock);

		const DXTAlphaBlock3BitLinear &block = m_pBlock->alpha;
		m_alphas[0] = block.alpha[0];
		m_alphas[1] = block.alpha[1];
		if (m_alphas[0] > m_alphas[1]) {
			// 8 alpha block
			for (int i = 0; i < 6; i++) {
				m_alphas[i + 2] = ((6 - i) * m_alphas[0] + (1 + i) * m_alphas[1] + 3) / 7;
			}
		}
		else {
			// 6 alpha block
			for (int i = 0; i < 4; i++) {
				m_alphas[i + 2] = ((4 - i) * m_alphas[0] + (1 + i) * m_alphas[1] + 2) / 5;
			}
			m_alphas[6] = 0;
			m_alphas[7] = 0xFF;
		}

	}

	void SetY (int y) {
		base::SetY (y);
		int i = y / 2;
		const DXTAlphaBlock3BitLinear &block = m_pBlock->alpha;
		m_alphaBits = unsigned(block.data[0 + i * 3]) | (unsigned(block.data[1 + i * 3]) << 8)
			| (unsigned(block.data[2 + i * 3]) << 16);
		m_offset = (y & 1) * 12;
	}

	void GetColor (int x, int y, Color8888 &color) {
		base::GetColor (x, y, color);
		unsigned bits = (m_alphaBits >> (x * 3 + m_offset)) & 7;
		color.a = (BYTE)m_alphas[bits];
	}
};

template <class DECODER> void DecodeDXTBlock (BYTE *dstData, const BYTE *srcBlock, long dstPitch, int bw, int bh) {
	DECODER decoder;
	decoder.Setup (srcBlock);
	for (int y = 0; y < bh; y++) {
		BYTE *dst = dstData - y * dstPitch;
		decoder.SetY (y);
		for (int x = 0; x < bw; x++) {
			decoder.GetColor (x, y, (Color8888 &)*dst);

#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_RGB 
			INPLACESWAP(dst[FI_RGBA_RED], dst[FI_RGBA_BLUE]);
#endif 
			dst += 4;
		}
	}
}

// ==========================================================
// Plugin Interface
// ==========================================================

static int s_format_id;

// ==========================================================
// Internal functions
// ==========================================================

struct IO {
    const std::vector<uint8_t>* data;
    unsigned int idx;
    void seek(unsigned idx) { this->idx = idx; }
    template<typename T>
    void read(T* dst, unsigned cnt) {
        memcpy(dst, &data->at(idx), cnt);
        idx += cnt;
    }
};

inline std::vector<uint8_t> load_rgb(DDSURFACEDESC2 &desc, IO& io) {
	int width = (int)desc.dwWidth & ~3;
	int height = (int)desc.dwHeight & ~3;
	int bpp = (int)desc.ddpfPixelFormat.dwRGBBitCount;
	
    int bytespp = bpp/8;
    std::vector<uint8_t> result(width*height*bytespp);
	
	// read the file
	int line   = CalculateLine(width, bpp);
	int filePitch = (desc.dwFlags & DDSD_PITCH) ? (int)desc.dwPitchOrLinearSize : line;
	long delta = (long)filePitch - (long)line;
	for (int i = 0; i < height; i++) {
        BYTE* pixels = &result[line*i];
        io.read(pixels, line);
        io.idx += delta;
 		for(int x = 0; x < width; x++) {
            std::swap(pixels[FI_RGBA_RED],pixels[FI_RGBA_BLUE]);
			pixels += bytespp;
		}
	}
	
	// enable transparency
	//FreeImage_SetTransparent (dib, (desc.ddpfPixelFormat.dwFlags & DDPF_ALPHAPIXELS) ? TRUE : FALSE);

	//if (!(desc.ddpfPixelFormat.dwFlags & DDPF_ALPHAPIXELS) && bpp == 32) {
	//	// no transparency: convert to 24-bit
	//	FIBITMAP *old = dib;
	//	dib = FreeImage_ConvertTo24Bits (old);
	//	FreeImage_Unload (old);
	//}
	return result;
}

template <class DECODER> static void 
load_dxt_helper (IO& io, std::vector<uint8_t>& result, int width, int height, int line) {
	typedef typename DECODER::INFO INFO;
	typedef typename INFO::Block Block;

	Block *input_buffer = new Block[(width + 3) / 4];

	int widthRest = (int) width & 3;
	int heightRest = (int) height & 3;
	int inputLine = (width + 3) / 4;
	int y = 0;

	if (height >= 4) {
        for (; y < height; y += 4) {
			io.read(input_buffer, sizeof (typename INFO::Block) * inputLine);
			// TODO: probably need some endian work here
			BYTE *pbSrc = (BYTE *)input_buffer;
            BYTE *pbDst = &result[width*4*(height-y-1)];

			if (width >= 4) {
				for (int x = 0; x < width; x += 4) {
					DecodeDXTBlock <DECODER> (pbDst, pbSrc,	line, 4, 4);
					pbSrc += INFO::bytesPerBlock;
					pbDst += 4 * 4;
				}
			}
			if (widthRest) {
				DecodeDXTBlock <DECODER> (pbDst, pbSrc, line, widthRest, 4);
			}
		}
	}
	if (heightRest)	{
		io.read(input_buffer, sizeof (typename INFO::Block) * inputLine);
		// TODO: probably need some endian work here
		BYTE *pbSrc = (BYTE *)input_buffer;
        BYTE *pbDst = &result[line*y];

		if (width >= 4) {
			for (int x = 0; x < width; x += 4) {
				DecodeDXTBlock <DECODER> (pbDst, pbSrc,	line, 4, heightRest);
				pbSrc += INFO::bytesPerBlock;
				pbDst += 4 * 4;
			}
		}
		if (widthRest) {
			DecodeDXTBlock <DECODER> (pbDst, pbSrc,	line, widthRest, heightRest);
		}

	}

	delete [] input_buffer;
}

inline std::vector<uint8_t> load_dxt(int type, DDSURFACEDESC2 &desc, IO& io) {
	int width = (int)desc.dwWidth & ~3;
	int height = (int)desc.dwHeight & ~3;

	// allocate a 32-bit dib
    std::vector<uint8_t> result(width*height*4);

	int bpp = 32;
	int line = CalculateLine (width, bpp);

	// select the right decoder
	switch (type) {
		case 1:
			load_dxt_helper <DXT_BLOCKDECODER_1> (io, result, width, height, line);
			break;
		case 3:
			load_dxt_helper <DXT_BLOCKDECODER_3> (io, result, width, height, line);
			break;
		case 5:
			load_dxt_helper <DXT_BLOCKDECODER_5> (io, result, width, height, line);
			break;
	}
    // but now the lines are fucked! lets hack it correct
    std::vector<uint8_t> buf(width*4);
    for (int y = 0; y < height/2; y++) {
        unsigned up = y*width*4, bot = (height-y-1)*width*4;
        memcpy(&buf.front(), &result[up], width*4);
        memmove(&result[up], &result[bot], width*4);
        memcpy(&result[bot], &buf.front(), width*4);
    }

    return result;
}

Image read_dds(const std::vector<uint8_t>& data) {
    DDSHEADER header;
    memset(&header, 0, sizeof(header));

    IO io{&data, 0};

    io.read(&header, sizeof(header));
//#ifdef FREEIMAGE_BIGENDIAN
//	SwapHeader(&header);
//#endif

    Image image;
    auto& desc = header.surfaceDesc;
	image.width = (int)desc.dwWidth & ~3;
	image.height = (int)desc.dwHeight & ~3;
	image.bpp = (int)desc.ddpfPixelFormat.dwRGBBitCount;

	if (header.surfaceDesc.ddpfPixelFormat.dwFlags & DDPF_RGB) {
        image.data = load_rgb(header.surfaceDesc, io);
	}

	else if (header.surfaceDesc.ddpfPixelFormat.dwFlags & DDPF_FOURCC) {
		switch (header.surfaceDesc.ddpfPixelFormat.dwFourCC) {
			case FOURCC_DXT1:
                image.data = load_dxt(1, header.surfaceDesc, io);
                break;
			case FOURCC_DXT3:
                image.data = load_dxt(3, header.surfaceDesc, io);
                break;
			case FOURCC_DXT5:
                image.data = load_dxt(5, header.surfaceDesc, io);
                break;
            default:
                break;
		}
	}

    return image;
}
