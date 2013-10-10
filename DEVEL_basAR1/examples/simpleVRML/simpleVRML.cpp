// ============================================================================
//	Includes
// ============================================================================

#include <windows.h>
#include <process.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glut.h>

#include <AR/config.h>
#include <AR/video.h>
#include <AR/param.h>
#include <AR/ar.h>
#include <AR/gsub_lite.h>
#include <AR/arvrml.h>

#include <list>



#include "Arpe.h"
#include "Actuator.h"
#include "Rules.h"
#include "InfraSource.h"
#include "InfraStructure.h"
#include "ActuatorARTKSM.h"
#include "InfraARTKSM.h"
#include "ipObject.h"
#include "Serial.h"
#include "ipDist.h"

using namespace std;

/* ============================================================================
//	Constants
============================================================================ */

#define VIEW_SCALEFACTOR		0.025		// 1.0 ARToolKit unit becomes 0.025 of my OpenGL units.
#define VIEW_SCALEFACTOR_1		1.0			// 1.0 ARToolKit unit becomes 1.0 of my OpenGL units.
#define VIEW_SCALEFACTOR_4		4.0			// 1.0 ARToolKit unit becomes 4.0 of my OpenGL units.
#define VIEW_DISTANCE_MIN		4.0			// Objects closer to the camera than this will not be displayed.
#define VIEW_DISTANCE_MAX		4000.0		// Objects further away from the camera than this will not be displayed.

// ============================================================================
//	Global variables
// ============================================================================

// Preferences.
//static int prefWindowed = FALSE;
//static int prefWidth = 640;					// Fullscreen mode width.
//static int prefHeight = 480;				// Fullscreen mode height.
//static int prefDepth = 32;					// Fullscreen mode bit depth.
//static int prefRefresh = 0;					// Fullscreen mode refresh rate. Set to 0 to use default rate.

// Image acquisition.
static ARUint8		*gARTImage = NULL;

// Marker detection.
static int			gARTThreshhold = 100;
static long			gCallCountMarkerDetect = 0;

// Transformation matrix retrieval.
static int			gPatt_found = FALSE;	// At least one marker.

// Drawing.
static ARParam		gARTCparam;
static ARGL_CONTEXT_SETTINGS_REF gArglSettings = NULL;

// Object Data.
Arpe arpe;


bool				cdChanged = false;
int					cd=100;
int					toggleBase = 0;
int					tr = 0;
// bool				projection = false;
// ============================================================================
//	Functions
// ============================================================================

static int setupCamera(const char *cparam_name, char *vconf, ARParam *cparam);
static void debugReportMode(void);
static void Quit(void);
static void Keyboard(unsigned char key, int x, int y);
static void Idle(void);
static void Visibility(int visible);
static void Reshape(int w, int h);
static void Display(void);
void poolingThread(void * pParams);
int main(int argc, char** argv);

static int setupCamera(const char *cparam_name, char *vconf, ARParam *cparam)
{	
    ARParam			wparam;
	int				xsize, ysize;
    // Open the video path.
	printf("\n arVideoOpen()");
	arVideoOpen(vconf);

 //   if (arVideoOpen(vconf) < 0) {
 //   	fprintf(stderr, "setupCamera(): Unable to open connection to camera.\n");
 //   	printf("\n setupCamera(): Unable to open connection to camera.\n");
 //  	    return (FALSE);
 //}
	
	printf("\n arVideoInqSize()");
	arVideoInqSize(&xsize, &ysize);

    //// Find the size of the window.
    //if (arVideoInqSize(&xsize, &ysize) < 0) return (FALSE);
    //fprintf(stdout, "Camera image size (x,y) = (%d,%d)\n", xsize, ysize);
	
	printf("\n arParamLoad()");
	arParamLoad(cparam_name, 1, &wparam);

	//// Load the camera parameters, resize for the window and init.
 //   if (arParamLoad(cparam_name, 1, &wparam) < 0) {
	//	fprintf(stderr, "setupCamera(): Error loading parameter file %s for camera.\n", cparam_name);
	//	printf("\n setupCamera(): Error loading parameter file %s for camera.\n", cparam_name);
 //       return (FALSE);
 //   }
    arParamChangeSize(&wparam, xsize, ysize, cparam);
    fprintf(stdout, "*** Camera Parameter ***\n");
    arParamDisp(cparam);
	
    arInitCparam(cparam);

	if (arVideoCapStart() != 0) {
    	fprintf(stderr, "setupCamera(): Unable to begin camera data capture.\n");
		printf("\n setupCamera(): Unable to begin camera data capture.\n");
		return (FALSE);		
	}
	
	return (TRUE);
}

