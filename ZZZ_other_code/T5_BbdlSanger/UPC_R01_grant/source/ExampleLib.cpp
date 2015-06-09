
#include "ExampleLib.h"

GLfloat XAxisAmbient[] = {1.00, 0.00, 0.00, 1.00};
GLfloat XAxisDiffuse[] = {1.00, 0.00, 0.00, 1.00};

GLfloat YAxisAmbient[] = {0.00, 1.00, 0.00, 1.00};
GLfloat YAxisDiffuse[] = {0.00, 1.00, 0.00, 1.00};

GLfloat ZAxisAmbient[] = {0.00, 0.00, 1.00, 1.00};
GLfloat ZAxisDiffuse[] = {0.00, 0.00, 1.00, 1.00};

GLfloat WorkSpaceAmbient[] = {1.00, 1.00, 0.00, 1.00};
GLfloat WorkSpaceDiffuse[] = {1.00, 1.00, 0.00, 1.00};

GLfloat LibSpecular[] = {1.00, 1.00, 1.00, 1.00};
GLfloat LibEmissive[] = {0.00, 0.00, 0.00, 1.00};
GLfloat LibShininess = {128.00};

GLfloat FrontArc[iNrSegments+1][3];
GLfloat BackArc[iNrSegments+1][3];

void XAxisMaterial()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, XAxisAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, XAxisDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, LibSpecular);
	glMaterialfv(GL_FRONT, GL_EMISSION, LibEmissive);
	glMaterialf(GL_FRONT, GL_SHININESS, LibShininess);
}


void YAxisMaterial()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, YAxisAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, YAxisDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, LibSpecular);
	glMaterialfv(GL_FRONT, GL_EMISSION, LibEmissive);
	glMaterialf(GL_FRONT, GL_SHININESS, LibShininess);
}


void ZAxisMaterial()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, ZAxisAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ZAxisDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, LibSpecular);
	glMaterialfv(GL_FRONT, GL_EMISSION, LibEmissive);
	glMaterialf(GL_FRONT, GL_SHININESS, LibShininess);
}

void WorkSpaceMaterial()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, WorkSpaceAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, WorkSpaceDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, LibSpecular);
	glMaterialfv(GL_FRONT, GL_EMISSION, LibEmissive);
	glMaterialf(GL_FRONT, GL_SHININESS, LibShininess);
}



void DrawAxes(void)
{
   
   glPushMatrix();

   //x-axis
   XAxisMaterial();
   glBegin(GL_LINES);
   glVertex3f(AxisLength, 0.0, 0.0);
   glVertex3f(-AxisLength, 0.0, 0.0);
   glEnd();

   //y-axis
   YAxisMaterial();
   glBegin(GL_LINES);
   glVertex3f(0.0, AxisLength, 0.0);
   glVertex3f(0.0, -AxisLength, 0.0);
   glEnd();


   //z-axis
   ZAxisMaterial();
   glBegin(GL_LINES);
   glVertex3f(0.0, 0.0, AxisLength);
   glVertex3f(0.0, 0.0, -AxisLength);
   glEnd();
  
   glPopMatrix();

}


void CreatePoints(void)
{
	float fAngle;
	float fAngleStart = LeftSideArc;
	float fAngleEnd = RightSideArc;
	float fAngleStep = (fAngleEnd-fAngleStart)/iNrSegments;
	int i;
   
	for(i=0; i<=iNrSegments; i++)
	{
		fAngle = fAngleStart + i * fAngleStep;
		FrontArc[i][X] = MinRadius*cos(fAngle);
		FrontArc[i][Y] = MinRadius*sin(fAngle);
		FrontArc[i][Z] = 0.0;

		BackArc[i][X] = MaxRadius*cos(fAngle);
		BackArc[i][Y] = MaxRadius*sin(fAngle);
		BackArc[i][Z] = 0.0;
	}	
}


void DrawWorkspace(void)
{
	int i;


	CreatePoints();

	WorkSpaceMaterial();

	glPushMatrix();
	glTranslatef(CenterOffset, 0.0, PosHeigth);
	glBegin(GL_LINE_STRIP);
	for (i=0; i<=iNrSegments; i++)
	{
		glVertex3f(FrontArc[i][X], FrontArc[i][Y], FrontArc[i][Z]);
	}
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(CenterOffset, 0.0, NegHeight);
	glBegin(GL_LINE_STRIP);
	for (i=0; i<=iNrSegments; i++)
	{
		glVertex3f(FrontArc[i][X], FrontArc[i][Y], FrontArc[i][Z]);
	}
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(CenterOffset, 0.0, PosHeigth);
	glBegin(GL_LINE_STRIP);
	for (i=0; i<=iNrSegments; i++)
	{
		glVertex3f(BackArc[i][X], BackArc[i][Y], BackArc[i][Z]);
	}
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(CenterOffset, 0.0, NegHeight);
	glBegin(GL_LINE_STRIP);
	for (i=0; i<=iNrSegments; i++)
	{
		glVertex3f(BackArc[i][X], BackArc[i][Y], BackArc[i][Z]);
	}
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(CenterOffset, 0.0, 0.0);
	glRotatef(LeftSideArc*180/Pi, 0.0, 0.0, 1.0); //To the left side 0.5 rad
	glBegin(GL_LINE_LOOP);
		glVertex3f(MaxRadius, 0.0, NegHeight);
		glVertex3f(MinRadius, 0.0, NegHeight);
		glVertex3f(MinRadius, 0.0, PosHeigth);
		glVertex3f(MaxRadius, 0.0, PosHeigth);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(CenterOffset, 0.0, 0.0);
	glRotatef(RightSideArc*180/Pi, 0.0, 0.0, 1.0); //To the right only 0.4 rad
	glBegin(GL_LINE_LOOP);
		glVertex3f(MaxRadius, 0.0, NegHeight);
		glVertex3f(MinRadius, 0.0, NegHeight);
		glVertex3f(MinRadius, 0.0, PosHeigth);
		glVertex3f(MaxRadius, 0.0, PosHeigth);
	glEnd();
	glPopMatrix();
}

