
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    int width, height;

    fseek(fptr_image, 18, SEEK_SET);

    fread(&width, sizeof(int), 1, fptr_image);
    fread(&height, sizeof(int), 1, fptr_image);

    printf("Width = %d\n", width);
    printf("Height = %d\n", height);

    return width * height * 3;
}

/* Get file size */
uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);
    uint size = ftell(fptr);
    rewind(fptr);

    return size;
}

/* Read and validate arguments */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    char *ext;

    /* Source image validation */
    ext = strrchr(argv[2], '.');

    if (ext == NULL || strcmp(ext, ".bmp") != 0)
    {
        printf("Source image must be BMP file\n");
        return e_failure;
    }

    encInfo->src_image_fname = argv[2];

    /* Secret file validation */
    ext = strrchr(argv[3], '.');

    if (ext == NULL || ext == argv[3])
    {
        printf("Invalid secret file\n");
        return e_failure;
    }

    if (strlen(ext) >= MAX_FILE_SUFFIX)
    {
        printf("Secret file extension is too long\n");
        return e_failure;
    }

    strcpy(encInfo->extn_secret_file, ext);
    encInfo->secret_fname = argv[3];

    /* Output image validation */
    if (argv[4] != NULL)
    {
        ext = strrchr(argv[4], '.');

        if (ext == NULL || strcmp(ext, ".bmp") != 0)
        {
            printf("Output image must be BMP file\n");
            return e_failure;
        }

        encInfo->stego_image_fname = argv[4];
    }
    else
    {
        encInfo->stego_image_fname = "stego.bmp";
    }

    return e_success;
}

/* Open files */
Status open_files(EncodeInfo *encInfo)
{
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");

    if (encInfo->fptr_src_image == NULL)
    {
        printf("Unable to open source image\n");
        return e_failure;
    }

    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");

    if (encInfo->fptr_secret == NULL)
    {
        printf("Unable to open secret file\n");
        fclose(encInfo->fptr_src_image);
        encInfo->fptr_src_image = NULL;
        return e_failure;
    }

    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");

    if (encInfo->fptr_stego_image == NULL)
    {
        printf("Unable to open output image\n");
        fclose(encInfo->fptr_src_image);
        fclose(encInfo->fptr_secret);

        encInfo->fptr_src_image = NULL;
        encInfo->fptr_secret = NULL;

        return e_failure;
    }

    return e_success;
}

/* Check capacity */
Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity =
        get_image_size_for_bmp(encInfo->fptr_src_image);

    encInfo->size_secret_file =
        get_file_size(encInfo->fptr_secret);

    long required_size =
        (strlen(MAGIC_STRING) * 8) +
        32 +
        (strlen(encInfo->extn_secret_file) * 8) +
        32 +
        (encInfo->size_secret_file * 8);

    if (encInfo->image_capacity >= required_size)
    {
        return e_success;
    }

    printf("Insufficient image capacity\n");
    return e_failure;
}

/* Copy BMP header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char header[54];

    rewind(fptr_src_image);

    if (fread(header, 1, 54, fptr_src_image) != 54)
    {
        return e_failure;
    }

    if (fwrite(header, 1, 54, fptr_dest_image) != 54)
    {
        return e_failure;
    }

    return e_success;
}

/* Encode magic string */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    for (int i = 0; magic_string[i] != '\0'; i++)
    {
        char image_buffer[8];

        if (fread(image_buffer, 1, 8, encInfo->fptr_src_image) != 8)
        {
            return e_failure;
        }

        encode_byte_to_lsb(magic_string[i], image_buffer);

        if (fwrite(image_buffer, 1, 8, encInfo->fptr_stego_image) != 8)
        {
            return e_failure;
        }
    }

    return e_success;
}

/* Encode extension size */
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    char image_buffer[32];

    if (fread(image_buffer, 1, 32, encInfo->fptr_src_image) != 32)
    {
        return e_failure;
    }

    encode_size_to_lsb(size, image_buffer);

    if (fwrite(image_buffer, 1, 32, encInfo->fptr_stego_image) != 32)
    {
        return e_failure;
    }

    return e_success;
}

