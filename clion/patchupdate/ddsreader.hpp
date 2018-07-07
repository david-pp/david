/*
 * DDS reader
 *
 * author: Juho Peltonen
 * license: GPL3, see license.txt
 */

#pragma once

#include <inttypes.h>
#include <vector>

/**
 * Structure representing image data
 *
 * data - raw byte data of the image
 * width and height - image dimensions
 * bpp - bits per pixel 
 */
struct Image {
    std::vector<uint8_t> data;
    int width, height, bpp;
};

/**
 * Read image from raw bytes
 */
Image read_dds(const std::vector<uint8_t>& data);

/**************************************************/
/* Butchered from FreeImage.h */

typedef uint32_t DWORD;
typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef int32_t BOOL;

#define FALSE 0
#define TRUE 1
#define SEEK_SET  0
#define SEEK_CUR  1
#define SEEK_END  2

#define DLL_CALLCONV

typedef void* fi_handle;
typedef unsigned (DLL_CALLCONV *FI_ReadProc) (void *buffer, unsigned size, unsigned count, fi_handle handle);
typedef unsigned (DLL_CALLCONV *FI_WriteProc) (void *buffer, unsigned size, unsigned count, fi_handle handle);
typedef int (DLL_CALLCONV *FI_SeekProc) (fi_handle handle, long offset, int origin);
typedef long (DLL_CALLCONV *FI_TellProc) (fi_handle handle);
#define FI_STRUCT(x)	struct x
FI_STRUCT (FIBITMAP) { void *data; };
FI_STRUCT (FIMULTIBITMAP) { void *data; };
FI_STRUCT(FreeImageIO) {
    FI_ReadProc  read_proc;     // pointer to the function used to read data
    FI_WriteProc write_proc;    // pointer to the function used to write data
    FI_SeekProc  seek_proc;     // pointer to the function used to seek
    FI_TellProc  tell_proc;     // pointer to the function used to aquire the current position
};

#if (defined(BYTE_ORDER) && BYTE_ORDER==BIG_ENDIAN) || \
	(defined(__BYTE_ORDER) && __BYTE_ORDER==__BIG_ENDIAN) || \
	defined(__BIG_ENDIAN__)
#define FREEIMAGE_BIGENDIAN
#endif // BYTE_ORDER

// This really only affects 24 and 32 bit formats, the rest are always RGB order.
#define FREEIMAGE_COLORORDER_BGR	0
#define FREEIMAGE_COLORORDER_RGB	1
#if defined(FREEIMAGE_BIGENDIAN)
#define FREEIMAGE_COLORORDER FREEIMAGE_COLORORDER_RGB
#else
#define FREEIMAGE_COLORORDER FREEIMAGE_COLORORDER_BGR
#endif

#ifndef FREEIMAGE_BIGENDIAN
#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
// Little Endian (x86 / MS Windows, Linux) : BGR(A) order
#define FI_RGBA_RED				2
#define FI_RGBA_GREEN			1
#define FI_RGBA_BLUE			0
#define FI_RGBA_ALPHA			3
#define FI_RGBA_RED_MASK		0x00FF0000
#define FI_RGBA_GREEN_MASK		0x0000FF00
#define FI_RGBA_BLUE_MASK		0x000000FF
#define FI_RGBA_ALPHA_MASK		0xFF000000
#define FI_RGBA_RED_SHIFT		16
#define FI_RGBA_GREEN_SHIFT		8
#define FI_RGBA_BLUE_SHIFT		0
#define FI_RGBA_ALPHA_SHIFT		24
#else
// Little Endian (x86 / MaxOSX) : RGB(A) order
#define FI_RGBA_RED				0
#define FI_RGBA_GREEN			1
#define FI_RGBA_BLUE			2
#define FI_RGBA_ALPHA			3
#define FI_RGBA_RED_MASK		0x000000FF
#define FI_RGBA_GREEN_MASK		0x0000FF00
#define FI_RGBA_BLUE_MASK		0x00FF0000
#define FI_RGBA_ALPHA_MASK		0xFF000000
#define FI_RGBA_RED_SHIFT		0
#define FI_RGBA_GREEN_SHIFT		8
#define FI_RGBA_BLUE_SHIFT		16
#define FI_RGBA_ALPHA_SHIFT		24
#endif // FREEIMAGE_COLORORDER
#else
#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
// Big Endian (PPC / none) : BGR(A) order
#define FI_RGBA_RED				2
#define FI_RGBA_GREEN			1
#define FI_RGBA_BLUE			0
#define FI_RGBA_ALPHA			3
#define FI_RGBA_RED_MASK		0x0000FF00
#define FI_RGBA_GREEN_MASK		0x00FF0000
#define FI_RGBA_BLUE_MASK		0xFF000000
#define FI_RGBA_ALPHA_MASK		0x000000FF
#define FI_RGBA_RED_SHIFT		8
#define FI_RGBA_GREEN_SHIFT		16
#define FI_RGBA_BLUE_SHIFT		24
#define FI_RGBA_ALPHA_SHIFT		0
#else
// Big Endian (PPC / Linux, MaxOSX) : RGB(A) order
#define FI_RGBA_RED				0
#define FI_RGBA_GREEN			1
#define FI_RGBA_BLUE			2
#define FI_RGBA_ALPHA			3
#define FI_RGBA_RED_MASK		0xFF000000
#define FI_RGBA_GREEN_MASK		0x00FF0000
#define FI_RGBA_BLUE_MASK		0x0000FF00
#define FI_RGBA_ALPHA_MASK		0x000000FF
#define FI_RGBA_RED_SHIFT		24
#define FI_RGBA_GREEN_SHIFT		16
#define FI_RGBA_BLUE_SHIFT		8
#define FI_RGBA_ALPHA_SHIFT		0
#endif // FREEIMAGE_COLORORDER
#endif // FREEIMAGE_BIGENDIAN


