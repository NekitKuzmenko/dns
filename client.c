#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include "addr.hpp"



void uint16_to_str(unsigned short number, char *buffer, unsigned long offset) {

    buffer[offset] = number / 256;
    buffer[offset+1] = number % 256;
        
}

void uint32_to_str(unsigned int number, char *buffer, unsigned long offset) {

    buffer[offset] = number >> 24;
    buffer[offset+1] = number >> 16;
    buffer[offset+2] = number >> 8;
    buffer[offset+3] = number;
        
}

void uint64_to_str(unsigned long number, char *buffer, unsigned long offset) {

    buffer[offset] = number >> 56;
    buffer[offset+1] = number >> 48;
    buffer[offset+2] = number >> 40;
    buffer[offset+3] = number >> 32;
    buffer[offset+4] = number >> 24;
    buffer[offset+5] = number >> 16;
    buffer[offset+6] = number >> 8;
    buffer[offset+7] = number;
        
}

void str_to_uint16(char *buffer, unsigned long offset, unsigned short *number) {

    *number = (unsigned char)buffer[offset] * 256 + (unsigned char)buffer[offset+1];
    
}

void str_to_uint32(char *buffer, unsigned long offset, unsigned int *number) {

    *number = (unsigned char)buffer[offset] * 16777216 + (unsigned char)buffer[offset+1] * 65536 + (unsigned char)buffer[offset+2] * 256 + (unsigned char)buffer[offset+3];

}

void str_to_uint64(char *buffer, unsigned long offset, unsigned long *number) {

    *number = (unsigned char)buffer[offset] * 72057594037927936 + (unsigned char)buffer[offset+1] * 281474976710656 + (unsigned char)buffer[offset+2] * 1099511627776 + (unsigned char)buffer[offset+3] * 4294967296 + (unsigned char)buffer[offset+4] * 16777216 + (unsigned char)buffer[offset+5] * 65536 + (unsigned char)buffer[offset+6] * 256 + (unsigned char)buffer[offset+7];

}


void str_fill(char *str, unsigned long offset, char *substr, unsigned long substr_len) {

    for(unsigned long i = 0; i < substr_len; i++) {
        str[offset+i] = substr[i];
    }

}



int main() {
    timespec time1, time2;

    unsigned int address = (62 << 24) + (144 << 16) + (67 << 8) + 172;
    unsigned short port = 25820;
    char *addr = new char[15];

    clock_gettime(CLOCK_REALTIME, &time1);

    for(unsigned short i = 0; i < 10000; i++) invert16(&port);

    clock_gettime(CLOCK_REALTIME, &time2);

    //uint32_to_str(address, addr, 0);

    //std::cout << (unsigned char)addr[0]+0 << " "  << (unsigned char)addr[1]+0 << " " << (unsigned char)addr[2]+0 << " " << (unsigned char)addr[3]+0 << "\n";
    std::cout << port << "\n" << (double)(time2.tv_nsec - time1.tv_nsec) / (double)10000 << "\n";
    exit(0);

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    sockaddr_in server_addr, sender_addr;
    unsigned int sender_len = sizeof(sender_addr);
    char *request, *response = new char[4];
    int request_len, response_len;
    unsigned short request_id = 0, request_count = 0, domen_len = 0, domen_part_len = 0, data_len;
    unsigned char byte;
    
    char* domen = "moder.nk-sys.ru";
    unsigned int pointer;

    struct dns_info {
        short TYPE, CLASS;
        unsigned int TTL, IPv4;
    };

    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("8.8.8.8");
    server_addr.sin_port = 53 << 8;

    for(;;) {

        clock_gettime(CLOCK_REALTIME, &time1);

        request_id++;

        for(;;) {
            if(domen[domen_len] == 0) break; else domen_len++;
        }
    
        request_len = 18+domen_len;
        request = new char[request_len];
        
        uint16_to_str(request_id, request, 0);

        byte = 0;
        byte << 1;
        byte << 4;
        byte << 1;
        byte << 1;
        byte++;
        
        request[2] = byte;

        byte = 0;
        byte << 1;
        byte << 3;
        byte << 4;
        
        request[3] = byte;

        request_count = 1;

        uint16_to_str(request_count, request, 4);
        uint16_to_str(0, request, 6);
        uint16_to_str(0, request, 8);
        uint16_to_str(0, request, 10);

        for(unsigned short i = 0; i < domen_len; i++) {

            if(domen[i] == '.') {

                request[13+i-domen_part_len-1] = domen_part_len;

                domen_part_len = 0;

            } else {

                request[13+i] = domen[i];

                domen_part_len++;

            }

        }
        
        request[13+domen_len-domen_part_len-1] = domen_part_len;

        request[13+domen_len] = 0;

        //std::cout << domen_len << " | " << 13+domen_len+4 << "\n";

        uint16_to_str(1, request, 13+domen_len+1);
        uint16_to_str(1, request, 13+domen_len+3);
    
        sendto(sock, request, request_len, 0, (sockaddr*)&server_addr, sizeof(sockaddr_in));

        response_len = recvfrom(sock, response, 100, 0, (sockaddr*)&sender_addr, &sender_len);

        std::cout << response_len;

        pointer = 12+domen_len+2+4+10;
        std::cout << "\n" << pointer << "\n";
        for(unsigned short i = 0; i < response_len; i++ ) std::cout << i << " - " << response[i] << "(" << (unsigned char)response[i]+0 << ")\n";

        str_to_uint16(response, pointer, &data_len);
        std::cout << pointer << " " << data_len << "\n";
        exit(0);
        for(unsigned short i = pointer; i < pointer+data_len; i++ ) std::cout << i << " - " << response[i] << "(" << (unsigned char)response[i]+0 << ")\n";

        clock_gettime(CLOCK_REALTIME, &time2);
        
        std::cout << "responsed in: " << (time2.tv_nsec - time1.tv_nsec) / 1000 << " micro sec\n";

        exit(0);

        sleep(1);

    }

}
