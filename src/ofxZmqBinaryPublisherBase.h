#pragma once
#include "ofxZmq.h"
#include "ofxBinaryStreamerTypes.h"

#ifndef OF_VERSION_MINOR
#include <vector>
#include <string>
using namespace std;
#endif

// This class does not depend on OF.
class ofxZmqBinaryPublisherBase {
protected:
	ofxZmqPublisher pub;
	uint64_t ts;

public:
	void setup(string addr) {
		pub.bind(addr);
		ts = 0;
	}

	void update(uint64_t t) {
		ts = t;
	}

	template <typename T>
	bool sendPixels(string identifer, const T* ptr, int width, int height, int numChannels) {
		ofxZmqBinaryHeader header(identifer, ts, 0, OFX_ZMQ_BINARY_SINGLE_IMAGE);

		ofxZmqPixelsHeader h;
		h.width = width;
		h.height = height;
		h.bytesPerChannel = sizeof(T);
		h.numChannels = numChannels;
		header.elementSize = h.width * h.height * h.bytesPerChannel * h.numChannels + sizeof(ofxZmqPixelsHeader);

		size_t newSz = sizeof(ofxZmqBinaryHeader) + sizeof(ofxZmqPixelsHeader) + header.elementSize;
		uint8_t* buffer = new uint8_t[newSz];
		memcpy(buffer, &header, sizeof(ofxZmqBinaryHeader));
		memcpy(buffer + sizeof(ofxZmqBinaryHeader), &h, sizeof(ofxZmqPixelsHeader));
		memcpy(buffer + sizeof(ofxZmqBinaryHeader) + sizeof(ofxZmqPixelsHeader), ptr, header.elementSize);
		bool ret = pub.send(buffer, newSz);
		delete [] buffer;
		return ret;
	}

	bool send(string identifer, const void *data, size_t len) {
		ofxZmqBinaryHeader header(identifer, ts, len);
		return send(header, data, len);
	}

	template <typename T>
	bool sendObject(string identifer, const T& data) {
		return send(identifer, (const void*) &data, sizeof(T));
	}

	template <typename T>
	bool sendArray(string identifer, const vector<T>& data) {
		ofxZmqBinaryHeader header(identifer, ts, sizeof(T), OFX_ZMQ_BINARY_OBJECT_ARRAY, data.size());
		return send(header, (const void*) &data.front(), sizeof(T) * data.size());
	}

protected:
	bool send(const ofxZmqBinaryHeader& header, const void *data, size_t len) {
		size_t newSz = sizeof(ofxZmqBinaryHeader) + len;
		uint8_t* buffer = new uint8_t[newSz];
		memcpy(buffer, &header, sizeof(ofxZmqBinaryHeader));
		memcpy(buffer + sizeof(ofxZmqBinaryHeader), data, len);
		bool ret = pub.send(buffer, newSz);
		delete [] buffer;
		return ret;
	}
};
