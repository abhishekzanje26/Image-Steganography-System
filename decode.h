
#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "types.h"

typedef struct _DecodeInfo
{
    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    /* Output Secret File Info */
    char *output_fname;
    FILE *fptr_output;

    char extn_secret_file[8];
    int size_secret_file;

} DecodeInfo;

/* Function Prototypes */

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

Status do_decoding(DecodeInfo *decInfo);

Status open_decode_files(DecodeInfo *decInfo);

Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

Status decode_secret_file_extn_size(int *size, DecodeInfo *decInfo);

Status decode_secret_file_extn(char *file_extn, int size, DecodeInfo *decInfo);

Status decode_secret_file_size(int *file_size, DecodeInfo *decInfo);

Status decode_secret_file_data(int size, DecodeInfo *decInfo);

Status decode_byte_from_lsb(char *image_buffer, char *data);

Status decode_size_from_lsb(char *image_buffer, int *size);

#endif
