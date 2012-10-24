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
#include <Wt/WPaintedWidget>
#include <Wt/WContainerWidget>
#include <Wt/WPainter>
#include <Wt/WTable>
#include <Wt/WEnvironment>
#include "drawshape.h"
#include <Wt/WFileUpload>
#include <Wt/WHBoxLayout>
#include <Wt/WVBoxLayout>
#include <Wt/WBorderLayout>
#include <Wt/WPushButton>
#include <Wt/WGridLayout>
#include <Wt/Http/Request>
#include <Wt/WCheckBox>
#include <Wt/WLineEdit>
#include <Wt/WDialog>
#include<Wt/Ext/ToolBar>
#include <Wt/WSlider>
#include<Wt/WImage>
#include<Wt/WPainter>
#include<Wt/WColor>
#include<Wt/WBreak>
#include<Wt/WAnchor>
#include<Wt/WFileResource>
#include<Wt/WScrollArea>
#include<Wt/WSvgImage>
using namespace Wt;
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/filesystem.hpp>
#include <vector>
#include <ctime>
#include <sys/stat.h>
#include <boost/bind.hpp>
#include "postgresql/libpq-fe.h"
#include <Wt/WRandom>
#include "wgis.h"
#include "attribute_table.h"
#include "ColorPickDialog.h"
#include "LabelDialog.h"
#include<Wt/WSlider>
#include "help.h"
using namespace std;

/***************************************************************/
void CloseConn(PGconn *conn);
PGconn *ConnectDB()
{
	PGconn *conn = NULL;
	// Make a connection to the database
	conn = PQconnectdb("user=postgres password=iiit123ankush dbname=shpviewer hostaddr=10.2.32.16 port=5432");
	// Check to see that the backend connection was successfully made
	if (PQstatus(conn) != CONNECTION_OK)
	{
		printf("Connection to database failed");
		CloseConn(conn);
		return conn;
	}
	printf("Connection to database - OK\n");

	return conn;
}
void CloseConn(PGconn *conn)
{
	PQfinish(conn);

}

