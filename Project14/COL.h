#include <allegro5/allegro.h>

typedef struct mask
{
  int w,h;
  int *bits;
} mask_t;

//create a new mask
mask_t *Mask_New(ALLEGRO_BITMAP *bmp);
//creation helper function
mask_t *Mask_Create(int w, int h);

//clears all bits in a mask
void Mask_Clear(mask_t *m);
//fills all bits in a mask
void Mask_Fill(mask_t *m);
//frees up memory that has been assigned
void Mask_Delete(mask_t *m);
//sets a single bit
void Mask_SetBit(mask_t *m, int x, int y);
//clears a single bit
void Mask_UnsetBit(mask_t *m, int x, int y);

//tests for collision. Returns a 1 of a collision has occured. When calculating the offset
//it is assumed that you will subtract the x and y values of "b" from "a". Example call:
/*
int result = Mask_Collide(foo1.mask, foo2.mask, foo1.x - foo2.x, foo1.y, foo2.y);
*/
int Mask_Collide(const mask_t *a, const mask_t *b, int xoffset, int yoffset);

//draws the mask to the screen
void Mask_Draw(mask_t *m, int x, int y);

//color helper functions
int Color_Equiv(ALLEGRO_COLOR col1, ALLEGRO_COLOR col2);
int Transparent(ALLEGRO_COLOR col1);