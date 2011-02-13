#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"
#include "ofxOpenNI.h"

#define		RightHandUp			8
#define		RightHandDown		9
#define		RightHandRight		10
#define		RightHandLeft		11
#define		RightHandFront		12
#define		RightHandBack		13
#define		LeftHandFront		12
#define		LeftHandBack		13

class testApp : public ofBaseApp{

	public:
		int	_screenWidth, _screenHeight, gestureLeft, oldGestureLeft, gestureRight, oldGestureRight;
    bool isTracking;
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		ofxOpenNIContext context;
		ofxDepthGenerator depth;
		ofxUserGenerator user;
		
		void PostMouseEvent(CGMouseButton button, CGEventType type, CGPoint point);
};

#endif
