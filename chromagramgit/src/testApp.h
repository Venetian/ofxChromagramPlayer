#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "chromaGram.h"
#include "ChordDetect.h"
#include "sndfile.h"


#define FRAMESIZE 512
#define ENERGY_LENGTH 80000
#define CHROMA_LENGTH 4000
#define CHROMA_CONVERSION_FACTOR 16 //16 times as many frames in energy as in chroma
//length in terms of frames (at 512 samples per frame - there are 90 per second) => 900: 10 seconds

class testApp : public ofBaseApp{

	public:


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

		void audioRequested 	(float * input, int bufferSize, int nChannels);
		void loadSndfile();
		void getEnergyOfFrame();
		void drawEnergy();
		void drawChromoGram();
		
		void loadSoundFiles();
		
		float 	pan;
		int		sampleRate;
		bool 	bNoise;
		float 	volume;

		float 	* lAudio;
		float   * rAudio;

		bool moveOn, togglePlaying, playingStarted;
		float frame[FRAMESIZE]; 
		int frameIndex;
		float energy[ENERGY_LENGTH];
		
		float chromoGramVector[CHROMA_LENGTH][12];
		int rootChord[CHROMA_LENGTH];
		
		int energyIndex;
		int totalFrames;
		
		int scrollWidth;// 1600

		//------------------- for the simple sine wave synthesis
		float 	targetFrequency;
		float 	phase;
		float 	phaseAdder;
		float 	phaseAdderTarget;
		
		string sndfileInfoString, textString;
		int xIndex;
		
		ofSoundPlayer  loadedAudio;
		float audioPosition;
		float width, height;
		int chromaIndex;	
		int totalNumberOfFrames;
		int currentPlayingFrame;
		string chordString;
	
		Chromagram chromoGramm;
		
		ChordDetect chord;
		SNDFILE    *outfile ;
			//sndfile part
	SNDFILE *infile; // define input and output sound files
	SF_INFO sfinfo ; // struct to hold info about sound file

		
};

#endif
