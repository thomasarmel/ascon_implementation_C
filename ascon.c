#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

const bool SHOULD_ACTIVATE_10_BITS_S_BOX = true;

typedef uint64_t bit64;
typedef uint32_t bit32;

bit64 state[5] = {0}, t[5] = {0};
bit64 constants[16] = {0xf0, 0xe1, 0xd2, 0xc3, 0xb4, 0xa5, 0x96, 0x87, 0x78, 0x69, 0x5a, 0x4b, 0x3c, 0x2d, 0x1e, 0x0f};

const bit32 SBOX10[1024] = {144, 348, 448, 658, 257, 777, 552, 879, 527, 654, 764, 889, 532, 133, 702, 617, 914,
                            856, 865, 713, 571, 748, 440, 85, 145, 424, 138, 642, 847, 878, 814, 175, 530, 892,
                            248, 275, 688, 473, 866, 715, 153, 488, 667, 623, 284, 117, 620, 443, 1004, 39, 915,
                            744, 996, 590, 670, 851, 373, 846, 410, 62, 211, 806, 694, 550, 849, 724, 984, 863,
                            353, 499, 975, 45, 1016, 279, 420, 901, 467, 946, 525, 178, 197, 971, 995, 671, 743,
                            166, 627, 668, 355, 528, 29, 131, 414, 407, 633, 818, 720, 905, 951, 904, 733, 397,
                            83, 893, 850, 326, 434, 985, 585, 219, 752, 1018, 293, 480, 108, 1017, 115, 465, 290,
                            611, 278, 130, 693, 40, 297, 181, 840, 782, 90, 291, 909, 333, 706, 137, 925, 444,
                            343, 150, 430, 616, 100, 89, 217, 988, 651, 684, 351, 136, 831, 378, 907, 141, 601,
                            867, 339, 731, 871, 119, 120, 789, 96, 952, 1013, 943, 404, 347, 68, 938, 769, 286,
                            86, 700, 287, 625, 819, 560, 657, 301, 69, 853, 884, 56, 146, 63, 935, 182, 610, 209,
                            88, 898, 506, 220, 803, 168, 340, 194, 405, 493, 15, 796, 942, 594, 994, 349, 756,
                            656, 312, 635, 810, 740, 712, 957, 180, 111, 931, 595, 189, 451, 299, 362, 841, 330,
                            1014, 380, 169, 49, 398, 763, 963, 33, 55, 123, 770, 652, 887, 229, 118, 1022, 714,
                            732, 513, 599, 323, 880, 269, 184, 545, 1023, 50, 563, 983, 171, 531, 270, 554, 381,
                            728, 852, 762, 875, 198, 711, 953, 266, 861, 66, 576, 441, 730, 214, 188, 110, 402,
                            854, 567, 246, 32, 105, 215, 586, 549, 802, 127, 544, 237, 534, 366, 741, 508, 896,
                            559, 813, 325, 225, 222, 59, 109, 77, 103, 494, 483, 239, 183, 645, 675, 439, 132,
                            704, 913, 628, 600, 945, 792, 778, 547, 2, 855, 210, 459, 476, 363, 466, 725, 641,
                            755, 507, 981, 172, 470, 1006, 234, 766, 102, 318, 540, 450, 479, 129, 372, 848, 604,
                            565, 122, 746, 317, 155, 780, 719, 710, 10, 251, 71, 663, 504, 218, 964, 1011, 679,
                            553, 939, 421, 692, 661, 798, 622, 42, 304, 1002, 543, 751, 640, 230, 80, 192, 902,
                            361, 717, 163, 329, 804, 413, 542, 159, 899, 314, 636, 191, 474, 445, 44, 774, 162,
                            815, 358, 306, 352, 515, 775, 701, 95, 240, 344, 212, 557, 360, 82, 612, 614, 322,
                            566, 930, 858, 60, 650, 808, 447, 937, 665, 512, 1003, 596, 660, 947, 365, 707, 295,
                            485, 468, 797, 271, 759, 475, 779, 319, 788, 76, 575, 579, 516, 890, 4, 573, 918, 258,
                            991, 461, 630, 598, 140, 106, 541, 336, 838, 597, 519, 998, 621, 65, 976, 979, 511,
                            114, 453, 917, 829, 997, 1020, 772, 151, 265, 341, 70, 221, 649, 332, 205, 428, 462,
                            609, 548, 388, 411, 303, 773, 767, 92, 23, 589, 941, 820, 555, 750, 249, 500, 735,
                            683, 199, 569, 521, 584, 412, 261, 881, 227, 524, 509, 510, 877, 18, 19, 911, 256,
                            568, 311, 5, 327, 164, 972, 900, 961, 113, 35, 771, 280, 870, 520, 533, 375, 370, 910,
                            26, 124, 882, 765, 170, 452, 934, 501, 385, 432, 53, 860, 390, 583, 177, 320, 674,
                            999, 1012, 529, 978, 800, 37, 634, 624, 139, 505, 817, 236, 1019, 536, 469, 51, 666,
                            967, 79, 416, 174, 613, 956, 273, 389, 958, 982, 38, 201, 677, 906, 523, 274, 152, 7,
                            394, 570, 223, 669, 41, 101, 307, 824, 438, 25, 383, 455, 302, 369, 337, 262, 833,
                            538, 72, 897, 646, 655, 167, 267, 927, 791, 582, 776, 843, 309, 43, 648, 28, 57, 176,
                            300, 97, 342, 696, 783, 20, 729, 705, 496, 960, 208, 615, 231, 781, 929, 673, 753,
                            790, 409, 919, 949, 206, 698, 695, 874, 374, 491, 54, 16, 845, 433, 832, 517, 591,
                            173, 31, 738, 1000, 264, 367, 727, 664, 989, 425, 442, 437, 886, 276, 837, 73, 232,
                            785, 959, 12, 193, 161, 632, 46, 872, 64, 449, 992, 894, 418, 709, 382, 328, 522, 827,
                            247, 253, 406, 93, 966, 30, 537, 844, 526, 842, 87, 202, 415, 179, 546, 203, 316, 928,
                            149, 825, 379, 626, 446, 268, 954, 721, 143, 386, 535, 431, 263, 686, 243, 795, 495,
                            422, 134, 245, 460, 703, 34, 395, 1009, 252, 933, 716, 315, 260, 241, 1015, 313, 292,
                            400, 754, 126, 259, 244, 916, 723, 490, 204, 472, 310, 644, 637, 940, 213, 98, 187,
                            27, 24, 821, 639, 603, 52, 484, 396, 1010, 761, 125, 857, 393, 94, 298, 74, 346, 487,
                            399, 659, 384, 185, 112, 148, 392, 593, 359, 895, 969, 158, 14, 224, 742, 9, 47, 908,
                            17, 830, 368, 21, 834, 48, 156, 0, 699, 196, 514, 678, 685, 973, 1001, 334, 962, 868,
                            377, 11, 672, 793, 13, 285, 948, 690, 587, 993, 1007, 558, 607, 277, 936, 294, 749,
                            1021, 350, 605, 435, 869, 228, 401, 970, 3, 602, 436, 335, 75, 681, 662, 426, 331,
                            739, 456, 489, 1008, 356, 142, 619, 708, 691, 760, 955, 423, 990, 932, 128, 987, 592,
                            321, 891, 454, 482, 556, 308, 497, 835, 84, 481, 147, 836, 968, 977, 689, 697, 226,
                            477, 903, 794, 200, 417, 680, 408, 238, 564, 471, 747, 687, 920, 888, 885, 859, 154,
                            458, 653, 726, 580, 345, 67, 354, 876, 486, 561, 403, 816, 809, 578, 463, 577, 822,
                            736, 283, 250, 272, 376, 364, 492, 950, 581, 608, 737, 427, 682, 165, 464, 6, 107,
                            912, 338, 207, 1, 768, 457, 288, 562, 305, 387, 255, 539, 242, 498, 58, 757, 391, 638,
                            986, 121, 864, 190, 324, 980, 826, 676, 965, 718, 91, 186, 921, 923, 81, 216, 787,
                            157, 784, 812, 289, 643, 629, 235, 588, 296, 786, 839, 135, 503, 551, 758, 371, 722,
                            734, 922, 944, 807, 281, 572, 233, 883, 99, 618, 502, 805, 8, 574, 606, 647, 478, 745,
                            823, 22, 862, 61, 873, 429, 254, 974, 36, 195, 828, 357, 518, 160, 78, 116, 924, 811,
                            631, 419, 282, 799, 104, 1005, 926, 801};

