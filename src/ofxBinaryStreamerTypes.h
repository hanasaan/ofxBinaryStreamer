#pragma once

#ifndef OF_VERSION_MINOR
#include <stdint.h>
#endif

enum ofxZmqBinaryType {
	OFX_ZMQ_BINARY_SINGLE_OBJECT,
	OFX_ZMQ_BINARY_SINGLE_IMAGE,
	OFX_ZMQ_BINARY_SINGLE_IMAGE_JPEG,
	OFX_ZMQ_BINARY_OBJECT_ARRAY,
};

struct ofxZmqBinaryHeader {
	char fourCc[4];
	ofxZmqBinaryType type;
	uint64_t sentTs;
	int32_t arraySize;
	int32_t elementSize;

	ofxZmqBinaryHeader(
		string identifier, uint64_t ts, int size,
		ofxZmqBinaryType t = OFX_ZMQ_BINARY_SINGLE_OBJECT, int arrSize = 0) 
		: sentTs(ts), type(t), elementSize(size), arraySize(arrSize) {
		memcpy(fourCc, identifier.c_str(), 4);
	}
};

struct ofxZmqPixelsHeader {
	int32_t width;
	int32_t  height;
	int32_t bytesPerChannel;
	int32_t numChannels;
};
