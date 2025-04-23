#ifndef TLV_H
#define TLV_H

#include <stddef.h>

typedef struct TLV {
    unsigned char tag;
    int length;
    unsigned char *value;
    int sub_tlv_count;
    struct TLV **sub_tlvs;
} TLV;

// ساخت TLV ساده
TLV *tlv_create(unsigned char tag, const unsigned char *value, int length);

// ساخت TLV کانتینر
TLV *tlv_create_container(unsigned char tag);

// افزودن زیرتگ
void tlv_add_sub_tlv(TLV *parent, TLV *child);

// چاپ درختی
void print_tlv(const TLV *tlv, int level, int is_last);

// آزادسازی
void tlv_free(TLV *tlv);

// بسته‌بندی
int tlv_pack(const TLV *tlv, unsigned char *buffer);

// آن‌پک
TLV *tlv_unpack(const unsigned char *buffer, int buffer_len, int *bytes_read);

#endif
