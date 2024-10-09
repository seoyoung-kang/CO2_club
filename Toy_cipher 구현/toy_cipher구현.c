#include <stdio.h>
#include <stdint.h>

#define CONST1 0xaa
#define CONST2 0xbb
#define CONST3 0xcc
#define CONST4 0xdd
#define CONST5 0xee
#define CONST6 0xff
#define CONST7 0x88
#define CONST8 0x99

uint8_t Hamming_Weight(uint8_t byte){
    uint8_t count = 0;
    while(byte){ //오른쪽으로 계속 밀다가 00000000이렇게 나오면 while문이 멈춘다. 
        count += byte & 1; //최하위 비트가 1인지 확인하여 count에 and연산을 통해 더함
        // 만약 최하위 byte가 0이라면 0, byte가 1이라면 count에 더함. 
        byte >>= 1; //byte를 오른쪽으로 1비트 이동시켜 다음 최하위 비트 검사
        // 계속 오른쪽으로 밀다 보면 00000000이렇게 출력된다. 
    }
    return count;
}

uint8_t XOR(uint8_t byte){
    uint8_t HW = Hamming_Weight(byte);
    return byte ^ HW; //^:XOR 연산자
}


uint8_t Add(uint8_t byte, int i){
    switch(i){ //지금 들어온 byte가 몇 번 byte인지 모르기에, switch문으로 만들었음.
        case 0: return CONST1 + byte;
        case 1: return CONST2 + byte;
        case 2: return CONST3 + byte;
        case 3: return CONST4 + byte;
        case 4: return CONST5 + byte;
        case 5: return CONST6 + byte;
        case 6: return CONST7 + byte;
        case 7: return CONST8 + byte;
        default: return byte;
    } 
}

uint8_t swap_4bit(uint8_t byte){
    return ((byte & 0x0F) << 4) | ((byte & 0xF0) >> 4);

    /*
    1. 비트 마스크와 시프트 연산을 사용하여 상위 4비트와 하위 4비트를 분리:
	   - byte & 0x0F는 byte의 하위 4비트를 추출합니다.
	   - byte & 0xF0는 byte의 상위 4비트를 추출합니다.
	2. 비트 이동:
	   - << 4 연산은 하위 4비트를 상위 4비트로 이동시킵니다.
	   - >> 4 연산은 상위 4비트를 하위 4비트로 이동시킵니다.
	3. 비트 결합:
	   - | 연산자를 사용하여 이동된 상위 4비트와 하위 4비트를 결합합니다.
    */

}

void toy_cipher(uint8_t* pt, uint8_t* ct){
    for (int i = 0; i<8; i++){
        uint8_t byte = pt[i];
        
        byte = XOR(byte);
        byte = Add(byte,i);
        byte = swap_4bit(byte);

        ct[i] = byte;
    }
}

int main() {
    uint8_t pt[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    uint8_t ct[8];

    printf("plain text : ");
    for(int i = 0; i < 8; i++){
        printf("%02x", pt[i]);  //02x가 16진수로 출력되게 하는 것
    }
    printf("\n");

    toy_cipher(pt,ct);

    printf("cipher text : ");
    for(int i = 0; i < sizeof(pt); i++){
        printf("%02x", ct[i]);
    }
    printf("\n");

    return 0;
}

