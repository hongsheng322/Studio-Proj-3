#include "Level2.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "MeshBuilder.h"
#include "../Application.h"
#include "Utility.h"
#include "LoadTGA.h"
#include <sstream>
#include "KeyboardController.h"
#include "MouseController.h"
#include "SceneManager.h"
#include "GraphicsManager.h"
#include "ShaderProgram.h"
#include "../EntityManager.h"

#include "../GenericEntity.h"
#include "../GroundEntity.h"
#include "../TextEntity.h"
#include "../SpriteEntity.h"
#include "../Light.h"
#include "../SkyBox/SkyBoxEntity.h"
#include "../ReadFile/FileManager.h"
#include "../GameUI/GameUI.h"
#include "../MoneyManager/Money.h"
#include "..//Trees/Trees.h"
#include "../Enemy/RadarScan.h"
#include "../Enemy/Shield.h"
#include "RenderHelper.h"
#include "..//Enemy/Turrets/Turrets.h"

#include <iostream>
using namespace std;

Level2* Level2::sInstance = new Level2(SceneManager::GetInstance());

Level2::Level2()
	:theMouse(NULL),
	theKeyboard(NULL)
{
}

Level2::Level2(SceneManager* _sceneMgr)
	:theMouse(NULL),
	theKeyboard(NULL)
{
	_sceneMgr->AddScene("Level2", this);
}

Level2::~Level2()
{
	if (theMouse)
	{
		delete theMouse;
		theMouse = NULL;
	}
	if (theKeyboard)
	{
		delete theKeyboard;
		theKeyboard = NULL;
	}
}

