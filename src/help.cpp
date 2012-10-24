
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
#include <Wt/Chart/WCartesianChart>
#include <Wt/WStandardItemModel>
#include<Wt/WStandardItem>
#include <boost/filesystem.hpp>
#include <vector>
#include <ctime>
#include <sys/stat.h>
#include <boost/bind.hpp>
#include "postgresql/libpq-fe.h"
#include<Wt/WScrollArea>
#include<Wt/WBorder>
#include<Wt/WDialog>
#include<Wt/WTable>
#include<Wt/WSlider>
#include<Wt/WColor>
#include<iostream>
#include<stdio.h>
#include <ogrsf_frmts.h>
#include "help.h"

helpDialog::helpDialog(): WDialog("About LSI Viewer")
{	
/*
	resize(1224,600);
		WScrollArea *cont= new WScrollArea();		
		WGridLayout *layout = new WGridLayout();
		OGRRegisterAll();
		OGRDataSource       *poDS;
		cont->resize(720, 450);
		
		WAbstractItemModel *model=numfileschart();	
	if(model->rowCount() && model->columnCount()){
	chart= new WCartesianChart();
	chart->setMargin(10, Top | Bottom);
  	chart->setMargin(WLength::Auto, Left | Right);
	chart->setPlotAreaPadding(50, Left);
  	//chart->setPlotAreaPadding(30, Top | Bottom);
	chart->resize(700, 420);
	
	chart->setModel(model);  
  	chart->setXSeriesColumn(0);    // set the column that holds the categories
  	WDataSeries series(1, BarSeries);
  	series.setShadow(WShadow(3, 3, WColor(0, 0, 0, 127), 3));
	series.setFillRange(MinimumValueFill);
	chart->setBarMargin(0.5);
	chart->addSeries(series);
	chart->setTitle("Shapefiles viewed");
        chart->initLayout();
	chart->setLegendColumns(0,WLength(40));
	chart->decorationStyle().setBackgroundColor(WColor(216,216,216));
	cont->setWidget(chart);
	

}
		
		
		//to close the dialog box
		WPushButton *b= new WPushButton("Close");
		b->clicked().connect(this, &WDialog::accept);
		contents()->enterPressed().connect(this, &WDialog::accept);


		//adding widgets
		WText *textInfo = new WText("LSI viewer (Libre Spatial Information Viewer) was developed for users who may not have access to a \
					suite of GIS software tools but would like to just view/display and print/export the data they have. \
					LSIViewer is an Online Spatial Map Viewer for your Local Data including Map Styling. Currently it can be used as a \
					Map Viewer for Shapefile (.shp) format data. Other data formats are planned to be supported in later versions. <br/> \
					This online tool has been developed using a set of <b>FOSS </b> tools and we thank all FOSS contributors for making this work \
					possible. <br/> <br/> The tool will be released as an Open Soure Project soon. <br/> If interested, please contact any one of the \
					developers. <br/><br/><i>This version is the new release of Online Shapefile Viewer</i><br/><br/> Developed and Maintained by <b><i>Lab for Spatial informatics, IIIT Hyderabad</i> </b><br/>\
					K S Rajan : rajan [at] iiit [dot] ac [dot] in,<br/> \
					Mohammed Rashad : rashadkm [at] gmail [dot] com, <br/> \
					Vaidehi Chhajer : vaidehi [dot] 3591 [at] gmail [dot] com <br/>");
		layout->addWidget(textInfo,0,0,AlignCenter | AlignMiddle);

		layout->addWidget(cont,0, 1,AlignCenter | AlignMiddle);
		stringstream str;
		str<<numUsers();
		layout->addWidget(new WText("Number of registered users :"+str.str()),1,1,AlignMiddle|AlignCenter);
		layout->addWidget(b,2, 1,AlignCenter | AlignMiddle);
		contents()->setLayout(layout, AlignTop | AlignJustify);

		//show dialog box	
		show();
}

WAbstractItemModel * helpDialog::numfileschart() {
	int filecountarray[1000][12]={{0}};
	int min_mon=13,min_yr=INT_MAX,max_mon=-1,max_yr=-1;
	struct tm * timeinfo;struct stat fileinfo;
	int yr; int mon;
	string months[]= {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
	std::string ext = ".shp";
	for ( boost::filesystem::directory_iterator it( boost::filesystem::system_complete(boost::filesystem::path( "g_4326/")));it != boost::filesystem::directory_iterator(); ++it )
	{
		if ( boost::filesystem::is_regular_file( it->status() ) && boost::algorithm::to_lower_copy( it->path().extension() ) == ext )
		{
//cout <<  it->path().filename() << endl;
			stat((("g_4326/"+it->path().filename())).c_str(), &fileinfo);
			timeinfo = gmtime(&(fileinfo.st_ctime));
			mon=(*timeinfo).tm_mon;
			yr=(*timeinfo).tm_year-111;
			if(yr>=0)
			{
				
				filecountarray[yr][mon]+=1;
				if(min_yr>yr||(min_yr==yr && min_mon > mon))
				{
					min_mon=mon;
					min_yr=yr;
				}
				if(max_yr<yr||(max_yr==yr && max_mon < mon))
				{
					max_mon=mon;
					max_yr=yr;
				}
			}
		}
	}
for ( boost::filesystem::directory_iterator it( boost::filesystem::system_complete(boost::filesystem::path( "../shpviewer/g_4326/")));it != boost::filesystem::directory_iterator(); ++it )
        {
                if ( boost::filesystem::is_regular_file( it->status() ) && boost::algorithm::to_lower_copy( it->path().extension() ) == ext )
                {
//cout <<  it->path().filename() << endl;
                        stat((("../shpviewer/g_4326/"+it->path().filename())).c_str(), &fileinfo);
                        timeinfo = gmtime(&(fileinfo.st_ctime));
                        mon=(*timeinfo).tm_mon;
                        yr=(*timeinfo).tm_year-111;
                        if(yr>=0)
                        {

                                filecountarray[yr][mon]+=1;
                                if(min_yr>yr||(min_yr==yr && min_mon > mon))
                                {
                                        min_mon=mon;
                                        min_yr=yr;
                                }
                                if(max_yr<yr||(max_yr==yr && max_mon < mon))
                                {
                                        max_mon=mon;
                                        max_yr=yr;
                                }
                        }
                }
}
	int Count=0,cnt=0;;
	for(int i=min_yr;i<=max_yr;i++)
		for(int j=(i==min_yr?min_mon:0);( i==max_yr?j<=max_mon:j<12);j++)Count++;

	WStandardItemModel *model=new WStandardItemModel(Count,2,this);
	
	for(int i=min_yr;i<=max_yr;i++)
		for(int j=(i==min_yr?min_mon:0);( i==max_yr?j<=max_mon:j<12);j++)
		{
			std::string s;
			std::stringstream out;
			out << (i+2011);
			s = out.str();
			model->setData(cnt,0,Wt::WString::fromUTF8(months[j]+" "+s));
			model->setData(cnt,1,boost::any(filecountarray[i][j]));
			cnt++;
		}
		
return model;
*/
} 

void CloseConnection(PGconn *conn)
{
	PQfinish(conn);

} 
PGconn *ConnectDb()
{
	PGconn *conn = NULL;
	// Make a connection to the database
	conn = PQconnectdb("user=postgres password=iiit123ankush dbname=shpviewer hostaddr=10.2.32.16 port=5432");
	// Check to see that the backend connection was successfully made
	if (PQstatus(conn) != CONNECTION_OK)
	{
		printf("Connection to database failed");
		CloseConnection(conn);
		return conn;
	}
	printf("Connection to database - OK\n");

	return conn;
}




int helpDialog::numUsers() {

	PGconn *conn;
	conn =  ConnectDb();
        string query =  "SELECT * FROM kgis;";
	PGresult *res = PQexec(conn, query.c_str());
	int numusers=PQntuples(res);
	PQclear(res);
	return numusers;
}

