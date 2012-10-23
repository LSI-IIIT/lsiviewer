LSIViewer
=========

Libre Spatial Information Viewer
 

== Requirements ==
  1. cmake 
  2. boost libraries (development)
  3. Wt 

For Installing requirements, Please refer to [1]


== Building LSIViewer ==

  1. cd path/to/lsiviewer
  2. mkdir build && cd build
  3. cmake ..
  4. make

== Running LSIViewer ==

  sudo ./lsiviewer.wt --docroot ='path/to/resources' --http-address 0.0.0.0 --http-port 8080
 
  There are two methods with which you can deploy lsiviewer 
  A. Using FastCGI
  B. Using wthttpd
  See here [2] for details



For BUG reporting, feature request, open discussion and help. Please use our forums[3]


[1] http://www.webtoolkit.eu/wt/doc/reference/html/InstallationUnix.html (accessed on Oct 2012)

[2] http://www.webtoolkit.eu/wt/doc/reference/html/InstallationUnix.html#examples (accessed on Oct 2012)

[3] http://lsi.iiit.ac.in/redmine/projects/lsiviewer (accessed on Oct 2012)

