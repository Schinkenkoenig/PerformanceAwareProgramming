#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned char uint8;

enum register_part_kind { FULL, HIGH, LOW };

static char *register_name_loookup[16];
static char *simple_file = "listing_0037_single_register_mov";
static char *complex_file = "listing_0038_many_register_mov";

uint8 *get_binary_for_uint8(uint8 c) {
  uint8 *binaryStr = malloc(9); // 8 bits + null terminator
  if (binaryStr == NULL)
    return NULL;

  for (int i = 7; i >= 0; i--) {
    binaryStr[7 - i] = ((c >> i) & 1) ? '1' : '0';
  }
  binaryStr[8] = '\0';
  return binaryStr;
}

char *get_register_name_from_binary(uint8 register_binary, uint8 wide) {
  // mod(2) reg(3) r/m(3)
  // mod is assumed to be 11 since we only doing register to register
  // reg(3) is a register just fine
  // r/m too in this case
  // we need the wide uint8 for that, so wr2r1r0
  uint8 full_register_index = (wide << 3) | register_binary;

  // check boundary for mishaps
  assert(full_register_index < 16);

  return register_name_loookup[full_register_index];
}

char *get_op_code_str(uint8 op_code) {

  if (op_code == 0b100010) {
    return "mov";
  }

  // just to check for now
  return "notmov";
}

void setup_register_lookup() {
  register_name_loookup[0] = "al";
  register_name_loookup[1] = "cl";
  register_name_loookup[2] = "dl";
  register_name_loookup[3] = "bl";
  register_name_loookup[4] = "ah";
  register_name_loookup[5] = "ch";
  register_name_loookup[6] = "dh";
  register_name_loookup[7] = "bh";
  register_name_loookup[8] = "ax";
  register_name_loookup[9] = "cx";
  register_name_loookup[10] = "dx";
  register_name_loookup[11] = "bx";
  register_name_loookup[12] = "sp";
  register_name_loookup[13] = "bp";
  register_name_loookup[14] = "si";
  register_name_loookup[15] = "di";
}

void disassemble(char *filepath) {
  FILE *file_ptr;
  uint8 byte;

  file_ptr = fopen(filepath, "r");

  // first print bits 16 plus new lines
  printf("bits 16\n\n");

  // read instruction
  for (;;) {
    // instruction binary
    uint8 instruction_0;
    uint8 instruction_1;

    // register name
    char *source;
    char *destination;

    // instruction flags
    uint8 w;
    uint8 d;
    uint8 op_code;

    // binary code for registers in the form wr2r1r0
    uint8 reg_binary_0;
    uint8 reg_binary_1;

    instruction_0 = fgetc(file_ptr);

    if ((char)instruction_0 == EOF) {
      break;
    }
    instruction_1 = fgetc(file_ptr);

    if ((char)instruction_1 == EOF) {
      break;
    }

    op_code = instruction_0 >> 2;
    w = instruction_0 & 1;
    d = instruction_0 & 2;

    reg_binary_0 = (instruction_1 & 0b00111000) >> 3;
    reg_binary_1 = instruction_1 & 0b00000111;

    char *op_code_str = get_op_code_str(op_code);

    if (d) {
      source = get_register_name_from_binary(reg_binary_1, w);
      destination = get_register_name_from_binary(reg_binary_0, w);

    } else {
      source = get_register_name_from_binary(reg_binary_0, w);
      destination = get_register_name_from_binary(reg_binary_1, w);
    }

    printf("%s %s, %s\n", op_code_str, destination, source);
  }

  fclose(file_ptr);
}

int main() {
  setup_register_lookup();

  // printf("###########\n");
  // printf("#SIMPLE#\n");
  // printf("###########\n");
  // disassemble(simple_file);

  printf("\n\n\n###########\n");
  printf("#COMPLEX#\n");
  printf("###########\n");
  disassemble(complex_file);
}
