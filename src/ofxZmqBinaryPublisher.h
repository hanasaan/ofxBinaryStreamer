#pragma once
#include "ofMain.h" // We assume subscriber is always Mac.
#include "ofxZmq.h"
#include "ofxBinaryStreamerTypes.h"
#include "ofxZmqBinaryPublisherBase.h"

class ofxZmqBinaryPublisher  : public ofxZmqBinaryPublisherBase {
protected:
#ifdef USE_OFX_TURBO_JPEG
    ofxTurboJpeg turbo;
#endif
public:
	void update() {
		ts = ofGetElapsedTimeMillis();
	}

	template <typename T>
	bool sendPixels(string identifer, ofPixels_<T>& pixel, bool compression = true, int quality = 90) {
		ofxZmqBinaryHeader header(identifer, ts, 0);
		ofBuffer buffer;

		if (compression) {
			header.type = OFX_ZMQ_BINARY_SINGLE_IMAGE_JPEG;

			ofBuffer imageBuffer;
#ifdef USE_OFX_TURBO_JPEG
            turbo.save(imageBuffer, pixel, quality);
#else
			ofSaveImage(pixel, imageBuffer, OF_IMAGE_FORMAT_JPEG, OF_IMAGE_QUALITY_HIGH);
#endif
			buffer.append(imageBuffer.getBinaryBuffer(), imageBuffer.size());
			header.elementSize = imageBuffer.size();
		} else {
			header.type = OFX_ZMQ_BINARY_SINGLE_IMAGE;

			ofxZmqPixelsHeader h;
			h.width = pixel.getWidth();
			h.height = pixel.getHeight();
			h.bytesPerChannel = pixel.getBytesPerChannel();
			h.numChannels = pixel.getNumChannels();
			buffer.append((const char*) &h, sizeof(ofxZmqPixelsHeader));
			buffer.append((const char*) pixel.getPixels(), pixel.size() * pixel.getBytesPerChannel());
			header.elementSize = buffer.size() + sizeof(ofxZmqPixelsHeader);
		}

		return send(header, buffer.getBinaryBuffer(), buffer.size());
	}
};