SHPViewer::SHPViewer(const WEnvironment& env): WApplication(env)
{
	pan=false;
	delta=1;
	setCssTheme("default");
	setTitle("LSI Viewer v2"); 
	currentWidth = 0;



	//declaring the main container widget
	WContainerWidget *mapcontainer = new WContainerWidget(root());
	mapcontainer->decorationStyle().setBackgroundImage("icons/back1.png",Wt::WCssDecorationStyle::NoRepeat,Right);
	
	p = new DrawShape(640,480);
	p->labelindex=0;
//	p->decorationStyle().setBorder(Wt::WBorder(Wt::WBorder::Solid, Wt::WBorder::Thin));
	//p->select=0;
	p->decorationStyle().setBorder(WBorder(WBorder::Solid));


	// container widget for file uploader
	WContainerWidget *cw = new WContainerWidget();
	WVBoxLayout * vlayout  = new WVBoxLayout();

	//viewshapefile button
	view = new WPushButton("View shapefile");
	view->clicked().connect(this,&SHPViewer::viewShape);
	view->resize(WLength::Auto,WLength::Auto);
	view->setToolTip("Click to view the uploaded shapefile");

	WPushButton *about = new WPushButton("About");
        about->clicked().connect(this,&SHPViewer::aboutlaunch);
        about->resize(WLength::Auto,30);
        about->setToolTip("About LSI Viewer");


	attributes = new WPushButton("Attribute Table");
	attributes->clicked().connect(this,&SHPViewer::viewAttributeTable);
	attributes->resize(WLength::Auto,30);
	attributes->setToolTip("View Attributes of the features");

	incr = new WPushButton("-");
	incr->resize(30,30);
	incr->setToolTip("Decrease penwidth for drawing");
	decr = new WPushButton("+");
	decr->resize(30,30);
	decr->setToolTip("Increase penwidth for drawing");
	
	incrL = new WPushButton("");
	incrL->resize(30,30);
	incrL->setIcon("icons/fontsizedown.png");
	incrL->setToolTip("Decrease Label size");
	decrL = new WPushButton("");
	decrL->resize(30,30);
	decrL->setIcon("icons/fontsizeup.png");
	decrL->setToolTip("Increase Label size");
	
	
	p->bordercolor=WColor(black);
	p->fillcolor=WColor(white);
	p->backcolor=WColor(white);
	p->labelcolor=WColor(black);

	borderColor = new WPushButton("");
	borderColor->setMargin(5, Left);                                 // add 5 pixels margin
	borderColor->resize(60,25);
	borderColor->decorationStyle().setBackgroundColor(WColor(black));
	borderColor->clicked().connect(this,&SHPViewer::changebordercolor);
	borderColor->setToolTip("Change pen color");
	fillColor = new WPushButton("");
	fillColor->setMargin(5, Left);                                 // add 5 pixels margin
	fillColor->resize(60,25); 
	fillColor->decorationStyle().setBackgroundColor(WColor(white));
	fillColor->clicked().connect(this,&SHPViewer::changefillcolor);
	fillColor->setToolTip("Change fill color");

	backColor = new WPushButton("");
	backColor->setMargin(5, Left);                                 // add 5 pixels margin
	backColor->resize(60,25); 
	backColor->decorationStyle().setBackgroundColor(WColor(white));
	backColor->clicked().connect(this,&SHPViewer::changebackcolor);
	backColor->setToolTip("Change background color");
	
	labelColor = new WPushButton("");
	labelColor->setMargin(5, Left);                                 // add 5 pixels margin
	labelColor->resize(60,25); 
	labelColor->decorationStyle().setBackgroundColor(WColor(black));
	labelColor->clicked().connect(this,&SHPViewer::changelabelcolor);
	labelColor->setToolTip("Change label color");
	exportb = new WPushButton("Export");
	exportb->clicked().connect(this,&SHPViewer::Exportfile);
	exportb->resize(WLength::Auto,30);
	exportb->setToolTip("Save as an image in selected format");

	dropdown = new WComboBox();
	dropdown->addItem("SVG");
	dropdown->addItem("PNG");
	dropdown->addItem("JPEG");
	dropdown->setCurrentIndex(0);
	dropdown->resize(WLength::Auto,30);
	dropdown->setToolTip("Select image format");


	LabelContents = new WPushButton("Label");
	LabelContents->clicked().connect(this,&SHPViewer::Labelfile);	
	LabelContents->resize(WLength::Auto,30);
	LabelContents->setToolTip("Label the features");

	incr->clicked().connect(this, &SHPViewer::decrPen);
	decr->clicked().connect(this, &SHPViewer::incrPen);

	incrL->clicked().connect(this, &SHPViewer::decrLabel);
	decrL->clicked().connect(this, &SHPViewer::incrLabel);


//unload().connect(this,&SHPViewer::delDataFiles);	

	WString contact ="<br/>Online Shapefile Viewer . developed and maintained by Lab for spatial informatics IIIT Hyderabad<br/>";
	contact +="contact: rajan [at] iiit [dot] ac [dot] in, rashadkm [at] gmail [dot] com, vaidehi [dot] 3591 [at] gmail [dot] com";

	WString info = "<h1>Online shapefile <br/> (.shp) viewer <br/> utility<br/></h1>";
	info+= "<p>It allows one to view the contents of shapefile <br/>within a browser with <b>NO GIS TOOLS</b> being installed</p>";
	WBorderLayout *mainlayout = new WBorderLayout();

	WContainerWidget *north= new WContainerWidget();
	//north->decorationStyle().setBackgroundColor(WColor(199,238,239));
	lbl_Uname =new WText(" <b>Username: </b>",north);
	new WText("",north);
	uname = new WLineEdit(north);
	lbl_passwd = new WText("<b> Password: </b>",north);
	passwd = new WLineEdit(north);
	passwd->setEchoMode(WLineEdit::Password);
	rememberMe = new WCheckBox(" Stay signed in",north);
	new WText("    ",north);
	login = new WText(" <a href=\"#\">Login</a>",north);
	login->clicked().connect(this , &SHPViewer::checkLogin);
	passwd->enterPressed().connect(this , &SHPViewer::checkLogin);

	new WText("",north);
	reg = new WText("<a href=\"#\" style='float:right'> Register</a>",north);
	reg->clicked().connect(this , &SHPViewer::doRegister);

	new WText("    ",north);
	//username = new WText("<b>admin </b>",north);

	new WText("    ",north);
	logout = new WText("<a href=\"#\" style='float:right' > Logout</a>",north);
	logout->clicked().connect(this , &SHPViewer::logOut);
	new WBreak(north);
	new WBreak(north);
	shapeupload = new WFileUpload(north);
	shapeupload->setPositionScheme(Relative);
	shapeupload->setMultiple(true);
	shapeupload->setFileTextSize(50);
	shapeupload->changed().connect(shapeupload, &WFileUpload::upload);
	shapeupload->uploaded().connect(this, &SHPViewer::ShapeFileUploaded);
	north->addWidget(view);

	//north->decorationStyle().setBorder(WBorder(WBorder::Solid));
	mainlayout->addWidget(north,WBorderLayout::North);

	up = new WPushButton("");
	up->resize(30,30);
	up->setIcon("icons/up.png");
	up->setToolTip("Pan up");
	up->mouseWentDown().connect(this,&SHPViewer::panup);	
	down = new WPushButton("");
	down->resize(30,30);
	down->setIcon("icons/down.png");
	down->setToolTip("Pan down");
	down->mouseWentDown().connect(this,&SHPViewer::pandown);	
	left = new WPushButton("");
	left->mouseWentDown().connect(this,&SHPViewer::panleft);
	left->setIcon("icons/left.png");
	left->resize(30,30);
	left->setToolTip("Pan left");	
	right = new WPushButton("");
	right->setIcon("icons/right.png");
	right->mouseWentDown().connect(this,&SHPViewer::panright);
	right->resize(30,30);	
	right->setToolTip("Pan right");
	reset = new WPushButton("");
	reset->setIcon("icons/globe.png");
	reset->resize(30,30);
	reset->mouseWentDown().connect(this,&SHPViewer::resetmap);	
	reset->setToolTip("Reset map");

	select = new WPushButton("");
	select->setIcon("icons/select.png");
	select->resize(30,30);
	select->setToolTip("Attribute select tool");
	select->mouseWentDown().connect(this,&SHPViewer::selecttool);	
	
	

	p->mouseWheel().connect(this,&SHPViewer::mousewheel);
	p->decorationStyle().setCursor(PointingHandCursor);
	p->mouseWentDown().connect(this,&SHPViewer::mousepan1);
	p->mouseWentUp().connect(this,&SHPViewer::mousepan);
	p->mouseMoved().connect(this,&SHPViewer::mousepan2);
	p->keyPressed().connect(this,&SHPViewer::handlekey);



	WGridLayout *layout = new WGridLayout();
	WContainerWidget *MapsAndLayerTree = new WContainerWidget();    
	
	WVBoxLayout *vbox = new WVBoxLayout();
	WVBoxLayout *vbox1 = new WVBoxLayout();
	WHBoxLayout *hbox = new WHBoxLayout();
	WText *description = new WText(info);
	WContainerWidget *controls = new WContainerWidget(); 
	vbox->addWidget(cw);
	vbox->addLayout(hbox, 1,0);
	hbox->addWidget(p);
	hbox->addLayout(vbox1,1 ,0);
	//vbox->addWidget(description,1);

	//vbox->setResizable(0);

	layout->addLayout(vbox,0, 0);

	//MapsAndLayerTree->resize(WLength::Auto, WLength::Auto);

	zoominb = new WPushButton("");
	zoominb->resize(30,30);
	zoominb->setIcon("icons/zoomin.png");
	zoominb->mouseWentDown().connect(this,&SHPViewer::zoomin);
	zoominb->setToolTip("Zoom in");
	zoomoutb = new WPushButton("");
	zoomoutb->resize(30,30);
	zoomoutb->setIcon("icons/zoomout.png");
	zoomoutb->mouseWentDown().connect(this,&SHPViewer::zoomout);	

	WScrollArea *cont= new WScrollArea();	
	cont->resize(690,500);
	cont->decorationStyle().setBackgroundColor(WColor(245,247,252));

		string rme = "<div style='text-align:justify;padding:15pxfont-size:10;' >"
"LSI Viewer allows you to <b>view Shapefiles</b> without any GIS tools installed.<br/> "
"LSI Viewer is best viewed with Mozilla Firefox and Chrome.<br/>"
"Best View Resolution: 1280x768<br/>"
"It does not work with Internet Explorer9, as multiple file selection is not allowed in IE9. <br/>"
"For BUG reporting, Feature Requests, support and discussion please visit <a href='http://lsi.iiit.ac.in/redmine/projects/lsiviewer'>LSI Redmine</a><br/><br/>"
"<h2 style='font-size:24px;color:grey'><u>Instructions: </u></h2><br/>"
"<center>"
"<table style='text-align:justify;width=\"90%\"'><ul>"
"<tr>"
"<td width='30%'><li><b>Upload Shapefile : </b></li></td>"
"<td> Click Browse... and choose your shapefile <br/> Press Ctrl button to select relevant files (.shp, .shx and .dbf) </td> "
"</tr>"
"<tr>"
"<td><li><b>View Shapefile : </b></li></td>"
"<td> Click on the &quot;View Shapefile&quot; button to view your shapefile </td>"
"</tr>"
"<tr>"
"<td><li><b>Zoom and Pan : </b></li></td>"
"<td>Use the zoom "
"<img src='icons/zoomout.png'/>&nbsp;"
"<img src='icons/zoomin.png'/> and pan "
"<img src='icons/up.png'/> &nbsp;"
"<img src='icons/down.png'/> &nbsp;"
"<img src='icons/left.png'/>&nbsp;"
"<img src='icons/right.png'/> &nbsp;"
"buttons to view different parts of the shapefile.<br/>"
"In addition:<br/>For pan - can use mouse drag<br/>"
"For zoom in/out - Can use scroll wheel</td>"
"</tr>	"
"<tr>"
"<td><li><b>Reset Shapefile : </b></li></td> "
"<td> Reset the shapefile's zoom and pan parameters."
"<img src='icons/globe.png'/> &nbsp;</td>"
" </tr>"
"<tr>"
"<tr>"
"<td><li><b>View Attribute table : </b></li></td>"
" <td> Click on the &quot;Attribute Table&quot; button to view the .dbf file contents and attributes of different features. </td> "
"</tr>"
"<tr>"
"<td><li><b>Selection tool : </b></li></td>"
" <td>Use the &quot;Select Tool&quot; "
"<img src='icons/select.png'/> &nbsp; "
"to select and view the attributes of a specific feature.  </td>"
" </tr>"
"<tr>"
"<td><li><b>Background color : </b></li></td> "
"<td>Use Background Color palette for choosing the suitable canvas color. </td>" 
"</tr> "
"<tr>"
"<td><li><b>Fill color : </b></li></td> "
"<td>Use Fill Color palette for choosing the suitable feature fill colors. </td> "
"</tr> "
"<tr>"
"<td><li><b>Pen width : </b></li></td> "
"<td> Adjust the penwidth by clicking on increase (+)  and decrease (-) penwidth buttons as required. </td> "
"</tr>	"
"<tr>"
"<td><li><b>Pen color : </b></li></td> "
"<td>Use pen color palette for choosing the suitable line color</td> "
"</tr> "
"<td><li><b>Label Shapefile : </b></li></td> "
"<td> Label the shapefile features using the  &quot;Label&quot; button </td> "
"</tr>"
"<tr>"
"<td><li><b>Label Size : </b></li></td>"
" <td>Adjust the label size by clicking of the increase "
"<img src='icons/fontsizeup.png'/> &nbsp; and decrease "
"<img src='icons/fontsizedown.png'/> &nbsp;label buttons. </td>"
"</tr>"
"<tr>"
"<td><li><b>Label color : </b></li></td> "
"<td>Use label color palette for choosing the suitable label color</td> "
"</tr>	"
"<tr>"
"<td><li><b>Export : </b></li></td> "
"<td>To export MapView as SVG,JPEG or PNG images. </td>" 
"</tr> "
"</ul>"
"</table>"
"</center>"
"<br/><br/>"
"</div>";
	WText *inst=new WText(rme);
	inst->setMargin(20);
	cont->setWidget(inst);
	
	vbox1->addWidget(cont,1,AlignMiddle|AlignRight);

	

	Check= new Wt::WCheckBox("Enable zoom & pan");
 	Check->setChecked(true);
	Check->changed().connect(this,&SHPViewer::enable);	
	MapsAndLayerTree->setLayout(layout);
	Wt::Ext::ToolBar *toolbar=new Wt::Ext::ToolBar(cw);
	Wt::Ext::ToolBar *toolbar1=new Wt::Ext::ToolBar(cw);
	toolbar->resize(850,WLength::Auto);
	toolbar1->resize(850,WLength::Auto);
	toolbar->addSeparator();
	toolbar->add(Check);
	toolbar->addSeparator();
	toolbar->add(up);
	toolbar->add(down);
	toolbar->add(left);
	toolbar->add(right);
	toolbar->addSeparator();
	toolbar->add(zoominb);
	toolbar->add(zoomoutb);
	toolbar->add(reset);
	toolbar->addSeparator();
	toolbar->add(attributes);
	toolbar->addSeparator();
	toolbar->add(select);
	toolbar->addSeparator();
	toolbar->add(new WText(" Background Color : "));
	toolbar->add(backColor);
	toolbar->addSeparator();
	toolbar->add(new WText(" Fill Color : "));
	toolbar->add(fillColor);
	toolbar->addSeparator();


	toolbar1->add(new WText(" Pen Width : "));
	toolbar1->add(incr);
	toolbar1->add(decr);
	toolbar1->addSeparator();
	toolbar1->add(new WText(" Pen Color : "));
	toolbar1->add(borderColor);
	toolbar1->addSeparator();
	toolbar1->add(LabelContents);
	toolbar1->add(new WText(" Label Size : "));
	toolbar1->add(incrL);
	toolbar1->add(decrL);
	toolbar1->add(new WText(" Label Color : "));
	toolbar1->add(labelColor);
	toolbar1->addSeparator();


	toolbar1->addSeparator();
	toolbar1->add(exportb);
	toolbar1->add(dropdown);
	toolbar1->addSeparator();
	toolbar1->add(about);
	toolbar1->addSeparator();
	cw->addWidget(new WBreak());
	
	mainlayout->addWidget(MapsAndLayerTree,WBorderLayout::Center);

	WContainerWidget *BottomWidget = new WContainerWidget();
	//new WText(contact,BottomWidget);
	//BottomWidget->decorationStyle().setBackgroundColor(WColor(243,245,255));
	//new WBreak(BottomWidget);
	
  	//mainlayout->addWidget(BottomWidget,WBorderLayout::South);
	mapcontainer->setLayout(mainlayout);
	
	string      token; //commented purposefully
	try {

		WApplication * app = WApplication::instance();
		token = app->environment().getCookie("bloglogin");
		} catch (...) { }

		if (!token.empty()) {
	
	view->setEnabled(true);
	attributes->setEnabled(true);
	exportb->setEnabled(true);
	uname->setHidden(true,anim);

	passwd->setHidden(true,anim);
	lbl_Uname->setHidden(true,anim);
	lbl_passwd->setHidden(true,anim);
	rememberMe->setHidden(true,anim);
	login->setHidden(true);
	reg->setHidden(true);
	logout->setHidden(false);

		}
		else {

		view->setEnabled(false);
		attributes->setEnabled(false);
		exportb->setEnabled(false);
		logout->setHidden(true);
		login->setHidden(false);
		reg->setHidden(false);
		}
	
	//lbl_Uname->setHidden(true,anim);
	
}

