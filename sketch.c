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
  //stage 2
  bool flag;
  unsigned int opcurrent;
  int dt;

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
//  s->pen = true;
  //new stuff for stage 2
  s->flag = false;
  s->opcurrent = 0;
  s->dt = 0;
  return s;
}

int opcode(int code)
{
  int opcode = (code >> 6);
  return opcode;
}

int getoperand(state *s, int code)
{
  int operand;
  if(s->flag == false)
  {
    operand = code & 0x3F;
    if((operand>>5) == 1)
    {
      operand |= 0xffffffc0;
    }

  }

  if(s->flag == true)
  {
    operand = code & 0x3f;
  }

  // int operand = code & 0x3F;
  // if((operand>>5) == 1)
  // {
  //   operand |= 0xffffffc0;
  // }
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
  //s->xcurrent = s->xcurrent + operand;
  s->opcurrent = (s->opcurrent << 6) | operand;
  s->xcurrent = s->xcurrent + s->opcurrent;
  s->flag = false;
  s->opcurrent = 0;
}

void movey(state *s, display *d, int operand)
{
//  s->ycurrent = s->ycurrent + operand;
  s->opcurrent = (s->opcurrent << 6) | operand;
  s->ycurrent = s->ycurrent + s->opcurrent;
  if(s->pen == true)
  {
    printf("your x values called by line are %d, %d\n",s->xprevious, s->xcurrent);
    line(d,s->xprevious, s->yprevious, s->xcurrent,s->ycurrent);

  }
  s->xprevious = s->xcurrent;
  printf("your x values are %d, %d\n", s->xprevious, s->xcurrent);
  s->yprevious = s->ycurrent;
  s->flag = false;
  s->opcurrent = 0;
}

void append(state *s, display *d, int code)
{
  if(s->flag == true)
  {
    s->opcurrent = (s->opcurrent << 6) | (code & 0x3f);
  }

  else
  {
    s->opcurrent = getoperand(s,code);
    s->flag = true;
  }
}


void operation(display *d, state *s, int code)
{
  int opc = opcode(code);
  int oper = getoperand(s,code);

  if(opc == 0)
  {
    movex(s,oper);
  }

  if(opc == 1)
  {
    movey(s,d,oper);
  }

  if(opc == 2)
  {
    append(s,d,code);
  }

  if(opc == 3)
  {
    if(code == 0xc0)
    {
      s->pen = s->pen ^ 1;
      // s->xprevious = s->xcurrent;
      // s->yprevious = s->ycurrent;
    }

    if(code == 0xc1)
    {
      if(s->opcurrent == 0)
      {
        pause(d, s->dt);
      }
      else
      {
        s->dt = s->opcurrent;
        pause(d, s->dt);
      }
      // s->opcurrent = 0;
      // s->flag = false;
    }

    if(code == 0xc2)
    {
      clear(d);
    }

    if(code == 0xc3)
    {
      key(d);
    }

    if(code == 0xc4)
    {
      colour(d, s->opcurrent);
    }
    s->opcurrent = 0;
    s->flag = false;
  }


  // if(opc == 3)
  // {
  //   s->pen = s->pen ^ 1;
  //   s->xprevious = s->xcurrent;
  //   s->yprevious = s->ycurrent;
  // }

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
  //  int oper = getoperand(code);
    //pause(d,50);
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
  fclose(in);
}

void testOpCode()
{
  assert(opcode(0x5f) == 0x01);
  assert(opcode(0x0f) == 0x00);
  assert(opcode(0xff) == 0x03);
  assert(opcode(0x8e) == 0x02);
}

// void testgetoperand()
// {
//   assert(getoperand(0x5f) == 0x1f);
//   assert(getoperand(0x0f) == 0x0f);
//   assert(getoperand(0xff) == -1);
//   assert(getoperand(0x8e) == 0x0e);
// }

void testMovex()
{
  state *s = malloc(sizeof(state));
  s->xcurrent = 0;
  movex(s,8);
  assert(s->xcurrent == 8);
  free(s);
}

void test()
{
  testOpCode();
  //testgetoperand();
  testMovex();
  printf("all tests passed\n");
}


int main(int n, char *args[n])
{
  char *t = "tests";
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