/****************************************************************/

/* From Utilities.h */
inline unsigned
CalculateLine(unsigned width, unsigned bitdepth) {
    return (unsigned)( ((unsigned long long)width * bitdepth + 7) / 8 );
}
/********************/

// ----------------------------------------------------------
//   Definitions for the DDS format
// ----------------------------------------------------------

#ifdef _WIN32
#pragma pack(push, 1)
#else
#pragma pack(1)
#endif

typedef struct tagDDPIXELFORMAT {
    DWORD dwSize;	// size of this structure (must be 32)
    DWORD dwFlags;	// see DDPF_*
    DWORD dwFourCC;
    DWORD dwRGBBitCount;	// Total number of bits for RGB formats
    DWORD dwRBitMask;
    DWORD dwGBitMask;
    DWORD dwBBitMask;
    DWORD dwRGBAlphaBitMask;
} DDPIXELFORMAT;

// DIRECTDRAW PIXELFORMAT FLAGS
enum {
    DDPF_ALPHAPIXELS = 0x00000001l,	// surface has alpha channel
    DDPF_ALPHA		 = 0x00000002l,	// alpha only
    DDPF_FOURCC		 = 0x00000004l,	// FOURCC available
    DDPF_RGB		 = 0x00000040l	// RGB(A) bitmap
};

typedef struct tagDDCAPS2 {
    DWORD dwCaps1;	// Zero or more of the DDSCAPS_* members
    DWORD dwCaps2;	// Zero or more of the DDSCAPS2_* members
    DWORD dwReserved[2];
} DDCAPS2;

// DIRECTDRAWSURFACE CAPABILITY FLAGS
enum {
    DDSCAPS_ALPHA	= 0x00000002l, // alpha only surface
    DDSCAPS_COMPLEX	= 0x00000008l, // complex surface structure
    DDSCAPS_TEXTURE	= 0x00001000l, // used as texture (should always be set)
    DDSCAPS_MIPMAP	= 0x00400000l  // Mipmap present
};

enum {
    DDSCAPS2_CUBEMAP			= 0x00000200L,
    DDSCAPS2_CUBEMAP_POSITIVEX	= 0x00000400L,
    DDSCAPS2_CUBEMAP_NEGATIVEX	= 0x00000800L,
    DDSCAPS2_CUBEMAP_POSITIVEY	= 0x00001000L,
    DDSCAPS2_CUBEMAP_NEGATIVEY	= 0x00002000L,
    DDSCAPS2_CUBEMAP_POSITIVEZ	= 0x00004000L,
    DDSCAPS2_CUBEMAP_NEGATIVEZ	= 0x00008000L,
    DDSCAPS2_VOLUME				= 0x00200000L
};

typedef struct tagDDSURFACEDESC2 {
    DWORD dwSize;	// size of this structure (must be 124)
    DWORD dwFlags;	// combination of the DDSS_* flags
    DWORD dwHeight;
    DWORD dwWidth;
    DWORD dwPitchOrLinearSize;
    DWORD dwDepth;	// Depth of a volume texture
    DWORD dwMipMapCount;
    DWORD dwReserved1[11];
    DDPIXELFORMAT ddpfPixelFormat;
    DDCAPS2 ddsCaps;
    DWORD dwReserved2;
} DDSURFACEDESC2;

