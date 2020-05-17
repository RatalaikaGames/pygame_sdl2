#include <algorithm>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

static void error(const char* message)
{
	fprintf(stderr, message);
	assert(0);
	exit(-1);
}

extern "C"
{
	#include "surface.h"
}

static void clipRectangle(int w, int h, SDL_Rect* r)
{
	//not 100% sure this is right

	if(r->x < 0)
	{
		r->w += r->x;
		r->x = 0;
	}
	if(r->y < 0)
	{
		r->h += r->y;
		r->y = 0;
	}

	int right = r->x + r->w;
	int bottom = r->y + r->h;

	if(right > w)
		r->w -= (right-w);
	if(bottom > h)
		r->h -= (bottom-h);

	if(r->w < 0) r->w = 0;
	if(r->h < 0) r->h = 0;
}

//return -1 for errors
//assume the "dst" has pixel alpha
extern "C" int pygame_Blit(SDL_Surface * src, SDL_Rect * srcrect, SDL_Surface * dst, SDL_Rect * dstrect, int the_args)
{
	//not 100% sure this is right

	SDL_Rect srcr, dstr;

	//get the basic work rectangles
	if(dstrect) dstr = *dstrect;
	else 
	{
		dstr.x = dstr.y = 0;
		dstr.w = dst->w;
		dstr.h = dst->h;
	}
	if(srcrect) srcr = *srcrect;
	else 
	{
		srcr.x = srcr.y = 0;
		srcr.w = src->w;
		srcr.h = src->h;
	}

	clipRectangle(src->w, src->h, &srcr);
	clipRectangle(dst->w, dst->h, &dstr);

	int todow = std::min(src->w,dst->w);
	int todoh = std::min(src->h,dst->h);

	if(todow == 0 || todoh == 0)
		return 0;

	SDL_LockSurface(src);
	SDL_LockSurface(dst);

	uint8_t* srcp = (uint8_t*)src->pixels;
	uint8_t* dstp = (uint8_t*)dst->pixels;
	srcp += src->pitch * srcr.y + srcr.x * src->format->BytesPerPixel;
	dstp += dst->pitch * dstr.y + dstr.x * dst->format->BytesPerPixel;

	if(src->format->BitsPerPixel != 32) error("Unexpected src bpp on pygs blit");
	if(dst->format->BitsPerPixel != 32) error("Unexpected dst bpp on pygs blit");
	if(!src->format->Amask) error("Unexpected src AMask on pygs blit");
	if(!dst->format->Amask) error("Unexpected dst AMask on pygs blit");

	#define UNPACK \
		uint8_t &src_r = srcp[0]; uint8_t &src_g = srcp[1]; uint8_t &src_b = srcp[2]; uint8_t &src_a = srcp[3]; \
		uint8_t &dst_r = dstp[0]; uint8_t &dst_g = dstp[1]; uint8_t &dst_b = dstp[2]; uint8_t &dst_a = dstp[3];

	switch(the_args)
	{
		case 0:
			//simple alpha blit
			//(possibly a simple copy operation if we have no amask)
			for(int y=0;y<todoh;y++)
			{
				for(int x=0;x<todow;x++)
				{
					UNPACK;
					//have to copy formula from GPL code to preserve semantics
					dst_r = (((src_r - dst_r) * src_a) >> 8) + dst_r;
					dst_g = (((src_g - dst_g) * src_a) >> 8) + dst_g;
					dst_b = (((src_b - dst_b) * src_a) >> 8) + dst_b;
					dst_a = src_a + dst_a - ((src_a * dst_a) / 255);
					srcp += 4;
					dstp += 4;
				}
				srcp += src->pitch - todow*4;
				dstp += dst->pitch - todow*4;
			}
			break;

		default:
			error("Unexpected blend arg on pygs blit");
	}

	return 0;
}