void Level2::Init()
{
	currProg = GraphicsManager::GetInstance()->LoadShader("default", "Shader//Texture.vertexshader", "Shader//Texture2.fragmentshader");

	// Tell the shader program to store these uniform locations
	currProg->AddUniform("MVP");
	currProg->AddUniform("MV");
	currProg->AddUniform("MV_inverse_transpose");
	currProg->AddUniform("material.kAmbient");
	currProg->AddUniform("material.kDiffuse");
	currProg->AddUniform("material.kSpecular");
	currProg->AddUniform("material.kShininess");
	currProg->AddUniform("lightEnabled");
	currProg->AddUniform("numLights");
	currProg->AddUniform("lights[0].type");
	currProg->AddUniform("lights[0].position_cameraspace");
	currProg->AddUniform("lights[0].color");
	currProg->AddUniform("lights[0].power");
	currProg->AddUniform("lights[0].kC");
	currProg->AddUniform("lights[0].kL");
	currProg->AddUniform("lights[0].kQ");
	currProg->AddUniform("lights[0].spotDirection");
	currProg->AddUniform("lights[0].cosCutoff");
	currProg->AddUniform("lights[0].cosInner");
	currProg->AddUniform("lights[0].exponent");
	currProg->AddUniform("lights[1].type");
	currProg->AddUniform("lights[1].position_cameraspace");
	currProg->AddUniform("lights[1].color");
	currProg->AddUniform("lights[1].power");
	currProg->AddUniform("lights[1].kC");
	currProg->AddUniform("lights[1].kL");
	currProg->AddUniform("lights[1].kQ");
	currProg->AddUniform("lights[1].spotDirection");
	currProg->AddUniform("lights[1].cosCutoff");
	currProg->AddUniform("lights[1].cosInner");
	currProg->AddUniform("lights[1].exponent");
	currProg->AddUniform("colorTextureEnabled");
	currProg->AddUniform("colorTexture");
	currProg->AddUniform("textEnabled");
	currProg->AddUniform("textColor");

	// Tell the graphics manager to use the shader we just loaded
	GraphicsManager::GetInstance()->SetActiveShader("default");

	lights[0] = new Light();
	GraphicsManager::GetInstance()->AddLight("lights[0]", lights[0]);
	lights[0]->type = Light::LIGHT_DIRECTIONAL;
	lights[0]->position.Set(0, 20, 0);
	lights[0]->color.Set(1, 1, 1);
	lights[0]->power = 1;
	lights[0]->kC = 1.f;
	lights[0]->kL = 0.01f;
	lights[0]->kQ = 0.001f;
	lights[0]->cosCutoff = cos(Math::DegreeToRadian(45));
	lights[0]->cosInner = cos(Math::DegreeToRadian(30));
	lights[0]->exponent = 3.f;
	lights[0]->spotDirection.Set(0.f, 1.f, 0.f);
	lights[0]->name = "lights[0]";

	lights[1] = new Light();
	GraphicsManager::GetInstance()->AddLight("lights[1]", lights[1]);
	lights[1]->type = Light::LIGHT_DIRECTIONAL;
	lights[1]->position.Set(1, 1, 0);
	lights[1]->color.Set(1, 1, 0.5f);
	lights[1]->power = 0.4f;
	lights[1]->name = "lights[1]";

	currProg->UpdateInt("numLights", 1);
	currProg->UpdateInt("textEnabled", 0);

	// Create the playerinfo instance, which manages all information about the player
	playerInfo = CPlayerInfo::GetInstance();
	playerInfo->Init();


	//	std::cout << _DEBUG << std::endl;

	// Create and attach the camera to the scene
	camera.Init(playerInfo->GetPos(), playerInfo->GetTarget(), playerInfo->GetUp());
	playerInfo->AttachCamera(&camera);
	GraphicsManager::GetInstance()->AttachCamera(&camera);

	// Create entities into the scene

	BombTarget = Create::Bomb3D("BombTarget", Vector3(0, 10, 0), Vector3(15, 15, 15));
	IndicatorTarget = Create::Entity("IndicatorTarget", Vector3(0, -10, 0), Vector3(10, 10, 10));

	groundEntity = Create::Ground("SKYBOX_BOTTOM", "SKYBOX_BOTTOM");
	Create::Sprite2DObject("crosshair", Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f));

	SkyBoxEntity* theSkyBox = Create::SkyBox("SKYBOX_FRONT", "SKYBOX_BACK",
		"SKYBOX_LEFT", "SKYBOX_RIGHT",
		"SKYBOX_TOP", "SKYBOX_BOTTOM");

	// Customise the ground entity
	groundEntity->SetPosition(Vector3(0, 0, 0));
	groundEntity->SetScale(Vector3(100.0f, 150.0f, 150.0f));
	groundEntity->SetGrids(Vector3(10.0f, 1.0f, 10.0f));
	playerInfo->SetTerrain(groundEntity);

	// Setup the 2D entities
	float halfWindowWidth = Application::GetInstance().GetWindowWidth() / 2.0f;
	float halfWindowHeight = Application::GetInstance().GetWindowHeight() / 2.0f;
	float fontSize = 25.0f;
	float halfFontSize = fontSize / 2.0f;
	//Creating textOBj
	textObj[0] = Create::Text2DObject("text", Vector3(-halfWindowWidth, halfWindowHeight - halfFontSize, 0.0f), "", Vector3(fontSize, fontSize, fontSize), Color(0.0f, 1.0f, 0.0f));
	textObj[1] = Create::Text2DObject("text", Vector3(-halfWindowWidth + fontSize * 2.f, -halfWindowHeight + fontSize * 2.5f, 0.1f), "", Vector3(fontSize, fontSize, fontSize), Color(0.0f, 1.0f, 0.0f));
	textObj[3] = Create::Text2DObject("text", Vector3(0 - fontSize * 2.f, halfWindowHeight - fontSize, 0.1f), "", Vector3(fontSize, fontSize, fontSize), Color(0.0f, 1.0f, 0.0f));

	theKeyboard = new CKeyboard();
	theKeyboard->Create(playerInfo);
	theMouse = new CMouse();
	theMouse->Create(playerInfo);
	//Trees::GetInstance()->init();
	FileManager::GetInstance()->ReadFile("ReadFiles//Level2.csv");
	FileManager::GetInstance()->CreateObjects();
	Money::GetInstance()->SetMoney(100);
	Money::GetInstance()->SetMoneyRate(10);
	elapsed_time = 0.0f;
	complete_time = 180.0f;

	Trees::GetInstance()->SetMaxCount(20);
	Trees::GetInstance()->SetSpawnRate(5);
	GameUI::GetInstance()->SetLevelName("Level2");
	GameUI::SetBombRender(false);

	WinLose = WinLoseScreen::GetInstance();
}

