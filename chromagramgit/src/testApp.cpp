#include "testApp.h"
#include "stdio.h"
#include "aubio.h"


//--------------------------------------------------------------
void testApp::setup(){

	ofBackground(255,255,255);

	// 2 output channels,
	// 0 input channels
	// 22050 samples per second
	// 256 samples per buffer
	// 4 num buffers (latency)
	

//	ofSetDataPathRoot("../bin/data/");


	sampleRate 			= 44100;
	phase 				= 0;
	phaseAdder 			= 0.0f;
	phaseAdderTarget 	= 0.0f;
	volume				= 0.1f;
	bNoise 				= false;
	lAudio = new float[256];
	rAudio = new float[256];
	ofSoundStreamSetup(2,0,this, sampleRate,256, 4);

	ofSetFrameRate(60);
	
	fvec_t * my_fvec_t;
	aubio_onset_t* my_aubio_result;
	aubio_onsetdetection_t * my_onset_detection;
	
	scrollWidth = 1600;
	
	int readcount = 0; // counts number of samples read from sound file

	//string inputFilename = "sound/GetIntoTheGroove.wav";// input file name placed in bin

	sfinfo.format = 0;
	
	moveOn = true;
	
	energyIndex = 0;
	frameIndex = 0;
	chromaIndex = 0;
	chromoGramm.initialise(FRAMESIZE,2048);//framesize 512 and hopsize 2048

	loadSoundFiles();
			
	readcount = 1;
	
	while(readcount != 0 && moveOn == true)
	{
	
		// read FRAMESIZE samples from 'infile' and save in 'data'
		readcount = sf_read_float(infile, frame, FRAMESIZE);
		
	//processing frame - downsampled to 11025Hz
	//8192 samples per chroma frame
		chromoGramm.processframe(frame);

		if (chromoGramm.chromaready)
		{
			if (chromaIndex < ENERGY_LENGTH/CHROMA_CONVERSION_FACTOR){// && energyIndex < ENERGY_LENGTH){//dont exceed vector length
			for (int i = 0;i<12;i++){
				chromoGramVector[chromaIndex][i] = chromoGramm.rawChroma[i] / chromoGramm.maximumChromaValue;
			}	
			//this would do chord detection
			chord.C_Detect(chromoGramm.chroma,chromoGramm.chroma_low);
			rootChord[chromaIndex] = chord.root;

			chromaIndex++;	
			}
		
		

		}
		
	//	printf ("Root %i", chord.root);
			
		//get energy of the current frame and wait
		getEnergyOfFrame();
														
		// write 'readcount' samples from 'frame' to 'outfile' 
		sf_write_float(outfile, frame, readcount) ;
		
		

	}
	
totalNumberOfFrames = frameIndex;

printf("Total frames %i energy index %i and Chroma index %i", frameIndex, energyIndex, chromaIndex);

/*
////// NEw soundfile test
 // Open the sound file
        SF_INFO FileInfos;
			string Filename = "../../../GetIntoTheGroove.wav";
        SNDFILE* File = sf_open(Filename.c_str(), SFM_READ, &FileInfos);

        if (File)
        {
            sf_close(File);
			printf("SF CLOSE : closed file okay");
		}
        else
        {
            switch(sf_error(File))
            {
                case SF_ERR_UNRECOGNISED_FORMAT:
                    ofLog(OF_LOG_ERROR,"ofxSoundPlayer loading file - unrecognised format");
                    break;
                case SF_ERR_SYSTEM:
                    ofLog(OF_LOG_ERROR,"ofxSoundPlayer loading file - system error");
                    break;
                case SF_ERR_MALFORMED_FILE:
                    ofLog(OF_LOG_ERROR,"ofxSoundPlayer loading file - malformed file");
                    break;
                case SF_ERR_UNSUPPORTED_ENCODING:
                    ofLog(OF_LOG_ERROR,"ofxSoundPlayer loading file - unsupported encoding");
                    break;
                default:
                    ofLog(OF_LOG_ERROR,"ofxSoundPlayer loading file - unknown error");
                    break;
            };
        }
		*/
    

//end new soundfile ftest



}


