//---------------------------------------------------------------------------
#ifndef MapH
#define MapH
#include <vcl.h>
#include "icpPointToPoint.h"
//---------------------------------------------------------------------------
class Map {
	TCanvas *canvas;
	IcpPointToPoint *icp;
	//------------------------------------------------------------------------
	float xOff, yOff;
	float scale;
	float pointGap;
	//------------------------------------------------------------------------
	float linex[ 50 ], liney[ 50 ];
	int lineCount;
	//------------------------------------------------------------------------
	double m[ 1000 ];
	int mCount;
	//------------------------------------------------------------------------
	float xx,yy,deg,h;
	float icpx , icpy , icpdeg;
	Matrix *R, *t;
	float rA,rB,rC,rD,Tx,Ty;
	float Match[ 640 ];
	//------------------------------------------------------------------------
	float VehicalHeadingX,VehicalHeadingY;
	//------------------------------------------------------------------------
	float stepDeg, stepRad;
	//------------------------------------------------------------------------
	bool selected;
	bool angleSelected;

	bool alt;
	float g0, g1, sqrt2pi;
	//------------------------------------------------------------------------
	float getSide( float x0, float y0, float x1, float y1, float x, float y )
	{
		return( x0 * y1 + x1 * y + x * y0 - x1 * y0 - x * y1 - x0 * y );
	}
	//------------------------------------------------------------------------
	void getScan();
	//------------------------------------------------------------------------

	void locate( double *T, int countT );
	//------------------------------------------------------------------------
public:
	double scanner[ 640 ];
	int scannerCount;
	//------------------------------------------------------------------------
	Map( float width, float height, int xOff, int yOff, float scale );
	Map();
	~Map();
	void draw( TCanvas *canvas, int width, int heightw );
	void setPos( float x, float y, float a );
	void getPos( float& x, float& y, float& a );
	int getLineCount();
	void addLine( float x0, float y0, float x1, float y1 );
	bool onMouseDown( int x, int y, TMouseButton Button );
	bool onMouseMove( int x, int y );
	void onMouseUp( int x, int y );
	void drawMatch( TCanvas *canvas, double *T, int countT );
	void drawMatchLocation( TCanvas *canvas, float x, float y, float a );
	void drawScan( TCanvas *canvas, int x, int y );

	// to use update, pass in line scan from Kinect, x, y and a
	// function will update x, y and a to a more accurate value
	void update( double Scan[ 640 ], int ScanCount,
					 float& x, float& y, float& a );

	void updateFromOpticalFlow( float vx, float vy );

	float getGaussian( float mu, float sigma, float& weight );
	float getWeight( float mu, float sigma, float x );
};
//---------------------------------------------------------------------------
#endif
