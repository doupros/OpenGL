#include <iostream>
#include "SOIL.h"


unsigned int object = 0;
glGentextures(1, &object);
glBindTexture(GL_TEXTURE_2D, object);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glBindTexture(GL_TEXTURE_2D, 0);