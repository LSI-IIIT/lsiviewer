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

#ifndef WGIS_H_
#define WGIS_H_
#include <Wt/WApplication>
#include <Wt/WPaintedWidget>
#include <Wt/WEnvironment>
#include "drawshape.h"
#include <Wt/WFileUpload>
#include <Wt/WPushButton>
#include <Wt/WCheckBox>
#include <Wt/WCheckBox>
#include <Wt/WLineEdit>
#include<Wt/WComboBox>
#include <stdlib.h>
#include <fstream>
#include <iostream>

#include <boost/bind.hpp>
#include <Wt/Ext/ToolBar>

using namespace Wt;

using namespace std;
class SHPViewer : public WApplication
{
	public:
		SHPViewer(const WEnvironment& env);
		WFileUpload *shapeupload ;
		DrawShape *p;
		void ShapeFileUploaded();
		void delDataFiles();
		void incrPen();
		void decrPen();
		void incrLabel();
		void decrLabel();
		void aboutlaunch();
		void changebordercolor();
		void changelabelcolor();
		void changefillcolor();
		void changebackcolor();
		void viewShape();
		void viewAttributeTable();
		void checkLogin();
		void Labelfile();
		void selecttool();
		void enable();
		string ss, clientfile;

		void logOut();
		void updatePenWidth(int);
		WText *logout;
		WText *reg;
		WLineEdit *fname  ;
		WLineEdit *UserName;
		WLineEdit *passwd1 ;
		WLineEdit *passwd2 ;
		WLineEdit *email;
		WLineEdit *company;
		WText *lbl_Uname,*lbl_passwd;
		WDialog *d;
		WSlider *scaleSlider;
		void doRegister();
		
		void shpfiles();
		bool pan;
		int x,y;
		double delta;
		void Exportfile();
		WText *login;
		vector<string> filenames_delete;
	protected:
		virtual void unload();
		
	private:
		WLineEdit *uname;
		WComboBox *dropdown;
		WLineEdit *passwd;
		WText * username;
		WPushButton *up,*down,*left,*right,*zoominb,*zoomoutb,*reset;
		WPushButton *incr, *decr;
		WPushButton *incrL, *decrL;
		WPushButton *view;
		WPushButton *attributes;
		WPushButton *exportb;
		Wt::WCheckBox *Check ;
		WPushButton *select;
		WPushButton *borderColor;
		WPushButton *fillColor;
		WPushButton *backColor;
		WPushButton *labelColor;
		WPushButton *LabelContents;
		void mousewheel(const WMouseEvent &);
		void mousepan1(const WMouseEvent &);
		void mousepan2(const WMouseEvent &);
		void mousepan(const WMouseEvent &);
		void handlekey(const WKeyEvent&);
		void panup();
		void pandown();
		void panleft();
		void panright();
		void resetmap();
		void zoomin();
		void zoomout();
		WCheckBox *rememberMe;
		int currentWidth;
		~SHPViewer();
		const WAnimation anim;

};
#endif // DRAWSHAPE_H_

