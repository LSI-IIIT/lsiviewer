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


// For seeing the attribute table

#include <Wt/WDialog>
#include <vector>
#include <string>
#include <Wt/Chart/WCartesianChart>
#include <boost/bind.hpp>
#ifndef help_
#define help_
using namespace Wt;
using namespace std;
namespace Wt{
class WAbstractItemModel;
};
using namespace Wt::Chart;
class helpDialog : public WDialog
{
	public:
		helpDialog();
		WCartesianChart *chart;
		WAbstractItemModel* numfileschart();
		int numUsers();

};
#endif // help_