//--------------------------------------------------------------
void testApp::update(){
	textString = "energy index [";
	textString += ofToString(xIndex);
	textString += "] = ";
	textString += ofToString(energy[xIndex]);
	
		

	chordString = "Chord : ";
	chordString += ofToString(rootChord[currentPlayingFrame/CHROMA_CONVERSION_FACTOR]);



	
	audioPosition = loadedAudio.getPosition() * totalNumberOfFrames;//frameIndex;//the position in number of frames
	currentPlayingFrame = audioPosition;
	audioPosition = (int) audioPosition % scrollWidth ;
	audioPosition /= scrollWidth;
}

//--------------------------------------------------------------
void testApp::draw(){

drawChromoGram();

/*

	// draw the left:
	ofSetColor(0x333333);
	ofRect(100,100,256,200);
	ofSetColor(0xFFFFFF);
	for (int i = 0; i < 256; i++){
		ofLine(100+i,200,100+i,200+lAudio[i]*100.0f);
	}

	// draw the right:
	ofSetColor(0x333333);
	ofRect(600,100,256,200);
	ofSetColor(0xFFFFFF);
	for (int i = 0; i < 256; i++){
		ofLine(600+i,200,600+i,200+rAudio[i]*100.0f);
	}

	ofSetColor(0x333333);
	char reportString[255];
	sprintf(reportString, "volume: (%f) modify with -/+ keys\npan: (%f)\nsynthesis: %s", volume, pan, bNoise ? "noise" : "sine wave");
	if (!bNoise) sprintf(reportString, "%s (%fhz)", reportString, targetFrequency);

	ofDrawBitmapString(reportString,80,380);
*/
ofDrawBitmapString(textString,80,480);
	drawEnergy();

	ofSetColor(0xFFFFFF);
	ofLine(audioPosition*width, 0, audioPosition*width, height);

	ofDrawBitmapString(chordString,80,580);
}

void testApp::drawEnergy(){
ofSetColor(0x6666FF);
	float screenHeight = ofGetHeight() ;
	float screenWidth = ofGetWidth();
	float heightFactor = 8;
int i, j, startingFrame;
startingFrame = currentPlayingFrame / scrollWidth;//i.e. number of scroll widths in
startingFrame *= scrollWidth;

	for (i = 0; i < scrollWidth - 1; i++){
	j = i + startingFrame;
	ofLine(i*screenWidth/scrollWidth, screenHeight - (energy[j]*screenHeight/heightFactor),
	 screenWidth*(i+1)/scrollWidth, screenHeight - (energy[j+1]*screenHeight/heightFactor));

	}
}


void testApp::drawChromoGram(){
	float screenHeight = ofGetHeight() ;
	float screenWidth = ofGetWidth();
	float heightFactor = 8;
int i, j, startingFrame;
startingFrame = currentPlayingFrame / scrollWidth;//i.e. number of scroll widths in
startingFrame *= scrollWidth;//starting frame in terms of energy frames
startingFrame /= CHROMA_CONVERSION_FACTOR; //in terms of chroma frames

float chromoLength = scrollWidth/CHROMA_CONVERSION_FACTOR;
	for (i = 0; i < chromoLength; i++){
	j = i + startingFrame;
		for (int y = 0;y < 12;y++){
		ofSetColor(255*chromoGramVector[j][11-y],0,0);
		ofRect(i*screenWidth/chromoLength,y*screenHeight/12,screenWidth/chromoLength,screenHeight/12);
		}//end y
	}//end i

}


