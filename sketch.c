#include <stdio.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "display.h"

struct state
{
  int xprevious;
  int yprevious;
  int xcurrent;
  int ycurrent;
  bool pen;
  //bool flag;
  //bool operandcurrent;

};
typedef struct state state;

state *newState()
{
  state *s = malloc(sizeof(state));
  s->xprevious = 0;
  s->yprevious = 0;
  s->xcurrent = 0;
  s->ycurrent = 0;
  s->pen = false;

  return s;
}

int opcode(int code)
{
  int opcode = (code >> 6);
  return opcode;
}

int operand(int code)
{
  int operand = code & 0x3F;
  if((operand>>5) == 1)
  {
    operand |= 0xffffffc0;
  }
  return operand;
}

// read byte -> separate into opcode and operand
// -> based on opcode, call either of these 2 funcitons:
// opcode 0 <=> move on x axis (parameter = operand) + update current pos
// opcode 1 <=> move on y axis (parameter = operand) + uodate both current and prev positions
//          <=> if pen == true then write the line
// opcode 3 <=> pen = pen ^ 1;

// state *draw(state *s, display *d, int code)
// {
//
//
// }

void movex(state *s, int operand)
{
  printf("%d\n",operand );
  s->xcurrent = s->xcurrent + operand;
}

void movey(state *s, display *d, int operand)
{
  s->ycurrent = s->ycurrent + operand;
  if(s->pen)
  {
    line(d,s->xprevious, s->yprevious, s->xcurrent,s->ycurrent);
  }
  s->xprevious = s->xcurrent;
  s->yprevious = s->ycurrent;
}

void operation(display *d, state *s, int code)
{
  int opc = opcode(code);
  int oper = operand(code);

  if(opc == 0)
  {
    movex(s,oper);
  }

  if(opc == 1)
  {
    movey(s,d,oper);
  }

  if(opc == 3)
  {
    s->pen = s->pen ^ 1;
    s->xprevious = s->xcurrent;
    s->yprevious = s->ycurrent;
  }

}

void readf(display *d, state *s, char *filename)
{
  FILE *in = fopen(filename, "rb");
  unsigned char byte = fgetc(in);
  //int byte = fgetc(in);
  while (!feof(in))
  {
    //int byte = fgetc(in);
    int code = (byte & 0xFF);
  //  int opc = opcode(code);
  //  int oper = operand(code);
    pause(d,400);
    operation(d,s,code);
    // if(opc == 0)
    // {
    //   movex(s,oper);
    // }
    //
    // if(opc == 1)
    // {
    //   movey(s,d,oper);
    // }
    //
    // if(opc == 3)
    // {
    //   s->pen = s->pen ^ 1;
    //   s->xprevious = s->xcurrent;
    //   s->yprevious = s->ycurrent;
    // }
    byte = fgetc(in);
  }
  int code = (byte & 0xFF);
  pause(d,400);
  operation(d,s,code);
}

void testOpCode()
{
  assert(opcode(0x5f) == 0x01);
  assert(opcode(0x0f) == 0x00);
  assert(opcode(0xff) == 0x03);
  assert(opcode(0x8e) == 0x02);
}

void testOperand()
{
  assert(operand(0x5f) == 0x1f);
  assert(operand(0x0f) == 0x0f);
  assert(operand(0xff) == 0x3f);
  assert(operand(0x8e) == 0x0e);
  assert(operand(0x88ef) == 0x8ef);
}

// void testMovex()
// {
//   state *s = malloc(sizeof(state));
//   s->xcurrent = 0;
//   movex(s,8)
//   assert(s->xcurrent == 8);
//   free(s);
//
// }

void test()
{
  testOpCode();
  testOperand();
  // testMovex();
  printf("all tests passed\n");
}


int main(int n, char *args[n])
{
  char *t = "test2";
  if (strcmp(args[1],t) == 0)
  {
    test();
    exit(1);
  }

    display *d = newDisplay(args[1],200,200);
    state *s = newState();
    readf(d,s,args[1]);
    free(s);
    end(d);
    return 0;
}
