
#include "Tlv.h"
#include <stdio.h>
#include <string.h>

int main() {
    int i = 0;
    unsigned char val1[] = "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111";
    unsigned char val2[] = "22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222";
    unsigned char val3[] = "333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333";

    TLV *rootFF = tlv_create_container(0xFF);
    
    TLV *rootC0 = tlv_create_container(0xC0);
    
    TLV *tlv_80 = tlv_create(0x80, val1, strlen((const char*)val1));
    tlv_add_sub_tlv(rootC0, tlv_80);

    TLV *tlv_D1 = tlv_create_container(0xD1);
    TLV *tlv_D1_80 = tlv_create(0x80, val2,strlen((const char*)val2));
    TLV *tlv_D1_81 = tlv_create(0x81, val3, strlen((const char*)val3));

    tlv_add_sub_tlv(tlv_D1, tlv_D1_80);
    tlv_add_sub_tlv(tlv_D1, tlv_D1_81);
    
    tlv_add_sub_tlv(rootC0, tlv_D1);
    
    tlv_add_sub_tlv(rootFF,rootC0);

    printf("TLV Tree View:\n");
    print_tlv(rootFF, 0, 1);

    unsigned char buffer[1024];
    int packed_len = tlv_pack(rootFF, buffer);

    printf("\nPacked TLV (hex): ");
    for (i = 0; i < packed_len; i++) {
        printf("%02X", buffer[i]);
    }
    printf("\n");

    // آن‌پک
    int read = 0;
    TLV *unpacked = tlv_unpack(buffer, packed_len, &read);

    printf("\nUnpacked TLV Tree View:\n");
    print_tlv(unpacked, 0, 1);

    tlv_free(rootFF);
    tlv_free(unpacked);

    return 0;
}



