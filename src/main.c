#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>

#include "Map.h"
#include "Tower.h"
#include "tools.h"

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600

// 1 image pour 100 ms (0.1 secondes) = 10 images pour 1000 ms
static const Uint32 FRAMERATE_MILLISECONDS = 10 / 1000;

GLuint mapBackground;
GLuint texture;

void reshape() {
  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0., 600., 0., 600.);
}

int main(int argc, char** argv) {
   // Initialisation des variables
   int positionX, positionY;

   int nbTowers = -1;
   int xClicked = 0, yClicked = 0;
   int towerTest = 0;

   Tower* t_first = NULL;
   Tower* t_last = NULL;
   Tower* t = NULL;

   // Initialisation SDL
   if(-1 == SDL_Init(SDL_INIT_VIDEO)) {
      fprintf(stderr, "Impossible d'initialiser la SDL. Fin du programme.\n");
      return EXIT_FAILURE;
   }
   SDL_Surface* screen = NULL;
   if(NULL == (screen = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 32, SDL_OPENGL | SDL_RESIZABLE))) {
      fprintf(stderr, "Impossible d'ouvrir la fenetre. Fin du programme.\n");
      return EXIT_FAILURE;
   }

   SDL_WM_SetCaption("ImacTowerDefense", NULL);

   // Chargement carte itd
   Map map = loadMap("data/map-test.itd");
   // Chargement carte ppm
   char fileName[256] = "images/"; strcat(fileName, map.image);
   SDL_Surface* background = IMG_Load(fileName);
   if(background == NULL) {
      fprintf(stderr, "Impossible de charger l'image %s\n", fileName);
      return EXIT_FAILURE;
   }
   mapBackground = loadTexture(fileName);

   // Monstres
   SDL_Surface* boutin = IMG_Load("images/boutin.png");
   if(boutin == NULL) {
      fprintf(stderr, "impossible de charger l'image boutin.png \n");
      return EXIT_FAILURE;
   }
   texture = loadTexture("images/boutin.png");
   // Noeuds
   Node* root = map.listNodes;
   Node* node = root;
   Node* first = root;
   // Initialisation de la position des monstres
   positionX = node->x;
   positionY = node->y;

   reshape();


   // Boucle événements
   int loop = 1;
   while(loop) {
      root = first;
      Uint32 startTime = SDL_GetTicks();
      glClear(GL_COLOR_BUFFER_BIT);

      // Carte
      glEnable(GL_TEXTURE_2D);
         glBindTexture(GL_TEXTURE_2D, mapBackground);
         glBegin(GL_QUADS);
            glColor3ub(255, 255, 255); // couleur neutre
            glTexCoord2d(0, 0); glVertex2f(0, background->h);
            glTexCoord2d(0, 1); glVertex2f(0, 0);
            glTexCoord2d(1, 1); glVertex2f(background->w, 0);
            glTexCoord2d(1, 0); glVertex2f(background->w, background->h);
         glEnd();
      glDisable(GL_TEXTURE_2D);
      
      // Dessin du chemin
      glColor3ub(map.pathColor.r, map.pathColor.g, map.pathColor.b);
      drawPath(root);

      // Monstres
      if(node->next != NULL) {
         if(node->next->y == positionY) {
            if(node->next->x > positionX) {
               positionX += 1;
            }
            else {
               positionX -= 1;
            }   
         }
         else {
            if(node->next->y > positionY) {
               positionY += 1;
            }
            else {
               positionY -= 1;
            }
         }

         if(positionX == node->next->x && positionY == node->next->y) {
            node = node->next;
         }

         glEnable(GL_TEXTURE_2D);
         glEnable(GL_BLEND);
         glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
         glBindTexture(GL_TEXTURE_2D, texture);

         glBegin(GL_QUADS);
         glColor3ub(255, 255, 255); // couleur neutre
         glTexCoord2d(0, 1); glVertex2d(positionX - boutin->w * 0.5, 600 - positionY - boutin->h * 0.5);
         glTexCoord2d(0, 0); glVertex2d(positionX - boutin->w * 0.5, 600 - positionY + boutin->h * 0.5);
         glTexCoord2d(1, 0); glVertex2d(positionX + boutin->w * 0.5, 600 - positionY + boutin->h * 0.5);
         glTexCoord2d(1, 1); glVertex2d(positionX + boutin->w * 0.5, 600 - positionY - boutin->h * 0.5);
         glEnd();

         glBindTexture(GL_TEXTURE_2D, 0);
         glDisable(GL_BLEND);
         glDisable(GL_TEXTURE_2D);
      }

      // Tours
      if(t_first != NULL) {
         constructTower(t_first);
      }
      /*unsigned char pick_col[3];
      glReadPixels(xClicked, yClicked, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pick_col);*/


      SDL_GL_SwapBuffers();
      /* ****** */

      SDL_Event e;
      while(SDL_PollEvent(&e)) {
         if(e.type == SDL_QUIT) {
            loop = 0;
            break;
         }
         switch(e.type) {
            case SDL_MOUSEBUTTONDOWN:
               switch(e.button.button) {
                  case SDL_BUTTON_LEFT:
                     xClicked = e.button.x;
                     yClicked = e.button.y;
                     //printf("%d %d\n", xClicked, yClicked);
                     nbTowers++;
                     // Création de la première tour
                     if(nbTowers == 0) {
                        t_first = createTower(ROCKET, xClicked, yClicked);
                        nbTowers++;
                        t_last = t_first;
                     }
                     // Autres tours
                     else if(nbTowers > 1) {
                        // Vérification de la position
                        towerTest = checkPosTower(t_first, xClicked, yClicked);
                        if(towerTest == 1) {
                           t = createTower(ROCKET, xClicked, yClicked);
                           (*t_last).next = t;
                           t_last = t;
                        }
                     }
                     //printf("%d %d %d\n", pick_col[0], pick_col[1], pick_col[2]);
                     break;
                  case SDL_BUTTON_RIGHT:
                     break;
                  default:
                     break;
               }
               break;
            /*case SDL_VIDEORESIZE:
            WIDTH = e.resize.w;
            HEIGHT = e.resize.h;
            setVideoMode();
            break;*/
            case SDL_KEYDOWN:
               switch(e.key.keysym.sym) {
                  case 's' :
                     break;
                  case 'q' :
                     loop = 0;
                     break;
                  case SDLK_ESCAPE: 
                     loop = 0;
                     break;
                  default: 
                     break;
               }
               break;
            default:
               break;
         }
      }
      Uint32 elapsedTime = SDL_GetTicks() - startTime;
      if(elapsedTime < FRAMERATE_MILLISECONDS) {
         SDL_Delay(FRAMERATE_MILLISECONDS - elapsedTime);
      }
   }

   // Destruction des données des images chargées
   SDL_FreeSurface(background);
   SDL_FreeSurface(boutin);

   SDL_Quit();
   return EXIT_SUCCESS;
}