void SHPViewer::delDataFiles()
{
doJavaScript("alert(0)");

}

void SHPViewer::doRegister() {

	PGconn *conn;

	d = new WDialog();
	d->setWindowTitle("Registration form");
	d->resize(300,480);
	new WBreak(d->contents());

	new WText("Full name <br/>",d->contents());
	fname = new WLineEdit(d->contents());  
	fname->setTextSize(30);  

	new WText("<br/>Username <br/>",d->contents());
	UserName = new WLineEdit(d->contents());  
	UserName->setTextSize(30); 

	new WText("<br/>Password <br/>",d->contents());
	passwd1 = new WLineEdit(d->contents());  
	passwd1->setTextSize(30); 
	passwd1->setEchoMode(WLineEdit::Password);

	new WText("<br/>Retype Password <br/>",d->contents());
	passwd2 = new WLineEdit(d->contents());  
	passwd2->setTextSize(30); 
	passwd2->setEchoMode(WLineEdit::Password);

	new WText("<br/>Email <br/>",d->contents());
	email = new WLineEdit(d->contents());  
	email->setTextSize(30); 

	new WText("<br/>Company <br/>",d->contents());
	company = new WLineEdit(d->contents());  
	company->setTextSize(30); 

	WText *error = new WText("",d->contents());

	new WText("<br/> <br/>",d->contents());

	(new WPushButton("Register", d->contents()))
		->clicked().connect(d, &WDialog::accept);


	(new WPushButton("cancel", d->contents()))
		->clicked().connect(d, &WDialog::reject);

	if(d->exec() == WDialog::Accepted)
	{
		error->setText("Password Mismatch");
		if (passwd1->text() != passwd2->text()) {
			WApplication::instance()->doJavaScript("alert('Password mismatch. Please do registration again')");
			return;
		}
		conn =  ConnectDB();

		string query =  "SELECT * FROM kgis WHERE uname='" + uname->text().narrow() +  "'";
		PGresult *res = PQexec(conn, query.c_str());

		if(PQntuples(res)!=0) {
			WApplication::instance()->doJavaScript("alert('Userid exists. Please do registration again')");
			return;
		}
		else {

			if( (fname->text()=="") || (passwd1->text().toUTF8()=="") || 
					(UserName->text().narrow() == "") || (email->text().narrow() == "")
					||(company->text().narrow()== "")) {
				WApplication::instance()->doJavaScript("alert('All fields are required. Please do registration again')");
				return;
			}
			else if (UserName->text().narrow().size()<4) {
				WApplication::instance()->doJavaScript("alert('username too short. Miminum 4 characters required')");

			}else if(passwd1->text().toUTF8().size()<5) {
				WApplication::instance()->doJavaScript("alert('password too short. Miminum 5 characters required')");
			}
			else {
				conn =  ConnectDB();

				string query =  "INSERT INTO kgis (name,uname,passwd,email,company) VALUES('" +
					fname->text().narrow() + "','" + UserName->text().narrow() + "',md5('" + passwd1->text().toUTF8() + "'),'" +
					email->text().narrow() + "','" + company->text().narrow() + "')";

				res = PQexec(conn, query.c_str());

				if (PQresultStatus(res) != PGRES_COMMAND_OK)
				{
					PQclear(res);
					CloseConn(conn);
				}
				else {WApplication::instance()->doJavaScript("alert('Thankyou for registering')");


		//			WDialog *readme = new WDialog();
		//			readme->setWindowTitle("ReadMe");
		//			d->resize(200,200);
		//			new WBreak(readme->contents());

		string rme = "<div style='text-align:justify;padding:15pxfont-size:10;' >"
"LSI Viewer allows you to view shapefiles without any GIS tools installed.<br/><br/>"
"<h2 style='font-size:24px;color:grey'><u>Instructions: </u></h2><br/>"
"<center>"
"<table style='text-align:justify;width=\"90%\"'><ul>"
"<tr>"
"<td width='30%'><li><b>Upload Shapefile : </b></li></td>"
"<td> Click Browse... and choose your shapefile <br/> Press Ctrl button to select relevant files (.shp, .shx and .dbf) </td> "
"</tr>"
"<tr>"
"<td><li><b>View Shapefile : </b></li></td>"
"<td> Click on the &quot;View Shapefile&quot; button to view your shapefile </td>"
"</tr>"
"<tr>"
"<td><li><b>Zoom and Pan : </b></li></td>"
"<td>Use the zoom "
"<img src='icons/zoomout.png'/>&nbsp;"
"<img src='icons/zoomin.png'/> and pan "
"<img src='icons/up.png'/> &nbsp;"
"<img src='icons/down.png'/> &nbsp;"
"<img src='icons/left.png'/>&nbsp;"
"<img src='icons/right.png'/> &nbsp;"
"buttons to view different parts of the shapefile.<br/>"
"In addition:<br/>For pan - can use mouse drag<br/>"
"For zoom in/out - Can use scroll wheel</td>"
"</tr>	"
"<tr>"
"<td><li><b>Reset Shapefile : </b></li></td> "
"<td> Reset the shapefile's zoom and pan parameters."
"<img src='icons/globe.png'/> &nbsp;</td>"
" </tr>"
"<tr>"
"<tr>"
"<td><li><b>View Attribute table : </b></li></td>"
" <td> Click on the &quot;Attribute Table&quot; button to view the .dbf file contents and attributes of different features. </td> "
"</tr>"
"<tr>"
"<td><li><b>Selection tool : </b></li></td>"
" <td>Use the &quot;Select Tool&quot; "
"<img src='icons/select.png'/> &nbsp; "
"to select and view the attributes of a specific feature.  </td>"
" </tr>"
"<tr>"
"<td><li><b>Background color : </b></li></td> "
"<td>Use Background Color palette for choosing the suitable canvas color. </td>" 
"</tr> "
"<tr>"
"<td><li><b>Fill color : </b></li></td> "
"<td>Use Fill Color palette for choosing the suitable feature fill colors. </td> "
"</tr> "
"<tr>"
"<td><li><b>Pen width : </b></li></td> "
"<td> Adjust the penwidth by clicking on increase (+)  and decrease (-) penwidth buttons as required. </td> "
"</tr>	"
"<tr>"
"<td><li><b>Pen color : </b></li></td> "
"<td>Use pen color palette for choosing the suitable line color</td> "
"</tr> "
"<td><li><b>Label Shapefile : </b></li></td> "
"<td> Label the shapefile features using the  &quot;Label&quot; button </td> "
"</tr>"
"<tr>"
"<td><li><b>Label Size : </b></li></td>"
" <td>Adjust the label size by clicking of the increase "
"<img src='icons/fontsizeup.png'/> &nbsp; and decrease "
"<img src='icons/fontsizedown.png'/> &nbsp;label buttons. </td>"
"</tr>"
"<tr>"
"<td><li><b>Label color : </b></li></td> "
"<td>Use label color palette for choosing the suitable label color</td> "
"</tr>	"
"<tr>"
"<td><li><b>Export : </b></li></td> "
"<td>To export MapView as SVG,JPEG or PNG images. </td>" 
"</tr> "
"</ul>"
"</table>"
"</center>"
"<br/><br/>"
"</div>";				//	new WText(rme,readme->contents());

				//	(new WPushButton("Ok", readme->contents()))
				//		->clicked().connect(readme, &WDialog::accept);
				//	if(readme->exec() == WDialog::Accepted) {
				//		delete readme; readme = 0;
				//	}


				}

			}
		}
		//PQclear(res);
	}
}


