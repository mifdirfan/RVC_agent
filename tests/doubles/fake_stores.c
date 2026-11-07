#include "fake_stores.h"
#include "obstacle_location.h"
#include "dust_existence.h"

static ObstacleLocation s_obs;
static bool s_dust;
static int s_obs_gets, s_dust_gets;

void fake_stores_reset(void)
{
    s_obs.front = s_obs.left = s_obs.right = false;
    s_dust = false;
    s_obs_gets = s_dust_gets = 0;
}
void fake_stores_set(bool front, bool left, bool right, bool dust)
{
    s_obs.front = front;
    s_obs.left = left;
    s_obs.right = right;
    s_dust = dust;
}
int fake_stores_obstacle_gets(void) { return s_obs_gets; }
int fake_stores_dust_gets(void) { return s_dust_gets; }

void obstacle_location_init(void) { fake_stores_reset(); }
void obstacle_location_set_front(bool front) { s_obs.front = front; }
void obstacle_location_set_sides(bool left, bool right) { s_obs.left = left; s_obs.right = right; }
ObstacleLocation obstacle_location_get(void) { s_obs_gets++; return s_obs; }
void dust_existence_init(void) { s_dust = false; }
void dust_existence_set(bool dust) { s_dust = dust; }
bool dust_existence_get(void) { s_dust_gets++; return s_dust; }
