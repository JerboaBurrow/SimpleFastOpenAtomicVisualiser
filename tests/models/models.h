#ifndef MODELS_H
#define MODELS_H

#include <sstream>
#include <chrono>

#include <jGL/jGL.h>
#include <jGL/OpenGL/openGLInstance.h>
#include <jGL/Display/desktopDisplay.h>
#include <jGL/orthoCam.h>

#include <icon.h>

#include <atom.h>
#include <atomRenderer.h>
#include <util.h>


const unsigned int resX = 512;
const unsigned int resY = resX;

const float dr = (1.0)*0.5;
const float dtheta = (3.14)*0.025;
const float dphi = (2.0*3.14)*0.05;
const int count = 28;
const int cols = 4;

std::unique_ptr<jGL::jGLInstance> jGLInstance;

#endif /* MODELS_H */
