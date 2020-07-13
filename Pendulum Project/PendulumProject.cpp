#include <iostream>
#include <cmath>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h> //for linux
//#include <SDL.h> //for win & mac

using namespace std;

//screen dimension constants
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 480;

//The window's title
const char* gWindow_title = "Research Project: Pendulum 2-D";

//Create the window
SDL_Window* gWindow = NULL;

//Creates a renderer to render our graphics
SDL_Renderer* gRenderer = NULL;

bool gINIT(){

  bool success = true;

  if( SDL_Init( SDL_INIT_EVERYTHING ) != 0 ) {

    cout << "error initializing SDL: " << SDL_GetError() << "\n";
    success = false;
  } else {

    //create window
    gWindow = SDL_CreateWindow( gWindow_title, SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,
				SCREEN_WIDTH, SCREEN_HEIGHT,SDL_WINDOW_SHOWN );

    if( gWindow == NULL ) {
      cout << "Window could not be created! SDL_Error: " << SDL_GetError() << "\n";
      success = false;
    } else {

      // creates a renderer to render our images
      gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );

      if( gRenderer == NULL ) {
	cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << "\n";
	success = false;
      }
    }
  }

  return success;
}

void gCLOSE(){

  //Destroy renderer
  SDL_DestroyRenderer( gRenderer );
  gRenderer = NULL;

  //Destroy window
  SDL_DestroyWindow( gWindow );
  gWindow = NULL;

  //Quit SDL
  SDL_Quit();
}

class PEN {

public:
  double g, a11, a12, a22;
  double theta[2],omega[2],L[2],m[2],pX[2],pY[2],X[2],Y[2],F[2],P[2];

  void setPosition(double th1, double th2) {
    theta[0] = th1;
    theta[1] = th2;
  }

  void setVelocity(double om1, double om2) {
    omega[0] = om1;
    omega[1] = om2;
  }

  void setLength(double L1, double L2) {
    L[0] = L1;
    L[1] = L2;
  }

  void setMass(double m1, double m2) {
    m[0] = m1;
    m[1] = m2;
  }

  void movePendulum(double dt) {
    theta[0] = theta[0] + omega[0]*dt;
    theta[1] = theta[1] + omega[1]*dt;
    P[0] = P[0] + F[0]*dt;
    P[1] = P[1] + F[1]*dt;
  }

  void updateMomentum() {
    a11 = (m[0]+m[1])*(L[0]*L[0]); //A11 = (m1+m2)*l1^2
    a22 = m[1]*L[1]*L[1]; //A22 = m2*l2*l2
    a12 = m[1]*L[0]*L[1]*cos(theta[0]-theta[1]);

    P[0] = a11*omega[0]+a12*omega[1];
    P[1] = a12*omega[0]+a22*omega[1];
  }

  void updateForce() {
    F[0] = (-m[1]*L[0]*L[1]*omega[0]*omega[1]*sin(theta[0]-theta[1])) -
      ((m[0]+m[1]) * g*L[0]*sin(theta[0])); // Force of first pendulum

    F[1] = (m[1]*L[0]*L[1]*omega[0]*omega[1]*sin(theta[0]-theta[1])) -
      (m[1]*g*L[1]*sin(theta[1])); //Force of second Pendulum
  }

  void updateOmega() {
    omega[0] = ((a22*P[0]) - (a12*P[1])) / ((a11*a22) - (a12*a12));
    omega[1] = ((a11*P[1]) - (a12*P[0])) / ((a11*a22) - (a12*a12));
  }

  double getEnergy() {
    return (m[0]+m[1])*L[0]*L[0]*omega[0]*omega[0]/2 + m[1]*L[1]*L[1]*omega[1]*omega[1]/2 +
      m[1]*L[0]*L[1]*omega[0]*omega[1]*cos(theta[0]-theta[1]) - (m[0]+m[1])*g*L[0]*cos(theta[0])
          - m[1]*g*L[1]*cos(theta[1]);
  }

