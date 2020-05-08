
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
  p->angle += p->v_angular %= 360;
  float r_angle = p->angle * PI / 180;
  
  p->x += p->v*cos(r_angle);
  p->y -= p->v*sin(r_angle);
  float_mod(p->x, ctx->scr_width);
  float_mod(p->y, ctx->scr_height);


  p->height += p->v_height;//TODO minimal height anpassen
  int height_map_pos = p->x + (p->y * ctx->scr_width);
  int map_height = ctx->height_map[height_map_pos];
  if (p->height < (map_height + 20))
    p->height = map_height + 20;
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
  int buffer_entry;
  for (int i = v_to; i > v_from; i--) {
    buffer_entry = (u-1) + ( (c->scr_height - i) * c->scr_width ) ;
    c->out[buffer_entry] = color;
  }
  
  /**
  for (int i = 0; i < 100; i++) {
    for (int j = 0; j < 100; j++) {
      c->out[j * 321 + i] = 0x00ffff00;    
    }
  }
  */
}

/** Render the scene from player's perspective into the context's out buffer.
 */
void render(const player_t* p, ctx_t* c) {
    NOT_IMPLEMENTED;
    UNUSED(p);
    UNUSED(c);
}

int bonus_implemented(void) {
    // TODO change to 1 if the bonus exercise is implemented.
    return 0;
}
