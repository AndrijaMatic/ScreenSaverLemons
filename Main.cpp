#define WIN32_LEAN_AND_MEAN
#include <windows.h>			/* must include this before GL/gl.h */
#include <GL/gl.h>				/* OpenGL header file */
#include <GL/glu.h>				/* OpenGL utilities header file */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
 
//Timer
#define TIMER_ID            101
#define TIMER_RATE          50
//Za odre­ivanje koordinata volumena
const GLfloat POL_SIRINE =100.0f;
 
static HDC   hDC=NULL;				//device context
static HGLRC hRC=NULL;				//opengl context
//U kojem modu radi SSaver
//Modovi se dobivaju kikanjem po Control Panel->Display->Screen Saver dialogu
// i kada SS statrta
enum TScrMode {smNone,smConfig,smPassword,smPreview,smSaver};
TScrMode ScrMode=smNone; 
 
 
//ProraÀun kru×nice i njezina obrada
//kru×nice izgledaju kao elipsa zbog koordinatnog sustava
class Kruznica{
private:
	GLUquadricObj   *obj;
	GLfloat 		polumjer;
	GLint			polutnik;
	GLint			polarnik;
	GLfloat			XSmjer,YSmjer;				//Smjer kretanja
	GLfloat 		X,Y;						//Translacija
	GLfloat         kut;						//Kut rotacije	
public:
	Kruznica(){									//Konstruktor
		polumjer=25;
		polutnik=40;
		polarnik=40;
		X=0.0f,Y=0.0f;
		XSmjer=static_cast<GLfloat>(rand()%8-4);
		YSmjer=static_cast<GLfloat>(rand()%8-4);
		kut=0.0f;
		obj=gluNewQuadric();
		gluQuadricDrawStyle(obj,GLU_FILL);
		gluQuadricNormals(obj, GLU_SMOOTH);
		   gluQuadricTexture ( obj, GL_TRUE    );
	}
	~Kruznica(){gluDeleteQuadric(obj);}			//Destruktor
	int CrtajIducu();
}Krug[3];

int Kruznica::CrtajIducu()
	{							//Crta iduŠu kru×nicu
		kut=kut+5.0f;
		if(kut>=360.0f)
			kut=0.0f;

		if((X<-POL_SIRINE) || (X>POL_SIRINE))
			XSmjer=-XSmjer;
		if((Y<-POL_SIRINE) || (Y>POL_SIRINE))
			YSmjer=-YSmjer;
			X=X-XSmjer;
			Y=Y-YSmjer;
			glPushMatrix();
			
			glTranslatef(X,Y,0.0f);
			glRotatef(kut,1,1,1);
		    gluSphere(obj,polumjer,polutnik,polarnik);
		    glPopMatrix();
			
			return 0;
		}


/*

   Usage:
   load_texture("earth_r.raw", 640, 320, 3);

   Notice how you don't need to repeat all those texture parameters?
*/ //////////////////////////////////////////////////////////////////////////
void LoadAllTextures(char *file_name="smaili.raw", int width=160, int height=160, int depth=3) 
{
   GLubyte *raw_bitmap ;
   FILE *file;

   if ((file = fopen(file_name, "rb"))==NULL )
   {
      MessageBox(NULL,"ERROR","File Not Found:smaili.raw",MB_SYSTEMMODAL |MB_ICONSTOP );
      exit   ( 1 );
   }

   raw_bitmap = (GLubyte *) malloc ( width * height * depth * ( sizeof(GLubyte)) );

   if (raw_bitmap == NULL)
   {
      printf ( "Cannot allocate memory for texture\n" );
      fclose ( file);
      exit   ( 1 );
   }
 
   fread  ( raw_bitmap , width * height * depth, 1 , file );
   fclose ( file);
 
   //  Set Filtering type
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  
 

   //  Set Texture Evironment
   glTexEnvf ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL  );

   //  Build Mipmaps
   gluBuild2DMipmaps ( GL_TEXTURE_2D, GL_RGB, 160, 160,
                       GL_RGB, GL_UNSIGNED_BYTE, raw_bitmap );
 
   //  Free up the array
   free ( raw_bitmap );
 
}


//Iniciranje openGL scene
void Iniciranje(void)
{
	LoadAllTextures();
//Koordinate i podaci za svijetlo
GLfloat ambientLight[]   = { 0.3f, 0.3f, 0.3f, 1.0f };
GLfloat diffuseLight[]   = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat lightPos[]       = { 0.f, 0.0f, -50.0f, 1.0f };
GLfloat ambientLight2[]  = { 0.7f, 0.2f, 0.2f, 1.0f };
GLfloat diffuseLight2[]  = { 1.0f, 0.4f, 0.3f, 1.0f };
GLfloat lightPos2[]      = { -50.f, -50.0f, -50.0f, 1.0f };
 glEnable ( GL_TEXTURE_2D );
//Osposobljavamje svijetlosti
glEnable(GL_LIGHTING);
// PodeÜavanje i osposobljavanje svijetla 0
glLightfv(GL_LIGHT0,GL_AMBIENT,ambientLight);
glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuseLight);
glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
glEnable(GL_LIGHT0);
// PodeÜavanje i osposobljavanje svijetla 1
glLightfv(GL_LIGHT1,GL_AMBIENT,ambientLight2);
glLightfv(GL_LIGHT1,GL_DIFFUSE,diffuseLight2);
glLightfv(GL_LIGHT1,GL_POSITION,lightPos2);
glLightf(GL_LIGHT1,GL_SPOT_EXPONENT,128.0f);
glEnable(GL_LIGHT1);

  glEnable(GL_CULL_FACE);  
