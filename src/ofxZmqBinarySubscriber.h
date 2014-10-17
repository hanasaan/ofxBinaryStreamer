#pragma once
#include "ofMain.h" // We assume subscriber is always Mac.
#include "ofxZmq.h"
#include "ofxBinaryStreamerTypes.h"

class ofxZmqBinarySubscriber {
private:
	ofxZmqSubscriber sub;
	ofBuffer tmpBuffer;
	ofxZmqBinaryType tmpType;
	bool tmpBufferEnabled;
#ifdef USE_OFX_TURBO_JPEG
    ofxTurboJpeg turbo;
#endif
public:
    ofxZmqSubscriber& getSub() {
        return sub;
    }
    
    void getLatestBuffer(ofBuffer** ptr) {
        *ptr = &tmpBuffer;
    }
    
	void setup(string addr) {
		sub.connect(addr);
	}

	bool hasWaitingMessage(long timeout_millis = 0) {
		return sub.hasWaitingMessage(timeout_millis);
	}

	bool fetchNextMessageType(ofxZmqBinaryType& outType, string& outFourCc, bool isFakeInput = false) {
        if (!isFakeInput) {
            sub.getNextMessage(tmpBuffer);
        }
        if (tmpBuffer.size()) {
            ofxZmqBinaryHeader* header = (ofxZmqBinaryHeader*) tmpBuffer.getBinaryBuffer();
            outType = header->type;
            tmpType = header->type;
            outFourCc = getFourCc(tmpBuffer);
            tmpBufferEnabled = true;
            return true;
        } else {
            return false;
        }
	}

#ifdef USE_OFX_TURBO_JPEG
    bool getNextMessageAsPixels(ofPixels &data) {
        bool ret = tmpBufferEnabled && (tmpType == OFX_ZMQ_BINARY_SINGLE_IMAGE || tmpType == OFX_ZMQ_BINARY_SINGLE_IMAGE_JPEG);
        if (ret) {
            const char* ptr = tmpBuffer.getBinaryBuffer() + sizeof(ofxZmqBinaryHeader);
            if (tmpType == OFX_ZMQ_BINARY_SINGLE_IMAGE) {
                ofxZmqPixelsHeader* header = (ofxZmqPixelsHeader*) ptr;
                const unsigned char* imgPtr = (const unsigned char*)ptr + sizeof(ofxZmqPixelsHeader);
                data.setFromPixels(imgPtr, header->width, header->height, header->numChannels);
            } else if (tmpType == OFX_ZMQ_BINARY_SINGLE_IMAGE_JPEG) {
                ofxZmqBinaryHeader* h = (ofxZmqBinaryHeader*) tmpBuffer.getBinaryBuffer();
                ofBuffer imageBuffer;
                int actualSize = h->elementSize;
                imageBuffer.set(ptr, actualSize);
                return turbo.load(imageBuffer, data);
            }
        }
        return ret;
    }
#endif
    
	template <typename T, typename P>
	bool getNextMessageAsPixels(ofPixels_<P> &data) {
		bool ret = tmpBufferEnabled && (tmpType == OFX_ZMQ_BINARY_SINGLE_IMAGE || tmpType == OFX_ZMQ_BINARY_SINGLE_IMAGE_JPEG);
		if (ret) {
			const char* ptr = tmpBuffer.getBinaryBuffer() + sizeof(ofxZmqBinaryHeader);
			if (tmpType == OFX_ZMQ_BINARY_SINGLE_IMAGE) {
				ofxZmqPixelsHeader* header = (ofxZmqPixelsHeader*) ptr;
				const char* imgPtr = ptr + sizeof(ofxZmqPixelsHeader);
				data.setFromPixels((const P*) imgPtr, header->width, header->height, header->numChannels); 
			} else if (tmpType == OFX_ZMQ_BINARY_SINGLE_IMAGE_JPEG) {
                ofxZmqBinaryHeader* h = (ofxZmqBinaryHeader*) tmpBuffer.getBinaryBuffer();
				ofBuffer imageBuffer;
				int actualSize = h->elementSize;
                imageBuffer.set(ptr, actualSize);
                return ofLoadImage(data, imageBuffer);
			}
		}
		return ret;
	}

	template <typename T>
	bool getNextMessageAsObject(T& data) {
		int sz = tmpBuffer.size() - sizeof(ofxZmqBinaryHeader);
		bool ret = tmpBufferEnabled && (tmpType == OFX_ZMQ_BINARY_SINGLE_OBJECT) && (sizeof(T) == sz);
		if (ret) {
			const char* ptr = tmpBuffer.getBinaryBuffer() + sizeof(ofxZmqBinaryHeader);
			T* objPtr = (T*) ptr;
			data = *objPtr;
		}
		return ret;
	}

	template <typename T>
	bool getNextMessageAsArray(vector<T>& data) {
		int sz = tmpBuffer.size() - sizeof(ofxZmqBinaryHeader);
		ofxZmqBinaryHeader* h = (ofxZmqBinaryHeader*) tmpBuffer.getBinaryBuffer(); 
		bool ret = tmpBufferEnabled && (tmpType == OFX_ZMQ_BINARY_OBJECT_ARRAY);
		if (ret) {
			const char* ptr = tmpBuffer.getBinaryBuffer() + sizeof(ofxZmqBinaryHeader);
			T* objPtr = (T*) ptr;
            
            if (sizeof(T) == h->elementSize) {
                data.clear();
                data.insert(data.end(), objPtr, objPtr + sz / sizeof(T));
            } else {
                // TODO
                cerr << "sizeofT does not match! : " << sizeof(T) << "," << h->elementSize <<  endl;
            }
		}
		return ret;
	}
    
private:
    string getFourCc(ofBuffer& buffer) {
        ofxZmqBinaryHeader* h = (ofxZmqBinaryHeader*) buffer.getBinaryBuffer();
		string str;
        str.append(h->fourCc, 4);
        return str;
    }
};