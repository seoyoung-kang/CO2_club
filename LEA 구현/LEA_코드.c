#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define ROUND_SIZE 4

void LEA_enc_KeySchedule(uint32_t *K, uint32_t* RK);
void LEA_dec_KeySchedule(uint32_t *K, uint32_t* RK);
void LEA_Encrypt(uint32_t *P, uint32_t *C, uint32_t *RK);
void LEA_Decrypt(uint32_t *C, uint32_t *D, uint32_t *RK);
void Printtext(uint32_t* X, int size);

const uint32_t R[ROUND_SIZE] = {0xc3efe9db, 0x44626b02, 0x79e27c8a, 0x78df30ec};

uint32_t rotateLeft(uint32_t value, unsigned int count){
    return (value << count) | (value >> (32 - count));
}

uint32_t rotateRight(uint32_t value, unsigned int count){
    return (value >> count) | (value << (32 - count));
}

void keyScheduleprint(uint32_t* RK){
    printf("\nkeySchedule : \n");
    for (int i =0; i<144; i++) {
        if(i%6 == 0)
            printf("\n");
        printf("%08x ", RK[i]);}
}

void LEA_enc_KeySchedule(uint32_t* K, uint32_t* RK){
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

void LEA_dec_KeySchedule(uint32_t* K, uint32_t* RK){
    uint32_t T[4];
    memcpy(T, K, 16);
    uint32_t i = 0, j=138;

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
        j-=6;
    }
}

void LEA_Encrypt(uint32_t * P, uint32_t * C, uint32_t *RK){
    uint32_t X[4];  uint32_t j=0;
    memcpy(X, P, 16);

    for (int i = 0; i < 24; i++){
        C[0] = rotateLeft((X[0]^RK[j++]) + (X[1]^RK[j++]),9);
        C[1] = rotateRight((X[1]^RK[j++]) + (X[2]^RK[j++]),5);
        C[2] = rotateRight((X[2]^RK[j++]) + (X[3]^RK[j++]),3);
        C[3] = X[0];
    memcpy(X, C, 16);
    }
}

void LEA_Decrypt(uint32_t *C, uint32_t *D, uint32_t *RK){
    uint32_t X[4];      uint32_t j=0;
    memcpy(X, C, 16);

    for (int i = 0; i < 24; i++){
        D[0] = X[3];

        D[1] = rotateRight(X[0], 9);
        D[1] = (D[1] - (D[0]^RK[j++])) ^ RK[j++];

        D[2] = rotateLeft(X[1], 5);
        D[2] = (D[2] - (D[1]^RK[j++])) ^ RK[j++];

        D[3] = rotateLeft(X[2], 3);
        D[3] = (D[3] - (D[2]^RK[j++])) ^ RK[j++];
  
        memcpy(X, D, 16);
    }
}

void Printtext(uint32_t* X, int size){
    for (int i =0; i<size; i++) {
        if(i% ROUND_SIZE == 0)
            printf("\n");
        printf("%08x ", X[i]);
    }
}

int main() {
    uint32_t K[4] = { 0x3c2d1e0f, 0x78695a4b, 0xb4a59687, 0xf0e1d2c3 };
    uint32_t P[4] = { 0x13121110, 0x17161514, 0x1b1a1918, 0x1f1e1d1c };
    uint32_t RK[144] = {0,};
    uint32_t C[128] = {0,};
    uint32_t D[128] = {0,};

    printf("\nplaintext :");
    Printtext(P, 4);

    printf("\nciphertext :");
    LEA_enc_KeySchedule(K,RK);
    LEA_Encrypt(P, C, RK);
    Printtext(C, 4);

    printf("\ndecryption text :");
    LEA_dec_KeySchedule(K, RK);
    LEA_Decrypt(C, D, RK);
    Printtext(D, 4);
}