enum {
    DDSD_CAPS			= 0x00000001l,
    DDSD_HEIGHT			= 0x00000002l,
    DDSD_WITH			= 0x00000004l,
    DDSD_PITCH			= 0x00000008l,
    DDSD_ALPHABITDEPTH  = 0x00000080l,
    DDSD_PIXELFORMAT	= 0x00001000l,
    DDSD_MIPMAPCOUNT	= 0x00020000l,
    DDSD_LINEARSIZE		= 0x00080000l,
    DDSD_DEPTH			= 0x00800000l
};

typedef struct tagDDSHEADER {
    DWORD dwMagic;			// FOURCC: "DDS "
    DDSURFACEDESC2 surfaceDesc;
} DDSHEADER;

#define MAKEFOURCC(ch0, ch1, ch2, ch3) \
	((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |   \
    ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))

#define FOURCC_DXT1	MAKEFOURCC('D','X','T','1')
#define FOURCC_DXT2	MAKEFOURCC('D','X','T','2')
#define FOURCC_DXT3	MAKEFOURCC('D','X','T','3')
#define FOURCC_DXT4	MAKEFOURCC('D','X','T','4')
#define FOURCC_DXT5	MAKEFOURCC('D','X','T','5')

// ----------------------------------------------------------
//   Structures used by DXT textures
// ----------------------------------------------------------

typedef struct tagColor8888 {
    BYTE b;
    BYTE g;
    BYTE r;
    BYTE a;
} Color8888;

typedef struct tagColor565 {
    WORD b : 5;
    WORD g : 6;
    WORD r : 5;
} Color565;

typedef struct tagDXTColBlock {
    Color565 colors[2];
    BYTE row[4];
} DXTColBlock;

typedef struct tagDXTAlphaBlockExplicit {
    WORD row[4];
} DXTAlphaBlockExplicit;

typedef struct tagDXTAlphaBlock3BitLinear {
    BYTE alpha[2];
    BYTE data[6];
} DXTAlphaBlock3BitLinear;

typedef struct tagDXT1Block
{
    DXTColBlock color;
} DXT1Block;

typedef struct tagDXT3Block {		// also used by dxt2
    DXTAlphaBlockExplicit alpha;
    DXTColBlock color;
} DXT3Block;

typedef struct tagDXT5Block {		// also used by dxt4
    DXTAlphaBlock3BitLinear alpha;
    DXTColBlock color;
} DXT5Block;

#ifdef _WIN32
#	pragma pack(pop)
#else
#	pragma pack()
#endif

// ----------------------------------------------------------
//   Internal functions
// ----------------------------------------------------------
#ifdef FREEIMAGE_BIGENDIAN
static void
SwapHeader(DDSHEADER *header) {
	SwapLong(&header->dwMagic);
	SwapLong(&header->surfaceDesc.dwSize);
	SwapLong(&header->surfaceDesc.dwFlags);
	SwapLong(&header->surfaceDesc.dwHeight);
	SwapLong(&header->surfaceDesc.dwWidth);
	SwapLong(&header->surfaceDesc.dwPitchOrLinearSize);
	SwapLong(&header->surfaceDesc.dwDepth);
	SwapLong(&header->surfaceDesc.dwMipMapCount);
	for(int i=0; i<11; i++) {
		SwapLong(&header->surfaceDesc.dwReserved1[i]);
	}
	SwapLong(&header->surfaceDesc.ddpfPixelFormat.dwSize);
	SwapLong(&header->surfaceDesc.ddpfPixelFormat.dwFlags);
	SwapLong(&header->surfaceDesc.ddpfPixelFormat.dwFourCC);
	SwapLong(&header->surfaceDesc.ddpfPixelFormat.dwRGBBitCount);
	SwapLong(&header->surfaceDesc.ddpfPixelFormat.dwRBitMask);
	SwapLong(&header->surfaceDesc.ddpfPixelFormat.dwGBitMask);
	SwapLong(&header->surfaceDesc.ddpfPixelFormat.dwBBitMask);
	SwapLong(&header->surfaceDesc.ddpfPixelFormat.dwRGBAlphaBitMask);
	SwapLong(&header->surfaceDesc.ddsCaps.dwCaps1);
	SwapLong(&header->surfaceDesc.ddsCaps.dwCaps2);
	SwapLong(&header->surfaceDesc.ddsCaps.dwReserved[0]);
	SwapLong(&header->surfaceDesc.ddsCaps.dwReserved[1]);
	SwapLong(&header->surfaceDesc.dwReserved2);
}
#endif

