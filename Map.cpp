//---------------------------------------------------------------------------
#pragma hdrstop
#include <math.h>
#include <stdio.h>
#include "UnitMain.h"
#include "Map.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
Map::Map( float width, float height, int xOff, int yOff, float scale )
{
	this->scale = scale;
	this->xOff = xOff;
	this->yOff = yOff;
	for( int i = 0; i < scannerCount; i++ ) scanner[ i ] = 0;
 //---------------------------------------------------------------------------
	scannerCount = 320;
	lineCount = 0;
	mCount = 0;
	pointGap = 0.1;
	stepDeg = 58.0 / ( float )scannerCount;
	stepRad = stepDeg * M_PI / 180.0;
	sqrt2pi = sqrt( 2.0 * M_PI );
 //---------------------------------------------------------------------------
	// outer walls
	addLine( 0.0, 0.0, 14.0, 0.0 );
	addLine( 14.0, 14.0, 12.0, 14.0 );
	addLine( 10.5, 14.0, 3.5, 14.0 );
	addLine( 2.0, 14.0, 0.0, 14.0 );
	//testing
//	addLine( 15.0, 0.0, 15.0, 14.0 );
//	addLine( -1.0, -0.0, 0.0, 14.0 );
//	addLine( -1.0, 18.0, 15.0, 18.0 );

	//inner walls
	addLine( 7.0, 14.0, 7.0, 8.0 );
	addLine( 7.0, 6.0, 7.0, 0.0 );

	//door
	addLine( 7.0, 8.0,5.59,6.59 );
 //---------------------------------------------------------------------------
	icp = new IcpPointToPoint( m, mCount, 2 );
	R = new Matrix( 2, 2 );
	t = new Matrix( 2, 1 );
 //---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
Map::Map()
{

}
//---------------------------------------------------------------------------
Map::~Map()
{
	delete icp;
	delete R;
	delete t;
}
//---------------------------------------------------------------------------
void Map::draw( TCanvas *canvas, int width, int height )
{
	canvas->Pen->Width = 1.0 ;
	canvas->Pen->Color = clBlack;
	canvas->Brush->Color = clBlack;
	canvas->Rectangle( 0.0 , 0.0 , width  , height);
	//area A------------------------------------------------------------------
	canvas->Pen->Color = clWhite;
	canvas->Brush->Color = clMoneyGreen;
	canvas->Font->Color = clBlack;

	canvas->Rectangle( 0.0 * scale + xOff , 14.0 * scale + yOff ,
							 14.0 * scale + xOff , 17.8 * scale + yOff );
	canvas->Font->Size = 7.0 ;
	canvas->TextOut( 5.9 * scale + xOff, 14.25 * scale + yOff, "area A" );
	//area B------------------------------------------------------------------
	canvas->Brush->Color = clSkyBlue;
	canvas->Rectangle( 0.0 * scale + xOff , 0.0 * scale + yOff ,
							 7.0 * scale + xOff , 14.0 * scale + yOff);
	canvas->TextOut( 4.9 * scale + xOff , 13.0 * scale + yOff, "area B" );
	//area C------------------------------------------------------------------
	canvas->Brush->Color = clYellow;
	canvas->Rectangle( 7.0 * scale + xOff , 0.0 * scale + yOff ,
							 14.0 * scale + xOff , 14.0 * scale + yOff );
	canvas->TextOut( 7.25 * scale + xOff , 13.0 * scale + yOff, "area C" );

	// walls------------------------------------------------------------------
	canvas->Pen->Color = clBlack;
	canvas->Pen->Width = 3;
	for( int i = 0; i < lineCount; i++ ) {
		for( int i = 0; i < lineCount; i++ ) {
		canvas->MoveTo( linex[ 2 * i ] * scale + xOff, liney[ 2 * i ] * scale + yOff );
		canvas->LineTo( linex[ 2 * i + 1 ] * scale + xOff, liney[ 2 * i + 1 ] * scale + xOff );
	}
	}
	//ball drop --------------------------------------------------------------
	canvas->Pen->Width = 1;
	canvas->Pen->Color = clBlack;

	canvas->Brush->Color = clBlue;
	canvas->Ellipse( 2.0 * scale + xOff , 2.0 * scale + yOff ,
									 4.0 * scale + xOff , 4.0 * scale + yOff);
	canvas->Brush->Color = clRed;
	canvas->Ellipse( 2.25 * scale + xOff , 2.25 * scale  + yOff ,
									 3.75 * scale + xOff  , 3.75 * scale  + yOff );
	canvas->Brush->Color = clYellow;
	canvas->Ellipse( 2.5 * scale  + xOff , 2.5 * scale + yOff ,
									 3.5 * scale + xOff , 3.5 * scale + yOff);
  //Take Off Pad---------------------------------------------------------------
	canvas->Pen->Width = 2;
	canvas->Pen->Color = clBlack ;
	canvas->Brush->Color = clBlue;

	canvas->Rectangle( 2.25 * scale  + xOff , 16.8 * scale  + yOff ,
										3.25 * scale  + xOff , 17.8 * scale + yOff );
	canvas->Font->Size = 7.0 ;
	canvas->Font->Color = clWhite;
	canvas->TextOut( 2.45 * scale + xOff , 16.95 * scale + yOff , "H" );
  //Landing Pad---------------------------------------------------------------
	canvas->Pen->Width = 2;
	canvas->Pen->Color = clBlack ;
	canvas->Brush->Color = clYellow;

	canvas->Rectangle( 6.5 * scale + xOff , 16.8 * scale + yOff ,
								7.5 * scale + xOff , 17.8 * scale + yOff );
	canvas->Font->Color = clBlack;
	canvas->TextOut( 6.75 * scale + xOff , 16.95 * scale + yOff , "H" );
	//Radar---------------------------------------------------------------------
	getScan();
	float aRad;
	float xs, ys, xe, ye;
	int filterCount = 0;

	aRad = ( 241.0 + deg ) * M_PI / 180.0;

	canvas->MoveTo( xx * scale + xOff , yy * scale + yOff );
	for( int i = 0; i < scannerCount; i++ ) {

		float x = xx + scanner[ i ] * cos( aRad );
		float y = yy + scanner[ i ] * sin( aRad );

		canvas->Pen->Width = 2;
		canvas->Pen->Color = clRed;
		canvas->LineTo( x * scale + xOff, y * scale + yOff );

		aRad += stepRad;
	}
	canvas->LineTo( xx * scale + xOff, yy * scale + yOff );

	//Vehical Heading ---------------------------------------------------------
	VehicalHeadingX = 1.5 * cos( ( deg + 270.0 ) / 180.0 * M_PI )+ xx;
	VehicalHeadingY = 1.5 * sin( ( deg + 270.0 ) / 180.0 * M_PI )+ yy;

	canvas->Pen->Width= 3;
	canvas->Pen->Color = clLime;
	canvas->MoveTo( xx * scale + xOff, yy * scale + yOff );
	canvas->LineTo( VehicalHeadingX * scale + xOff , VehicalHeadingY * scale + yOff );

	//Vehical Position --------------------------------------------------------
	canvas->Pen->Width = 2;
	canvas->Pen->Color = clBlack;
	canvas->Brush->Color = clMenu;

	int s = 0.7 * scale / 2.0 ;
	canvas ->Ellipse( xx * scale - s + xOff, yy * scale - s + yOff,
							xx * scale + s + xOff, yy * scale + s + yOff);
}
//---------------------------------------------------------------------------
void Map::setPos( float x, float y, float a )
{
	xx = x;
	yy = y;
	deg = a;
}
//---------------------------------------------------------------------------
void Map::getPos( float& x, float& y, float& a )
{
	x = xx;
	y = yy;
	a = deg;
}
//---------------------------------------------------------------------------
int Map::getLineCount()
{
	return( lineCount );
}
//---------------------------------------------------------------------------
void Map::addLine( float x0, float y0, float x1, float y1 )
{
	linex[ 2 * lineCount ] = x0;
	liney[ 2 * lineCount ] = y0;
	linex[ 2 * lineCount + 1 ] = x1;
	liney[ 2 * lineCount + 1 ] = y1;
	lineCount++;
	//get distance
	float dx = x1 - x0;
	float dy = y1 - y0;
	float d = sqrt( dx * dx + dy * dy );
	//calculate number of ellipses to draw
	int numPoints = d / pointGap;
	dx /= numPoints;
	dy /= numPoints;
	//points for ellipses to be drawn
	float px = x0;
	float py = y0;
	// store all points into an array
	for( int i = 0; i <= numPoints; i++ ) {
		m[ 2 * mCount ] = px;
		m[ 2 * mCount + 1 ] = py;
		px += dx;
		py += dy;
		mCount++;
	}
}
//---------------------------------------------------------------------------
bool Map::onMouseDown( int x, int y, TMouseButton button )
{
	//scale down coordinates
	float xR = x / scale;
	float yR = y / scale;

	if( button == mbLeft ) {
		//compare difference in coordinates
		float dx = xR - xx;
		float dy = yR - yy;
		// determine if click is within range
		float d = sqrt( dx * dx + dy * dy );
		if( d < 2.0 ) {
			selected = true;
		}
	}
	if( button == mbRight){
		float dx = xR - xx;
		float dy = yR - yy;
		float d = sqrt( dx * dx + dy * dy );
		if( d < 2.0 ) {
			angleSelected = true;
		}
	}
	return( selected || angleSelected );
}
//---------------------------------------------------------------------------
bool Map::onMouseMove( int x, int y )
{
	if( selected ) {
		xx = x / scale;
		yy = y / scale;
		if( xx < 0.35 ) xx = 0.35;
		if( xx > 13.65 ) xx = 13.65;
		if( yy < 0.35 ) yy = 0.35;
		if( yy > 17.45 ) yy = 17.45;
	}
	if( angleSelected ) {
		float xR = x / scale;
		float yR = y / scale;
		float dx = xR - xx;
		float dy = yR - yy;
		float aa = 0.0;
		if( ( dx != 0.0 ) || ( dy != 0.0 ) ) {
			aa = atan2( dy, dx ) * 180.0 / M_PI + 90;
			if( aa > 360.0 ) aa -= 360.0;
			if( aa < 0.0 ) aa += 360.0;
		}
		deg = aa;
	}
	return( selected || angleSelected );
}
//---------------------------------------------------------------------------
void Map::onMouseUp( int x, int y )
{
	selected = false;
	angleSelected = false;
}
//---------------------------------------------------------------------------
void Map::getScan()
{
	float s0, s1, s2, s3;
	float dMin;
	float eX, eY;
	float range, rangeSqr;

	range = 8.0;
	rangeSqr = range * range;

	for( int i = 0; i < scannerCount; i++ ) scanner[ i ] = 0;

	for( int aa = 0; aa <= ( scannerCount - 1 ); aa++ ) {
		float ang = ( float )aa * stepDeg + 241.0 ;
		float aRad = ( float )( ang + deg ) * M_PI / 180.0;
		float vX = range * cos( aRad );
		float vY = range * sin( aRad );
		float px = xx + vX;
		float py = yy + vY;
		eX = px;
		eY = py;
		dMin = rangeSqr;
		for( int i = 0; i < lineCount; i++ ) {
			s0 = getSide( xx, yy, px, py, linex[ 2 * i ], liney[ 2 * i ] );
			s1 = getSide( xx, yy, px, py, linex[ 2 * i + 1 ], liney[ 2 * i + 1 ] );
			s2 = getSide( linex[ 2 * i ], liney[ 2 * i ], linex[ 2 * i + 1], liney[ 2 * i + 1 ], px, py );
			s3 = getSide( linex[ 2 * i ], liney[ 2 * i ], linex[ 2 * i + 1], liney[ 2 * i + 1 ], xx, yy );
			if( ( s0 * s1 <= 0.0 ) && ( s2 * s3 <= 0.0 ) ) {
				float tefX = linex[ 2 * i + 1] - linex[ 2 * i ];
				float tefY = liney[ 2 * i + 1] - liney[ 2 * i ];
				float dh = ( vY * tefX - vX * tefY );
				float dk = ( vY * ( linex[ 2 * i + 1 ] - linex[ 2 * i ] ) - vX * ( liney[ 2 * i +1 ] - liney[ 2 * i ] ) );
				if( dh != 0.0 ) {
					float h = ( ( liney[ 2 * i ] - yy ) * tefX - ( linex[2 * i ] - xx ) * tefY ) / dh;
					eX = xx + h * vX;
					eY = yy + h * vY;
				} else if( dk != 0.0 ) {
					float k = ( vY * ( xx - linex[ 2 * i ] ) - vX * ( yy - liney[ 2 * i ] ) ) / dk;
					eX = linex[ 2 * i ] + k * tefX;
					eY = liney[ 2 * i ] + k * tefY;
				}
				float dx = eX - xx;
				float dy = eY - yy;
				float d = dx * dx + dy * dy;
				if( d < dMin ) {
					dMin = d ;
				}
			}
		}
		scanner[ aa ] = sqrt( dMin );
	}
}
//---------------------------------------------------------------------------
void Map::locate( double *T, int countT )
{
}
//---------------------------------------------------------------------------
void Map::drawMatch( TCanvas *canvas, double *T, int countT )
{
	canvas->Pen->Color = clLime;
	canvas->Brush->Color = clLime;

	// drawing the matched points --------------------------------------------
	 for( int i = 0; i < countT; i++ ) {
		Match[ i * 2 ] = R->val[ 0 ][ 0 ] * T[ i * 2 ] +
							  R->val[ 0 ][ 1 ] * T[ i * 2 + 1 ] + t->val[ 0 ][ 0 ];
		Match[ i * 2 + 1 ] = R->val[ 1 ][ 0 ] * T[ i * 2 ] +
									R->val[ 1 ][ 1 ] * T[ i * 2 + 1 ] + t->val[ 1 ][ 0 ];
		Match[ i * 2 ] *= scale ;
		Match[ i * 2 + 1 ] *= scale ;
	}

	for (int i = 0; i < countT; i++) {

	 canvas->Ellipse(	Match[ i * 2]- 1.5 + xOff ,Match[ i * 2 + 1] - 1.5 + yOff,
							Match[ i * 2] + 1.5 + xOff, Match[ i * 2 + 1] + 1.5 + yOff );
	}
}
//---------------------------------------------------------------------------
void Map::drawMatchLocation( TCanvas *canvas, float x, float y, float a )
{
 // icp Vehical Position*********************************************************

	VehicalHeadingX =  1.5 * cos( ( a + 270.0 ) / 180.0 * M_PI ) + x ;
	VehicalHeadingY =  1.5 * sin( ( a + 270.0 ) / 180.0 * M_PI ) + y ;

	canvas->Pen->Color = clRed;
	canvas->Pen->Width = 3;
	canvas->MoveTo( x * scale + xOff, y * scale + yOff );
	canvas->LineTo( VehicalHeadingX * scale + xOff, VehicalHeadingY * scale + yOff );

	canvas->Pen->Width = 2;
	canvas->Pen->Color = clGray;
	canvas->Brush->Color = clMenu;

	int s = 0.7 * scale / 2.0;
	canvas ->Ellipse( x * scale - s + xOff, y * scale - s + yOff,
							x * scale + s + xOff, y * scale + s + yOff );

}
//---------------------------------------------------------------------------
void Map::drawScan( TCanvas *canvas, int width, int height )
{
	getScan();
	float aRad;
	float xs, ys, xe, ye;
	int filterCount = 0;
	this->canvas = canvas;
	aRad = ( 241.0 + deg ) * M_PI / 180.0;
	canvas->MoveTo( xx * scale, yy * scale );
	for( int i = 0; i < scannerCount; i++ ) {

		float x = xx + scanner[ i ] * cos( aRad );
		float y = yy + scanner[ i ] * sin( aRad );

		canvas->Pen->Width = 2;
		canvas->Pen->Color = clRed;
		canvas->LineTo( x * scale, y * scale );

		aRad += stepRad;

	}


}
//---------------------------------------------------------------------------
void Map::update( double Scan[ 640 ], int ScanCount, float& x, float& y, float& a )
{
	 int error = 1;
	 int Count = 0;
	 float da = 0.0;
	 float SumOfAngles = 0.0;
	 float Ra = 0.0;
	 float rA_limiter = 10.0;

	 if( ScanCount > 50 ) {

		t->val[ 0 ][ 0 ] = x;
		t->val[ 1 ][ 0 ] = y;

		float aRad = ( a + 0.0 ) / 180.0 * M_PI;
		R->val[ 0 ][ 0 ] = cos( aRad );
		R->val[ 0 ][ 1 ] = -sin( aRad );
		R->val[ 1 ][ 0 ] = sin( aRad );
		R->val[ 1 ][ 1 ] = cos( aRad );

		icp->fit( Scan, ScanCount, *R, *t, -1 );

// icp deg = reflection angle + vechical heading ------------------------------
		icpdeg = 0.0;
		if( R->val[ 0 ][ 0 ] * R->val[ 1 ][ 1 ] -
			R->val[ 1 ][ 0 ] * R->val[ 0 ][ 1 ] > 0.0 )  {

			if( ( R->val[ 0 ][ 0 ] != 0.0 ) || ( R->val[ 1 ][ 0 ] != 0.0 ) ) {

				icpdeg =  atan2( R->val[ 1 ][ 0 ], R->val[ 0 ][ 0 ]  ) / M_PI * 180.0;
				if( icpdeg >= 360.0 ) icpdeg -= 360.0;
				if( icpdeg < 0.0 ) 	icpdeg += 360.0;
			}
		}
		if( R->val[ 0 ][ 0 ] * R->val[ 1 ][ 1 ] -
			R->val[ 1 ][ 0 ] * R->val[ 0 ][ 1 ] < 0.0 )  {
			//FormMain->Memo1->Lines->Add( "reflection" );
			if( ( R->val[ 0 ][ 0 ] != 0.0 ) || ( R->val[ 1 ][ 0 ] != 0.0 ) ) {

				SumOfAngles =  atan2( R->val[ 1 ][ 0 ], R->val[ 0 ][ 0 ]  ) / M_PI * 180.0;
				if( SumOfAngles >= 360.0 ) 	SumOfAngles -= 360.0;
				if( SumOfAngles < 0.0 )   	SumOfAngles += 360.0;
			}

			while( error != 0 ) {

				if (SumOfAngles > ( 2.0 * Ra ) ) {
					//FormMain->Memo1->Lines->Add( "CW" );
					icpdeg = SumOfAngles - 2.0 * Ra ;
					if( icpdeg >= 360.0 ) icpdeg -= 360.0;
					if( icpdeg < 0.0 ) 	icpdeg += 360.0;

					if( a >= 360.0 ) a -= 360.0;
					if( a < 0.0 ) 	a += 360.0;

					da = a - icpdeg ;
					//FormMain->Memo1->Lines->Add( a );
					//FormMain->Memo1->Lines->Add( icpdeg );
					if( da >= 180.0 ) da -= 360.0;
					if( da < -180.0 ) da += 360.0;
					if( fabs( da ) < rA_limiter ) {
						error = 0;
						Count = 0;
						break;
					}else {
						error = 1;
					}

				}
				if (SumOfAngles < ( 2.0 * Ra ) ) {
					//FormMain->Memo1->Lines->Add( "AntiCW" );
					icpdeg = 2.0 * Ra - SumOfAngles ;
					if( icpdeg >= 360.0 ) icpdeg -= 360.0;
					if( icpdeg < 0.0 ) 	icpdeg += 360.0;
					da = a - icpdeg ;
					if( da >= 180.0 ) da -= 360.0;
					if( da < -180.0 ) da += 360.0;
					//FormMain->Memo1->Lines->Add( Count );
					//FormMain->Memo1->Lines->Add( icpdeg );

					if( fabs( da ) < rA_limiter ) {

						error = 0;
						Count = 0;
						break;
					}else {
					error = 1;
					}
				}

				Ra = Ra + 45.0;
				if( Ra >= 360.0 ) Ra -= 360.0;
				if( Ra < 0.0 ) 	Ra += 360.0;
				//	FormMain->Memo1->Lines->Add( Count );
				Count++;
				if ( Count > 9 ) {
					error = 0;
					icpdeg = a;
					//FormMain->Memo1->Lines->Add( "escaped" );
				}
			}
		}

		da = 0.0;
		da = a - icpdeg ;
		if( da >= 180.0 ) da -= 360.0;
		if( da < -180.0 ) da += 360.0;
		if( fabs( da ) > 20.0 ) icpdeg = a;

		// as long as heading is correct the postion will be correct
		icpx = t->val[ 0 ][ 0 ] +
			Scan[ 0 ] * ( R->val[ 0 ][ 0 ] - cos ( icpdeg / 180.0 * M_PI ) ) +
			Scan[ 1 ] * ( R->val[ 0 ][ 1 ] + sin ( icpdeg / 180.0 * M_PI ) );

		icpy = t->val[ 1 ][ 0 ] +
			Scan[ 0 ] * ( R->val[ 1 ][ 0 ] - sin ( icpdeg / 180.0 * M_PI ) )+
			Scan[ 1 ] * ( R->val[ 1 ][ 1 ] - cos ( icpdeg / 180.0 * M_PI ) );

		x = icpx;
		y = icpy;
		a = icpdeg;
	} else {
		// if scanCount is less than 50
		x = x;
		y = y;
		a = a;
	}


/*
	// print values for debugging---------------------------------------------
	 if (selected == true || angleSelected == true) {

		char buffer[ 200 ];
		sprintf( buffer, "xx = %5.2f	yy = %5.2f", xx,yy );
		FormMain->Memo1->Lines->Add( buffer );
		sprintf( buffer, "icpx = %5.2f	icpy = %5.2f", icpx,icpy );
		FormMain->Memo1->Lines->Add( buffer );
		sprintf( buffer, "deg = %5.2f	icpdeg = %5.2f", deg,icpdeg );
		FormMain->Memo1->Lines->Add( buffer );
		sprintf( buffer, "da = %5.2f ", da );
		FormMain->Memo1->Lines->Add( buffer );
		FormMain->Memo1->Lines->Add( "R:" );
		sprintf( buffer, "%5.2f %5.2f", R->val[ 0 ][ 0 ], R->val[ 0 ][ 1 ] );
		FormMain->Memo1->Lines->Add( buffer );
		sprintf( buffer, "%5.2f %5.2f", R->val[ 1 ][ 0 ], R->val[ 1 ][ 1 ] );
		FormMain->Memo1->Lines->Add( buffer );
	}
*/
}
 //---------------------------------------------------------------------------
float Map::getGaussian( float mu, float sigma, float& weight )
{

	float u, v, w;

	if( alt ) {
		alt = false;
		weight = getWeight( mu, sigma, mu + g1 * sigma );
		return( mu + g1 * sigma );
	} else {
		do {
			u = 2.0 * ( float )rand() / ( float )RAND_MAX - 1.0;
			v = 2.0 * ( float )rand() / ( float )RAND_MAX - 1.0;
			w = u * u + v * v;
		} while( w >= 1.0 );
		w = sqrt( ( -2.0 * log( w ) ) / w );
		g0 = u * w;
		g1 = v * w;
		alt = true;
		weight = getWeight( mu, sigma, mu + g0 * sigma );
		return( mu + g0 * sigma );
	}
}
//---------------------------------------------------------------------------
float Map::getWeight( float mu, float sigma, float x )
{
	float d = x - mu;
	return( exp( -( d * d ) / 2.0 / sigma / sigma ) / sqrt2pi / sigma );

}
//---------------------------------------------------------------------------
void Map::updateFromOpticalFlow( float vx, float vy )
{
	float mag, arg;
	float aRad;
	float dx, dy;

	mag = sqrt( ( vx * vx ) + ( vy * vy ) );
	arg = 0.0;
	if( ( vx != 0.0 ) || ( vy != 0.0 ) ) {
		arg = atan2( vy, vx ) * Globals::rad2deg;
	}
	aRad = ( deg + arg ) * Globals::deg2rad;
	dx = mag * cos( aRad );
	dy = mag * sin( aRad );
	xx += ( dx * Globals::T );
	yy += ( dy * Globals::T );
//   if( x < 0.0 ) x = 0.0;
//   if( x > width ) x = width;
//   if( y < 0.0 ) y = 0.0;
//   if( y > height ) y = height;
}
//---------------------------------------------------------------------------