void SHPViewer::logOut() {
	view->setEnabled(false);
	attributes->setEnabled(false);
	exportb->setEnabled(false);
	uname->setText("");

	passwd->setText("");
	WApplication::instance()->setCookie("bloglogin", "", 0);
	lbl_Uname->setHidden(false,anim);
	lbl_passwd->setHidden(false,anim);
	uname->setHidden(false,anim);
	passwd->setHidden(false,anim);
	rememberMe->setHidden(false,anim);
	login->setHidden(false);
	logout->setHidden(true);
	reg->setHidden(false);

}


void SHPViewer::checkLogin() {
	
	   PGconn *conn =  ConnectDB();


	   string query =  "SELECT * FROM kgis WHERE uname='" + uname->text().narrow() + "' AND passwd=md5('" + passwd->text().toUTF8() + "')";
	   PGresult *res = PQexec(conn, query.c_str());





	   if(PQntuples(res)==0) {
	   WApplication::instance()->doJavaScript("alert('Invalid username/password')");
	   view->setEnabled(false);

	   uname->setText("");
	//username->setText("");
	passwd->setText("");
	return;
	}

	else {
	string loggedin = PQgetvalue(res, 0, 1);
	//username->setText(loggedin);
	view->setEnabled(true);
	attributes->setEnabled(true);
	exportb->setEnabled(true);

	if(rememberMe->checkState() == Checked) {
	std::string token = Wt::WRandom::generateId(16);
	WApplication *app = WApplication::instance();
	app->setCookie("bloglogin", token, 60*60*24*14);
	}

	WApplication::instance()->doJavaScript("alert('You are logged in.')");
	view->setEnabled(true);
	attributes->setEnabled(true);
	exportb->setEnabled(true);
	uname->setHidden(true,anim);

	passwd->setHidden(true,anim);
	lbl_Uname->setHidden(true,anim);
	lbl_passwd->setHidden(true,anim);
	rememberMe->setHidden(true,anim);
	login->setHidden(true);
	reg->setHidden(true);
	logout->setHidden(false);

	}



	PQclear(res);


//	view->setEnabled(true);//line added

}