void Level2::Update(double dt)
{
	elapsed_time += (float)dt;
	if (!GameUI::GetInstance()->GetBombRender())
	{
		BombTarget->SetPosition(Vector3(1000, 0, 0));
	}
	else
	{
		BombTarget->SetPosition(test);
		IndicatorTarget->SetPosition(Vector3(1000, 0, 0));
	}

	if (elapsed_time >= 3.f)
	{	
		complete_time -= (float)dt;
		if (complete_time <= 0)
			complete_time = 0.0f;
		//Hardware Abstraction
		if (theKeyboard)
			theKeyboard->Read((float)dt);

		if (theMouse)
		{
			theMouse->Read((float)dt);
			theMouse->SetTroopMovement(*BombTarget, *IndicatorTarget, test);
		}
		spawnDelay += (float)dt;

		if (spawnDelay >= coolDown)
		{
			EntityManager::GetInstance()->GenerateNinja(groundEntity, dt);
			spawnDelay = 0.f;
		}

		Shield::GetInstance()->Update(dt);
		if (GameUI::GetInstance()->GetShieldIsPressed())
		{
			Shield::GetInstance()->Update(dt);
		}
		RadarScan::GetInstance()->Update(dt);
		Money::GetInstance()->UpdateMoney(dt);
		GameUI::GetInstance()->Update(groundEntity);
		// Update our entities
		EntityManager::GetInstance()->Update(dt);
		// Update the player position and other details based on keyboard and mouse inputs
		playerInfo->Update(dt);
	}

	// THIS WHOLE CHUNK TILL <THERE> CAN REMOVE INTO ENTITIES LOGIC! Or maybe into a scene function to keep the update clean
	if (KeyboardController::GetInstance()->IsKeyDown('1'))
		glEnable(GL_CULL_FACE);
	if (KeyboardController::GetInstance()->IsKeyDown('2'))
		glDisable(GL_CULL_FACE);
	if (KeyboardController::GetInstance()->IsKeyDown('3'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (KeyboardController::GetInstance()->IsKeyDown('4'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if (KeyboardController::GetInstance()->IsKeyDown('5'))
	{
		lights[0]->type = Light::LIGHT_POINT;
	}
	else if (KeyboardController::GetInstance()->IsKeyDown('6'))
	{
		lights[0]->type = Light::LIGHT_DIRECTIONAL;
	}
	else if (KeyboardController::GetInstance()->IsKeyDown('7'))
	{
		lights[0]->type = Light::LIGHT_SPOT;
	}

	if (KeyboardController::GetInstance()->IsKeyDown('I'))
		lights[0]->position.z -= (float)(10.f * dt);
	if (KeyboardController::GetInstance()->IsKeyDown('K'))
		lights[0]->position.z += (float)(10.f * dt);
	if (KeyboardController::GetInstance()->IsKeyDown('J'))
		lights[0]->position.x -= (float)(10.f * dt);
	if (KeyboardController::GetInstance()->IsKeyDown('L'))
		lights[0]->position.x += (float)(10.f * dt);
	if (KeyboardController::GetInstance()->IsKeyDown('O'))
		lights[0]->position.y -= (float)(10.f * dt);
	if (KeyboardController::GetInstance()->IsKeyDown('P'))
		lights[0]->position.y += (float)(10.f * dt);

	GraphicsManager::GetInstance()->UpdateLights(dt);

	// Update the 2 text object values. NOTE: Can do this in their own class but i'm lazy to do it now :P
	// Eg. FPSRenderEntity or inside RenderUI for LightEntity
	std::ostringstream ss;
	ss.precision(5);
	float fps = (float)(1.f / dt);
	ss << "FPS: " << fps;
	textObj[0]->SetText(ss.str());
	ss.str("");
	ss << Money::GetInstance()->GetMoney();
	textObj[1]->SetText(ss.str());
	ss.str("");
	ss.precision(3);
	ss << complete_time;
	textObj[3]->SetText(ss.str());
	if (KeyboardController::GetInstance()->IsKeyPressed('0') || EntityManager::GetInstance()->GetOtherList().size() == 1)
	{
		SceneManager::GetInstance()->SetActiveScene("Level3");
	}

	if (complete_time <= 0.0f || Money::GetInstance()->GetMoney() <= 0)
	{
		WinLose->SetStates(1);	// Lose Screen
		WinLose->SetLevel("2");	// Level 2
		WinLose->SetSwitchLevel(true);	// Boolean true to restart this level
		SceneManager::GetInstance()->SetActiveScene("WinLoseScreen");
	}
}

void Level2::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GraphicsManager::GetInstance()->UpdateLightUniforms();

	// Setup 3D pipeline then render 3D
	GraphicsManager::GetInstance()->SetPerspectiveProjection(45.0f, 4.0f / 3.0f, 0.1f, 10000.0f);
	GraphicsManager::GetInstance()->AttachCamera(&camera);
	EntityManager::GetInstance()->Render();

	//Enable blend mode
	glEnable(GL_BLEND);

	// Setup 2D pipeline then render 2D
	int halfWindowWidth = Application::GetInstance().GetWindowWidth() / 2;
	int halfWindowHeight = Application::GetInstance().GetWindowHeight() / 2;
	GraphicsManager::GetInstance()->SetOrthographicProjection(-halfWindowWidth, halfWindowWidth, -halfWindowHeight, halfWindowHeight, -10, 10);
	GraphicsManager::GetInstance()->DetachCamera();
	EntityManager::GetInstance()->RenderUI();
	MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
	if (elapsed_time <= 3.f)
	{
		//Do you rendering on screen here. Centre of screen is (0,0)
		modelStack.PushMatrix();
		modelStack.Translate(0, 0, 2);
		modelStack.Scale(800, 600, 1);
		RenderHelper::RenderMesh(MeshBuilder::GetInstance()->GetMesh("Loading"));
		modelStack.PopMatrix();
	}
	else
	{
		//Do you rendering on screen here. Centre of screen is (0,0)
		modelStack.PushMatrix();
		modelStack.Translate(0, 0, 0);
		modelStack.Scale(800, 600, 1);
		if (RadarScan::GetInstance()->GetCooldown() || RadarScan::GetInstance()->GetDuration() > 0.f)
		{
			RenderHelper::RenderMesh(MeshBuilder::GetInstance()->GetMesh("UILevel2Rno"));
		}
		else
		{
			RenderHelper::RenderMesh(MeshBuilder::GetInstance()->GetMesh("UILevel2"));
		}		modelStack.PopMatrix();
	}
	glDisable(GL_BLEND);

}

void Level2::Exit()
{
	// Detach camera from other entities
	GraphicsManager::GetInstance()->DetachCamera();
	playerInfo->DetachCamera();

	if (playerInfo->DropInstance() == false)
	{
#if _DEBUGMODE==1
		cout << "Unable to drop PlayerInfo class" << endl;
#endif
	}
	// Delete the lights
	//delete lights[0];
	//delete lights[1];
	GraphicsManager::GetInstance()->RemoveLight("lights[0]");
	GraphicsManager::GetInstance()->RemoveLight("lights[1]");
	EntityManager::GetInstance()->ClearEntityList();
	Trees::GetInstance()->SetCountOfTrees(0);
	theMouse = NULL;
	theKeyboard = NULL;
}
