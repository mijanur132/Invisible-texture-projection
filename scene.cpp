#include "stdafx.h"

#include "scene.h"

#include "V3.h"
#include "M33.h"
#include "ppc.h"
#include "TMesh.h"

Scene* scene;

using namespace std;

#include <iostream>


Scene::Scene() {


	gui = new GUI();
	gui->show();

#if 0
	prepare frame buffers
#endif 

		int u0 = 20;
	int v0 = 100;
	int h = 400;
	int w = 700;

	fb0 = new FrameBuffer(u0, v0, w, h, 0);
	fb0->label("camera 0");
	fb0->show();
	fb0->redraw();

	fb1 = new FrameBuffer(u0 + w + 30, v0, w, h, 0);
	fb1->label("camera 1");
	fb1->show();
	fb1->redraw();

	fb3 = new FrameBuffer(u0 + 2*w + 30, v0, w, h, 0);
	fb3->label("camera 1");
	fb3->show();
	fb3->redraw();

	gui->uiw->position(u0, v0 + h + 50);

	V3 col = V3(0, 0, 1.0f);
	V3 col1 = V3(1.0f, 0, 0);


#if 0
	prepare the tmesh objects of the scene
#endif 

	tmeshesN = 3;
	tmeshes = new TMesh[tmeshesN];

	V3 cc0(0.0f, 0.0f, 0.0f);

	tmeshes[0].LoadBin("geometry/auditorium.bin");
	//tmeshes[0].DrawPlanerRect(cc0, 100, col1.GetColor());	
	tmeshes[0].Rotate(tmeshes[0].GetCenter(), V3(1, 0, 0), -90.0f);
	tmeshes[0].Rotate(tmeshes[0].GetCenter(), V3(0, 1, 0), 180.0f);
	tmeshes[0].SetCenter(cc0);
	tmeshes[0].onFlag = 0;


	tmeshes[1].LoadBin("geometry/teapot1K.bin");
	//tmeshes[1].LoadBin("geometry/teapot57K.bin");
	tmeshes[1].SetCenter(V3(0.0f, 0.0f, -100.0f));  //***************************center of 1
	tmeshes[1].onFlag = 1;
	//tmeshes[1].Rotate(tmeshes[1].GetCenter(), V3(1, 0, 0), -90);

	
	tmeshes[2].DrawPlanerRect(V3(0,0,-50), 10, col1.GetColor());	
	tmeshes[2].onFlag = 1;


#if 0
	prepare cameras
#endif 

	float hfov = 90.0f;
	ppc0 = new PPC(hfov, fb0->w, fb0->h);
	ppc1 = new PPC(hfov, fb1->w, fb1->h);

	ppc0->SetPose(V3(0, 0, -5), V3(0, 0, -100), V3(0, 1, 0));  //ppc0 is the projector cam
	float roll = -90.0f;
	//ppc0->Roll(roll);
	ppc1->PanLeftRight(0.0f);

#if 0
	our lightsource.
#endif

		L = V3(0.0f, 0.0f, 0.0f);  //*********************************************center of light camera
	float hfov1 = 90.0f;
	LightSrcPPC = new PPC(hfov1, fb1->w, fb1->h);
	//LightSrcPPC->SetPose(L, tmeshes[1].GetCenter(), V3(0, 1, 0));
	ka = 0.2f;


#if 0
	our drawing or other rendering operation
#endif

		fb0->redraw();
	fb1->redraw();

	for (int i = 0; i < 1; i++)
	{
		cout << i << endl;
		RenderProjector2makeInvisible(fb0, fb1, ppc0, ppc1);
		//ppc0->Roll(roll * -1.0f);
		//ppc0->PanLeftRight(-0.30f);
		//ppc0->Roll(roll);
		//ppc1->PanLeftRight(-0.20f);
		tmeshes[2].SetCenter(tmeshes[2].GetCenter() + V3(0.02, 0, 0));
		
	}



	//Render(fb1, ppc1);
	//Render(fb, ppc,  &tmeshes[1]);
	//Render();

}
void Scene::DBG() {
	RenderProjector2makeInvisible(fb0, fb1, ppc0, ppc1);
	tmeshes[2].SetCenter(tmeshes[2].GetCenter() + V3(1, 0, 0));
	if (tmeshes[2].GetCenter()[0] > 25) {
		tmeshes[2].SetCenter(tmeshes[2].GetCenter() + V3(-50, 0, 0));
	}
}


