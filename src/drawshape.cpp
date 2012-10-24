/*
Copyright 2010 KS Rajan, Mohammed Rashad KM

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*/


#include <boost/lexical_cast.hpp>
#include <iostream>
#include "drawshape.h"
#include <Wt/WCssDecorationStyle>
#include <Wt/WPainter>
#include<Wt/WPaintDevice>
#include <Wt/WPainterPath>
#include <Wt/WPointF>
#include <Wt/WRectF>
#include <Wt/WFont>
#include <Wt/WEvent>
#include<Wt/WSvgImage>
#include <Wt/WText>
#include <Wt/WApplication>
#include <cmath>
#include <iostream>
#include <fstream>
#include <Wt/WFileUpload>
#include <boost/lexical_cast.hpp>
#include <Wt/WRandom>
#include<string>
#include "attribute_table.h"
#include <vector>

using namespace std;
using namespace Wt;

using namespace boost;

#define MIN_POINT(x,y) (x < y ? x : y)
#define MAX_POINT(x,y) (x > y ? x : y)
#define INSIDE 0
#define OUTSIDE 1

struct LABELS
{
	double x;
	double y;
	string label;
};
void DrawShape::updatePaint(string clientfile,string spoolfile){
	cfile = clientfile;
	sfile = spoolfile;
	
	update();
}

DrawShape::DrawShape(int width, int height, WContainerWidget *parent): WPaintedWidget(parent)
{
	setSelectable(false);
	iwidth = 0.0015;
	x_pos_shift=0;
	y_pos_shift=0;
	increase_width=0;
	increase_height=0;
	labelpercentage=1;
	resize(WLength(width), WLength(height));
	select=false;

}
DrawShape::~DrawShape()
{

//	delete font;
//	delete text;
	if(!svgFile.empty()){
			string delfile="rm "+svgFile;
			int ret=system(delfile.c_str());}
}
void DrawShape::reset()
{
	x_pos_shift=0;
	y_pos_shift=0;
	increase_width=0;
	increase_height=0;
	labelindex=0;
}
void DrawShape::setPenWidth(int w)  {
	double cwidth = pen.width().value();
	//cout << cwidth << "VCCC" << w << "DDD" << endl;
	if(w > cwidth)
		iwidth = iwidth*3;
	else
		iwidth = iwidth/3;
	
	update();
}
void DrawShape::incrWidth() {
	iwidth = iwidth*3;

	update();
}
void DrawShape::decrWidth() {
	iwidth = iwidth/3;

	update();
}

void DrawShape::incrLabelWidth() {
	labelpercentage*=1.2;
	update();
}
void DrawShape::decrLabelWidth() {
	labelpercentage/=1.2;
	update();
}


void DrawShape::changeBorderColor(WColor color) {
	bordercolor= color;
	update();
}

void DrawShape::changeFillColor(WColor color) {
	fillcolor= color;
	update();
}
void DrawShape::paintondevice(WPaintDevice *p) {
	paintEvent(p);
}
void DrawShape::mouseMove(const WMouseEvent& e) {
	//cout << e.screen().x << ":::" << e.screen().y << endl;
	//string coords = boost::lexical_cast<string>(e.document().x) + "::" + boost::lexical_cast<string>(e.document().y);
	//text->setText(coords);
}

