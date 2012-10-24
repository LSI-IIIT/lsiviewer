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


//scrollable dialog box to read the .dbf file .. presently use the hardcoded file name.. some problem with permissions
#include <Wt/WApplication>
#include <Wt/WGridLayout>
#include <Wt/WPushButton>
#include <Wt/WText>
#include<Wt/WScrollArea>
#include<Wt/WBorder>
#include<Wt/WDialog>
#include<Wt/WTable>
#include<Wt/WSlider>
#include<Wt/WColor>
#include<iostream>
#include<stdio.h>
#include <ogrsf_frmts.h>
#include "attribute_table.h"

DBFDialog::DBFDialog(string clientfile,string spoolfile,bool flagFeature,int index): WDialog("Data")
{	

	cout<<index << flagFeature;
	if(!spoolfile.empty()) {

		WScrollArea *cont= new WScrollArea();
		WTable* DataTable = new WTable();		
		WGridLayout *layout = new WGridLayout();
		OGRRegisterAll();
		OGRDataSource       *poDS;
		cont->resize(700, 300);
		cont->setWidget(DataTable);

		//text to be commented 
		cfile = clientfile;
		sfile = spoolfile;
		string dfile = sfile  + ".dbf";
		string dbf = "g_4326/" + sfile + ".dbf";
		poDS = OGRSFDriverRegistrar::Open(dbf.c_str(), FALSE );

		if( poDS == NULL )
		{
			printf( "Open failed.\n" );
			exit( 1 );
		}
		OGRLayer  *poLayer;
		poLayer = poDS->GetLayer(0);
		OGRFeature *poFeature;

		bool flag=false;
		poLayer->ResetReading();
		int row=1;
		//std::cerr<<flagFeature<<" "<<index<<"\n";
		while( (poFeature = poLayer->GetNextFeature()) != NULL )
		{
			//std::cerr<<"inside\n";
			if (!flagFeature || row-1==index){
			OGRFeatureDefn *poFDefn = poLayer->GetLayerDefn();
			int iField;
			for( iField = 0; iField < poFDefn->GetFieldCount(); iField++ )
			{
				OGRFieldDefn *poFieldDefn = poFDefn->GetFieldDefn( iField );
				char s[100];
				if(flag==false){

					//for the field headers
					DataTable->elementAt(0,iField)->addWidget(new Wt::WText(poFieldDefn->GetNameRef()));	
					DataTable->elementAt(0,iField)->decorationStyle().setBackgroundColor(WColor(0,185,146));
					DataTable->elementAt(0,iField)->decorationStyle().setBorder(Wt::WBorder(Wt::WBorder::Solid, Wt::WBorder::Thin));
					DataTable->elementAt(0,iField)->decorationStyle().setForegroundColor(WColor(white));
					DataTable->elementAt(0,iField)->setPadding(15);
				}
				if( poFieldDefn->GetType() == OFTInteger )
					sprintf(s, "%d", poFeature->GetFieldAsInteger( iField ) );
				else if( poFieldDefn->GetType() == OFTReal )
					sprintf(s, "%.3f", poFeature->GetFieldAsDouble(iField) );
				else if( poFieldDefn->GetType() == OFTString )
					sprintf(s, "%s", poFeature->GetFieldAsString(iField) );
				else
					sprintf(s, "%s", poFeature->GetFieldAsString(iField) );
				//printf("%s", s);

				//setting text			
				DataTable->elementAt(row,iField)->addWidget(new Wt::WText(s));
				DataTable->elementAt(row,iField)->decorationStyle().setBorder(Wt::WBorder(Wt::WBorder::Solid, Wt::WBorder::Thin));
				DataTable->elementAt(row,iField)->setPadding(5);
			}
			flag=true;
			OGRFeature::DestroyFeature( poFeature );
			printf("\n"); 
			if(flagFeature)break;
			}
			row++;
		}
		//to close the dialog box
		WPushButton *b= new WPushButton("Close");
		b->clicked().connect(this, &WDialog::accept);
		contents()->enterPressed().connect(this, &WDialog::accept);

		OGRDataSource::DestroyDataSource( poDS );

		//adding widgets
		layout->addWidget(cont,0, 0,AlignCenter | AlignMiddle);
		layout->addWidget(b,1, 0,AlignCenter | AlignMiddle);
		contents()->setLayout(layout, AlignTop | AlignJustify);

		//show dialog box	
		show();
	}
}
