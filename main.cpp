#include "include/raylib.h"
#include <iostream>
#include <math.h>
#define P2 PI/2
#define P3 3*PI/2

using namespace std;

// window dimensions
int nWindowWidth{1024};
int nWindowHeight{512};
Color oWindowBackground = BLACK;

// player pos
float fPlayerX{512.0}, fPlayerY{256.0};
float fPlayerAngle{0.0f};
float fPlayerSpeed{3.0f};
float fPlayerDX{cosf(fPlayerAngle)*fPlayerSpeed}, fPlayerDY{sinf(fPlayerAngle)*fPlayerSpeed};

// player rendering 2d
float fPlayerRadius{4};
Color oPlayerColor = RED;

// the map
int nMapWidth = 8, nMapHeight = 8, nMapCubeSize = 64;
int map[] =
{
  1,1,1,1,1,1,1,1,
  1,0,1,0,0,0,0,1,
  1,0,1,0,0,0,0,1,
  1,0,1,0,0,0,0,1,
  1,0,0,0,0,0,0,1,
  1,0,0,0,0,1,0,1,
  1,0,0,0,0,0,0,1,
  1,1,1,1,1,1,1,1,
};

void DrawMap2D()
{
  for (int y=0; y<nMapHeight; y++)
  {
    for (int x=0; x<nMapWidth; x++)
    {
      if (map[y*nMapWidth+x]==1){DrawRectangle(x*nMapCubeSize+1, y*nMapCubeSize+1, nMapCubeSize-1, nMapCubeSize-1, WHITE);}
      if (map[y*nMapWidth+x]==0){DrawRectangle(x*nMapCubeSize+1, y*nMapCubeSize+1, nMapCubeSize-1, nMapCubeSize-1, BLACK);}
    }
  }
}

// player movement function
void MovePlayer(int nPlayerSpeed)
{
  if (IsKeyDown(KEY_A)) {fPlayerAngle-=0.1f; if(fPlayerAngle<0)   {fPlayerAngle+=2*PI;} fPlayerDX=cosf(fPlayerAngle)*fPlayerSpeed; fPlayerDY=sinf(fPlayerAngle)*fPlayerSpeed;}
  if (IsKeyDown(KEY_D)) {fPlayerAngle+=0.1f; if(fPlayerAngle>2*PI){fPlayerAngle-=2*PI;} fPlayerDX=cosf(fPlayerAngle)*fPlayerSpeed; fPlayerDY=sinf(fPlayerAngle)*fPlayerSpeed;}
  if (IsKeyDown(KEY_W)) {fPlayerX += fPlayerDX; fPlayerY += fPlayerDY;}
  if (IsKeyDown(KEY_S)) {fPlayerX -= fPlayerDX; fPlayerY -= fPlayerDY;}
}

// draw player function
void DrawPlayer()
{
  
  DrawCircle(fPlayerX, fPlayerY, fPlayerRadius, oPlayerColor);
  DrawLine(fPlayerX, fPlayerY, fPlayerX+fPlayerDX*5, fPlayerY+fPlayerDY*5, oPlayerColor);
}


// cast rays

float dist(float ax, float ay, float bx, float by, float angle)
{
  return sqrt((bx - ax) * (bx - ax) + (by - ay) * (by - ay));
}

