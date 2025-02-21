#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "conversao.h"
#include "bmp.h"

#define GRAY_LEVELS 256

/* Define a ordem de leitura da imagme */
int ordem_de_leitura(height)
    long height;
{
    if(height > 0){
        return 1;
    } else {
        return 0;
    }
}

/* pixel **read_color_table(file_name,bmheader,rgb) *
 * Entrada :                                        *
 * char *file_name - Nome do arquivo da imagem      *
 * bitmapheader *bmheader - Cabecalho da imagem     *
 * pixel **rgb - mapa de pixel da imagem            *
 * Saida :                                          *
 * pixel **imagem - mapa de saida                   */
pixel **read_color_table(file_name,bmheader,rgb)
    char *file_name;
    bitmapheader *bmheader;
    pixel **rgb;
{

    FILE *fp;
    int position;
    char buffer[10];
    long height, width;
    int x, y, pad;

    fp = fopen(file_name,"rb");
    position = fseek(fp,54,SEEK_SET);
    height = bmheader->height;
    width = bmheader->width;
    pad = calculate_pad(width);
    int bytes = 54;

    if(ordem_de_leitura(width) == 1){
        for(y = height-1;y > -1;y--){
            for(x = 0;x < width;x++){
                fread(buffer,sizeof(char),3,fp);
                rgb[y][x].blue  = buffer[0];
                rgb[y][x].green = buffer[1];
                rgb[y][x].red   = buffer[2];
            }
            fread(buffer,sizeof(char),pad,fp);
        }
    } else {
        for(y = 0;y < height;y++){
            for(x = 0;x < width;x++){
                fread(buffer,1,3,fp);
                rgb[y][x].blue  = buffer[0];
                rgb[y][x].green = buffer[1];
                rgb[y][x].red   = buffer[2];
            }
            if(pad != 0){
                position = fseek(fp,pad,SEEK_CUR);
            }
        }
    }

    fclose(fp);
    return rgb;

}

/* Funcao que retorna o cabecalho do arquivo             *
 * Entradas :                                            *
 * char *file_name - nome do arquivo que contem a imagem *
 * bmpfileheader - cabecalho do arquivo                  */
int read_bmp_file_header(file_name,file_header)
    char *file_name;
    bmpfileheader *file_header;
{

    char buffer[10];
    long ll;
    short ss;
    unsigned short uss;
    unsigned long ull;
    FILE *fp;

    if((fp = fopen(file_name,"rb")) == NULL){
        printf("Arquivo: %s não encontrado\n",file_name);
        return 1;
    }

    if(1 == 1){
        fread(buffer,1,2,fp);
        extract_ushort_from_buffer(buffer,1,0,&uss);
        file_header->filetype = uss;

        fread(buffer,1,4,fp);
        extract_ulong_from_buffer(buffer,1,0,&ull);
        file_header->filesize = ull;

        fread(buffer,1,2,fp);
        extract_short_from_buffer(buffer,1,0,&ss);
        file_header->reserved1 = ss;

        fread(buffer,1,2,fp);
        extract_short_from_buffer(buffer,1,0,&ss);
        file_header->reserved2 = ss;

        fread(buffer,1,4,fp);
        extract_ulong_from_buffer(buffer, 1, 0, &ull);
        file_header->offset = ull;
    }

    if(ull != 54){
        printf("Arquivo corrompido");
        return 1;
    }

    fclose(fp);

    return 0;

}

/* Funcao que retorna o cabecalho da imagem       *
 * Entrada :                                      *
 * char *file_name - nome do arquivo da imagem    *
 * bitmapheader *bmp_header - cabecalho da imagem */
