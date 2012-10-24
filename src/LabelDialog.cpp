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
#include <Wt/WComboBox>
#include <Wt/WPushButton>
#include <Wt/WText>
#include<Wt/WDialog>
#include<iostream>
#include<stdio.h>
#include <ogrsf_frmts.h>
#include<Wt/WSvgImage>
#include "LabelDialog.h"
#include "drawshape.h"

LabelDialog::LabelDialog(string clientfile,string spoolfile,int *label,DrawShape *shape): WDialog("Label")
{	

	if(!spoolfile.empty()) {
	labelindex=label;
	lab=*label;
	shape1=shape;
	WGridLayout *layout = new WGridLayout();
	OGRRegisterAll();
	OGRDataSource       *poDS;
	dropdown = new WComboBox();
  	dropdown->addItem("NoLabel");
  

	//text to be commented 
	  cfile = clientfile;
	  sfile = spoolfile;
	  string dfile = sfile  + ".dbf";
	  string dbf = "g_4326/" + sfile + ".dbf";
	  poDS = OGRSFDriverRegistrar::Open(dbf.c_str(), FALSE );

	//poDS = OGRSFDriverRegistrar::Open( "AFG_adm1.dbf", FALSE );
	if( poDS == NULL )
	{
		std::cerr<< "Open failed.\n" ;
		exit( 1 );
	}
	OGRLayer  *poLayer;
	poLayer = poDS->GetLayer(0);
	OGRFeature *poFeature;

	poLayer->ResetReading();
	int row=1;
	while( (poFeature = poLayer->GetNextFeature()) != NULL )
	{
		OGRFeatureDefn *poFDefn = poLayer->GetLayerDefn();
		int iField;
		for( iField = 0; iField < poFDefn->GetFieldCount(); iField++ )
		{
			OGRFieldDefn *poFieldDefn = poFDefn->GetFieldDefn( iField );
			dropdown->addItem(poFieldDefn->GetNameRef());
			dropdown->changed().connect(this, &LabelDialog::labelchanged);
		}
		break;
	}
	dropdown->setCurrentIndex(lab);
	//to close the dialog box
	WPushButton *b= new WPushButton("Ok");
	b->clicked().connect(this, &WDialog::accept);
	contents()->enterPressed().connect(this, &WDialog::accept);

	WPushButton *c= new WPushButton("Cancel");
	c->clicked().connect(this, &WDialog::reject);

	OGRDataSource::DestroyDataSource( poDS );

	//adding widgets
	layout->addWidget(new WText("Label :" ),0, 0,AlignCenter | AlignMiddle);
	layout->addWidget(dropdown,0, 1,AlignCenter | AlignMiddle);
	layout->addWidget(b,1, 0,AlignCenter | AlignMiddle);
	layout->addWidget(c,1, 1,AlignCenter | AlignMiddle);
	contents()->setLayout(layout, AlignTop | AlignJustify);
	finished().connect(this, &LabelDialog::handleFinish);
	//show dialog box	
	show();
	}
}
void LabelDialog::labelchanged()
{
	lab=dropdown->currentIndex();
}
void LabelDialog::handleFinish()
{
	if (result() == WDialog::Accepted && *labelindex!=lab)
	{ *labelindex=lab;	
	  // WSvgImage *SvgImage=new WSvgImage(640, 480);
	 //  shape1->paintTempSVG(SvgImage);
	   shape1->update(); 	
	}
}