// Enable color tracking
glEnable(GL_COLOR_MATERIAL);
// Set Material properties to follow glColor values
glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
glClearColor(0.0f, 0.0f, 0.0f, 1.0f );
SwapBuffers(hDC);
glAccum(GL_LOAD,1.0);
SwapBuffers(hDC); 
glFlush();
}

/*=========================================
Crtanje sadr×aja
Ono Üto se vidi na monitoru
==========================================*/
//WM_PAINT
void display(void)
{  
 
 
// glClear(GL_COLOR_BUFFER_BIT);//LiÜÀenje buffer 
	glAccum(GL_RETURN,0.0);
	Krug[0].CrtajIducu();
	 
	Krug[1].CrtajIducu();
	 
	Krug[2].CrtajIducu();
 
    
  glFlush(); 
  	SwapBuffers(hDC);
  
}

/*=========================================
PodeÜavanje koordinatnog sustava GL scene
==========================================*/
//WM_SIZE
void Resize(int w=800,int h=600)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho (-POL_SIRINE, POL_SIRINE*1.3, -POL_SIRINE*1.3, POL_SIRINE,-POL_SIRINE/2,POL_SIRINE/2);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


/*======================================
Glavna funkcija za obradu doga­aja
=====================================*/
LONG WINAPI
WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{          
	//Varijable za MiÜa
    static WORD OldxPos=0, OldyPos=0;
	static BOOL bMouseFlag=FALSE;
		   WORD xPos, yPos;

    switch(uMsg) {
	case WM_TIMER:
		InvalidateRect(hWnd,NULL,FALSE);
		return 0;

	case WM_PAINT:
		display();
	 

	
		ValidateRect(hWnd,NULL);
		return 0;
 
	case WM_CREATE:
		SetTimer(hWnd,TIMER_ID,TIMER_RATE,NULL);
		ShowCursor(FALSE);
		return 0;
 
    case WM_SIZE:
		Resize(LOWORD(lParam), HIWORD(lParam));
		display();
		return 0;

/*	case WM_MOUSEMOVE:
		//Da li je miÜ stvarno pomaknut
		xPos=LOWORD(lParam); yPos=HIWORD(lParam);
		if(!bMouseFlag)
		{	OldyPos=yPos;OldxPos=xPos; bMouseFlag=TRUE; }
		if((OldxPos+2>xPos) &&(OldxPos-2>xPos)) {
			PostMessage(hWnd,WM_CLOSE,0,0);return 0;}
		if((OldyPos+2>yPos) &&(OldyPos-2>yPos)) {
			PostMessage(hWnd,WM_CLOSE,0,0);return 0;}
		else {
			OldyPos=yPos;OldxPos=xPos; return 0;}*/
//Ostali doga­aji koji mogu prekinuti SS
	case WM_CHAR:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDOWN:
			 PostMessage(hWnd,WM_CLOSE,0,0);
			 return 0;
//Zatvaranje SS
	case WM_CLOSE:
		KillTimer(hWnd,TIMER_ID);
		ShowCursor(TRUE);
		PostQuitMessage(0);
		return 0;
 
	
    }
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

/*=============================================
Iniciranje GL prozora
============================================*/
HWND
CreateOpenGLWindow( int width, int height)
{
    int         pf;
    HWND        hWnd;
    WNDCLASS    wc;
    static HINSTANCE hInstance=NULL;
//Registrira klasu samo jednom, koristi hInstance kao flag
	if (!hInstance) {
	hInstance = GetModuleHandle(NULL);
	wc.style         =  CS_OWNDC;			//Da se ne mora stalno pozivati GetDC()
	wc.lpfnWndProc   = (WNDPROC)WindowProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(NULL,IDI_WINLOGO );
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "Screen Saver";
	if (!RegisterClass(&wc)) {
	    MessageBox(NULL, "RegisterClass() failed:  "
		       "Cannot register window class.", "Error", MB_OK);
	    return NULL;
		}
	}

//Napravi prozor
	if (!(hWnd=CreateWindow(    "Screen Saver",						// Class Name
								"SS",						// Window Title
								WS_POPUP |							// Window Style
								WS_CLIPSIBLINGS |					
								WS_CLIPCHILDREN,					 
								0, 0,								// Window Position
								width,								// Width
								height,								// Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								hInstance,							// Instance
								NULL)))	

    {
	MessageBox(NULL,"CreateWindow() failed:  Cannot create a window.",
		   "Error",MB_OK);
	return NULL;
    }
    /*==========================================
    Podesi pixel format
    =======================================*/
	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		32,					      					// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		0,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
    hDC = GetDC(hWnd);
    pf = ChoosePixelFormat(hDC, &pfd);  //odaberi pixel format
    if (pf == 0) {
	MessageBox(NULL, "ChoosePixelFormat() failed:  "
		   "Cannot find a suitable pixel format.", "Error", MB_OK); 
	return 0;
    } 
 
    if (SetPixelFormat(hDC, pf, &pfd) == FALSE) { //podesi 
	MessageBox(NULL, "SetPixelFormat() failed:  "
		   "Cannot set format specified.", "Error", MB_OK);
	return 0;
    } 
 

  

    return hWnd;
}

/*==========================================
Poruke koje se dobiju kad se klikne na Change i Settings
u Display Properties
=======================================*/
void ConfigPoruka(void)
{
	MessageBeep(MB_OK);
	MessageBox(NULL,"Screen Saver: \"Limuni\" \n Izradio: Andrija MatiŠ \n E-mail: andmatic@inet.hr \n rujan 2001.",
		"O Screen Saveru", MB_OK | MB_APPLMODAL | MB_ICONINFORMATION) ; }

void PassPoruka(void)
{
		MessageBeep(MB_OK);
	MessageBox(NULL,"Ovaj Screen Saver nema password \n Izradio: Andrija MatiŠ \n E-mail: andmatic@inet.hr \n rujan 2001.",
		"O Screen Saveru", MB_OK | MB_APPLMODAL | MB_ICONINFORMATION) ; }

/*========================================
Ulaz u glavnu funkciju
========================================*/
int APIENTRY
WinMain(HINSTANCE hCurrentInst, HINSTANCE hPreviousInst,
	LPSTR lpszCmdLine, int nCmdShow)
{

    HWND  hWnd=NULL;
    MSG   msg;
	srand((unsigned int)time( NULL ) ); //Za generator slu. brojeva

	char *c=GetCommandLine();			//Daj mi komandnu liniju
 
//RazliÀite verzije Windowsa za razliÀite argumente
//Do­i do mjesta odakle poÀinju opcije ScrSav
  if (*c=='\"') {c++; while (*c!=0 && *c!='\"') c++;} else {while (*c!=0 && *c!=' ') c++;}
  if (*c!=0) c++;
  while (*c==' ') c++;
  if (*c==0) {ScrMode=smConfig;ConfigPoruka(); return 0;}  //Konfiguracja
//Prikazivanje u malom SS prozoru Control Panel->Display->Scr. Saver
//Nisam napisao kod za ovu opciju pa Še mali SS biti crne boje 
  else
  { if (*c=='-' || *c=='/') c++;
    if (*c=='p' || *c=='P' || *c=='l' || *c=='L')
    { c++; while (*c==' ' || *c==':') c++;
	hWnd=(HWND)atoi(c);
      ScrMode=smPreview;
	  return 0;
    }
//SS je startao
    else if (*c=='s' || *c=='S') {
		//Provjera dali je SS veŠ aktivan
		hWnd=FindWindow("Screen Saver","SS");
		if(hWnd==NULL)
		ScrMode=smSaver;
		else
		return 0;}

//Konfiguriranje SS-a
    else if (*c=='c' || *c=='C') {c++; while (*c==' ' || *c==':') c++; if (*c==0)  ScrMode=smConfig;ConfigPoruka(); return 0;}
//Lozinka
    else if (*c=='a' || *c=='A') {c++; while (*c==' ' || *c==':') c++; hWnd=(HWND)atoi(c); ScrMode=smPassword; PassPoruka(); return 0;}
  }
//SS je startao ?
if(ScrMode==smSaver)
 
  {
    hWnd = CreateOpenGLWindow(GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN)); // :-)
    if (hWnd == NULL)
	exit(1);
  }

	 
		hRC = wglCreateContext(hDC);
		wglMakeCurrent(hDC, hRC);
		SetForegroundWindow(hWnd);						// Slightly Higher Priority
		SetFocus(hWnd);									// Sets Keyboard Focus To The Window

	Resize();								//PodeÜavanje GL scene
	Iniciranje();							//Iniciranje GL scene
	ShowWindow(hWnd,SW_MAXIMIZE);
//Glavna petlja
	while(GetMessage(&msg,hWnd,0,0)) {
	TranslateMessage(&msg);
	DispatchMessage(&msg);
    }

//Izlaz iz programa
	wglMakeCurrent(NULL, NULL);
	ReleaseDC(hWnd,hDC);
	wglDeleteContext(hRC);
	DestroyWindow(hWnd);
	return msg.wParam;
}