static int initAppHWandGL(){

	char glutGamemode[32];
	char           *cparam_name    = "Data/camera_para.dat";

#ifdef _WIN32
	char			*vconf = "Data\\WDM_camera_flipV.xml";
#else
	char			*vconf = "";
#endif
	printf("\n calling setupCamera()");
	if (!setupCamera(cparam_name, vconf, &gARTCparam)) {
		fprintf(stderr, "main(): Unable to set up AR camera.\n");
		printf("\n main(): Unable to set up AR camera.\n");
		exit(-1);
	}
	printf("\n setupCamera() ... Ok");
#ifdef _WIN32
	CoInitialize(NULL);
#endif

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	if (!arpe.prefWindowed) {
		if (arpe.prefRefresh) sprintf(glutGamemode, "%ix%i:%i@%i", arpe.prefWidth, arpe.prefHeight, arpe.prefDepth, arpe.prefRefresh);
		else sprintf(glutGamemode, "%ix%i:%i", arpe.prefWidth, arpe.prefHeight, arpe.prefDepth);
		glutGameModeString(glutGamemode);
		glutEnterGameMode();
	} else {
		glutInitWindowSize(gARTCparam.xsize, gARTCparam.ysize);
		glutCreateWindow(arpe.appName);
	}
	printf("\n openGl ok");


	// Setup argl library for current context.
	if ((gArglSettings = arglSetupForCurrentContext()) == NULL) {
		fprintf(stderr, "main(): arglSetupForCurrentContext() returned error.\n");
		exit(-1);
	}
	debugReportMode();
	arUtilTimerReset();
	return 1;
}


void callArduinoThread(void *data){ 
	iPoint *p = static_cast<iPoint *>(data);
	p->poolingThread(); }


static int initAppData(){
	int counterActuator, counterBase, counterPoint;

//	InitializeCriticalSection(&arpe.cs);
	counterActuator = 1;
	counterBase = 1;
	counterPoint = 1;

	printf("\n 1.");
	// Start setting up the Kernel
	if( arpe.arpeReadFiles() == -1) {printf("\n ****** ERROR ON basAR"); exit(0);} 
	InitializeCriticalSection(&arpe.parserCS);
	printf("\n parserCS set.");

	initAppHWandGL();


	printf("\n 2.");
	// Setup actuator
	list<Actuator*>::iterator itA;
	for(itA = arpe.listActuator.begin(); itA != arpe.listActuator.end(); itA++){
		switch((*(*itA)).type){
		case 1:{ // Single ARToolKit Marker actuator
			(*(*itA)).myArpe = &arpe;
			ActuatorARTKSM* a = static_cast<ActuatorARTKSM*>(*itA);
			if( (*a).actuatorReadFile() == -1) {printf("\n ****** ERROR ON ACTUATOR"); exit(0);}  // Mandar id do ultimo atuador
			(*a).id = counterActuator;
			counterActuator++;
			printf("\n %s, ID %d correctly read!",(*(*itA)).name, (*itA)->id);
			break;}
		default: break;
		}
	}


	printf("\n 3.");
	// Setup bases 
	list<Base*>::iterator itB;
	for(itB = arpe.listBase.begin(); itB != arpe.listBase.end(); itB++){
		(*(*itB)).myArpe = &arpe;
		if( (*(*itB)).baseReadFile() == -1) {printf("\n ****** ERROR ON BASE"); exit(0);} // Mandar id da ultima base
		(*(*itB)).id = counterBase;
		counterBase++;
		// Setup iPoint Objects
		list<iPoint*>::iterator itP;
		for(itP = (*(*itB)).listPoint.begin(); itP != (*(*itB)).listPoint.end(); itP++){
			iPoint* ip = (*itP);
			(*ip).myBase = (*itB);
			ip->id = counterPoint;
			switch( (*ip).type){
				case 1:{	// INTERN IPOINT
					InitializeCriticalSection(&ip->animCS);
					if( (*ip).iPointReadFile() == -1) {printf("\n ****** ERROR ON IPOINT"); exit(0);}
					else { 
					}
					break;}

				case 2:{	// EXTERN IPOINT
					InitializeCriticalSection(&ip->commCS);
					if( (*ip).externiPointReadFile() == -1) {printf("\n ****** ERROR ON IPOINT"); exit(0);} 
					else { 
						//Initialize hardware
						Sleep(1000);
						_beginthread(callArduinoThread,0,ip);
					} 
					break;}
				default: { 
					printf("\n ****** ERROR ON IPOINT"); exit(0); }
			}


			//Copy actuator list to the iPoints
			list<Actuator*>::iterator itA;
			for(itA = arpe.listActuator.begin(); itA != arpe.listActuator.end(); itA++){
				ipDist *ipdist = new ipDist();
				ipdist->actuator = (*itA);
				ipdist->distance = 0;
				(*itP)->addActuator(ipdist);
			}

			counterPoint++;
		}
	}

	printf("\n 4.");
	// Setup Rules
	Rules* r = arpe.myRules;
	(*r).myArpe = &arpe;
	(*r).rulesReadFile();

	//printf("\n 5.");
	//if( arpe.arduino !=0){ // If it specifies an Arduino
	//	arpe.arduino->serialReadFile();
	//}

	printf("\n 6.");
	printf("\n --------------------------------------------------------------------------");
	printf("\n Verify objects...");
	printf("\n --------------------------------------------------------------------------");

	// Test render all the VRML objects.
    printf(" \n 6.1. Pre-rendering the VRML objects... ");
	arpe.verifyConsistency();

	//Verify data consistency.
    printf(" \n 6.2. Verify action consistency... ");
	arpe.myRules->verifyConsistency();

	//if Serial is applyable
	printf(" \n 6.2. Verify ARDUINO... ");
//	if( arpe.arduino != 0) {
//		//arpe.arduino->serialSetup();
//
//		if( arpe.arduino->isConnected()){
//			printf(" \n ARDUINO CONNECTED... sending alive signal");
//			if(arpe.arduino->writeData("a",1)){ // a
//				printf(" ... sent a ");
//	/*			int br = -1;
//				while(  br != 3){ br = arpe.arduino->readData(arpe.arduino->buffer,3); }
//				printf(" ... received %s ... (%d bytes).",arpe.arduino->buffer, br); */
//
//				// START READING THREAD
//				_beginthread(poolingThread,0,NULL);
//				arpe.arduino->enableReceive = 1;
//
//			}else { 
//				printf("\n Error on alive signal request, please try to reset your ARDUINO");
//				arpe.arduino = 0;
//			}
//
//		}
//
//	} else {
//	arpe.arduino = 0;
//	printf(" \n -- No extern hardware connected ... ");
//}

	printf("\n 7.");
	// Execute app
	return 1;
}

