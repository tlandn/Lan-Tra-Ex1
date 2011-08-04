#include <djmodulemanager.h>
#include <djostime.h>
#include <dj2d.h>
#include <dj2dutil.h>
#include <djgamesounds.h>
#include <djfont.h>
#include <iostream>

#include <djos.h>
#include <djbuffer.h>
#include <djosrandom.h>


using namespace std;

int main() {

	djRandomSetSeed((djuint32)(djGetSystemTimeFloat() * 65536.0f));
	DJVector2 v1 = DJVector2(1,0);
	cout <<djRandomGetFloat() << endl;
	v1 = v1.RotatedZ(djRandomGetFloat() * DJ_2PIf);
	cout << v1.x() << " " << v1.y()<< endl; 
	DJVector2 v2(1,0);
	cout <<  (v1 % v2);


	system("PAUSE");

	return 0;


}