void testApp::loadSoundFiles(){
		
		//assume libsndfile looks in the folder where the app is run
		//therefore ../../../ gets to the bin folder
		//we then need dat/sounds/to get to the sound folder
		//this is different to the usual OF default folder
		const char	*infilename = "../../../data/sound/GetIntoTheGroove2.wav";//PicturesMixer6.aif";//
		//ac_guitar_final.wav
		string loadfilename = "sound/GetIntoTheGroove2.wav";//PicturesMixer6.aif";
				
		loadedAudio.loadSound(loadfilename);
		
		// Open Input File with lib snd file
    if (! (infile = sf_open (infilename, SFM_READ, &sfinfo)))
    {   // Open failed
        printf ("SF OPEN routine Not able to open input file %s.\n", infilename) ;
        // Print the error message from libsndfile. 
        puts (sf_strerror (NULL)) ;

        } else{
		printf("SF OPEN opened file %s okay.\n", infilename);
		sndfileInfoString = "Opened okay ";
			
		};
		
//end adam

//end libsndfile load part


  

  const char	*outfilename = "../../../output.wav" ; // output file name

    // Open the output file. 
    if (! (outfile = sf_open (outfilename, SFM_WRITE, &sfinfo)))
    {   printf ("Not able to open output file %s.\n", outfilename) ;
	sndfileInfoString = "NOT OPENED";
        puts (sf_strerror (NULL)) ;
        } ;

}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	if (key == '-'){
		volume -= 0.05;
		volume = MAX(volume, 0);
	} else if (key == '+'){
		volume += 0.05;
		volume = MIN(volume, 1);
	}
	
	if (key == OF_KEY_UP){
		if (scrollWidth > 600)
		scrollWidth += 400;
		else
		scrollWidth *= 2;
	}
	
	if (key == OF_KEY_DOWN){
		if (scrollWidth > 600)
		scrollWidth -= 400;
		else
		scrollWidth /= 2;
	}
		
	if (key == ' '){
		togglePlaying = !togglePlaying;
		

		
		if (togglePlaying){
		loadedAudio.setPaused(false);
		}
		else {
			loadedAudio.setPaused(true);	
		}
		
		if (!playingStarted){
			playingStarted = true;
			loadedAudio.play();
		}
		
	}
	
	if (key == 'o'){
	//open audio file
	}
	
}

//--------------------------------------------------------------
void testApp::keyReleased  (int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	width = ofGetWidth();
	pan = (float)x / (float)width;
	float height = (float)ofGetHeight();
	float heightPct = ((height-y) / height);
	targetFrequency = 2000.0f * heightPct;
	phaseAdderTarget = (targetFrequency / (float) sampleRate) * TWO_PI;
	xIndex = (int)(pan*ENERGY_LENGTH);
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	width = ofGetWidth();
	pan = (float)x / (float)width;
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	bNoise = true;
	moveOn = true;
}


//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	bNoise = false;
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
width = w;
height = h;
}
//--------------------------------------------------------------
void testApp::audioRequested 	(float * output, int bufferSize, int nChannels){
	//pan = 0.5f;
	float leftScale = 1 - pan;
	float rightScale = pan;

	// sin (n) seems to have trouble when n is very large, so we
	// keep phase in the range of 0-TWO_PI like this:
	while (phase > TWO_PI){
		phase -= TWO_PI;
	}

/*	if ( bNoise == true){
		// ---------------------- noise --------------
		for (int i = 0; i < bufferSize; i++){
			lAudio[i] = output[i*nChannels    ] = ofRandomf() * volume * leftScale;
			rAudio[i] = output[i*nChannels + 1] = ofRandomf() * volume * rightScale;
		}
	} else {
		phaseAdder = 0.95f * phaseAdder + 0.05f * phaseAdderTarget;
		for (int i = 0; i < bufferSize; i++){
			phase += phaseAdder;
			float sample = sin(phase);
			lAudio[i] = output[i*nChannels    ] = sample * volume * leftScale;
			rAudio[i] = output[i*nChannels + 1] = sample * volume * rightScale;
		}
	}
*/
}


void testApp::loadSndfile(){


}

void testApp::getEnergyOfFrame(){

	//printf("calling drawSndFile %i", frameIndex);
	frameIndex++;
	
	float totalEnergyInFrame = 0;
	
	for (int i = 0;i<FRAMESIZE;i++){


	/*	if (i == 455){
		printf("Frame : %f ", frame[455]);
		}
	*/
	totalEnergyInFrame += (frame[i] * frame[i]);
	
	}
	totalEnergyInFrame = sqrt(totalEnergyInFrame);
	
	if (energyIndex < ENERGY_LENGTH){
	energy[energyIndex] = totalEnergyInFrame;
	energyIndex++;
	}


	//moveOn = false;
}

