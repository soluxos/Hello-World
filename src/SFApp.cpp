#include "SFApp.h"
#include <SDL/SDL_ttf.h>
#include <iostream>

SFApp::SFApp() : fire(0), is_running(true) {

  //random enemy spawn
  int enemySpawn = rand()% 430;

  //Positions
  surface = SDL_GetVideoSurface();
  app_box = make_shared<SFBoundingBox>(Vector2(surface->w/2, surface->h/2), surface->w/2, surface->h/2);
  player  = make_shared<SFAsset>(SFASSET_PLAYER);
  auto player_pos = Point2(surface->w/2, 125.0f);
  player->SetPosition(player_pos);


  //Alien Spawing
  const int number_of_aliens = 5;
  for(int i=0; i<number_of_aliens; i++) {
  // place an alien at width/number_of_aliens * i
  auto alien = make_shared<SFAsset>(SFASSET_ALIEN);
  auto pos = Point2( 65+ (surface->w/number_of_aliens) * i, 600.0f);
  alien->SetPosition(pos);
  aliens.push_back(alien);
  }

  // Asset deleters
  for(int i = 1; i <= 1; i++) {
  auto assetdeleter = make_shared<SFAsset>(SFASSET_ASSETDELETER);
  auto pos = Point2(surface->w/2, 10.0f);
  assetdeleter->SetPosition(pos);
  assetdeleters.push_back(assetdeleter);
  }

  //Coin Spawn
  auto coin = make_shared<SFAsset>(SFASSET_COIN);
  auto pos = Point2((surface->w/2), 600.0f);
  coin->SetPosition(pos);
  coins.push_back(coin);
}

SFApp::~SFApp() {
}

/**
 * Handle all events that come from SDL.
 * These are timer or keyboard events.
 */
void SFApp::OnEvent(SFEvent& event) 
  {
  SFEVENT the_event = event.GetCode();
  switch (the_event) {
  case SFEVENT_QUIT:
    is_running = false;
    break;
  case SFEVENT_UPDATE:
    OnUpdateWorld();
    OnRender();
    break;
  case SFEVENT_PLAYER_LEFT:
    player->GoWest();
    break;
  case SFEVENT_PLAYER_RIGHT:
    player->GoEast();
    break;
  case SFEVENT_FIRE:
    fire ++;
    FireProjectile();
    break;
  }
}

int SFApp::OnExecute() {
  // Execute the app
  SDL_Event event;
  while (SDL_WaitEvent(&event) && is_running) {
    // if this is an update event, then handle it in SFApp,
    // otherwise punt it to the SFEventDispacher.
    SFEvent sfevent((const SDL_Event) event);
    OnEvent(sfevent);
  }
}

void SFApp::OnUpdateWorld() {

  // Update projectile positions
  for(auto p: projectiles) {
    p->GoNorth();
  }
  //Update coin positions
  for(auto c: coins) {
    c->GoSouth();
  }

  // Update enemy positions
  for(auto a : aliens) {
    a->GoSouthSlow();
  }
  // Detect collisions between projectiles and aliens
  for(auto p : projectiles) {
    for(auto a : aliens) {
      if(p->CollidesWith(a)) {
        p->HandleCollision();
        a->HandleCollision();
        score += 1;
        std::stringstream sstm;
        sstm << "Score:  " << score;
        SDL_WM_SetCaption(sstm.str().c_str(),  sstm.str().c_str());
        if (score >= 50 ) {
          is_running = false;
          cout << "Well done, you win! With " << score << " points\n";
          if (bonus > 1)
          {
            cout << "You also got the bonus!\n";
          }
        }
      }
    }
  }
  //Detect collision between player and coin
  for(auto c : coins) {    
      if(c->CollidesWith(player)) {
        bonus += 10;
        c->HandleCollision();
        player->HandleCollision();
      }    
  }
  // Collision detection for shields
  for(auto b : assetdeleters) {
    for(auto a : aliens) {
      if(a->CollidesWith(b)) {
        a->HandleCollision();
        lives -= 1;
        if (lives == 0 ) {
          cout << "You lost all of your lives, good luck next time.\n";
          is_running = false;
        }
      }
    }
  }
  //collision detection between aliens and player
  for(auto a : aliens) {    
      if(a->CollidesWith(player)) {
        a->HandleCollision();
        player->HandleCollision();
        score += 1;
        cout << "Yeah, you definitely died, at least you got " << score << " points" << endl;
        is_running = false;
      }    
  }

  list<shared_ptr<SFAsset>> tmp1;
  for(auto c : coins) {
    if(c->IsAlive()) {
      tmp1.push_back(c);
    }
  }
  //if aliens are less than max then spawn alien to random position
  if(aliens.size() < maxEnemies){
    auto alien = make_shared<SFAsset>(SFASSET_ALIEN);
    auto pos   = Point2(50+rand()%540, 600.0f);
    alien->SetPosition(pos);
    aliens.push_back(alien);
  }

  coins.clear();
  coins = list<shared_ptr<SFAsset>>(tmp1);

  // remove dead aliens (the long way)
  list<shared_ptr<SFAsset>> tmp;
  for(auto a : aliens) {
    if(a->IsAlive()) {
      tmp.push_back(a);
    }
  }

  aliens.clear();
  aliens = list<shared_ptr<SFAsset>>(tmp);

  list<shared_ptr<SFAsset>> tmp2;
  for(auto p : projectiles) {
    if(p->IsAlive()) {
      tmp2.push_back(p);
    }
  }

  projectiles.clear();
  projectiles = list<shared_ptr<SFAsset>>(tmp2);

}

SDL_Surface *message = NULL;
SDL_Surface *image;
SDL_Surface *temp;

void SFApp::OnRender() {

  // clear the surface
  SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 20, 20, 20) );

  // draw the player
  player->OnRender(surface);

  //check if objects are alive
  for(auto p: projectiles) {
    if(p->IsAlive()) {p->OnRender(surface);}
  }

  for(auto a: aliens) {
    if(a->IsAlive()) {a->OnRender(surface);}
  }

  for(auto c: coins) {
    if(c->IsAlive()) {c->OnRender(surface);};
  }
  //create surface for asset deleter
  for(auto b: assetdeleters) {
    b->OnRender(surface);
  }


  // Switch the off-screen buffer to be on-screen
  SDL_Flip(surface);
}

void SFApp::FireProjectile() {
  auto pb = make_shared<SFAsset>(SFASSET_PROJECTILE);
  auto v  = player->GetPosition();
  pb->SetPosition(v);
  projectiles.push_back(pb);
}