  void updatePosition() {
    X[0] = pX[0] + L[0]*sin(theta[0]);
    X[1] = pX[1] + L[1]*sin(theta[1]);
    Y[0] = pY[0] + L[0]*cos(theta[0]);
    Y[1] = pY[1] + L[1]*cos(theta[1]);
  }

};


int main( int argc, char* argv[] ) {

  SDL_SetMainReady();

  //Initialize SDL
  if( gINIT() ) {

    bool quit = false;
    SDL_Event e;

    const int N=100;
    const double pi=4*atan(1.0);

    //layouts
    int box_X1=5, box_Y1=5, box_X2=SCREEN_WIDTH/2-5, box_Y2=SCREEN_HEIGHT-5;
    SDL_Rect pendulum_frameRect = { box_X1, box_Y1, box_X2-box_X1, box_Y2-box_Y1 };

    int energy_box_X1 = SCREEN_WIDTH/2+5, energy_box_Y1=5, energy_box_X2 = SCREEN_WIDTH-5, energy_box_Y2=SCREEN_HEIGHT-5;
    SDL_Rect energy_frameRect = { energy_box_X1, energy_box_Y1, energy_box_X2-energy_box_X1, energy_box_Y2-energy_box_Y1 };

    double offsetX1 = box_X1 + (box_X2-box_X1)/4, offsetX2 = box_X1 + 3*(box_X2-box_X1)/4;

    int Nt = 5000; // to slow down "physical" time
    double dt=0.2;
    double circR=10;
    double E0[2];

    int NE = 200;
    double E1[NE],E2[NE];

    int steps = 0;

    PEN p[2]; // p[0] is the left pendulum, p[1] is the right one

    //**********************************************************************************
    //Set initial positions and velocities
    p[0].g = 9.81;
    p[0].setPosition(pi/6,pi/3);
    p[0].setVelocity(0.0,0.0);
    p[0].setLength(150.0,150.0);
    p[0].setMass(1.0,1.0);

    p[1].g = 9.81;
    p[1].setPosition(pi/6,-pi/3);
    p[1].setVelocity(0,0.0);
    p[1].setLength(150.0,150.0);
    p[1].setMass(1.0,2.0);

    E0[0] = p[0].getEnergy();
    E0[1] = p[1].getEnergy();

    //**********************************************************************************
    //Set initial positions and velocities
    int NP=2; //Part 1
    double pivotX[NP], pivotY[NP], circX[NP], circY[NP];


    //**********************************************************************************

    // main loop
    while ( !quit ){ // will break is quit = true

      //Handle events on queue
      while( SDL_PollEvent( &e ) != 0 ) {
	//User requests quit
	if( e.type == SDL_QUIT ) {
	  quit = true;
	}
      }

      //**********************************************************************************
      //Update the positions and velocities
      p[0].pX[0] = 0;
      p[0].pY[0] = box_Y2/8;

      p[1].pX[0] = 0;
      p[1].pY[0] = box_Y2/8;

      for(int nt=0;nt<Nt;nt++) { //slowdown loop
	         for(int i=0;i<NP;i++)  {
	          p[i].updateMomentum();
	          p[i].updateForce();
	          p[i].movePendulum(dt/Nt);
	          p[i].updateOmega();
	         }// end-loop for i
      }// end-loop for nt

      for(int i=0;i<NP;i++) {

	       p[i].updatePosition();

	       for(int n = 1; n < NP; n++) {// Every pendulum pivot point after the first one
	          p[i].pX[n] = p[i].X[n-1];
	           p[i].pY[n] = p[i].Y[n-1];
         }
      }

      if(steps >= NE) steps=0;

      E1[steps] = p[0].getEnergy();
      E2[steps] = p[1].getEnergy();

      steps++;

      //**********************************************************************************

      //Clear the screen
      SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
      SDL_RenderClear( gRenderer );

      //Draw the layout
      SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0x00 );
      SDL_RenderDrawRect( gRenderer, &pendulum_frameRect );
      SDL_RenderDrawRect( gRenderer, &energy_frameRect ); //??

      //Draw the pendulum #1
      SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0xFF, 0xFF );

      for(int j=0;j<NP;j++){
	SDL_RenderDrawLine( gRenderer, offsetX1 + p[0].pX[j] + circR*sin(p[0].theta[j]), p[0].pY[j]+circR*cos(p[0].theta[j]),
			    offsetX1 + p[0].X[j] - circR*sin(p[0].theta[j]), p[0].Y[j] -circR*cos(p[0].theta[j]) );

	for(int i=0;i<N;i++){
	  SDL_RenderDrawPoint( gRenderer, offsetX1 + p[0].X[j] + circR*cos(2*pi/N*i), p[0].Y[j] + circR*sin(2*pi/N*i) );
	}
      }

      //Draw the pendulum #2
      SDL_SetRenderDrawColor( gRenderer, 0x00, 0xFF, 0x00, 0xFF );

      for(int j=0;j<NP;j++){
	SDL_RenderDrawLine( gRenderer, offsetX2 + p[1].pX[j] + circR*sin(p[1].theta[j]), p[1].pY[j]+circR*cos(p[1].theta[j]),
			    offsetX2 + p[1].X[j] - circR*sin(p[1].theta[j]), p[1].Y[j] -circR*cos(p[1].theta[j]) );

	for(int i=0;i<N;i++){
	  SDL_RenderDrawPoint( gRenderer, offsetX2 + p[1].X[j] + circR*cos(2*pi/N*i), p[1].Y[j] + circR*sin(2*pi/N*i) );
	}
      }

      //Draw Graphs: Energy 1
      int en_tstep = 3, en_r = 1;

      SDL_SetRenderDrawColor( gRenderer, 0xFF, 0x00, 0x00, 0x00 );
      SDL_RenderDrawLine( gRenderer, energy_box_X1, energy_box_Y2/2 - E0[0]/25, energy_box_X2, energy_box_Y2/2 - E0[0]/25); // E = E0 line

      SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0x00 );
      SDL_RenderDrawLine( gRenderer, energy_box_X1 , energy_box_Y2/2 , energy_box_X2 , energy_box_Y2/2 ); // x-axis

      SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0xFF, 0x00 );
      for(int i=0;i<N;i++)
	for(int j=0; j<NE;j++)
	  SDL_RenderDrawPoint( gRenderer, energy_box_X1 + j*en_tstep + en_r*cos(2*pi/N*i), energy_box_Y2/2 - E1[j]/25 + en_r*sin(2*pi/N*i) );

      //Draw Graphs: Energy 2
      SDL_SetRenderDrawColor( gRenderer, 0xFF, 0x00, 0x00, 0x00 );
      SDL_RenderDrawLine( gRenderer, energy_box_X1, energy_box_Y2/2 - E0[1]/25, energy_box_X2, energy_box_Y2/2 - E0[1]/25); // E = E0 line

      SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0x00 );
      SDL_RenderDrawLine( gRenderer, energy_box_X1 , energy_box_Y2/2 , energy_box_X2 , energy_box_Y2/2 ); // x-axis

      SDL_SetRenderDrawColor( gRenderer, 0x00, 0xFF, 0x00, 0x00 );

      for(int i=0;i<N;i++)
	for(int j=0; j<NE;j++)
	  SDL_RenderDrawPoint( gRenderer, energy_box_X1 + j*en_tstep + en_r*cos(2*pi/N*i), energy_box_Y2/2 - E2[j]/25 + en_r*sin(2*pi/N*i) );

      //Update the screen
      SDL_RenderPresent( gRenderer );

      //Delay
      SDL_Delay(1000/60);

    }
  }

  // free resources and close SDL
  gCLOSE();

  return 0;
}
