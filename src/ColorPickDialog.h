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
#ifndef COLOR_H_
#define COLOR_H_
#include<Wt/WSlider>
#include <Wt/WPushButton>
#include<Wt/WColor>
#include "drawshape.h"

using namespace Wt;
using namespace std;
class ColorPickDialog : public WDialog
{
	public:
		WPushButton *but;
		WSlider *red_slider;
		WSlider *green_slider;
		WSlider *blue_slider;
		ColorPickDialog(WColor *c,WPushButton *button, DrawShape *shape);
	private:
		WColor *currentcolor_;
		WColor currentcolor;
		WPushButton *p;
		DrawShape *shape1;
		void PickColor(WColor *color);
		void slidermoved(int v);
		void handleFinish();
};
#endif // COLOR_H_