void DrawShape::paintEvent(WPaintDevice *paintDevice)
{
	if(!sfile.empty()) {
		paintMap(paintDevice);
	}

}
void DrawShape::paintMap(WPaintDevice *paintDevice)
{
	WPainter painter(paintDevice);
	painter.setRenderHint(WPainter::LowQualityShadows);
	painter.save();
	if(!sfile.empty()) {

		vector<LABELS> label_list;
		OGRDataSource       *poDS,*PointDS;
		string dfile = sfile  + ".shp";
		string shp = "g_4326/" + sfile + ".shp";
		poDS = OGRSFDriverRegistrar::Open(shp.c_str(), FALSE );  //comment till here

		if(poDS==NULL)
		{
			printf( "Open failed.\n" );
			exit( 1 );
		}
		OGRLayer  *poLayer;
		poLayer = poDS->GetLayerByName( sfile.c_str() ); // comment here 
		OGRFeature *poFeature;
		OGREnvelope *   	 psExtent = new OGREnvelope();

		poLayer->GetExtent(psExtent);
		double xMin = psExtent->MinX;
		double yMin = psExtent->MinY;
		double xMax = psExtent->MaxX;
		double yMax = psExtent->MaxY;

		stringstream strm;
		strm << xMin;
		string exp;
		double scaleFactor;
		string bound_string = strm.str();
		int size = bound_string.size();
		size_t found=bound_string.find("+");
		if (found!=string::npos) {
			exp =  bound_string.substr(found+1,size);     
		}

		if(exp.empty()) {

			stringstream strExtent;
			strExtent << yMin;
			bound_string = strExtent.str();
			size = bound_string.size();
			found = bound_string.find("+");
			if(found!=string::npos) {
				exp = bound_string.substr(found+1,size); 
			}
		}
		if(exp.empty()) {
			stringstream strExtent;
			strExtent << xMax;
			bound_string = strExtent.str();
			size = bound_string.size();
			found = bound_string.find("+");
			if(found!=string::npos) {
				exp = bound_string.substr(found+1,size); 
			}
		}
		if(exp.empty()) {
			stringstream strExtent;
			strExtent << yMax;
			bound_string = strExtent.str();
			size = bound_string.size();
			found = bound_string.find("+");
			if(found!=string::npos) {
				exp = bound_string.substr(found+1,size); 
			}
		}
		//cout << "EXXP: " << exp << endl;
		if(!exp.empty()) {
			int exponent  = boost::lexical_cast<int>(exp);

			exponent-=3;
			scaleFactor = pow (10,exponent);
		}
		else
		{
			//cout << "EXXP is empty " << exp << endl;
			scaleFactor = 1;
		}

		xMin/=scaleFactor;
		xMax/=scaleFactor;
		yMin/=scaleFactor;
		yMax/=scaleFactor;

		double gWidth = xMax - xMin;
		double gHeight = yMax - yMin;
		double widthFactor = 1;
		double pwidth = abs(gWidth-gHeight);
		double s = gWidth - gHeight;
		if(s<0.16)
			gWidth = gHeight + 0.16;

		double ratio=gWidth/gHeight;
		//for zoom n pan
		if(increase_width<100 && increase_height<100){
			painter.setWindow(xMin +(-x_pos_shift+increase_width/2)/100*gWidth* widthFactor, yMax+(y_pos_shift-increase_height/2)/100*gHeight * widthFactor, gWidth* widthFactor*(100-increase_width)/100, gHeight * widthFactor*(-1+increase_height/100));

			brush.setStyle(SolidPattern);
			brush.setColor(backcolor);
			painter.setBrush(brush);
			painter.drawRect(xMin +(-x_pos_shift+increase_width/2)/100*gWidth* widthFactor, yMax+(y_pos_shift-increase_height/2)/100*gHeight * widthFactor, gWidth* widthFactor*(100-increase_width)/100, gHeight * widthFactor*(-1+increase_height/100));
}
		else{
			painter.setWindow(xMin +x_pos_shift, yMax-y_pos_shift, 0, 0);
}

		// for normal picture

		//painter.setWindow(xMin , yMax, gWidth* widthFactor, -gHeight * widthFactor);
		pwidth/=480;
		pwidth*=4;
		//if(iwidth<0.06)
		//		iwidth=0.06;
		if(iwidth == 0.0015){
			pen.setWidth(pwidth);
		}
		else
		{
			pen.setWidth(iwidth);
		}

		//std::cerr<<pwidth<<" "<<iwidth<<"\n";
		pen.setColor(bordercolor);
		brush.setStyle(SolidPattern);
		brush.setColor(fillcolor);
		font= new WFont();
		font->setSize(WLength(labelpercentage*gWidth*widthFactor));
		painter.setFont(*font);
		painter.setPen(pen);
		painter.setBrush(brush);
		WPainterPath path;
		poLayer->ResetReading();


		OGRPoint *centroid = new OGRPoint();
		char label[100];
		while( (poFeature = poLayer->GetNextFeature()) != NULL )
		{
			centroid->empty();
			label[0]=0;
			if(labelindex>0)
			{
				OGRFeatureDefn *PointFDefn = poLayer->GetLayerDefn();
				OGRFieldDefn *PointFieldDefn = PointFDefn->GetFieldDefn(labelindex-1);
				if( PointFieldDefn->GetType() == OFTInteger )
					sprintf(label, "%d", poFeature->GetFieldAsInteger(labelindex-1) );
				else if( PointFieldDefn->GetType() == OFTReal )
					sprintf(label, "%.3f", poFeature->GetFieldAsDouble(labelindex-1) );
				else if( PointFieldDefn->GetType() == OFTString )
					sprintf(label, "%s", poFeature->GetFieldAsString(labelindex-1) );
				else
					sprintf(label, "%s", poFeature->GetFieldAsString(labelindex-1) );
			}
			OGRGeometry *poGeometry;

			poGeometry = poFeature->GetGeometryRef();

			if( poGeometry != NULL && wkbFlatten(poGeometry->getGeometryType()) == wkbPoint )
			{
				OGRPoint *poPoint = (OGRPoint *) poGeometry;
				double x = poPoint->getX();
				double y = poPoint->getY();
				//painter.drawPoint(x/scaleFactor,y/scaleFactor);
				painter.drawEllipse(x/scaleFactor-0.005*gWidth*widthFactor,y/scaleFactor-0.005*gWidth*widthFactor,0.01*gWidth*widthFactor,0.01*gWidth*widthFactor);
				poGeometry->Centroid(centroid);

			} //end wkbpoint

			else if( poGeometry != NULL  && wkbFlatten(poGeometry->getGeometryType()) == wkbLineString  )
			{
				OGRLineString *poPoint = (OGRLineString *) poGeometry;

				for(int i=0;i<poPoint->getNumPoints();i++)
				{
					double x=poPoint->getX(i) ;
					double y = poPoint->getY(i);
					x/=scaleFactor; y/=scaleFactor;

					if(i==0)
						path = WPainterPath( WPointF(x, y));
					else
						path.lineTo( x , y);
				}

				painter.drawPath(path); 
				poGeometry->Centroid(centroid); 
			}

			else if( (poGeometry != NULL)  &&  wkbFlatten(poGeometry->getGeometryType()) == wkbPolygon) 
			{
				OGRPolygon *poPoint = (OGRPolygon *) poGeometry;
				OGRLinearRing *extring = poPoint->getExteriorRing();

				int n = extring->getNumPoints();
				double x, y;
				for(int i=0;i<n;i++)
				{
					x = extring->getX(i);   y = extring->getY(i);
					x/=scaleFactor; y/=scaleFactor;
					if(i==0)
						path = WPainterPath( WPointF(x , y));

					else
						path.lineTo( x , y);
				}
				painter.drawPath(path);
				poGeometry->Centroid(centroid);

			}
			else if( (poGeometry != NULL)  &&  wkbFlatten(poGeometry->getGeometryType()) == wkbMultiPolygon) 
			{
				double x, y;
				OGRMultiPolygon *poPoint = (OGRMultiPolygon *) poGeometry;
				int p = poPoint->getNumGeometries();
				for(int k=0;k<p;k++) {
					OGRGeometry* geom = poPoint->getGeometryRef(k);
					OGRPolygon *poly = (OGRPolygon *) geom;
					OGRLinearRing  *ring = poly->getExteriorRing();

					for(int i=0;i<ring->getNumPoints();i++)
					{
						x = ring->getX(i);   y = ring->getY(i);
						x/=scaleFactor; y/=scaleFactor;
						if(i==0)
							path = WPainterPath( WPointF(x , y));
						else
							path.lineTo( x , y);

					}
					painter.drawPath(path); 
					poGeometry->Centroid(centroid);


				}

			}
			if(labelindex>0 && !centroid->IsEmpty()){
				LABELS l={centroid->getX(),centroid->getY(),label};
				label_list.push_back(l);
			}
		}
		painter.restore(); 


		//labelling the contents
if(increase_width<100 && increase_height<100){
		painter.setWindow(0.0, 0.0, (paintDevice->width()).value(),(paintDevice->height()).value());
		font= new WFont(WFont::SansSerif);
		font->setSize(WLength(10*labelpercentage));
		painter.setFont(*font);
		pen.setColor(labelcolor);
		painter.setPen(pen);
		std::vector<LABELS>::iterator the_iterator = label_list.begin();
		double x, y, minx=(xMin+(-x_pos_shift+increase_width/2)/100*gWidth* widthFactor),miny=(yMax+(y_pos_shift-increase_height/2)/100*gHeight*widthFactor);
		
		double multx=(paintDevice->width().value())/(gWidth* widthFactor*(100-increase_width)/100);
		double multy=(paintDevice->height().value())/(gHeight*widthFactor*(-1+increase_height/100));

		while( the_iterator != label_list.end() ) {
		
		x=((*the_iterator).x/scaleFactor-minx)*multx;
		y=((*the_iterator).y/scaleFactor-miny)*multy;
	
		painter.drawText(WRectF( x-(*the_iterator).label.size()*5*labelpercentage, y-5*labelpercentage, (*the_iterator).label.size()	*10*labelpercentage,10*labelpercentage),AlignCenter,(*the_iterator).label);
			++the_iterator;
		}
		pen.setColor(red);
		painter.setPen(pen);
		painter.setFont(*font);
		//painter.drawText(WRectF(paintDevice->width().value()-dfile.size()*10*labelpercentage,paintDevice->height().value()-10*labelpercentage*(paintDevice->height()).value(), dfile.size()*10*labelpercentage,10*labelpercentage ),AlignCenter,dfile); //this text is not seen in the picture when painted.
}
	}					



}

