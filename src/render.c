
#include "render.h"

#include <math.h>
#include <string.h>

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
  p->angle = float_mod(p->angle, 360);
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

/** Draw a vertical line into the context's out buffer in the screen column u
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
  int width = c->scr_width;
  int height = c->scr_height;
  //initializing the sky
  for (int x = 0; x < width; ++x) {
    for (int y = 0; y < height; ++y) {
      c->out[x + y * width] = c->sky_color;
    }
  }
  float r_angle = p->angle * PI / 180;
  float px = p->x;
  float py = p->y;
  int map_size = c->map_size;
  
  //printf("angle: %d\n", p->angle);
  //printf("x: %.2f y: %.2f\n", px, py );

  //für jede Entfernung d < D, absteigend:
  for (int d = c->distance; d > 0; d--) {
    //berechne die Endpunkte L und R der Senkrechten zur Beobachtungsrichtung mit Abstand d
    int a = cos(r_angle) * d;
    int b = sin(r_angle) * d;

    int lx = px + a - b;
    int ly = py - b - a;

    int rx = px + a + b;
    int ry = px - b + a;

    //wähle für jede Bildschirmspalte u einen Punkt Q_u auf der Strecke zwischen L und R
    //für jede Bildschirmspalte u:
    for (int u = 1; u <= width; u++) {
      //berechne Punkt Q_u
      float modifier = (float)(u-1) / (width - 1);
      //float quxRaw = (1 - modifier) * lx + modifier * rx;
      //float quyRaw = (1 - modifier) * ly + modifier * ry;
      int qux = float_mod(lerp(lx, rx, modifier), map_size);
      int quy = float_mod(lerp(ly, ry, modifier), map_size);
      int pos = qux + quy * map_size;
      
      //berechne die dargestelle Höhe v von Q_u auf dem Bildschirm
      int quHeight = c->height_map[pos];
      int screenHeight = (width/2) * ((quHeight - p->height)/(float) d) +(height/2);
      if (screenHeight < 1 || screenHeight > height)
        continue;
      //zeichne eine vertikale Linie in der Farbe von Q_u vom Boden des Bildschirms zu v in Spalte u
      draw_line(c, u, 0, screenHeight, c->color_map[pos]);

    }
  }
}

int bonus_implemented(void) {
    // TODO change to 1 if the bonus exercise is implemented.
    return 0;
}