static void debugReportMode(void)
{
	if(arFittingMode == AR_FITTING_TO_INPUT ) {
		fprintf(stderr, "FittingMode (Z): INPUT IMAGE\n");
	} else {
		fprintf(stderr, "FittingMode (Z): COMPENSATED IMAGE\n");
	}
	
	if( arImageProcMode == AR_IMAGE_PROC_IN_FULL ) {
		fprintf(stderr, "ProcMode (X)   : FULL IMAGE\n");
	} else {
		fprintf(stderr, "ProcMode (X)   : HALF IMAGE\n");
	}
	
	if (arglDrawModeGet(gArglSettings) == AR_DRAW_BY_GL_DRAW_PIXELS) {
		fprintf(stderr, "DrawMode (C)   : GL_DRAW_PIXELS\n");
	} else if (arglTexmapModeGet(gArglSettings) == AR_DRAW_TEXTURE_FULL_IMAGE) {
		fprintf(stderr, "DrawMode (C)   : TEXTURE MAPPING (FULL RESOLUTION)\n");
	} else {
		fprintf(stderr, "DrawMode (C)   : TEXTURE MAPPING (HALF RESOLUTION)\n");
	}
		
	if( arTemplateMatchingMode == AR_TEMPLATE_MATCHING_COLOR ) {
		fprintf(stderr, "TemplateMatchingMode (M)   : Color Template\n");
	} else {
		fprintf(stderr, "TemplateMatchingMode (M)   : BW Template\n");
	}
	
	if( arMatchingPCAMode == AR_MATCHING_WITHOUT_PCA ) {
		fprintf(stderr, "MatchingPCAMode (P)   : Without PCA\n");
	} else {
		fprintf(stderr, "MatchingPCAMode (P)   : With PCA\n");
	}
}

static void Quit(void)
{
	arglCleanup(gArglSettings);
	arVideoCapStop();
	arVideoClose();
	//puzzle.audioEngine->drop();
#ifdef _WIN32
	CoUninitialize();
#endif
	exit(0);
}

