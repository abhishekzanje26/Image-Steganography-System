
#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "common.h"

/* Open files */
Status open_decode_files(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");

    if (decInfo->fptr_stego_image == NULL)
    {
        printf("Unable to open stego image\n");
        return e_failure;
    }

    decInfo->fptr_output = fopen(decInfo->output_fname, "wb");

    if (decInfo->fptr_output == NULL)
    {
        printf("Unable to create output file\n");
        fclose(decInfo->fptr_stego_image);
        decInfo->fptr_stego_image = NULL;
        return e_failure;
    }

    return e_success;
}

/* Read and validate arguments */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    char *ext = strrchr(argv[2], '.');

    if (ext == NULL || strcmp(ext, ".bmp") != 0)
    {
        printf("Stego image must be BMP file\n");
        return e_failure;
    }

    decInfo->stego_image_fname = argv[2];

    if (argv[3] != NULL)
    {
        decInfo->output_fname = argv[3];
    }
    else
    {
        decInfo->output_fname = "decoded";
    }

    return e_success;
}

/* Decode one byte from image */
Status decode_byte_from_lsb(char *image_buffer, char *data)
{
    unsigned char ch = 0;

    for (int i = 0; i < 8; i++)
    {
        ch = (ch << 1) | (image_buffer[i] & 1);
    }

    *data = (char)ch;

    return e_success;
}

/* Decode 32-bit size */
Status decode_size_from_lsb(char *image_buffer, int *size)
{
    unsigned int value = 0;

    for (int i = 0; i < 32; i++)
    {
        value = (value << 1) | (image_buffer[i] & 1);
    }

    *size = (int)value;

    return e_success;
}

/* Decode magic string */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    char image_buffer[8];
    char data;

    for (int i = 0; magic_string[i] != '\0'; i++)
    {
        if (fread(image_buffer, 1, 8, decInfo->fptr_stego_image) != 8)
        {
            return e_failure;
        }

        decode_byte_from_lsb(image_buffer, &data);

        if (data != magic_string[i])
        {
            printf("Invalid stego image\n");
            return e_failure;
        }
    }

    return e_success;
}

/* Decode extension size */
Status decode_secret_file_extn_size(int *size, DecodeInfo *decInfo)
{
    char image_buffer[32];

    if (fread(image_buffer, 1, 32, decInfo->fptr_stego_image) != 32)
    {
        return e_failure;
    }

    return decode_size_from_lsb(image_buffer, size);
}

/* Decode extension */
Status decode_secret_file_extn(char *file_extn, int size, DecodeInfo *decInfo)
{
    char image_buffer[8];

    if (size <= 0 || size >= 8)
    {
        return e_failure;
    }

    for (int i = 0; i < size; i++)
    {
        if (fread(image_buffer, 1, 8, decInfo->fptr_stego_image) != 8)
        {
            return e_failure;
        }

        decode_byte_from_lsb(image_buffer, &file_extn[i]);
    }

    file_extn[size] = '\0';

    return e_success;
}

/* Decode secret file size */
Status decode_secret_file_size(int *file_size, DecodeInfo *decInfo)
{
    char image_buffer[32];

    if (fread(image_buffer, 1, 32, decInfo->fptr_stego_image) != 32)
    {
        return e_failure;
    }

    return decode_size_from_lsb(image_buffer, file_size);
}

/* Decode secret file data */
Status decode_secret_file_data(int size, DecodeInfo *decInfo)
{
    char image_buffer[8];
    char data;

    if (size < 0)
    {
        return e_failure;
    }

    for (int i = 0; i < size; i++)
    {
        if (fread(image_buffer, 1, 8, decInfo->fptr_stego_image) != 8)
        {
            return e_failure;
        }

        decode_byte_from_lsb(image_buffer, &data);

        if (fwrite(&data, 1, 1, decInfo->fptr_output) != 1)
        {
            return e_failure;
        }
    }

    return e_success;
}

/* Do decoding */
Status do_decoding(DecodeInfo *decInfo)
{
    int extn_size;
    int file_size;
    char magic[3] = {0};

    if (open_decode_files(decInfo) == e_failure)
    {
        return e_failure;
    }

    /* Skip BMP header */
    if (fseek(decInfo->fptr_stego_image, 54, SEEK_SET) != 0)
    {
        goto failure;
    }

    if (decode_magic_string(MAGIC_STRING, decInfo) == e_failure)
    {
        goto failure;
    }

    if (decode_secret_file_extn_size(&extn_size, decInfo) == e_failure)
    {
        goto failure;
    }

    if (decode_secret_file_extn(decInfo->extn_secret_file,
                                extn_size, decInfo) == e_failure)
    {
        goto failure;
    }

    if (decode_secret_file_size(&file_size, decInfo) == e_failure)
    {
        goto failure;
    }

    if (file_size < 0)
    {
        goto failure;
    }

    if (decode_secret_file_data(file_size, decInfo) == e_failure)
    {
        goto failure;
    }

    fclose(decInfo->fptr_stego_image);
    fclose(decInfo->fptr_output);

    return e_success;

failure:
    fclose(decInfo->fptr_stego_image);
    fclose(decInfo->fptr_output);

    return e_failure;
}
