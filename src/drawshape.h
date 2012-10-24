
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


#include <Wt/WPaintedWidget>
#include <Wt/WPainterPath>
#include <Wt/WEvent>
#include <Wt/WText>
#include <Wt/WPen>
#include <Wt/WColor>
#include <Wt/WBrush>
#include <vector>
#include <string>
#include <Wt/WFont>
#include "ogrsf_frmts.h"



#ifndef DRAWSHAPE_H_
#define DRAWSHAPE_H_

using namespace Wt;
using namespace std;


class DrawShape : public WPaintedWidget
{
	public:
		DrawShape(int width, int height, WContainerWidget *parent = 0);

		WText *text;
		void mouseMove(const WMouseEvent& e);
		string cfile,sfile;
		double iwidth;
		WColor bordercolor;
		WColor fillcolor;
		WColor backcolor;
		WColor labelcolor;
		double x_pos_shift,y_pos_shift,increase_width,increase_height;
		WFont *font;
		bool select;
		void updatePaint(string,string);
		void incrWidth();
		void changeBorderColor(WColor);
		void changeFillColor(WColor);
		void decrWidth();
		void decrLabelWidth();
		void incrLabelWidth();
		int labelindex;
		void setPenWidth(int);
		void paintondevice(WPaintDevice *device);
		void paintMap(WPaintDevice *paintDevice);
		void featuredetect(int,int);
		WPen pen;
		double labelpercentage;
		WBrush brush;
		void reset();
		string svgFile;
		~DrawShape();
		
	protected:
		virtual void paintEvent(WPaintDevice *paintDevice);
		


	private:
		WPainterPath path_;

};

#endif // DRAWSHAPE_H_