int read_bmp_header(file_name,bmp_header)
    char *file_name;
    bitmapheader *bmp_header;
{

    char buffer[10];
    long ll;
    short ss;
    unsigned short uss;
    unsigned long ull;
    FILE *fp;

    if((fp = fopen(file_name,"rb")) == NULL){
        printf("Arquivo: %s não encontrado\n",file_name);
        return 1;
    }

    fseek(fp,14,SEEK_SET);

    if(1 == 1){

        fread(buffer,1,4,fp);
        extract_ulong_from_buffer(buffer,1,0,&ull);
        bmp_header->size = ull;

        fread(buffer,1,4,fp);
        extract_long_from_buffer(buffer,1,0,&ll);
        bmp_header->width = ll;

        fread(buffer,1,4,fp);
        extract_long_from_buffer(buffer,1,0,&ll);
        bmp_header->height = ll;

        fread(buffer,1,2,fp);
        extract_ushort_from_buffer(buffer,1,0,&uss);
        bmp_header->planes = uss;

        fread(buffer,1,2,fp);
        extract_ushort_from_buffer(buffer,1,0,&uss);
        bmp_header->bitsperpixel = uss;

        fread(buffer,1,4,fp);
        extract_ulong_from_buffer(buffer,1,0,&ull);
        bmp_header->compression = ull;

        fread(buffer,1,4,fp);
        extract_ulong_from_buffer(buffer,1,0,&ull);
        bmp_header->sizeofbitmap = ull;

        fread(buffer,1,4,fp);
        extract_ulong_from_buffer(buffer,1,0,&ull);
        bmp_header->horzres = ull;

        fread(buffer,1,4,fp);
        extract_ulong_from_buffer(buffer,1,0,&ull);
        bmp_header->vertres = ull;

        fread(buffer,1,4,fp);
        extract_ulong_from_buffer(buffer,1,0,&ull);
        bmp_header->colorsused = ull;

        fread(buffer,1,4,fp);
        extract_ulong_from_buffer(buffer,1,0,&ull);
        bmp_header->colorsimp = ull;
    }

    fclose(fp);

    return 0;

}

/* read_bmp_image(file_name,array,file_header,bmheader) *
 * Entrada :                                            *
 *  char *file_name - nome do arquivo da imagem         *
 *  pixel *array - matriz de pixel                      *
 *  bmpfileheader *file_header - cabecalho da arquivo   *
 *  bitmapheader *bmheader - cabecalho da imagem        *
 * Saida :                                              *
 *  pixel **imagem - matriz de pixel da imagem          */
pixel **read_bmp_image(file_name,array,file_header,bmheader)
    char *file_name;
    pixel **array;
    bmpfileheader *file_header;
    bitmapheader *bmheader;
{

    FILE *fp;
    int x, y, pad = 0, place = 0, i;
    long colors = 0, height = 0, width = 0, position = 0;
    char buffer[10];
    unsigned char uc;
    long ll;

    width = bmheader->width;
    height = bmheader->height;

    if(bmheader->height < 0){
        height *= (-1);
    }

    if(bmheader->compression != 0){
        exit(1);
    }

    array = read_color_table(file_name,bmheader,array);

    return array;

}

/* calculte_pad(...)                                         *
 * Verifica se a largura da imagem é um numero divisivel por *
 * 4, se for não adiciona pads. Caso seja diferente, indica  *
 * o números de bytes adicionados para completar essa regra  */
int calculate_pad(width)
    long width;
{
    int pad = 0;
    pad = ((width%4) == 0) ? 0 : (4 - (width%4));
    return pad;
}

/* is_a_bmp(...)                                                *
 * This function looks at a file to see if it is a bmp file.    *
 * First look at the file extension. Next look at the filetype  *
 * to ensure it is 0x4d42                                       */
int is_a_bmp(file_name)
    char *file_name;
{

    char *cc;
    bmpfileheader file_header;

    cc = strstr(file_name,".bmp");
    if(cc == NULL){
        return 0;
    }

    read_bmp_file_header(file_name,&file_header);

    if(file_header.filetype != 0x4D42){
        return 0;
    } else {
        return 1;
    }

}

/* does_not_exist(...)
 * Verifica se o arquivo existe */
int does_not_exist(char file_name[]){

    FILE *image_file;

    image_file = fopen(file_name,"rb");
    if(image_file != NULL){
        fclose(image_file);
        return 0;
    }

    return 1;

}

