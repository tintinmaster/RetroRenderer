#include "render.h"

#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "util.h"

// useful for converting degrees to radians
#define PI 3.14159265358979323846

// useful for mapping f into the range [0, k)
float float_mod(float f, int k) {
    // map into (-k, k)
    float res = fmodf(f, k);

    // fold into [0, k]
    if (res < 0) {
        res += k;
    }

    // accommodate for floating point rounding errors
    if (res >= k) {
        res = 0.0;
    }
    return res;
}

/** Move the player according to its velocities.
 */
void update_player(player_t* p, ctx_t const* ctx) {
  //changing angle of player and making sure it is in [0-360]
  p->angle += p->v_angular;
  p->angle += 720;
  p->angle %= 360;
  //p->angle = float_mod(p->angle, 360);
  float r_angle = p->angle * PI / 180;

  //changing position and making sure player is in border
  p->x += p->v*cos(r_angle);
  p->y -= p->v*sin(r_angle);
  p->x = float_mod(p->x, ctx->map_size);
  p->y = float_mod(p->y, ctx->map_size);

  //changing height and checking if height is under min. height
  p->height += p->v_height; 
  int height_map_pos = p->x + (p->y * ctx->map_size);
  int map_height = ctx->height_map[height_map_pos];
  if ((p->height-20) < map_height){
    p->height = map_height + 20;
  }
}

/** Draw a vertical line into the contexts out buffer in the screen column u
 *  from screen row v_from (exclusively) to screen row v_to (inclusively).
 *
 *  For every call to this function, v_from <= v_to has to hold.
 *  If v_from == v_to holds, nothing is drawn.
 *
 *  Coordinates have their origin in the bottom left corner of the screen and
 *  are 1-based.
 */
void draw_line(ctx_t* c, int u, int v_from, int v_to, uint32_t color) {
  for (int i = v_to; i > v_from; i--) {
    int buffer_entry = (u-1) + ( (c->scr_height - i) * c->scr_width ) ;
    c->out[buffer_entry] = color;
  }
}

/** Render the scene from player's perspective into the context's out buffer.
 */


int lerp(float v0, float v1, float i){
  return round ((1 - i) * v0 + i * v1);
}


void render(const player_t* p, ctx_t* c) {
  //making the sky blue
  /**for (int x = 0; x < c->scr_width; x++){
    for (int y = 0; y < c->scr_height; y++) {
        c->out[x + y * c->scr_width] = c->sky_color;
    }
  }
  */

  float r_angle = p->angle * PI / 180;
  

  float a_factor = cos(r_angle);
  float b_factor = sin(r_angle);


  for (int column = 1; column < c->scr_width; column++) {
    
    uint32_t* row;
    row = (uint32_t*) calloc(c->scr_height + 1, sizeof(uint32_t));
    /**for (int i = 0; i < c->scr_height; i++) {
      row[i] = 0x00000000;
    }
    */
    
    int lastMaxHeight = 0;
    for (int d = 1; d <= c->distance; d++) {
      float a = a_factor * d;
      float b = b_factor * d;

      float lx = p->x + a - b;
      float ly = p->y - b - a; 
      float rx = p->x + a + b;
      float ry = p->y - b + a;

      float modifier = (float) (column-1) / (c->scr_width-1);
      int qux = float_mod(lerp(lx, rx, modifier), c->map_size);
      int quy = float_mod(lerp(ly, ry, modifier), c->map_size);
      
      int pos = qux + quy * c->map_size;

      int quz = c->height_map[pos];

      int calculatedHeight = (c->scr_width / 2) * ((quz - p-> height) / (float) d) + (c->scr_height / 2);
      


      if (calculatedHeight < 0 || c->scr_height < calculatedHeight)
        continue;
      if (calculatedHeight <= lastMaxHeight)
        continue;

      lastMaxHeight = calculatedHeight;
      //printf("%d\n",lastMaxHeight);
      //uint32_t test = c->color_map[pos];
      row[calculatedHeight] = c->color_map[pos];
      //printf("%d %d %d %d\n", qux, quy, a, b, a_factor, b_factor);
  
    }
    
    int lastPixel = c->scr_height;
    uint32_t lastColor = c->sky_color;
    for (int i = c->scr_height; i >= 0; i--) {
      if (row[i] == 0 && i != 0)
        continue;
      
      draw_line(c, column, i, lastPixel, lastColor);

      if (i == 0)
        continue;
      lastPixel = i;
      //printf("%d\n", lastColor);
      lastColor = row[i];

    }

    free(row);
    row = NULL;

  }

  /** Old implementation
  for(int d = c->distance; d > 0; d--) {
    //berechne die Endpunkte L und R der Senkrechten zur Beobachtungsrichtung mit Abstand d
    int a = cos(r_angle) * d;
    int b = sin(r_angle) * d;

    int lx = p->x + a - b;
    int ly = p->y - b - a; 

    int rx = p->x + a + b;
    int ry = p->y - b + a;

    for(int u=1; u <= c->scr_width; u++) {
      //berechne Punkt Q_u
      float modifier = (float)(u-1) / (c->scr_width-1);
      int qux = float_mod(lerp(lx, rx, modifier), c->map_size);
      int quy = float_mod(lerp(ly, ry, modifier), c->map_size);
      int pos = qux + quy * c->map_size;


      //berechne die dargestellte Höhe v von Q_u auf dem Bildschirm
      int quHeight = c->height_map[pos];
      int screenHeight = (c->scr_width / 2) * ((quHeight - p->height) / (float) d) + (c->scr_height / 2);
      if (screenHeight < 1 || screenHeight > c->scr_height)
        continue;
      
      //zeichne eine vertikale Linie in der Farbe von Q_u vom Boden des Bildschirms zu v in Spalte u
      draw_line(c, u, 0, screenHeight, c->color_map[pos]);
      
    }
  }
  */
}



int bonus_implemented(void) {
    // TODO change to 1 if the bonus exercise is implemented.
    return 1;
}