void DrawRays2D()
{
  float fRayAngle, fRayY, fRayX, fRayYOffset, fRayXOffset;
  int nDepthOfField, nMapX, nMapY, nMapPosition;
  fRayAngle = fPlayerAngle;

  for (int r = 0; r < 1; r++)
  {
    float fDistanceHorizontal = 1000000, fHorizontalRayX = fPlayerX, fHorizontalRayY = fPlayerY; 
    nDepthOfField = 0;
    float fATan = -1 / tan(fRayAngle);
    if (fRayAngle > PI)
    {
      fRayY = (((int)fPlayerY >> 6) << 6) - 0.0001;
      fRayX = (fPlayerY - fRayY) * fATan + fPlayerX;
      fRayYOffset = -64; 
      fRayXOffset = -fRayYOffset * fATan;
    }
    else if (fRayAngle < PI)
    {
      fRayY = (((int)fPlayerY >> 6) << 6) + 64;
      fRayX = (fPlayerY - fRayY) * fATan + fPlayerX;
      fRayYOffset = 64; 
      fRayXOffset = -fRayYOffset * fATan;
    }
    else
    {
      fRayX = fPlayerX;
      fRayY = fPlayerY;
      nDepthOfField = 8;
    }
    while (nDepthOfField < 8)
    {
      nMapX = (int) fRayX >> 6;
      nMapY = (int) fRayY >> 6;
      
      if (nMapX >= 0 && nMapX < nMapWidth && nMapY >= 0 && nMapY < nMapHeight)
      {
        nMapPosition = nMapY * nMapWidth + nMapX;
        if (map[nMapPosition] == 1)
        {
          nDepthOfField = 8;
          fHorizontalRayX = fRayX;
          fHorizontalRayY = fRayY;
          fDistanceHorizontal = dist(fPlayerX, fPlayerY, fHorizontalRayX, fHorizontalRayY, fRayAngle);
        } // hit wall
        else
        {
          fRayX += fRayXOffset;
          fRayY += fRayYOffset;
          nDepthOfField += 1;
        }
      }
      else
      {
        nDepthOfField = 8; // Terminate if out of bounds
      }
    }
    
    float fDistanceVertical = 1000000, fVerticalRayX = fPlayerX, fVerticalRayY = fPlayerY;
    nDepthOfField = 0;
    float fNegativeATan = -tan(fRayAngle);
    if (fRayAngle > P2 && fRayAngle < P3) // looking left
    {
      fRayX = (((int)fPlayerX >> 6) << 6) - 0.0001;
      fRayY = (fPlayerX - fRayX) * fNegativeATan + fPlayerY;
      fRayXOffset = -64; 
      fRayYOffset = -fRayXOffset * fNegativeATan;
    }
    else if (fRayAngle < P2 || fRayAngle > P3) // looking right
    {
      fRayX = (((int)fPlayerX >> 6) << 6) + 64;
      fRayY = (fPlayerX - fRayX) * fNegativeATan + fPlayerY;
      fRayXOffset = 64; 
      fRayYOffset = -fRayXOffset * fNegativeATan;
    }
    else
    {
      fRayX = fPlayerX;
      fRayY = fPlayerY;
      nDepthOfField = 8;
    }
    while (nDepthOfField < 8)
    {
      nMapX = (int) fRayX >> 6;
      nMapY = (int) fRayY >> 6;
      
      if (nMapX >= 0 && nMapX < nMapWidth && nMapY >= 0 && nMapY < nMapHeight)
      {
        nMapPosition = nMapY * nMapWidth + nMapX;
        if (map[nMapPosition] == 1)
        {
          nDepthOfField = 8;
          fVerticalRayX = fRayX;
          fVerticalRayY = fRayY;
          fDistanceVertical = dist(fPlayerX, fPlayerY, fVerticalRayX, fVerticalRayY, fRayAngle);
        } // hit wall
        else
        {
          fRayX += fRayXOffset;
          fRayY += fRayYOffset;
          nDepthOfField += 1;
        }
      }
      else
      {
        nDepthOfField = 8; // Terminate if out of bounds
      }
    }
    if (fDistanceHorizontal > fDistanceVertical) {fRayX = fVerticalRayX; fRayY = fVerticalRayY;}
    else {fRayX = fHorizontalRayX; fRayY = fHorizontalRayY;}
    DrawLine(fPlayerX, fPlayerY, fRayX, fRayY, RED);
  }
  
}

int main()
{

  InitWindow(nWindowWidth, nWindowHeight, "Raycaster");
  SetTargetFPS(60);

  // game loop
  while (1)
  {
    BeginDrawing();
    ClearBackground(oWindowBackground);
    DrawMap2D();
    DrawRays2D();
    MovePlayer(5);
    DrawPlayer();
    EndDrawing();
  }
  return 0;
}