/* Funcao que libera a memoria utilizada no armazenamento *
 * dos pixel da imagem                                    *
 * Entradas :                                             *
 * long maxLinhas - Tamanho maximo vertical da imagem     *
 * pixel imagem - Imagem a ser liberada                   */
int free_bmp(long maxLinha,pixel **imagem)
{
    int x, y;
    for(y = 0;y < maxLinha;y++){
        free(imagem[y]);
    }
    printf("Memoria liberada\n");
    return 1;
}

/* Escreve um arquivo da imagem                     *
 * Entradas :                                       *
 * char *file_name - nome do arquivo                *
 * bmpfilheader *file_header - cabecalho do arquivo *
 * bitmapheader *bmp_header - cabeacalho da imagem  *
 * pixel **rgb - matriz de pixel de imagem          */
int write_bmp(file_name,file_header,bmp_header,rgb)
    char *file_name;
    bmpfileheader *file_header;
    bitmapheader *bmp_header;
    pixel **rgb;
{

    char buffer[10];
    long ll;
    short ss;
    unsigned short uss;
    unsigned long ull;
    FILE *fp;
    int x, y, pad;
    long width = bmp_header->width;
    long height = bmp_header->height;

    fp = fopen(file_name,"wb");

    if(fp == NULL){
        printf("Arquivo: %s não encontrado\n",file_name);
        return 1;
    }
    
    {
        insert_ushort_into_buffer(buffer,0,file_header->filetype);
        fwrite(buffer,1,2,fp);

        insert_ulong_into_buffer(buffer,0,file_header->filesize);
        fwrite(buffer,1,4,fp);

        insert_short_into_buffer(buffer,0,file_header->reserved1);
        fwrite(buffer,1,2,fp);

        insert_short_into_buffer(buffer,0,file_header->reserved2);
        fwrite(buffer,1,2,fp);

        insert_ulong_into_buffer(buffer,0,file_header->offset);
        fwrite(buffer,1,4,fp);

        insert_ulong_into_buffer(buffer,0,bmp_header->size);
        fwrite(buffer,1,4,fp);

        insert_long_into_buffer(buffer,0,bmp_header->width);
        fwrite(buffer,1,4,fp);

        insert_long_into_buffer(buffer,0,bmp_header->height);
        fwrite(buffer,1,4,fp);

        insert_short_into_buffer(buffer,0,bmp_header->planes);
        fwrite(buffer,1,2,fp);

        insert_short_into_buffer(buffer,0,bmp_header->bitsperpixel);
        fwrite(buffer,1,2,fp);

        insert_ulong_into_buffer(buffer,0,bmp_header->compression);
        fwrite(buffer,1,4,fp);

        insert_ulong_into_buffer(buffer,0,bmp_header->sizeofbitmap);
        fwrite(buffer,1,4,fp);

        insert_ulong_into_buffer(buffer,0,bmp_header->horzres);
        fwrite(buffer,1,4,fp);

        insert_ulong_into_buffer(buffer,0,bmp_header->vertres);
        fwrite(buffer,1,4,fp);

        insert_ulong_into_buffer(buffer,0,bmp_header->colorsused);
        fwrite(buffer,1,4,fp);

        insert_ulong_into_buffer(buffer,0,bmp_header->colorsimp);
        fwrite(buffer,1,4,fp);
    }

    pad = calculate_pad(width);

    for(y = height-1;y > -1;y--){
        for(x = 0;x < width;x++){
            insert_long_into_buffer(buffer,0,rgb[y][x].blue);
            fwrite(buffer,sizeof(char),1,fp);
            insert_long_into_buffer(buffer,0,rgb[y][x].green);
            fwrite(buffer,sizeof(char),1,fp);
            insert_long_into_buffer(buffer,0,rgb[y][x].red);
            fwrite(buffer,sizeof(char),1,fp);
        }
        if(pad > 0){
            buffer[0] = 0x00; buffer[1] = 0x00; buffer[2] = 0x00;
            fwrite(buffer,sizeof(char),pad,fp);
        }
    }

    fclose(fp);

    return 0;

}