void SHPViewer::updatePenWidth(int w) 
{
	if(view->isEnabled()) {
		if(w > currentWidth) {
			p->incrWidth();
			currentWidth = w;
		}
		else {
			p->decrWidth();
			currentWidth = w;
		}

	}
}
void SHPViewer::incrPen() {
		if(view->isEnabled())
			p->incrWidth();

	}
void SHPViewer::enable() {

		if(Check->isChecked()){
		up->setEnabled(true);
		down->setEnabled(true);
		left->setEnabled(true);
		right->setEnabled(true);
		zoominb->setEnabled(true);
		zoomoutb->setEnabled(true);
		reset->setEnabled(true);
}
else{
		up->setEnabled(false);
		down->setEnabled(false);
		left->setEnabled(false);
		right->setEnabled(false);
		zoominb->setEnabled(false);
		zoomoutb->setEnabled(false);
		reset->setEnabled(false);
}

	}
void SHPViewer::incrLabel() {
		if(view->isEnabled())
			p->incrLabelWidth();

	}
void SHPViewer::decrLabel() {
		if(view->isEnabled())
			p->decrLabelWidth();

	}
void SHPViewer::unload()
{
	if(!p->svgFile.empty()){
			string delfile="rm "+p->svgFile;
			int ret=system(delfile.c_str());
	}
	
	std::vector<string>::iterator the_iterator = filenames_delete.begin();
	int ret;
	while( the_iterator != filenames_delete.end() ) {
	  ret=system(("rm "+(*the_iterator)).c_str());
	  ++the_iterator;
	}		
		
	filenames_delete.clear();
	quit();
}
//mouseup
void SHPViewer::mousepan1(const WMouseEvent &event) {
	if(view->isEnabled() && !p->select && Check->isChecked()){
		pan=true;
		x=(event.widget()).x;
		y=(event.widget()).y;
		p->decorationStyle().setCursor(OpenHandCursor);
	}
	if(view->isEnabled() && p->select)
	{
		p->featuredetect((event.widget()).x,(event.widget()).y);
	}
}
//mousemoved
void SHPViewer::mousepan2(const WMouseEvent &event) {
	if(view->isEnabled() && pan==true && Check->isChecked()){
		p->y_pos_shift=p->y_pos_shift-(y-event.widget().y)*100.0/(480*(1+p->increase_height/100));
		p->x_pos_shift=p->x_pos_shift-(x-event.widget().x)*100.0/(640*(1+p->increase_width/100));
		p->update();
		y=event.widget().y;
		x=event.widget().x;
	}
}
//mousedown
void SHPViewer::mousepan(const WMouseEvent &event) {
	if(view->isEnabled() && pan==true && Check->isChecked()){
		p->y_pos_shift=p->y_pos_shift-(y-event.widget().y)*100.0/(480*(1+p->increase_height/100));
		p->x_pos_shift=p->x_pos_shift-(x-event.widget().x)*100.0/(640*(1+p->increase_width/100));
		p->update();
		pan=false;
		p->decorationStyle().setCursor(PointingHandCursor);

	}
}
void SHPViewer::panup() {
	if(view->isEnabled()){
		p->y_pos_shift-=delta;
		p->update();
	}

}
void SHPViewer::pandown() {
	if(view->isEnabled()){
		p->y_pos_shift+=delta;
		p->update();}

}
void SHPViewer::panleft() {
	if(view->isEnabled()){
		p->x_pos_shift-=delta;
		p->update();}

}
void SHPViewer::selecttool() {
	if(p->select){
		p->select=false;
		p->decorationStyle().setCursor(PointingHandCursor);
		}
	else if(view->isEnabled()&& p->select==false){
		p->select=true;
		p->decorationStyle().setCursor(CrossCursor);
		
}
	

}
void SHPViewer::panright() {
	if(view->isEnabled()){
		p->x_pos_shift+=delta;
		p->update();}

}
void SHPViewer::zoomin() {
	if(view->isEnabled() && (100+p->increase_width >delta && 100+p->increase_height>delta ) ){
		p->increase_width+=delta;
		p->increase_height+=delta;
		p->update();}


}
void SHPViewer::zoomout() {
	if(view->isEnabled()){
		p->increase_width-=delta;
		p->increase_height-=delta;
		p->update();}

}
void SHPViewer::mousewheel(const WMouseEvent &event) {
	if(view->isEnabled() && Check->isChecked() && (event.wheelDelta()==1 || (100+p->increase_width >delta && 100+p->increase_height>delta ))){
		p->increase_width+=event.wheelDelta()*delta;
		p->increase_height+=event.wheelDelta()*delta;
		p->update();
	}
}
void SHPViewer::resetmap() {
	if(view->isEnabled()){
		p->y_pos_shift=0;
		p->x_pos_shift=0;
		p->increase_width=0;
		p->increase_height=0;
		p->update();}
}

