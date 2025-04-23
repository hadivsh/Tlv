#include "tlv.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int Utl_BcdToInt(const char *bcd,int length) {

    int result = 0;
    int i = 0;
    if (bcd == NULL || length <= 0) { // بررسی اعتبار آرایه و طول
        return -1;
    }

    for (i = 0; i < length; i++) {
        result = (result << 8) | bcd[i];
    }

    return result;
}

int Utl_IntToBcd(int value, char** bcd) {
    if (value < 0) { // بررسی اعتبار ورودی
        return -1; // مقدار نامعتبر
    }

    int length = 0;
    int temp = value;

    // محاسبه طول مورد نیاز برای ذخیره عدد
    while (temp > 0) {
        temp >>= 8;
        length++;
    }

    // تخصیص حافظه برای آرایه‌ی BCD
    *bcd = (char*)malloc(length * sizeof(char));
    if (*bcd == NULL) { // بررسی تخصیص حافظه
        return -1; // خطا در تخصیص حافظه
    }

    // تبدیل عدد صحیح به آرایه‌ی BCD
    for (int i = length - 1; i >= 0; i--) {
        (*bcd)[i] = value & 0xFF; // ذخیره بایت کمتر مهم
        value >>= 8;
    }

    return length; // بازگرداندن طول آرایه
}
TLV *tlv_create(unsigned char tag, const unsigned char *value, int length) {
    TLV *tlv = (TLV *)malloc(sizeof(TLV));
    tlv->tag = tag;
    tlv->length = length;
    tlv->value = (unsigned char *)malloc(length);
    memcpy(tlv->value, value, length);
    tlv->sub_tlv_count = 0;
    tlv->sub_tlvs = NULL;
    return tlv;
}

TLV *tlv_create_container(unsigned char tag) {
    TLV *tlv = (TLV *)malloc(sizeof(TLV));
    tlv->tag = tag;
    tlv->length = 0;
    tlv->value = NULL;
    tlv->sub_tlv_count = 0;
    tlv->sub_tlvs = NULL;
    return tlv;
}

void tlv_add_sub_tlv(TLV *parent, TLV *child) {
    int lenLen = 0;
    parent->sub_tlvs = (TLV **)realloc(parent->sub_tlvs, sizeof(TLV *) * (parent->sub_tlv_count + 1));
    parent->sub_tlvs[parent->sub_tlv_count++] = child;

    // بروزرسانی طول
    if(child->length<=0x7f)
    {
    }else if(child->length<=0xff)
    {
        lenLen =1;
    }else if(child->length<=0xffff)
    {
        lenLen = 2;
    }
    parent->length += 2 + child->length + lenLen;
}

void print_tlv(const TLV *tlv, int level, int is_last) {
    int i = 0;
  //  size_t j = 0;
    for (i = 0; i < level; i++) {
        printf("     ");
    }
    printf("%s %02X [%d]", is_last ? "└——" : "├——", tlv->tag,tlv->length);

    if (tlv->sub_tlv_count == 0 && tlv->value) {
        printf(" = \"");
        fwrite(tlv->value, 1, tlv->length, stdout);
        printf("\"\n");
    } else {
        printf("\n");
        for (size_t j = 0; j < tlv->sub_tlv_count; j++) {
            print_tlv(tlv->sub_tlvs[j], level + 1, j == tlv->sub_tlv_count - 1);
        }
    }
}

void tlv_free(TLV *tlv) {
    size_t i = 0;
    for (i = 0; i < tlv->sub_tlv_count; i++) {
        tlv_free(tlv->sub_tlvs[i]);
    }
    free(tlv->sub_tlvs);
    free(tlv->value);
    free(tlv);
}
 
size_t tlv_pack(const TLV *tlv, unsigned char *buffer) {
    size_t index = 0;
    size_t i = 0;
    buffer[index++] = tlv->tag;
    
        // بروزرسانی طول
    if(tlv->length<=0x7f)
    {
        buffer[index++] = (unsigned char)tlv->length;
    }else if(tlv->length<=0xff)
    {
        buffer[index++] = 0x81;
        buffer[index++] = (unsigned char)tlv->length;
    }else if(tlv->length<=0xffff)
    {
        int length = 0;
        char* bcd = NULL;
        buffer[index++] = 0x82;
        length = Utl_IntToBcd(tlv->length, &bcd);
        memcpy(buffer+index,bcd,length);
        index++;
        index++;
        free(bcd); // آزادسازی حافظه
    }

    if (tlv->sub_tlv_count > 0) {
        for (i = 0; i < tlv->sub_tlv_count; i++) {
            index += tlv_pack(tlv->sub_tlvs[i], buffer + index);
        }
    } else {
        memcpy(buffer + index, tlv->value, tlv->length);
        index += tlv->length;
    }

    return index;
}

TLV *tlv_unpack(const unsigned char *buffer, size_t buffer_len, size_t *bytes_read) {

    size_t index = 0;
    unsigned char tag = 0;
    int length = 0;
    int lenLen = 0;
    
    if (buffer_len < 2) return NULL;    
    
    tag = buffer[index++];
    if(tag < 0x80) return NULL;
    
    length = buffer[index++];
    
    
    switch(length)
    {
        case 0x81:
        {
            length = buffer[index++];
            lenLen=1;
            break;
        }
        case 0x82:
         {
             lenLen=2;
            memcpy(&length,buffer+index,lenLen);
            length = Utl_BcdToInt((const char *)buffer+index,lenLen);
            index++;
            index++;
            break;
         }
        default:
        break;
    }

    
    if (length > buffer_len - 2 -lenLen) return NULL;

    TLV *tlv = tlv_create_container(tag);
    size_t consumed = 0;

    while (consumed < length) {
        size_t read = 0;
        TLV *child = tlv_unpack(buffer + index + consumed , length - consumed, &read);
        if (!child) break;
        tlv_add_sub_tlv(tlv, child);
        consumed += read;
    }

    if (tlv->sub_tlv_count == 0) {
        // No sub-TLVs, treat as value
        free(tlv->sub_tlvs);
        free(tlv);
        TLV *leaf = (TLV *)malloc(sizeof(TLV));
        leaf->tag = tag;
        leaf->length = length;
        leaf->value = (unsigned char *)malloc(length);
        memcpy(leaf->value, buffer + index, length);
        leaf->sub_tlv_count = 0;
        leaf->sub_tlvs = NULL;
        *bytes_read = index + length;
        return leaf;
    }

    *bytes_read = index + length;
    return tlv;
}
