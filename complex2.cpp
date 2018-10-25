// Hyperbolic Rogue

// namespaces for complex features (whirlwind, whirlpool, elec, princess, clearing, 
// mirror, hive, heat + livecaves, etc.)

// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

#ifdef CAP_COMPLEX2

namespace hr {

namespace brownian {

  map<cell*, vector<pair<cell*, bool >> > futures;
  int centersteps = 0;
  int totalsteps = 0;
  
  void rise(cell *c, int val) {
    if(c->wall == waSea) c->wall = waNone;
    if(c->land == laOcean || c->land == laNone) {
      c->land = laBrownian;
      c->landparam = 0;
      }
    c->bardir = NOBARRIERS; 
    forCellCM(c1, c) {
      c1->bardir = NOBARRIERS;
      if(c1->mpdist > BARLEV) {
        setdist(c1, BARLEV, c);
        }
      if(c1->land == laOcean) {
        c1->land = laBrownian;
        c1->landparam = 0;
        }
      }
    c->landparam += val;
    }

  void recurse(cell *c, bool fat) {
    while(true) {
      totalsteps++;
      if(celldist(c) >= (fat ? 30 : 20) + celldist(cwt.at)) {
        cell *c1 = c;
        while(true) {
          cell *c2 = ts::left_parent(c1, celldist);
          if(!c2 || c2->mpdist < BARLEV) break;
          setdist(c2, BARLEV, c1);
          if(c2->land == laOcean) {
            c2->land = laBrownian;
            c2->landparam = 0;
            }
          c1 = c2;
          }
        futures[c1].emplace_back(c, fat);
        return;
        }
      if(c->mpdist <= 7) { centersteps++; return; }
      // while(hrand(1000) < 1000 * chance) recurse(c);
      if(fat) recurse(c, false);
      if(!fat && hrand(100000) == 0) recurse(c, true);
      rise(c, fat ? 256 : 1);
      c = c->cmove(hrand(c->type));
      }
    }

  void dissolve_brownian(cell *c, int x) {
    if(c->land == laBrownian) {
      if(among(c->wall, waNone, waStrandedBoat, waMineOpen, waFire)) {
        if(c->landparam >= 4 * level) c->landparam = 4 * level - 1;
        c->landparam -= level * x;
        c->wall = waNone;
        if(c->landparam < 0) c->wall = waSea, c->landparam = 0;
        if(c->landparam == 0) c->landparam = 1;
        }
      }
    }

  void dissolve(cell *c, int x) {
    destroyTrapsAround(c);
    if(c->land == laBrownian) 
      dissolve_brownian(c, x);
    else if(c->wall == waRed2) c->wall = waRed1;
    else if(c->wall == waRed3) c->wall = waRed2;
    else if(among(c->wall, waRed1, waDeadfloor2, waRubble, waBoat, waFire, waCIsland, waCIsland2, waBigBush, waSmallBush)) c->wall = waNone;
    else if(c->wall == waStrandedBoat) c->wall = waNone;
    else if(c->wall == waFrozenLake) c->wall = waLake;
    else if(among(c->wall, waReptile, waGargoyleFloor) || cellUnstable(c)) c->wall = waChasm;
    else if(among(c->wall, waNone, waDock, waBurningDock, waFloorA, waFloorB, waCavefloor, waDeadfloor, waMineMine, waMineUnknown, waMineOpen, waOpenGate, waClosePlate, waOpenPlate, waGargoyleBridge, waReptileBridge))
      c->wall = waSea;
    else if(cellHalfvine(c)) destroyHalfvine(c, waNone, 4);
    }
  
  void init(cell *c) {
    recurse(cwt.at, true);
    recurse(cwt.at, true);
    }

  void build(cell *c, int d) {
    if(futures.count(c)) {
      for(pair <cell*, bool> p: futures[c])
        recurse(p.first, p.second);
      futures.erase(c);
      printf("centersteps = %d futures = %d totalsteps = %d\n", centersteps, isize(futures), totalsteps);
      }

    ONEMPTY {
      if(hrand(8000) < 50 && c->landparam >= 4 && c->landparam < 24)
        c->item = itBrownian;
      if(hrand(8000) < 30)
        c->monst = moAcidBird;
      else if(hrand(8000) < 30)
        c->monst = moAlbatross;
      else if(hrand(8000) < 30) {
        c->monst = moBrownBug;
        c->hitpoints = 3;
        }
      }
    }
  
  }

}
#endif
