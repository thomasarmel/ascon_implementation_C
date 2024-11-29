#include <stdio.h>
#include <stdint.h>
typedef uint64_t bit64;

bit64 state[5] = { 0 }, t[5] = { 0 };
bit64 constants[16] = {0xf0, 0xe1, 0xd2, 0xc3, 0xb4, 0xa5, 0x96, 0x87, 0x78, 0x69, 0x5a, 0x4b, 0x3c, 0x2d, 0x1e, 0x0f};

void print_state(bit64 local_state[5]) {
   for(int i = 0; i < 5; i++){
      printf("%016lx\n", local_state[i]);
   } 
}

bit64 rotate(bit64 x, int l) {
   bit64 temp;
   temp = (x >> l) ^ (x << (64 - l));
   return temp;
}

void add_constant(bit64 local_state[5], int i, int a) {
   // Menambah konstan pada local_state blok ke 2 sesuai dengan spec Ascon
   local_state[2] = local_state[2] ^ constants[12 - a + i];
}
void sbox(bit64 x[5]) {
   // Mensubtitusikan angka menjadi angka baru pada state sesuai dengan sbox
   // Biasanya sbox dilakukan dengan menggunakan tabel lookup
   // tapi juga bisa menggunakan bitslice implementation sesuai dengan spec ascon
   // justru bitwise operation lebih bagus karena lebih ringan
   // dan juga menghindari penggunaan side channel attack.

   x[0] ^= x[4]; x[4] ^= x[3]; x[2] ^= x[1];
   t[0] = x[0]; t[1] = x[1]; t[2] = x[2]; t[3] = x[3]; t[4] = x[4];
   t[0] =~ t[0]; t[1] =~ t[1]; t[2] =~ t[2]; t[3] =~ t[3]; t[4] =~ t[4];
   t[0] &= x[1]; t[1] &= x[2]; t[2] &= x[3]; t[3] &= x[4]; t[4] &= x[0];
   x[0] ^= t[1]; x[1] ^= t[2]; x[2] ^= t[3]; x[3] ^= t[4]; x[4] ^= t[0];
   x[1] ^= x[0]; x[0] ^= x[4]; x[3] ^= x[2]; x[2] =~ x[2];
}
void linear(bit64 local_state[5]) {
   // Kita akan melakukan operasi rotasi terhadap local_state dengan tiap
   // 64 bit memiliki rotasi yang berbeda.
   // besar bit rotasi ditentukan pada spec ascon paper.
   
   bit64 temp0, temp1;
   temp0 = rotate(local_state[0], 19);
   temp1 = rotate(local_state[0], 28);
    local_state[0] ^= temp0 ^ temp1;
   temp0 = rotate(local_state[1], 61);
   temp1 = rotate(local_state[1], 39);
    local_state[1] ^= temp0 ^ temp1;
   temp0 = rotate(local_state[2], 1);
   temp1 = rotate(local_state[2], 6);
    local_state[2] ^= temp0 ^ temp1;
   temp0 = rotate(local_state[3], 10);
   temp1 = rotate(local_state[3], 17);
    local_state[3] ^= temp0 ^ temp1;
   temp0 = rotate(local_state[4], 7);
   temp1 = rotate(local_state[4], 41);
    local_state[4] ^= temp0 ^ temp1;
}

void p(bit64 local_state[5], int a){
   for (int i = 0; i < a; i++){
      add_constant(local_state, i, a);
      sbox(local_state);
      linear(local_state);
   }
}

void initialization(bit64 local_state[5], const bit64 key[2]) {
   p(local_state, 12);
    local_state[3] ^= key[0];
    local_state[4] ^= key[1];
}

void associated_data(bit64 local_state[5], int length, const bit64 associated_data_text[]) {
   for (int i = 0; i < length; i++){
       local_state[0] = associated_data_text[i] ^ local_state[0];
      p(local_state, 6);
   }
    local_state[5] = local_state[5] ^ 0x0000000000000001;
}

void finalization(bit64 local_state[5], const bit64 key[2]) {
    local_state[1] ^= key[0];
    local_state[2] ^= key[1];
   p(local_state, 12);
    local_state[3] ^= key[0];
    local_state[4] ^= key[1];

}

void encrypt(bit64 local_state[5], int length, const bit64 plaintext[], bit64 ciphertext[]) {
   ciphertext[0] = plaintext[0] ^ local_state[0];
   for (int i = 1; i < length; i++){
      p(local_state, 6);
      ciphertext[i] = plaintext[i] ^ local_state[0];
       local_state[0] = ciphertext[i];
   }
}

void decrypt(bit64 local_state[5], int length, bit64 plaintext[], const bit64 ciphertext[]){
   plaintext[0] = ciphertext[0] ^ local_state[0];
   for (int i = 1; i < length; i++){
      p(local_state, 6);
      plaintext[i] = ciphertext[i] ^ local_state[0];
       local_state[0] = ciphertext[i];
   }
}


int main() {
   // initialize nonce, key and IV
   bit64 nonce[2] = { 0x0000000000000001, 0x0000000000000002 };
   bit64 key[2] = { 0 };
   bit64 IV = 0x80400c0600000000;
   bit64 plaintext[] = {0x1234567890abcdef, 0x1234567890abcdef};
   bit64 ciphertext[2] = { 0 };
   bit64 associated_data_text[] = { 0x787878, 0x878787, 0x09090};

   //encryption
   //initialize state
   state[0] = IV;
   state[1] = key[0];
   state[2] = key[1];
   state[3] = nonce[0];
   state[4] = nonce[1];
   initialization(state,key);
   associated_data(state, 3, associated_data_text);
   print_state(state);
   encrypt(state, 2, plaintext, ciphertext);
   printf("\nciphertext: %016lx %016lx\n", ciphertext[0], ciphertext[1]);
   finalization(state, key);
   printf("tag: %016lx %016lx\n", state[3], state[4]);



   //decryption
        
   bit64 ciphertextdecrypt[2] = {0};
   for(int i = 0; i < 2; i++){
      ciphertextdecrypt[i] = ciphertext[i];
   }
   bit64 plaintextdecrypt[10] = { 0 };

   //initialize state
   state[0] = IV;
   state[1] = key[0];
   state[2] = key[1];
   state[3] = nonce[0];
   state[4] = nonce[1];

   initialization(state,key);
   print_state(state);
   associated_data(state, 3, associated_data_text);
   decrypt(state, 2, plaintextdecrypt, ciphertextdecrypt);
   printf("\nplaintext: %016lx %016lx\n", plaintextdecrypt[0], plaintextdecrypt[1]);
   finalization(state, key);
   printf("tag: %016lx %016lx\n", state[3], state[4]);}