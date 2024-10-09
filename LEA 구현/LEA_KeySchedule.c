#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define ROUND_SIZE 4

const uint32_t R[ROUND_SIZE] = {0xc3efe9db, 0x44626b02, 0x79e27c8a, 0x78df30ec};

uint32_t rotateLeft(uint32_t value, unsigned int count){
    return (value << count) | (value >> (32 - count));
}

void keySchedule(uint32_t* K, uint32_t* RK){
    uint32_t T[4];
    memcpy(T, K, 16);
    uint32_t i = 0, j = 0;

    for (i = 0; i < 24; i++){
        T[0] = (T[0]+rotateLeft(R[i & 3], i));
        T[0]  = rotateLeft(T[0] ,1);

        T[1] = (T[1]+rotateLeft(R[i & 3], i+1));
        T[1]  = rotateLeft(T[1] ,3);

        T[2] = (T[2]+rotateLeft(R[i & 3], i+2));
        T[2]  = rotateLeft(T[2] ,6);

        T[3] = (T[3]+rotateLeft(R[i & 3], i+3));
        T[3]  = rotateLeft(T[3] ,11);

        RK[j] = T[0];
        RK[j+1] = T[1];
        RK[j+2] = T[2];
        RK[j+3] = T[1];
        RK[j+4] = T[3];
        RK[j+5] = T[1];
        j+=6;
    }
}

void keyScheduleprint(uint32_t* RK){
    printf("\nkeySchedule : \n");
    for (int i =0; i<144; i++) {
        if(i%6 == 0)
            printf("\n");
        printf("%08x ", RK[i]);}
}

int main() {
    uint32_t K[4] = { 0x3c2d1e0f, 0x78695a4b, 0xb4a59687, 0xf0e1d2c3 };
    uint32_t RK[144] = {0,};
    keySchedule(K,RK);

    keyScheduleprint(RK);
}