void SHPViewer::handlekey(const WKeyEvent &event) {
	if(view->isEnabled()){
	switch(event.key())
	{
		case Key_Up: panup();break;
		case Key_Down: pandown();break;
		case Key_Left: panleft();break;
		case Key_Right: panright();break;
	}
	}
}

void SHPViewer::changebordercolor() {
	ColorPickDialog *c =new ColorPickDialog(&(p->bordercolor),borderColor,p);
}
void SHPViewer::changefillcolor() {
	ColorPickDialog *c =new ColorPickDialog(&(p->fillcolor),fillColor,p);
}
void SHPViewer::changebackcolor() {
	ColorPickDialog *c =new ColorPickDialog(&(p->backcolor),backColor,p);
}
void SHPViewer::changelabelcolor() {
	ColorPickDialog *c =new ColorPickDialog(&(p->labelcolor),labelColor,p);
}

void SHPViewer::decrPen() {
		if(view->isEnabled())
			p->decrWidth();

	}

void SHPViewer::viewShape() {
	
	p->updatePaint(clientfile,ss);
	
}
void SHPViewer::Labelfile() {

	LabelDialog *x =new LabelDialog(clientfile,ss,&(p->labelindex),p);
	
}
void SHPViewer::viewAttributeTable() {
	DBFDialog *x =new DBFDialog(clientfile,ss);
	
}
void SHPViewer::Exportfile()
{
	WSvgImage *SvgImage=new WSvgImage(640,480);
	p->paintondevice(SvgImage);
	string fname=WRandom::generateId(16);
	string filename="g_4326/"+fname+".svg";
	filenames_delete.push_back(filename);
	std::ofstream f(filename.c_str());
	SvgImage->write(f);
	f.close();
	WFileResource *res ;
	string command;
	string dfilename;
	if(dropdown->currentIndex()==0){
		res = new WFileResource(filename);
		dfilename = ss + ".svg";
		res->suggestFileName(ss +".svg");
		//filenames_delete.push_back(filename);

	}
	else if(dropdown->currentIndex()==1){
		command="convert g_4326/"+fname+".svg g_4326/"+fname+".png";
		//filenames_delete.push_back("g_4326/"+fname+".png");
		//filenames_delete.push_back("g_4326/"+fname+".svg");
		int sss =system(command.c_str());
		res = new WFileResource("g_4326/"+fname+".png");
		dfilename = ss + ".png";
		res->suggestFileName(ss +".png");
	}
	else if(dropdown->currentIndex()==2){
		command="convert g_4326/"+fname+".svg g_4326/"+fname+".jpeg";
		//filenames_delete.push_back("g_4326/"+fname+".jpeg");
		//filenames_delete.push_back("g_4326/"+fname+".svg");
		int sss = system(command.c_str());
		res = new WFileResource("g_4326/"+fname+".jpeg");
		dfilename = ss + ".jpeg";
		res->suggestFileName(ss +".jpeg");

	}

	WDialog *dialog = new WDialog();
	dialog->setWindowTitle("File Download");
	dialog->resize(550,300);

	new WBreak(dialog->contents());
	new WBreak(dialog->contents());
	new WText("Click on the link to download the '" + dfilename + "' image file",dialog->contents());
	new WBreak(dialog->contents()); 
	WAnchor *downloadlink_ = new WAnchor(res,"File Download",dialog->contents());
	new WBreak(dialog->contents()); 
	new WText("Note: When using firefox on windows, with &quot;Open With &quot; option - please note that the file maybe appended with &quot;.txt &quot;",dialog->contents());
	new WBreak(dialog->contents());
	new WBreak(dialog->contents());
//	WAnchor *downloadlink_ = new WAnchor(res,"File Download",dialog->contents());
//	new WBreak(dialog->contents());	
//	new WBreak(dialog->contents());
	downloadlink_->clicked().connect(dialog, &WDialog::accept);
	(new WPushButton("Close", dialog->contents()))->clicked().connect(dialog, &WDialog::reject);
	dialog->exec();
}
void SHPViewer::ShapeFileUploaded() {
	
	if(lbl_Uname->isHidden()) {
		//upload->spoolFileName();
		//WApplication::instance()->doJavaScript("alert('in uploaded ');");
		view->setEnabled(false);
		clientfile = shapeupload->clientFileName().narrow();
		string spoolfile =shapeupload->spoolFileName();

		vector<Http::UploadedFile> ufiles;
		ufiles = shapeupload->uploadedFiles();
		//cout << "SSS " << ufiles.size() << endl;

		for(vector<Http::UploadedFile>::iterator it = ufiles.begin();it!=ufiles.end();++it) {
			Http::UploadedFile uf = *it;
			string spfile = uf.clientFileName();
			ss = spfile.substr(0,(spfile.size()-4));
			string move;
			int res;
			move = "cp " + uf.spoolFileName() + " g_4326/" +  uf.clientFileName();
			//WApplication::instance()->doJavaScript("alert('"+move+"');");
			res = system (move.c_str());
			if(res!=0)
			{
				WApplication::instance()->doJavaScript("alert('error in uploading');");
				return;
			}
		}
		if(boost::filesystem::exists("g_4326/"+ss+".shp")&& boost::filesystem::exists("g_4326/"+ss+".shx") && boost::filesystem::exists("g_4326/"+ss+".dbf"))
{
		view->setEnabled(true);
		p->reset();
		//WApplication::instance()->doJavaScript("alert('Shapefiles uploaded');");
		}
		else
		{
			WApplication::instance()->doJavaScript("alert('Upload all the necesary files : *.shp , *.shx, *.dbf');");
		}

	}
	else
	{
		WApplication::instance()->doJavaScript("alert('you are not logged in.');");
	}

}


void SHPViewer::aboutlaunch() {

        helpDialog *x =new helpDialog();

}



SHPViewer::~SHPViewer()
{
/*
		delete chart;
		delete fname  ;
		delete UserName;
		delete passwd1 ;
		delete passwd2 ;
		delete email;
		delete company;
		delete lbl_Uname;
		delete lbl_passwd;
		delete d;
		delete scaleSlider;
		delete login;
		delete uname;
		delete passwd;
		delete username;
		delete incr;
		delete decr;
		delete view;
		delete attributes;
		delete exportb;
		delete select;
		delete borderColor;
		delete fillColor;
		delete LabelContents;
		delete rememberMe;
		
		delete p;
*/
}
WApplication *createApplication(const WEnvironment& env)
{
	return new SHPViewer(env);
}
int main(int argc, char **argv)
{
	OGRRegisterAll();
	return WRun(argc, argv, &createApplication);
}

