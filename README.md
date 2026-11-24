# Image Steganography Using C

## Project Overview

This project implements image steganography using the C programming language. It allows users to hide secret text messages inside BMP images and retrieve them without visibly changing the original image.

The project uses the Least Significant Bit (LSB) technique to embed secret data into image pixel information.

## Features

- Hide secret text messages inside BMP images.
- Extract hidden messages from stego images.
- Implemented using the Least Significant Bit (LSB) technique.
- Validate input file formats and image capacity.
- Support both encoding and decoding operations.
- Handle file opening and processing errors.

## Technologies Used

- **Programming Language:** C
- **Compiler:** GCC
- **IDE:** Visual Studio Code
- **Platform:** Linux / WSL

## Project Structure

| File       | Description 
| `main.c`   | Main program and operation selection 
| `encode.c` | Encoding implementation 
| `decode.c` | Decoding implementation 
| `encode.h` | Encoding function declarations 
| `decode.h` | Decoding function declarations 
| `common.h` | Common project definitions 
| `types.h`  | User-defined data types and status definitions 

## Compilation

Compile the project using GCC:

```bash
gcc main.c encode.c decode.c -o stegnography
```

## Execution
### Encoding
To hide a secret message inside a BMP image:

```bash
./stegnography -e beautiful.bmp secret.txt
```

The encoded image is generated as `stego.bmp`.

### Decoding

To retrieve the hidden message:

```bash
./stegnography -d stego.bmp
```

The decoded message is saved in a file named `decoded`.

## Working Principle

The project uses the Least Significant Bit (LSB) technique to embed secret message data into BMP image pixel data.

During encoding, the secret message is converted into binary data and embedded into the image.

During decoding, the embedded binary data is extracted to recover the original secret message.

## Learning Outcomes

- Practical implementation of file handling in C.
- Understanding bitwise operations and LSB manipulation.
- Working with pointers, structures and functions.
- Understanding image data processing.
- Developing modular programming applications.

## Author
**Abhishek Ashok Zanje**