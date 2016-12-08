#include "environment_classification_qt_user_function.h"
#include "environment_classification_loop_function.h"
#include "math.h"
#include "vector"
#define X_UP        2.0f 
#define X_DOWN     -2.0f
#define Y_UP        1.0f
#define Y_DOWN     -1.0f
#define INTENSITY   3.0f
#define STEP	    0.1f

/****************************************/
/****************************************/

CEnvironmentClassificationQTUserFunctions::CEnvironmentClassificationQTUserFunctions() :
   m_cEnvironmentClassificationLF(dynamic_cast<CEnvironmentClassificationLoopFunctions&>(CSimulator::GetInstance().GetLoopFunctions())){
}

/* This function retrieve data about the colour of each cell from environment_classification_loop_function (grid) and 
   draw a coloured squared in each position */

void CEnvironmentClassificationQTUserFunctions::DrawInWorld() {
  
CVector3 a;
CVector3 b;
CVector3 c;
CVector3 d;

Real x,y, yDim=2*Y_UP, xDim=2*X_UP;

std::vector<CVector3> angoli(4);

Real p = 0.02;
a.SetX(p);
b.SetX(xDim);
a.SetZ(0.001);
b.SetZ(0.001);
for (y = p; y < yDim ; y = y+STEP)
  {
	a.SetY(y);
	b.SetY(y); 
	DrawSegment(a,b, CColor::BLACK, INTENSITY);
}

a.SetY(yDim);
b.SetY(p);
for (x = p; x < xDim ; x = x+STEP)
    {
a.SetX(x);
b.SetX(x);
DrawSegment(a,b, CColor::BLACK, INTENSITY);
}
 

}

REGISTER_QTOPENGL_USER_FUNCTIONS(CEnvironmentClassificationQTUserFunctions, "environment_classification_qt_user_functions")
