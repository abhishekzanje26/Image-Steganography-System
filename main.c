#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "decode.h"

OperationType check_operation_type(char *);

int main(int argc, char *argv[])
{
    // step1 -> check  check_operation_type(argv[1]) is returning e_encode or not
    if(argc<3){
        printf("Invalid commands\n");
        return 0;
    }
    OperationType res=check_operation_type(argv[1]);
    if(res==e_encode)
    {
        // true -> EncodeInfo encInfo;
        EncodeInfo encInfo;
        if(argc<4 || argc>5){
            printf("Invalid commands\n");
            return 0;
        }
        //step 1 -> check read_and_validate_encode_args(argv, &encInfo) is returning e_success or not
        if( read_and_validate_encode_args(argv, &encInfo)==e_failure)
        {
            // e_failure -> print error msg and stop the program
            printf("Error message\n");
            return 0;
        }
        // e_success -> print succes msg goto step
        printf("Read and validate arguments successful\n");
        //step 2 -> check do_encoding(&encInfo) is success or not
        if(do_encoding(&encInfo)==e_failure)
        {
            // e_failure -> print error msg and stop the program
            printf("Error in Encoding\n");
            return 0;
        }
        // e_success -> print success msg and stop
        printf("Encoding Done Successfully\n");
    }
    else if(res==e_decode)
    {
        DecodeInfo decInfo;
        if(argc>4){
            printf("Invalid commands\n");
            return 0;
        }
        if(read_and_validate_decode_args(argv, &decInfo)==e_failure)
        {
            printf("Error message\n");
            return 0;
        }
        if(do_decoding(&decInfo)==e_failure)
        {
            printf("Error in Decoding\n");
            return 0;
        }
        printf("Decoding Done Successfully\n");
    }
    else{
        printf("Invalid commands\n");
    }
    return 0;
}

OperationType check_operation_type(char *symbol)
{
    // step 1 -> check symbol is -e or not
    if(strcmp(symbol, "-e")==0)
    {
        // yes -> return e_encode
        return e_encode;
    }
    // step 2 -> check symbol is -d or not
    if(strcmp(symbol,"-d")==0)
    {
        //yes -> return e_decode
        return e_decode;
    }
    // step 3 -> return e_unsupported
    return e_unsupported;
}