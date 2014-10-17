#pragma once

#define USE_OFX_TURBO_JPEG

#ifdef USE_OFX_TURBO_JPEG
#include "ofxTurboJpeg.h"
#endif

#include "ofxZmqBinaryPublisher.h"
#include "ofxZmqBinarySubscriber.h"