static void Keyboard(unsigned char key, int x, int y)
{
	int mode;
	switch (key) {
		case 0x1B:						// Quit.
		case 'Q':
		case 'q':
			printf("\n Closing");
			printf("\n Cleaning memory...");
			// WRITE ALL THE DESCTRUCTORS
			// arpe.~Arpe();
			printf("\n Ok!");
			Quit();
			break;
		case 'p':
		case 'P':
			printf("\n Projection");
			// TURN PROJECTION ON/OFF --- TURN THE VIDEO OFF.
			if( arpe.projection) {
				printf(" mode OFF");
				arpe.projection = false; } else {
					printf(" mode ON");
					arpe.projection = true;
				}

			break;
		case '-':
		case '_':
			gARTThreshhold--;
			printf("\n gARTThreshhold: %d",gARTThreshhold); 
			break;
		case '+':
		case '=':
			gARTThreshhold++;
			printf("\n gARTThreshhold: %d",gARTThreshhold); 
			break;
		//case 'P':
		//case 'p':
		//	if(arpe.arduino->writeData("p",1) == false){ printf("\n falhou");} // a
		//	break;
		//case 'r':
		//case 'R':
		//	{

		//	if(arpe.arduino->writeData("r",1) == false){ printf("\n falhou"); break;} 
		//	printf("\n ... sent r ");
		//	int br = -1;
		//	while(  br != 3){ br = arpe.arduino->readData(arpe.arduino->buffer,3); }
		//	printf("\n ... received %s ... (%d bytes).", arpe.arduino->buffer, br); 
		//	break;}
		//case 'B':
		//case 'b':{
		//		printf("\n Habilita leitura");
		//		arpe.tr = 1;
		//		break;
		//	}
		//case 'V':
		//case 'v':{
		//		printf("\n Desabilita leitura");
		//		arpe.tr = 0;
		//		break;}
		case 'C':
		case 'c':
			mode = arglDrawModeGet(gArglSettings);
			if (mode == AR_DRAW_BY_GL_DRAW_PIXELS) {
				arglDrawModeSet(gArglSettings, AR_DRAW_BY_TEXTURE_MAPPING);
				arglTexmapModeSet(gArglSettings, AR_DRAW_TEXTURE_FULL_IMAGE);
			} else {
				mode = arglTexmapModeGet(gArglSettings);
				if (mode == AR_DRAW_TEXTURE_FULL_IMAGE)	arglTexmapModeSet(gArglSettings, AR_DRAW_TEXTURE_HALF_IMAGE);
				else arglDrawModeSet(gArglSettings, AR_DRAW_BY_GL_DRAW_PIXELS);
			}
			fprintf(stderr, "*** Camera - %f (frame/sec)\n", (double)gCallCountMarkerDetect/arUtilTimer());
			gCallCountMarkerDetect = 0;
			arUtilTimerReset();
			debugReportMode();
			break;

		case '?':
		case '/':
			printf("Keys:\n");
			printf(" q or [esc]    Quit demo.\n");
			printf(" c             Change arglDrawMode and arglTexmapMode.\n");
			printf(" ? or /        Show this help.\n");
			printf("\nAdditionally, the ARVideo library supplied the following help text:\n");
			arVideoDispOption();
			break;
		default:
			break;
	}
}

