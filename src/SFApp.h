#ifndef SFAPP_H
#define SFAPP_H
#include <SDL/SDL_ttf.h>

#include <memory>   // Pull in std::shared_ptr
#include <iostream> // Pull in std::cerr, std::endl
#include <list>     // Pull in list
#include <sstream>

using namespace std;

#include "SFCommon.h"
#include "SFEvent.h"
#include "SFAsset.h"

/**
 * Represents the StarshipFontana application.  It has responsibilities for
 * * Creating and destroying the app window
 * * Processing game events
 */
class SFApp {
public:
  SFApp();
  virtual ~SFApp();
  void    OnEvent(SFEvent &);
  int     OnExecute();
  void    OnUpdateWorld();
  void    OnRender();

  void    FireProjectile();
private:
  SDL_Surface           * surface;
  bool                    is_running;

  shared_ptr<SFAsset>        player;
  shared_ptr<SFBoundingBox>  app_box;
  list<shared_ptr<SFAsset> > projectiles;
  list<shared_ptr<SFAsset> > aliens;
  list<shared_ptr<SFAsset> > coins;
  list<shared_ptr<SFAsset> > assetdeleters;

  //integers used in SFApp
  int fire;
  int score = 0;
  int lives = 3;
  int bonus = 0;
  int life = 1;
  const int maxEnemies = 5;

  SFError OnInit();
};
#endif