void DrawShape::featuredetect(int XPoint,int YPoint)
{
	if(!sfile.empty() && XPoint>=(x_pos_shift-increase_width/2)*6.40 && XPoint<=(x_pos_shift-increase_width/2)*6.40+(100+increase_width)*6.4 && YPoint>=(y_pos_shift-increase_height/2)*4.8 && YPoint<=(y_pos_shift-increase_height/2)*4.8+(100+increase_height)*4.8 )//check if point is inside the view area 
	{


		OGRDataSource       *poDS;
		string dfile = sfile  + ".shp";
		string shp = "g_4326/" + sfile + ".shp";
		poDS = OGRSFDriverRegistrar::Open(shp.c_str(), FALSE );  //comment till here
		if( poDS == NULL )
		{
			WApplication::instance()->doJavaScript("alert('Open failed')");
			cout << "Open failed";
		}
		OGRLayer  *poLayer;
		poLayer = poDS->GetLayerByName( sfile.c_str() ); // comment here 
		OGRFeature *poFeature;
		OGREnvelope *   	 psExtent = new OGREnvelope();
		poLayer->GetExtent(psExtent);

		double xMin = psExtent->MinX;
		double yMin = psExtent->MinY;
		double xMax = psExtent->MaxX;
		double yMax = psExtent->MaxY;

		stringstream strm;
		strm << xMin;
		string exp;
		double scaleFactor;
		string bound_string = strm.str();
		int size = bound_string.size();
		size_t found=bound_string.find("+");
		if (found!=string::npos) {
			exp =  bound_string.substr(found+1,size);     
		}

		if(exp.empty()) {

			stringstream strExtent;
			strExtent << yMin;
			bound_string = strExtent.str();
			size = bound_string.size();
			found = bound_string.find("+");
			if(found!=string::npos) {
				exp = bound_string.substr(found+1,size); 
			}
		}
		if(exp.empty()) {
			stringstream strExtent;
			strExtent << xMax;
			bound_string = strExtent.str();
			size = bound_string.size();
			found = bound_string.find("+");
			if(found!=string::npos) {
				exp = bound_string.substr(found+1,size); 
			}
		}
		if(exp.empty()) {
			stringstream strExtent;
			strExtent << yMax;
			bound_string = strExtent.str();
			size = bound_string.size();
			found = bound_string.find("+");
			if(found!=string::npos) {
				exp = bound_string.substr(found+1,size); 
			}
		}
		if(!exp.empty()) {
			int exponent  = boost::lexical_cast<int>(exp);

			exponent-=3;
			scaleFactor = pow (10,exponent);
		}
		else
		{
			scaleFactor = 1;
		}

		xMin/=scaleFactor;
		xMax/=scaleFactor;
		yMin/=scaleFactor;
		yMax/=scaleFactor;

		double gWidth = xMax - xMin;
		double gHeight = yMax - yMin;
		double widthFactor = 1;
		double pwidth = abs(gWidth-gHeight);
		double s = gWidth - gHeight;
		if(s<0.16)
			gWidth = gHeight + 0.16;

		double scaledX=(XPoint*(gWidth* widthFactor*(100-increase_width)/100)/640 + (xMin+(-x_pos_shift+increase_width/2)/100*gWidth* widthFactor))*scaleFactor;
		double scaledY=(YPoint*(gHeight*widthFactor*(-1+increase_height/100))/480 + (yMax+(y_pos_shift-increase_height/2)/100*gHeight*widthFactor))*scaleFactor;
		OGRPoint *poPoint =new OGRPoint();
		poPoint->setX(scaledX);
		poPoint->setY(scaledY);


		poLayer->ResetReading();

		int index=0;bool flagFeature=false;

		while( (poFeature = poLayer->GetNextFeature()) != NULL )
		{

			OGRGeometry *poGeometry;
			poGeometry = poFeature->GetGeometryRef();
			if(poGeometry->Distance(poPoint)/scaleFactor<=0.01*gWidth*widthFactor)
				{
					flagFeature=true;
					//std::cerr<<index<<" " <<flagFeature<<" "<<poGeometry->Distance(poPoint);
					break;
				}
			index++;
		}
		if(flagFeature)
			DBFDialog *attrtable =new DBFDialog(cfile,sfile,flagFeature,index);

			
	}
}


