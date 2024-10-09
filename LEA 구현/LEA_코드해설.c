#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define ROUND_SIZE 4

// 함수 선언부
void LEA_enc_KeySchedule(uint32_t *K, uint32_t* RK);
void LEA_dec_KeySchedule(uint32_t *K, uint32_t* RK);
void LEA_Encrypt(uint32_t *P, uint32_t *C, uint32_t *RK);
void LEA_Decrypt(uint32_t *C, uint32_t *D, uint32_t *RK);
void Printtext(uint32_t* X, int size);

// 라운드 상수 배열 (R)
const uint32_t R[ROUND_SIZE] = {0xc3efe9db, 0x44626b02, 0x79e27c8a, 0x78df30ec};

// 왼쪽으로 비트를 회전하는 함수
uint32_t rotateLeft(uint32_t value, unsigned int count){
    return (value << count) | (value >> (32 - count));
}

// 오른쪽으로 비트를 회전하는 함수
uint32_t rotateRight(uint32_t value, unsigned int count){
    return (value >> count) | (value << (32 - count));
}

// 라운드 키를 출력하는 함수
void keyScheduleprint(uint32_t* RK){
    printf("\nkeySchedule : \n");
    for (int i =0; i<144; i++) {
        if(i%6 == 0)
            printf("\n");
        printf("%08x ", RK[i]);
    }
}

// 암호화를 위한 키 스케줄링 함수 (라운드 키 생성)
void LEA_enc_KeySchedule(uint32_t* K, uint32_t* RK){
    uint32_t T[4];           // 임시 배열로 키 구성 요소 저장
    memcpy(T, K, 16);        // 키를 임시 배열에 복사
    uint32_t i = 0, j = 0;   // 반복문을 위한 변수

    for (i = 0; i < 24; i++){   // 24 라운드 키 생성 반복
        // 라운드 상수와 회전을 통해 T 값을 갱신
        T[0] = (T[0]+rotateLeft(R[i & 3], i));
        T[0]  = rotateLeft(T[0], 1);

        T[1] = (T[1]+rotateLeft(R[i & 3], i+1));
        T[1]  = rotateLeft(T[1], 3);

        T[2] = (T[2]+rotateLeft(R[i & 3], i+2));
        T[2]  = rotateLeft(T[2], 6);

        T[3] = (T[3]+rotateLeft(R[i & 3], i+3));
        T[3]  = rotateLeft(T[3], 11);

        // 라운드 키 저장
        RK[j] = T[0];
        RK[j+1] = T[1];
        RK[j+2] = T[2];
        RK[j+3] = T[1];
        RK[j+4] = T[3];
        RK[j+5] = T[1];
        j += 6;  // 다음 라운드 키 위치로 이동
    }
}

// 복호화를 위한 키 스케줄링 함수 (역방향으로 라운드 키 생성)
void LEA_dec_KeySchedule(uint32_t* K, uint32_t* RK){
    uint32_t T[4];           // 임시 배열로 키 구성 요소 저장
    memcpy(T, K, 16);        // 키를 임시 배열에 복사
    uint32_t i = 0, j=138;   // 반복문을 위한 변수 (역방향 시작)

    for (i = 0; i < 24; i++){   // 24 라운드 키를 역순으로 생성
        // 라운드 상수와 회전을 통해 T 값을 갱신
        T[0] = (T[0]+rotateLeft(R[i & 3], i));
        T[0]  = rotateLeft(T[0], 1);

        T[1] = (T[1]+rotateLeft(R[i & 3], i+1));
        T[1]  = rotateLeft(T[1], 3);

        T[2] = (T[2]+rotateLeft(R[i & 3], i+2));
        T[2]  = rotateLeft(T[2], 6);

        T[3] = (T[3]+rotateLeft(R[i & 3], i+3));
        T[3]  = rotateLeft(T[3], 11);

        // 라운드 키를 역순으로 저장
        RK[j] = T[0];
        RK[j+1] = T[1];
        RK[j+2] = T[2];
        RK[j+3] = T[1];
        RK[j+4] = T[3];
        RK[j+5] = T[1];
        j -= 6;  // 이전 라운드 키 위치로 이동
    }
}

