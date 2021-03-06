//
// Created by virgil on 2/22/20.
//

#include "packer.h"
#include <string.h>
#include <unistd.h>
#include <stdlib.h>


int readn(int fd,uint8_t* buf,int size)
{
    int n = read(fd, buf, size);
    if(n <= 0){
        return -1;
    }
    while(n < size)
    {
        int new_n = read(fd,buf+n, size - n);
        if(new_n <= 0){
            return -1;
        }
        n += new_n;
    }
    return n;
}

int sendn(int fd,uint8_t* buf,int size)
{
    int n;
    if((n = write(fd,buf,size)) <= 0){
        return -1;
    }
    while(n < size)
    {
        int new_n = write(fd,buf+n, size - n);
        if(new_n <= 0){
            return -1;
        }
        n += new_n;
    }
    return n;
}

int send_pack(int fd,struct pack_data* pk)
{
    int r;
    uint8_t* buf;
    if((buf = (uint8_t*)malloc(pk->len + 2)) == NULL){
        return -1;
    }
    buf[0] = PACK_HEAD;
    buf[1] = pk->len;
    for(int i=0;i<pk->len;i++)
    {
        buf[2+i] = pk->data[i];
    }
    if((r = sendn(fd,buf,pk->len+2)) <= 0){
        free(buf);
        return -2;
    }
    free(buf);
    return 0;
}

int recv_pack(int fd,struct pack_data* pk)
{
    int r;
    uint8_t buf[PACK_DATA_MAXSIZE + 16] = {0};
    while(buf[0] != PACK_HEAD){
        if((r = read(fd,buf,1)) <= 0){
            return -1;
        }
    }
    //get length
    if((r = read(fd,buf+1,1)) <= 0){
        return -1;
    }
    pk->len = (int)buf[1];
    //get data
    if((r = readn(fd,pk->data,pk->len)) <= 0){
        return -1;
    }
    return 0;
}

