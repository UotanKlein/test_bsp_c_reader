#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define HEADER_LUMPS 64

typedef struct lump_t
{
    int fileofs;
    int filelen;
    int version;
    char fourCC[4];
} LUMP_T;

typedef struct dheader_t
{
	int	ident;
	int	version;
	LUMP_T lumps[HEADER_LUMPS];
	int	mapRevision;
} DHEADER_T;

typedef struct dbrush_t
{
	int	firstside;
	int	numsides;
	int	contents;
} DBRUSH_T;

struct dbrushside_t
{
	unsigned short  planenum;
	short           texinfo;
	short           dispinfo;
	short           bevel;
} DBRUSHSIDE_T;

typedef struct bsp_info
{
    char file_path[260]; 
    DHEADER_T header;
} BSP_INFO;

LUMP_T* get_lump(BSP_INFO* bsp_i, int lump_id)
{
    if (lump_id > (HEADER_LUMPS - 1))
    {
        printf("lump_id не может быть больше или равен %u\n", HEADER_LUMPS);
        return NULL;
    }

    LUMP_T* lump = &bsp_i->header.lumps[lump_id];

    if (lump->fileofs == 0 || lump->filelen == 0)
    {
        printf("lump_%u: Неиспользуемый или пустой\n", lump_id);
        return NULL;
    }

    return lump;
}

DBRUSH_T* get_lump18(BSP_INFO* bsp_i)
{
    LUMP_T* lump18 = get_lump(bsp_i, 18);
    if (!lump18) {
        return NULL;
    }

    FILE* fs = fopen(bsp_i->file_path, "rb");
    if (!fs) {
        perror("Failed to open file");
        return NULL;
    }

    fseek(fs, lump18->fileofs, SEEK_SET);

    size_t brush_count = lump18->filelen / sizeof(DBRUSH_T);
    printf("test: %u\n", brush_count);
    DBRUSH_T* brushes = malloc(brush_count * sizeof(DBRUSH_T));
    if (!brushes) {
        perror("Memory allocation failed");
        fclose(fs);
        return NULL;
    }

    fread(brushes, sizeof(DBRUSH_T), brush_count, fs);
    fclose(fs);
    
    return brushes;
}

BSP_INFO* get_bsp_info(const char* bsp_link)
{
    FILE* fp = fopen(bsp_link, "rb");
    if (!fp) {
        perror("Failed to open file");
        return NULL;
    }

    BSP_INFO* bsp_i = malloc(sizeof(BSP_INFO));
    if (!bsp_i) {
        perror("Memory allocation failed");
        fclose(fp);
        return NULL;
    }

    strcpy(bsp_i->file_path, bsp_link);
    fread(&bsp_i->header, sizeof(DHEADER_T), 1, fp);
    fclose(fp);
    return bsp_i;
}

void print_lump(LUMP_T* target_lump)
{
    if (target_lump)
    {
        printf("\tfileofs: %i\n", target_lump->fileofs);
        printf("\tfilelen: %i\n", target_lump->filelen);
        printf("\tversion: %i\n", target_lump->version);
    }
}

void print_header(DHEADER_T* target_header)
{
    if (target_header)
    {
        int ident = target_header->ident;
        printf("\tident: %c%c%c%c\n", ident, ident >> 8, ident >> 16, ident >> 24);
        printf("\tversion: %i\n", target_header->version);
        printf("\tmapRevision: %i\n", target_header->mapRevision);
    }
}

int main(void) 
{
    char bsp_link[] = "./gm_construct.bsp";
    BSP_INFO* bsp_i = get_bsp_info(bsp_link);
    DBRUSH_T* lump18 = get_lump18(bsp_i);
    DBRUSH_T first_brush = lump18[2193];
    printf("%i_%i_%i\n", first_brush.firstside, first_brush.numsides, first_brush.contents);
    free(lump18);
    free(bsp_i);
    return 0;
}
