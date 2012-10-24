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

#include <Wt/WApplication>
#include <Wt/WGridLayout>
#include <Wt/WPushButton>
#include <Wt/WText>
#include<Wt/WBorder>
#include<Wt/WDialog>
#include<Wt/WTable>
#include<Wt/WSlider>
#include<Wt/WColor>
#include<iostream>
#include<stdio.h>
#include<Wt/WSvgImage>
#include "ColorPickDialog.h"
#include "drawshape.h"

ColorPickDialog::ColorPickDialog(WColor *c,WPushButton *button, DrawShape *shape): WDialog("COLOR PICK"),but(button)
{
	WGridLayout *layout = new WGridLayout();
	currentcolor_=c;
	currentcolor=*c;
	shape1=shape;
	resize(700, WLength::Auto);

	WPushButton *b;

	b = new WPushButton("Save");
	b->clicked().connect(this, &WDialog::accept);
	contents()->enterPressed().connect(this, &WDialog::accept);

	WPushButton *a;
	a=new WPushButton("Cancel");
	a->clicked().connect(this, &WDialog::reject);

	WTable* colorTable = new WTable();
	colorTable->decorationStyle().setCursor("icons/pencil.cur", CrossCursor);	
	p= new WPushButton("      ");
	p->resize(80,30);
	p->decorationStyle().setBackgroundColor(*currentcolor_);

// making the color swatch
	int x=0,y=25;
	for (int i=0; i<255; i+=30)
		for(int j=0; j<255; j+=60)
			for(int k=0;k<=255;k+=60)
			{
				colorTable->elementAt(x/y,x%y)->resize(25, 15);
				colorTable->elementAt(x/y,x%y)->decorationStyle().setBackgroundColor(* (new WColor(i,j,k,255)));
				colorTable->elementAt(x/y,x%y)->decorationStyle().setBorder(Wt::WBorder(Wt::WBorder::Solid, Wt::WBorder::Thin));
				colorTable->elementAt(x/y,x%y)->setMargin(5,Left|Right);
				colorTable->elementAt(x/y,x%y)->clicked().connect(boost::bind(&ColorPickDialog::PickColor,this,new WColor(i,j,k)));
				x++;
			}
	red_slider = new WSlider(Horizontal);
	red_slider->setMinimum(0);
	red_slider->setMaximum(255);
	red_slider->setTickPosition(WSlider::TicksBothSides);
	red_slider->setValue(currentcolor_->red());
	red_slider->setTickInterval(1);
	red_slider->resize(100,20);

	blue_slider = new WSlider(Horizontal);
	blue_slider->setMinimum(0);
	blue_slider->setMaximum(255);
	blue_slider->setTickPosition(WSlider::TicksBothSides);
	blue_slider->setValue(currentcolor_->blue());
	blue_slider->setTickInterval(1);
	blue_slider->resize(100,20);

	green_slider = new WSlider(Horizontal);
	green_slider->setMinimum(0);
	green_slider->setMaximum(255);
	green_slider->setTickPosition(WSlider::TicksBothSides);
	green_slider->setValue(currentcolor_->green());
	green_slider->setTickInterval(1);
	green_slider->resize(100,20);
	red_slider->sliderMoved().connect(this, &ColorPickDialog::slidermoved);
	green_slider->sliderMoved().connect(this, &ColorPickDialog::slidermoved);
	blue_slider->sliderMoved().connect(this, &ColorPickDialog::slidermoved);

	WContainerWidget *slider=new WContainerWidget();
	slider->setInline(true);
	new WText ("Red   : ",slider);
	slider->addWidget(red_slider);	
	new WBreak(slider);
	new WText ("Green : ",slider);
	slider->addWidget(green_slider);	
	new WBreak(slider);
	new WText ("Blue  : ",slider);
	slider->addWidget(blue_slider);	
	new WBreak(slider);

	layout->setColumnStretch(0,3);
	layout->setRowStretch(0,3);
	layout->addWidget(colorTable,0, 0,AlignCenter | AlignMiddle);;
	layout->addWidget(slider,0, 1);
	layout->addWidget(b,2, 0,AlignRight | AlignMiddle);
	layout->addWidget(a,2,1,AlignCenter | AlignMiddle);
	layout->addWidget(new WText("Chosen Color : "),1,0,AlignRight|AlignMiddle);
	layout->addWidget(p,1,1);
	contents()->setLayout(layout, AlignTop | AlignJustify);
	finished().connect(this, &ColorPickDialog::handleFinish);
	show();
}

void ColorPickDialog::PickColor(WColor *color)
{
	currentcolor=*color;
	red_slider->setValue(currentcolor.red());
	green_slider->setValue(currentcolor.green());
	blue_slider->setValue(currentcolor.blue());
	p->decorationStyle().setBackgroundColor(currentcolor);
}
void ColorPickDialog::slidermoved(int v)
{
	currentcolor.setRgb(red_slider->value(),green_slider->value(),blue_slider->value());
	p->decorationStyle().setBackgroundColor(currentcolor);
}
void ColorPickDialog:: handleFinish()
{
	if (result() == WDialog::Accepted)
	{ 
		*currentcolor_=currentcolor;
		but->decorationStyle().setBackgroundColor(*currentcolor_);
	//	WSvgImage *SvgImage=new WSvgImage(640, 480);
	//	shape1->paintTempSVG(SvgImage);
		shape1->update();	// update the widget
	}
}