static void Idle(void)
{
	static int ms_prev;
	int ms;
	float s_elapsed;
	ARUint8 *image;

	ARMarkerInfo    *marker_info;					// Pointer to array holding the details of detected markers.
    int             marker_num;						// Count of number of markers detected.
	

	// Find out how long since Idle() last ran.
	ms = glutGet(GLUT_ELAPSED_TIME);
	s_elapsed = (float)(ms - ms_prev) * 0.001;
	if (s_elapsed < 0.01f) return; // Don't update more often than 100 Hz.
	ms_prev = ms;
	
	// Update drawing.
	arVrmlTimerUpdate();

	gPatt_found = FALSE;	// Invalidate any previous detected markers.
	
	// Grab a video frame.
	if ((image = arVideoGetImage()) != NULL) {
		gARTImage = image;	// Save the fetched image.
		
		
		gCallCountMarkerDetect++; // Increment ARToolKit FPS counter.
		
		// Detect the markers in the video frame.
		if (arDetectMarker(gARTImage, gARTThreshhold, &marker_info, &marker_num) < 0) {
			exit(-1);
		}
	
		//--------------------------------------------------------------------------
		// CHECK FOR ACTUATOR VISIBILITY
		//--------------------------------------------------------------------------
		
		list<Actuator*>::iterator itAct;

		for( itAct = arpe.listActuator.begin(); itAct != arpe.listActuator.end(); itAct++){

			switch( (*(*itAct)).type){
			case 1:{	// ---------------------------------------------------------TREAT ARToolKit Marker
				ActuatorARTKSM* a = static_cast<ActuatorARTKSM*>(*itAct);
				gPatt_found = (*a).searchActuatorOnFrame(&marker_info, marker_num);
				break;}
			default: break;
			};
		}



		//--------------------------------------------------------------------------
		// CHECK FOR BASE VISIBILITY
		//--------------------------------------------------------------------------		

		list<Base*>::iterator b;
		list<InfraSource*>::iterator iSource;

		for( b = arpe.listBase.begin() ; b != arpe.listBase.end() ; b++){
			
			for( iSource = (*(*(*b)).myInfraStructure).listSource.begin(); iSource != (*(*(*b)).myInfraStructure).listSource.end(); iSource++){
				//printf("here");
				//if( (*(*iSource)).onUse)
					switch( (*(*iSource)).type ){
					case 1: { // Source of tracking is ARTKSM
						
						InfraARTKSM* iS = static_cast<InfraARTKSM*>(*iSource);
						gPatt_found = (*iS).searchBaseOnFrame(&marker_info, marker_num);
						break;}
					default: break;
					};
			}
		}

		//--------------------------------------------------------------------------
		// CHECK FOR INTERATIONS
		//--------------------------------------------------------------------------	
		
		arpe.interactionControl();

		// Tell GLUT to update the display.
		glutPostRedisplay();
	}
}

static void Visibility(int visible)
{
	if (visible == GLUT_VISIBLE) {
		glutIdleFunc(Idle);
	} else {
		glutIdleFunc(NULL);
	}
}

static void Reshape(int w, int h)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Call through to anyone else who needs to know about window sizing here.
}

static void Display(void)
{

    GLdouble p[16];
//	GLdouble m[16];
	
	// Select correct buffer for this context.
	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the buffers for new frame.
	
	if (arpe.projection == false ) arglDispImage(gARTImage, &gARTCparam, 1.0, gArglSettings);	// zoom = 1.0.
	arVideoCapNext();
	gARTImage = NULL; // Image data is no longer valid after calling arVideoCapNext().
				
	if (gPatt_found) {
		
		// Projection transformation.
		arglCameraFrustumRH(&gARTCparam, VIEW_DISTANCE_MIN, VIEW_DISTANCE_MAX, p);
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixd(p);
		glMatrixMode(GL_MODELVIEW);
		
		// Viewing transformation.
		glLoadIdentity();
		// Lighting and geometry that moves with the camera should go here.
		// (I.e. must be specified before viewing transformations.)
		//none
		
		// All other lighting and geometry goes here.
		// Calculate the camera position for each object and draw it.


		//--------------------------------------------------------------------------
		// Show BASE Objects
		//--------------------------------------------------------------------------
		list<Base*>::iterator b;

		for( b = arpe.listBase.begin(); b != arpe.listBase.end(); b++){
			if( (*(*(*b)).myInfraStructure).visible == 1){
				//printf("\n Base %s is visible", (*(*b)).name);

				(*(*b)).showBaseItens();
			}
		}

		//--------------------------------------------------------------------------
		// Show ACTUATOR Objects
		//--------------------------------------------------------------------------
		list<Actuator*>::iterator a;

		for( a = arpe.listActuator.begin(); a != arpe.listActuator.end(); a++){
			glPushMatrix();
			
			if( (*(*a)).onUse)
				switch( (*(*a)).type){
					case 1: {
							ActuatorARTKSM* aASM = static_cast<ActuatorARTKSM*>(*a);
							if( (*aASM).visible == 1) {
								//printf("\n Actuator %s is visible", (*aASM).name);
								(*aASM).showActuatorItens();
							}
							break;}
					default: break;
				};
			glPopMatrix();
		}




	} // gPatt_found
	
	// Any 2D overlays go here.
	//none
	
	glutSwapBuffers();
}

int main(int argc, char** argv)
{
	// Iniciar
	printf("\n glutInit()");
	glutInit(&argc, argv);

	initAppData();

	// START SOUNDTRACK AUDIO
	if( arpe.soundTrack != 0){
		arpe.soundTrack->play2D();
	}

	// START INTRODUCTION AUDIO
	if( arpe.startAudio != 0){
		arpe.startAudio->play2D();
	}
		
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutVisibilityFunc(Visibility);
	glutKeyboardFunc(Keyboard);
	
	glutMainLoop();

	return (0);
}

