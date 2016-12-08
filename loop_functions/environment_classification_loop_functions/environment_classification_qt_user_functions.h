#ifndef ENVIRONMENT_CLASSIFICATION_LOOP_QTUSER_FUNCTIONS_H
#define ENVIRONMENT_CLASSIFICATION_LOOP_QTUSER_FUNCTIONS_H

#include <argos3/plugins/simulator/visualizations/qt-opengl/qtopengl_user_functions.h>
#include <argos3/plugins/robots/e-puck/simulator/epuck_entity.h>



using namespace argos;

class CEnvironmentClassificationLoopFunctions;

class CEnvironmentClassificationQTUserFunctions : public CQTOpenGLUserFunctions {

public:

   CEnvironmentClassificationQTUserFunctions();


   virtual ~CEnvironmentClassificationQTUserFunctions() {}
   virtual void DrawInWorld();
  
private:
   
   void DrawGrid( );
   CEnvironmentClassificationLoopFunctions& m_cEnvironmentClassificationLF;

};

#endif
