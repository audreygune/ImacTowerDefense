#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "Node.h"
#include "tools.h"
#include "Monster.h"

int countMonsters(Monster* root) {
	int cpt = 1;
	while((*root).next != NULL) {
		cpt++;
		root = (*root).next;
	}
	return cpt;
}
Monster* createMonster(MonsterType type, int posX, int posY, Node* nextNode) {
	if(nextNode == NULL) {
		fprintf(stderr, "pointer is NULL in createMonster function \n");
		exit(1);
	}

	Monster* newMonster = (Monster*)malloc(sizeof(Monster));
	if(newMonster == NULL) {
		fprintf(stderr, "Error allocation memory \n");
		exit(1);
	}

	if(type == BOUTIN) {
		(*newMonster).life = 10;
		(*newMonster).resistance = 100;
		(*newMonster).move = 0;
		(*newMonster).speedDelay = 0;
	}
	else if(type == BARJOT) {
		(*newMonster).life = 20;
		(*newMonster).resistance = 200;
		(*newMonster).move = 1;
		(*newMonster).speedDelay = 1;
	}
	else {
		fprintf(stderr, "Unknown monster type \n");
		exit(1);
	}
	(*newMonster).posX = posX;
	(*newMonster).posY = posY;
	(*newMonster).nextNode = nextNode;
	(*newMonster).next = NULL;

	return newMonster;
}
MonsterList* createMonsterList() {
	MonsterList* newList = (MonsterList*)malloc(sizeof(MonsterList));
	if(newList == NULL) {
		fprintf(stderr, "Error allocation memory \n");
		exit(1);
	}
	(*newList).root = NULL;
	(*newList).nbMonsters = 0;

	return newList;
}
Monster* addMonster(Monster* monsterList, Monster* addMonster) {
	if(monsterList == NULL || addMonster == NULL) {
		fprintf(stderr, "pointer is NULL in addMonster function \n");
		exit(1);
	}	

	(*addMonster).next = monsterList;

	return addMonster;
}
Monster* rmvMonster(Monster* monsterList, Monster* monster) {
	if(monsterList == NULL) {
		fprintf(stderr, "pointer is NULL in rmvMonster function \n");
		exit(1);
	}

	Monster* root = monsterList;
	Monster* rmvMonster;

	if(monsterList == monster) {
		rmvMonster = monsterList;
		if((*monsterList).next != NULL) { 
			monsterList = (*monsterList).next;
			free(rmvMonster);
			return monsterList;
		}
		else {
			free(rmvMonster);
			return NULL;
		}
	}
	while((*monsterList).next != NULL) {
		if((*monsterList).next == monster) {
			rmvMonster = (*monsterList).next;
			if((*rmvMonster).next != NULL) {
				(*monsterList).next = (*rmvMonster).next;
			}
			else {
				(*monsterList).next = NULL;
				break;
			}
			free(rmvMonster);
		}
		monsterList = (*monsterList).next;
	}

	return root;
}
void rmvMonsterList(MonsterLists* lists, int index) {
	int i = 0;
	for(i = 0; i < (*lists).nbLists-1; i++) {
		if(i >= index) {
			(*lists).lists[i] = (*lists).lists[i+1]; 
		}
	}
	(*lists).nbLists -= 1;
}
int drawMonsters(MonsterLists lists) {
	SDL_Surface* boutin = IMG_Load("images/boutin.png");
	if(boutin == NULL) {
		fprintf(stderr, "impossible de charger l'image boutin.png \n");
		exit(1);
	}
	GLuint texture = loadTexture("images/boutin.png");

	Monster* monster;
	int i = 0;
	for(i = 0; i < lists.nbLists; i++) {
		monster = lists.lists[i]->root;
		while(monster != NULL) {
			if(drawMonster(monster, boutin, texture) == 0) {
				//return 0;
			}
			monster = (*monster).next;
		}
	}

	SDL_FreeSurface(boutin);

	return 1;
}
int drawMonster(Monster* monster, SDL_Surface* boutin, GLuint texture) {
	if(monster->nextNode != NULL) {
		if(monster->move == monster->speedDelay) {
			if(monster->nextNode->y == monster->posY) {
				if(monster->nextNode->x > monster->posX) {
					monster->posX += 1;
				}
				else {
					monster->posX -= 1;
				}   
			}
			else {
				if(monster->nextNode->y > monster->posY) {
					monster->posY += 1;
				}
				else {
					monster->posY -= 1;
				}
			}
			if(monster->posX == monster->nextNode->x && monster->posY == monster->nextNode->y) {
				monster->nextNode = monster->nextNode->next;
			}

			monster->move = 0;
		}
		else {
			monster->move += 1;
		}

		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBindTexture(GL_TEXTURE_2D, texture);

		glBegin(GL_QUADS);
		glColor3ub(255, 255, 255); // couleur neutre
		glTexCoord2d(0, 1); glVertex2d(monster->posX - boutin->w * 0.5, 600 - monster->posY - boutin->h * 0.5);
		glTexCoord2d(0, 0); glVertex2d(monster->posX - boutin->w * 0.5, 600 - monster->posY + boutin->h * 0.5);
		glTexCoord2d(1, 0); glVertex2d(monster->posX + boutin->w * 0.5, 600 - monster->posY + boutin->h * 0.5);
		glTexCoord2d(1, 1); glVertex2d(monster->posX + boutin->w * 0.5, 600 - monster->posY - boutin->h * 0.5);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);

		glBegin(GL_QUADS);
		glColor3ub(255, 0, 0);
		glVertex2d(monster->posX, 600 - monster->posY + 40);
		glVertex2d(monster->posX, 600 - monster->posY + 30);
		glVertex2d(monster->posX + monster->life * 2, 600 - monster->posY + 30);
		glVertex2d(monster->posX + monster->life * 2, 600 - monster->posY + 40);
		glEnd();

		return 1;
	}
	return 0;
}