// LEA 암호화를 수행하는 함수 (라운드 키 사용)
void LEA_Encrypt(uint32_t * P, uint32_t * C, uint32_t *RK){
    uint32_t X[4];           // 평문을 저장할 임시 배열
    uint32_t j=0;            // 라운드 키의 인덱스
    memcpy(X, P, 16);        // 평문을 X에 복사

    for (int i = 0; i < 24; i++){   // 24번의 암호화 라운드를 수행
        // 암호화 라운드 연산 수행
        C[0] = rotateLeft((X[0]^RK[j++]) + (X[1]^RK[j++]), 9);
        C[1] = rotateRight((X[1]^RK[j++]) + (X[2]^RK[j++]), 5);
        C[2] = rotateRight((X[2]^RK[j++]) + (X[3]^RK[j++]), 3);
        C[3] = X[0];  // 다음 라운드를 위해 X[0]을 C[3]에 저장
        memcpy(X, C, 16);  // 다음 라운드를 위해 X를 갱신
    }
}

// LEA 복호화를 수행하는 함수 (라운드 키 사용)
void LEA_Decrypt(uint32_t *C, uint32_t *D, uint32_t *RK){
    uint32_t X[4];          // 암호문을 저장할 임시 배열
    uint32_t j=0;           // 라운드 키의 인덱스
    memcpy(X, C, 16);       // 암호문을 X에 복사

    for (int i = 0; i < 24; i++){   // 24번의 복호화 라운드를 수행
        // 복호화 라운드 연산 수행
        D[0] = X[3];  // 암호화 과정에서 C[3]에 저장된 X[0]을 복원

        D[1] = rotateRight(X[0], 9);  // 암호화 시 수행된 회전 연산을 복원
        D[1] = (D[1] - (D[0]^RK[j++])) ^ RK[j++];

        D[2] = rotateLeft(X[1], 5);  // 암호화 시 수행된 회전 연산을 복원
        D[2] = (D[2] - (D[1]^RK[j++])) ^ RK[j++];

        D[3] = rotateLeft(X[2], 3);  // 암호화 시 수행된 회전 연산을 복원
        D[3] = (D[3] - (D[2]^RK[j++])) ^ RK[j++];
  
        memcpy(X, D, 16);  // 다음 라운드를 위해 X를 갱신
    }
}

// 평문, 암호문 또는 복호문을 출력하는 함수
void Printtext(uint32_t* X, int size){
    for (int i =0; i<size; i++) {
        if(i% ROUND_SIZE == 0)
            printf("\n");
        printf("%08x ", X[i]);
    }
}

int main() {
    // 샘플 키와 평문 (각각 128비트)
    uint32_t K[4] = { 0x3c2d1e0f, 0x78695a4b, 0xb4a59687, 0xf0e1d2c3 };
    uint32_t P[4] = { 0x13121110, 0x17161514, 0x1b1a1918, 0x1f1e1d1c };
    uint32_t RK[144] = {0,};   // 암호화/복호화를 위한 라운드 키 저장 배열
    uint32_t C[128] = {0,};    // 암호문 저장 배열
    uint32_t D[128] = {0,};    // 복호문 저장 배열

    // 평문 출력
    printf("\nplaintext :");
    Printtext(P, 4);

    // 평문 암호화
    printf("\nciphertext :");
    LEA_enc_KeySchedule(K, RK);  // 암호화를 위한 키 스케줄링
    LEA_Encrypt(P, C, RK);       // 암호화 수행
    Printtext(C, 4);             // 암호문 출력

    // 암호문 복호화
    printf("\ndecryption text :");
    LEA_dec_KeySchedule(K, RK);  // 복호화를 위한 키 스케줄링
    LEA_Decrypt(C, D, RK);       // 복호화 수행
    Printtext(D, 4);             // 복호문 출력 (원래의 평문과 일치해야 함)
}