void Scene::RenderProjector2makeInvisible(FrameBuffer* fb0, FrameBuffer* fb1, PPC* ppc0, PPC* ppc1) {
#if 0
	Prepare two cameraand framebuffer: ppc0, fb0 for the projectorand ppc1, fb1 for the output camera
#endif

	fb0->SetBGR(0xFFFFFFFF);
	fb0->ClearZB();
	fb0->ClearZB(fb0->zbL1);

	fb1->SetBGR(0xFFFFFFFF);
	fb1->ClearZB();
	fb1->ClearZB(fb1->zbL1);

	fb3->SetBGR(0xFFFFFFFF);
	fb3->ClearZB();
	fb3->ClearZB(fb1->zbL1);

#if 0
	first unflag the blocker
		calculate the unblcoked image on fb1and save it into the fb1->pix1
		set blocker flag 1 after
#endif
		tmeshes[2].onFlag = 0;
	for (int tmi = 0; tmi < tmeshesN; tmi++) {
		if (!tmeshes[tmi].onFlag)
			continue;
		tmeshes[tmi].RenderFilled(fb1, ppc1);		
	}

	fb1->copyPix(fb1->pix, fb1->pix1);  //source destination
	

	tmeshes[2].onFlag = 1;

	V3 col1 = V3(0, 1, 1);
	fb1->Draw3DPoint(ppc0->C, ppc1, col1.GetColor(), 10);
	
#if 0
	Now call the renderFilledProjector() function to update the camera.
#endif

		for (int tmi = 0; tmi < tmeshesN; tmi++) {
			if (!tmeshes[tmi].onFlag)
				continue;
			tmeshes[tmi].RenderFilled(fb1, ppc1);
			tmeshes[tmi].RenderFilled(fb3, ppc1);
			
		}


	for (int tmi = 0; tmi < tmeshesN; tmi++)
	{
		if (!tmeshes[tmi].onFlag)
			continue;
		tmeshes[tmi].DetectBlocker4mProjector(fb0,fb1, ppc0, ppc1);

	}

	

#if 0
	perform displaying operation
#endif
	fb3->redraw();
	fb0->redraw();
	fb1->redraw();
	Fl::check();

}

void Scene::Render() {
	for (int i = 0; i < 1; i++)
	{
		//tmeshes[1].Rotate(tmeshes[1].GetCenter(), V3(1, 0, 0), 2);
		cout << i << endl;
		Render(fb1, ppc1);
		ppc->PanLeftRight(0.5);
		Render(fb, ppc);

		fb0->redraw();
		fb1->redraw();
		Fl::check();
	}

}


//not improtant right now.

void Scene::Render(FrameBuffer* rfb, PPC* rppc) {

	for (int i = 0; i < 1; i++)
	{
		cout << i << endl;


		rfb->SetBGR(0xFFFFFFFF);
		rfb->ClearZB();
		rfb->ClearZB(rfb->zbL1);

		for (int tmi = 0; tmi < tmeshesN; tmi++) {
			if (!tmeshes[tmi].onFlag)
				continue;
			tmeshes[tmi].RenderShadowZmap(rfb, LightSrcPPC, rfb->zbL1);

		}

		for (int tmi = 0; tmi < tmeshesN; tmi++) {
			if (!tmeshes[tmi].onFlag)
				continue;

			V3 C(1.0f, 0.0f, 0.0f);
			tmeshes[tmi].Light(C, L, ka);
			tmeshes[tmi].RenderFilledWithShadow(rfb, rppc, rfb->zbL1, LightSrcPPC, C, L, ka);
			//tmeshes[tmi].DrawWireFrame(rfb, rppc, C.GetColor());


		}


		V3 col1 = V3(1, 0, 0);
		rfb->Draw3DPoint(LightSrcPPC->C, rppc, col1.GetColor(), 10);

		rfb->redraw();
		Fl::check();





		//tmeshes[1].Rotate(tmeshes[1].GetCenter(),V3(0,1,0),0.20f);
		//L=L.RotatePoint((tmeshes[1].GetCenter()), V3(0, 1, 0), -0.30f);
		//L = L + (V3(0.100f, 00.0f, 0.0f));
		//L = V3(140.0f, 0.0f, -100.0f);
		//LightSrcPPC->SetPose(L, tmeshes[1].GetCenter(), V3(0, 1, 0));
		//rppc->PanLeftRight(-10.0f);	


	}


	//tmeshes[0].SetCenter(V3 (50.0f, -15.0f, -100.0f));

}





void Scene::NewButton() {
	cerr << "INFO: pressed New Button" << endl;
}