void print_state(bit64 local_state[5]) {
    for (int i = 0; i < 5; i++) {
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

    if (SHOULD_ACTIVATE_10_BITS_S_BOX) {
        bit32 *x_as_32 = (bit32 *) x;
        for (uint8_t bit_index = 0; bit_index < 32; bit_index++) {
            uint16_t sbox_input = 0;
            for (uint8_t cell_index = 0; cell_index < 10; cell_index++) {
                sbox_input |= ((x_as_32[cell_index] >> bit_index) & 1) << cell_index;
            }
            assert(sbox_input < 1024);
            uint16_t sbox_output = SBOX10[sbox_input];
            assert(sbox_output < 1024);
            for (uint8_t cell_index = 0; cell_index < 10; cell_index++) {
                bit32 nand_mask = 0xffffffff ^ (1 << bit_index);
                x_as_32[cell_index] = (x_as_32[cell_index] & nand_mask) | (((sbox_output >> cell_index) & 1) << bit_index);
            }
        }
    } else {
        x[0] ^= x[4]; x[4] ^= x[3]; x[2] ^= x[1];
        t[0] = x[0]; t[1] = x[1]; t[2] = x[2]; t[3] = x[3]; t[4] = x[4];
        t[0] =~ t[0]; t[1] =~ t[1]; t[2] =~ t[2]; t[3] =~ t[3]; t[4] =~ t[4];
        t[0] &= x[1]; t[1] &= x[2]; t[2] &= x[3]; t[3] &= x[4]; t[4] &= x[0];
        x[0] ^= t[1]; x[1] ^= t[2]; x[2] ^= t[3]; x[3] ^= t[4]; x[4] ^= t[0];
        x[1] ^= x[0]; x[0] ^= x[4]; x[3] ^= x[2]; x[2] =~ x[2];
    }
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

void p(bit64 local_state[5], int a) {
    for (int i = 0; i < a; i++) {
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
    for (int i = 0; i < length; i++) {
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
    for (int i = 1; i < length; i++) {
        p(local_state, 6);
        ciphertext[i] = plaintext[i] ^ local_state[0];
        local_state[0] = ciphertext[i];
    }
}

void decrypt(bit64 local_state[5], int length, bit64 plaintext[], const bit64 ciphertext[]) {
    plaintext[0] = ciphertext[0] ^ local_state[0];
    for (int i = 1; i < length; i++) {
        p(local_state, 6);
        plaintext[i] = ciphertext[i] ^ local_state[0];
        local_state[0] = ciphertext[i];
    }
}


int main() {
    // initialize nonce, key and IV
    bit64 nonce[2] = {0x0000000000000001, 0x0000000000000002};
    bit64 key[2] = {0};
    bit64 IV = 0x80400c0600000000;
    bit64 plaintext[] = {0x1234567890abcdef, 0x1234567890abcdef};
    bit64 ciphertext[2] = {0};
    bit64 associated_data_text[] = {0x787878, 0x878787, 0x09090};

    //encryption
    //initialize state
    state[0] = IV;
    state[1] = key[0];
    state[2] = key[1];
    state[3] = nonce[0];
    state[4] = nonce[1];
    initialization(state, key);
    associated_data(state, 3, associated_data_text);
    print_state(state);
    encrypt(state, 2, plaintext, ciphertext);
    printf("\nciphertext: %016lx %016lx\n", ciphertext[0], ciphertext[1]);
    finalization(state, key);
    printf("tag: %016lx %016lx\n", state[3], state[4]);



    //decryption

    bit64 ciphertextdecrypt[2] = {0};
    for (int i = 0; i < 2; i++) {
        ciphertextdecrypt[i] = ciphertext[i];
    }
    bit64 plaintextdecrypt[10] = {0};

    //initialize state
    state[0] = IV;
    state[1] = key[0];
    state[2] = key[1];
    state[3] = nonce[0];
    state[4] = nonce[1];

    initialization(state, key);
    print_state(state);
    associated_data(state, 3, associated_data_text);
    decrypt(state, 2, plaintextdecrypt, ciphertextdecrypt);
    printf("\nplaintext: %016lx %016lx\n", plaintextdecrypt[0], plaintextdecrypt[1]);
    finalization(state, key);
    printf("tag: %016lx %016lx\n", state[3], state[4]);
}