/* Encode secret file extension */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    for (int i = 0; file_extn[i] != '\0'; i++)
    {
        char image_buffer[8];

        if (fread(image_buffer, 1, 8, encInfo->fptr_src_image) != 8)
        {
            return e_failure;
        }

        encode_byte_to_lsb(file_extn[i], image_buffer);

        if (fwrite(image_buffer, 1, 8, encInfo->fptr_stego_image) != 8)
        {
            return e_failure;
        }
    }

    return e_success;
}

/* Encode secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char image_buffer[32];

    if (file_size < 0 || file_size > INT_MAX)
    {
        return e_failure;
    }

    if (fread(image_buffer, 1, 32, encInfo->fptr_src_image) != 32)
    {
        return e_failure;
    }

    encode_size_to_lsb((int)file_size, image_buffer);

    if (fwrite(image_buffer, 1, 32, encInfo->fptr_stego_image) != 32)
    {
        return e_failure;
    }

    return e_success;
}

/* Encode secret file data */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char ch;

    while (fread(&ch, 1, 1, encInfo->fptr_secret) == 1)
    {
        char image_buffer[8];

        if (fread(image_buffer, 1, 8, encInfo->fptr_src_image) != 8)
        {
            return e_failure;
        }

        encode_byte_to_lsb(ch, image_buffer);

        if (fwrite(image_buffer, 1, 8, encInfo->fptr_stego_image) != 8)
        {
            return e_failure;
        }
    }

    return e_success;
}

/* Encode data to image */
Status encode_data_to_image(char *data, int size,
                            FILE *fptr_src_image,
                            FILE *fptr_stego_image)
{
    for (int i = 0; i < size; i++)
    {
        char image_buffer[8];

        if (fread(image_buffer, 1, 8, fptr_src_image) != 8)
        {
            return e_failure;
        }

        encode_byte_to_lsb(data[i], image_buffer);

        if (fwrite(image_buffer, 1, 8, fptr_stego_image) != 8)
        {
            return e_failure;
        }
    }

    return e_success;
}

/* Encode one byte into 8 image bytes */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    unsigned char ch = (unsigned char)data;

    for (int i = 0; i < 8; i++)
    {
        image_buffer[i] =
            (image_buffer[i] & 0xFE) |
            ((ch >> (7 - i)) & 1);
    }

    return e_success;
}

/* Encode size into 32 image bytes */
Status encode_size_to_lsb(int size, char *image_buffer)
{
    unsigned int value = (unsigned int)size;

    for (int i = 0; i < 32; i++)
    {
        image_buffer[i] =
            (image_buffer[i] & 0xFE) |
            ((value >> (31 - i)) & 1);
    }

    return e_success;
}

/* Copy remaining image data */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;

    while (fread(&ch, 1, 1, fptr_src) == 1)
    {
        if (fwrite(&ch, 1, 1, fptr_dest) != 1)
        {
            return e_failure;
        }
    }

    return e_success;
}

/* Do encoding */
Status do_encoding(EncodeInfo *encInfo)
{
    if (open_files(encInfo) == e_failure)
    {
        return e_failure;
    }

    if (check_capacity(encInfo) == e_failure)
    {
        goto failure;
    }

    if (copy_bmp_header(encInfo->fptr_src_image,
                        encInfo->fptr_stego_image) == e_failure)
    {
        goto failure;
    }

    if (encode_magic_string(MAGIC_STRING, encInfo) == e_failure)
    {
        goto failure;
    }

    if (encode_secret_file_extn_size(
            strlen(encInfo->extn_secret_file), encInfo) == e_failure)
    {
        goto failure;
    }

    if (encode_secret_file_extn(
            encInfo->extn_secret_file, encInfo) == e_failure)
    {
        goto failure;
    }

    if (encode_secret_file_size(
            encInfo->size_secret_file, encInfo) == e_failure)
    {
        goto failure;
    }

    if (encode_secret_file_data(encInfo) == e_failure)
    {
        goto failure;
    }

    if (copy_remaining_img_data(
            encInfo->fptr_src_image,
            encInfo->fptr_stego_image) == e_failure)
    {
        goto failure;
    }

    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_stego_image);

    return e_success;

failure:

    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_stego_image);

    return e_failure;
}
