#include "SceneManager_Play_L2.h"

SceneManager_Play_L2::SceneManager_Play_L2() :
spatialPartitionManager(NULL),
sceneGraph(NULL),
staticSceneGraph(NULL),
dynamicSceneGraph(NULL),
spawnNumber(0)/*,
miniMap(NULL)*/
{
	nodeList.clear();
}

SceneManager_Play_L2::~SceneManager_Play_L2()
{
	SceneManager_Play_L2::Exit();
}

void SceneManager_Play_L2::Init(const int width, const int height, ResourcePool *RM, InputManager* controls)
{
	SceneManagerGameplay::Init(width, height, RM, controls);
	resourceManager.stopAllSounds();
	resourceManager.retrieveSoundas2D("Game_BGM", true, true);
	srand(time(NULL));
	Config();
	Config("Config\\GameStateConfig\\CustomPlayConfig.txt");
	player->GetInstance();
	this->InitShader();
	this->InitSceneGraph();
	LoadHighScore();

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 1000 units
	Mtx44 perspective;
	perspective.SetToPerspective(45.0f, 4.0f / 3.0f, 0.1f, 10000.0f);
	//perspective.SetToOrtho(-80, 80, -60, 60, -1000, 1000);
	projectionStack.LoadMatrix(perspective);

	textMesh = resourceManager.retrieveMesh("FONT");

	playerbullet.setCollidable(true);
	playerbullet.setMesh(resourceManager.retrieveMesh("BULLET"));
	playerbullet.setName("playerbullet");
	playerbullet.setReflectLight(true);
	playerbullet.setHitbox(Vector3(0, 0, 0), 2, 2, 20, "playerbullet");

	theLaser.setCollidable(true);
	theLaser.setMesh(resourceManager.retrieveMesh("LASER"));
	theLaser.setName("theLaser");
	theLaser.setReflectLight(true);
	theLaser.setHitbox(Vector3(0, 0, 0), 2, 2, 100, "theLaser");

	/*
	miniMap = new MiniMap();
	miniMap->Init();
	*/
	//Read high score into vector
	
}

void SceneManager_Play_L2::Config()
{
	lua_State *Lua_Init = lua_open();

	//Load the libs
	luaL_openlibs(Lua_Init);

	//Initialise engine with values from Lua file
	luaL_dofile(Lua_Init, "Lua/GameStateConfig.Lua");

	if (luaL_loadfile(Lua_Init, "Lua/GameStateConfig.Lua") || lua_pcall(Lua_Init, 0, 0, 0))
	{
		printf("error: %s", lua_tostring(Lua_Init, -1));
	}

	//Init gameplay configs
	lua_getglobal(Lua_Init, "CONFIG_PLAY");
	if (!lua_isstring(Lua_Init, -1))
	{
		printf("Invalid config file specified\n");
	}

	string configFile = (string)lua_tostring(Lua_Init, 1);

	SceneManagerGameplay::Config(configFile);
	//Close the lua file
	lua_close(Lua_Init);
}

void SceneManager_Play_L2::Config(string directory)
{
	sceneBranch = TextTree::FileToRead(directory);

	for (vector<Branch>::iterator branch = sceneBranch.childBranches.begin(); branch != sceneBranch.childBranches.end(); ++branch)
	{
		if (branch->branchName == "SpatialPartition")
		{
			// create a spatial partition manager if it is not defined
			if (spatialPartitionManager == NULL)
			{
				spatialPartitionManager = new SpatialPartitionManager();
			}

			int type = 0;
			Vector3 partitionDimension;
			Vector2 partitionDimension2D;
			bool numPartitionBased = true;

			std::cout << "Setting up spatial partitioning" << std::endl;
			for (vector<Attribute>::iterator attri = branch->attributes.begin(); attri != branch->attributes.end(); ++attri)
			{
				Attribute tempAttri = *attri;
				string attriName = tempAttri.name;
				string attriValue = tempAttri.value;

				if (attriName == "Type")
				{
					type = stoi(attriValue);
				}

				else if (attriName == "WorldStart")
				{
					if (type == 3)
					{
						stringToVector(attriValue, world3DStart);
					}

					else
					{
						stringToVector(attriValue, world2DStart);
					}
				}

				else if (attriName == "WorldEnd")
				{
					if (type == 3)
					{
						stringToVector(attriValue, world3DEnd);
					}

					else
					{
						stringToVector(attriValue, world2DEnd);
					}
				}

				else if (attriName == "Partitions")
				{
					if (type == 3)
					{
						stringToVector(attriValue, partitionDimension);
					}

					else
					{
						stringToVector(attriValue, partitionDimension2D);
					}
				}

				else if (attriName == "PartitionBased")
				{
					stringToBool(attriValue, numPartitionBased);
				}
			}

			// 3D spatial partition
			if (type == 3)
			{
				spatialPartitionManager->Init(world3DStart, world3DEnd, partitionDimension, numPartitionBased, resourceManager.retrieveMesh("DEBUG_CUBE"));
			}

			// assume its 2D
			else
			{
				spatialPartitionManager->Init(world2DStart, world2DEnd, partitionDimension2D, numPartitionBased, resourceManager.retrieveMesh("DEBUG_QUAD"));
			}
		}

		else if (branch->branchName == "SceneNode")
		{
			std::cout << "Creating scenenode container!" << std::endl;

			for (vector<Attribute>::iterator attri = branch->attributes.begin(); attri != branch->attributes.end(); ++attri)
			{
				Attribute tempAttri = *attri;
				string attriName = tempAttri.name;
				string attriValue = tempAttri.value;

				if (attriName == "Container")
				{
					GameObject3D* object;
					SceneNode* node;
					int containerSize = stoi(tempAttri.value);
					for (int i = 0; i < containerSize; ++i)
					{
						object = new GameObject3D;
						node = new SceneNode;

						node->SetGameObject(object);

						nodeList.push_back(node);
					}
				}
			}
		}
	}
}

void SceneManager_Play_L2::Update(double dt)
{
	SceneManagerGameplay::Update(dt);
	
	//Update game scene
	if (Player->GetIsAlive())
	{
		UpdatePlayer(dt);
		UpdateEnemy(dt);
	}
	//Update gameover
	else
	{
		CompareHighScore();
		WriteHighScore();
	}
	
	//Update spatial partition
	UpdateSP(dt);
	
	projectileManager.Update(dt, 2000, 2000);
	
	if (Player->GetIsAlive())
		Player->UpdateMovement(tpCamera.GetAimMode(), dt);
	//Update camera position
	tpCamera.UpdatePosition(dynamicSceneGraph->GetChildNode("Player")->GetGameObject()->getPosition(), Player->GetDirection(), dynamicSceneGraph->GetChildNode("Player")->GetChildNode("Head")->GetWorldPosition(), dt, Player->GetIsMoving());
}

void SceneManager_Play_L2::UpdateSP(double dt)
{
	spatialPartitionManager->removeNode(dynamicSceneGraph);
	spatialPartitionManager->addNode(dynamicSceneGraph, this->spatialPartitionManager->type);

	// loop through partitions when player is alive
	if (Player->GetIsAlive())
	{
		for (unsigned i = 0; i < spatialPartitionManager->partitions.size(); ++i)
		{
			// check if partitions contain more than 1 node ( can further optimize by checking if contains dynamic nodes)
			if (spatialPartitionManager->partitions[i]->nodes.size() > 0)
			{
				for (vector<SceneNode*>::iterator j = spatialPartitionManager->partitions[i]->nodes.begin(); j != spatialPartitionManager->partitions[i]->nodes.end(); ++j)
				{
					SceneNode* firstNode = *j;

					for (vector<SceneNode*>::iterator k = j + 1; k != spatialPartitionManager->partitions[i]->nodes.end(); ++k)
					{
						SceneNode* secondNode = *k;
						string boxName = "";

						if (firstNode->getActive() && secondNode->getActive() && CheckSelfCollide(firstNode, secondNode) == false)
						{
							if (firstNode->ProcessCollision(secondNode))
							{
								//Bullets destroy everything
								if (firstNode->GetGameObject()->getName() == "playerbullet")
								{
									//projectileManager.RemoveProjectile(firstNode->GetGameObject());
									//spatialPartitionManager->removeNode(firstNode);
									//dynamicSceneGraph->RemoveChildNode(firstNode);
									spatialPartitionManager->removeNode(secondNode);
									dynamicSceneGraph->RemoveChildNode(secondNode);
									DeleteEnemy(secondNode);
									++score;
									j = spatialPartitionManager->partitions[i]->nodes.begin();
									break;
								}
								else if (secondNode->GetGameObject()->getName() == "playerbullet")
								{
									//projectileManager.RemoveProjectile(secondNode->GetGameObject());
									//spatialPartitionManager->removeNode(secondNode);
									//dynamicSceneGraph->RemoveChildNode(secondNode);
									spatialPartitionManager->removeNode(firstNode);
									dynamicSceneGraph->RemoveChildNode(firstNode);
									DeleteEnemy(firstNode);
									++score;
									j = spatialPartitionManager->partitions[i]->nodes.begin();
									break;
								}

								//Lasers
								if (firstNode->GetGameObject()->getName() == "theLaser")
								{
									//projectileManager.RemoveProjectile(firstNode->GetGameObject());
									//spatialPartitionManager->removeNode(firstNode);
									//dynamicSceneGraph->RemoveChildNode(firstNode);
									spatialPartitionManager->removeNode(secondNode);
									dynamicSceneGraph->RemoveChildNode(secondNode);
									DeleteEnemy(secondNode);
									++score;
									j = spatialPartitionManager->partitions[i]->nodes.begin();
									break;
								}
								else if (secondNode->GetGameObject()->getName() == "theLaser")
								{
									//projectileManager.RemoveProjectile(secondNode->GetGameObject());
									//spatialPartitionManager->removeNode(secondNode);
									//dynamicSceneGraph->RemoveChildNode(secondNode);
									spatialPartitionManager->removeNode(firstNode);
									dynamicSceneGraph->RemoveChildNode(firstNode);
									DeleteEnemy(firstNode);
									++score;
									j = spatialPartitionManager->partitions[i]->nodes.begin();
									break;
								}

								//Enemy_Beholder
								if (firstNode->GetGameObject()->getName() == "Enemy_Beholder" )
								{
									if (secondNode->GetGameObject()->getName() == "Head"
										|| secondNode->GetGameObject()->getName() == "Player"
										|| secondNode->GetGameObject()->getName() == "LeftHand"
										|| secondNode->GetGameObject()->getName() == "RightHand"
										|| secondNode->GetGameObject()->getName() == "LeftLeg"
										|| secondNode->GetGameObject()->getName() == "RightLeg")
									{
										Player->SetIsAlive(false);
										secondNode->setActive(false);
										resourceManager.retrieveSoundas2D("Enemy_Attack", false, false);
										resourceManager.retrieveSoundas2D("Player_Enemy_Beholder", false, false);
										j = spatialPartitionManager->partitions[i]->nodes.begin();
									}
									break;
								}
								else if (secondNode->GetGameObject()->getName() == "Enemy_Beholder")
								{
									if (firstNode->GetGameObject()->getName() == "Head"
										|| firstNode->GetGameObject()->getName() == "Player"
										|| firstNode->GetGameObject()->getName() == "LeftHand"
										|| firstNode->GetGameObject()->getName() == "RightHand"
										|| firstNode->GetGameObject()->getName() == "LeftLeg"
										|| firstNode->GetGameObject()->getName() == "RightLeg")
									{
										Player->SetIsAlive(false);
										firstNode->setActive(false);
										resourceManager.retrieveSoundas2D("Enemy_Attack", false, false);
										resourceManager.retrieveSoundas2D("Player_Enemy_Beholder", false, false);
										j = spatialPartitionManager->partitions[i]->nodes.begin();
									}
									break;
								}
							}
						}
					}
				}
			}
		}
	}
}

void SceneManager_Play_L2::UpdatePlayer(double dt)
{
	//Updates the weapon	
	Player->GetWeapon()->Update(dt);

	if (inputManager->getKey("LMB") && Player->GetWeapon()->getCurrentAmmo() == 0)
		resourceManager.retrieveSoundas2D("No_Ammo", false, false);

	//Only able to fire if weapon is ready and still have bullets
	if (inputManager->getKey("LMB") && Player->GetWeapon()->CanFire() && Player->GetWeapon()->getCurrentAmmo() > 0 && tpCamera.GetAimMode())
	{
		Vector3 FiringPos = dynamicSceneGraph->GetChildNode("Player")->GetGameObject()->getPosition();
		float offSet_X = (float)(-13 * sin(Math::DegreeToRadian(Player->GetAngle() + 157.5)));
		float offSet_Z = (float)(-13 * cos(Math::DegreeToRadian(Player->GetAngle() + 157.5)));

		FiringPos.x += offSet_X;
		FiringPos.y = dynamicSceneGraph->GetChildNode("Player")->GetGameObject()->getPosition().y + 6.75f;
		FiringPos.z += offSet_Z;
		playerbullet.setPosition(FiringPos);
		
		CProjectile* projectile = projectileManager.FetchProjectile(playerbullet, (tpCamera.getTarget() - tpCamera.getPosition()).Normalized(), 1000.f);
		projectile->setRotation(Player->GetAngle(), 0, 1, 0);
		GameObject3D* newProjectile = projectile;

		SceneNode* node;
		node = getNode();
		node->SetGameObject(newProjectile);
		dynamicSceneGraph->AddChildNode(node);
		Player->GetWeapon()->FireWeapon();
		resourceManager.retrieveSoundas2D("Pistol_Fire", true, false);
	}

	//FIRING THE LASER
	if (inputManager->getKey("MMB") && Player->GetWeapon()->CanFire() && Player->GetWeapon()->getCurrentAmmo() > 0 && tpCamera.GetAimMode())
	{
		Vector3 FiringPos = dynamicSceneGraph->GetChildNode("Player")->GetGameObject()->getPosition();
		float offSet_X = (float)(-63 * sin(Math::DegreeToRadian(Player->GetAngle() + 177.5)));
		float offSet_Z = (float)(-63 * cos(Math::DegreeToRadian(Player->GetAngle() + 177.5)));

		FiringPos.x += offSet_X;
		FiringPos.y = dynamicSceneGraph->GetChildNode("Player")->GetGameObject()->getPosition().y + 6.75f;
		FiringPos.z += offSet_Z;
		theLaser.setPosition(FiringPos);

		Vector3 FiringPoint = tpCamera.getTarget();
		FiringPoint.y -= 5.f;

		CProjectile* projectile = projectileManager.FetchProjectile(theLaser, (FiringPoint - tpCamera.getPosition()).Normalized(), 1000.f);
		projectile->setRotation(Player->GetAngle() + 2.5f, 0, 1, 0);
		GameObject3D* newProjectile = projectile;
		SceneNode* node;
		node = getNode();
		node->SetGameObject(newProjectile);
		dynamicSceneGraph->AddChildNode(node);
		Player->GetWeapon()->FireWeapon();
		resourceManager.retrieveSoundas2D("Pistol_Fire", true, false);
	}

	ammoTimer += dt;
	if (ammoTimer > AmmoSpawnInterval)
	{
		ammoTimer = 0.f;
		Player->GetWeapon()->addReservedAmmo(20);
		resourceManager.retrieveSoundas2D("Receive_Ammo", false, false);
	}

	//Reloading weapon
	if (inputManager->getKey("Reload") && Player->GetWeapon()->getCurrentAmmo() < 10)
	{
		Player->GetWeapon()->Reload();
		resourceManager.retrieveSoundas2D("Pistol_Reload", false, false);
	}

	if (inputManager->getKey("Up"))
	{
		Player->SetAngle(tpCamera.GetCamAngle());

		if (inputManager->getKey("Left"))
		{
			Player->SetAngle(tpCamera.GetCamAngle() + 45.f);
			tpCamera.SetStrafeLeft(1);
		}

		else if (inputManager->getKey("Right"))
		{
			Player->SetAngle(tpCamera.GetCamAngle() - 45.f);
			tpCamera.SetStrafeLeft(2);
		}

		Player->Update(dt, Player->GetAngle());

		dynamicSceneGraph->GetChildNode("Player")->GetGameObject()->setPosition(Vector3(
			dynamicSceneGraph->GetChildNode("Player")->GetGameObject()->getPosition().x,
			dynamicSceneGraph->GetChildNode("Player")->GetGameObject()->getPosition().y,
			dynamicSceneGraph->GetChildNode("Player")->GetGameObject()->getPosition().z));
		dynamicSceneGraph->GetChildNode("Player")->GetGameObject()->setRotation(Player->GetAngle(),
			0,
			1,
			0);

		resourceManager.retrieveSoundas2D("Player_Walk", false, false);
	}

	else if (inputManager->getKey("Down"))
	{
		Player->SetAngle(tpCamera.GetCamAngle() + 180.f);

		if (inputManager->getKey("Left"))
		{
			Player->SetAngle(tpCamera.GetCamAngle() + 135.f);
			tpCamera.SetStrafeLeft(1);
		}

		else if (inputManager->getKey("Right"))
		{
			Player->SetAngle(tpCamera.GetCamAngle() - 135.f);
			tpCamera.SetStrafeLeft(2);
		}
		Player->Update(dt, Player->GetAngle());

		dynamicSceneGraph->GetChildNode("Player")->GetGameObject()->setPosition(Vector3(
			dynamicSceneGraph->GetChildNode("Player")->GetGameObject()->getPosition().x,
			dynamicSceneGraph->GetChildNode("Player")->GetGameObject()->getPosition().y,
			dynamicSceneGraph->GetChildNode("Player")->GetGameObject()->getPosition().z));
		dynamicSceneGraph->GetChildNode("Player")->GetGameObject()->setRotation(Player->GetAngle(),
			0,
			1,
			0);
		resourceManager.retrieveSoundas2D("Player_Walk", false, false);
	}

	else if (inputManager->getKey("Left"))
	{
		Player->SetAngle(tpCamera.GetCamAngle() + 90.f);
		Player->Update(dt, Player->GetAngle());
		tpCamera.SetStrafeLeft(1);

		dynamicSceneGraph->GetChildNode("Player")->GetGameObject()->setPosition(Vector3(
			dynamicSceneGraph->GetChildNode("Player")->GetGameObject()->getPosition().x,
			dynamicSceneGraph->GetChildNode("Player")->GetGameObject()->getPosition().y,
			dynamicSceneGraph->GetChildNode("Player")->GetGameObject()->getPosition().z));

		dynamicSceneGraph->GetChildNode("Player")->GetGameObject()->setRotation(Player->GetAngle(),
			0,
			1,
			0);
		resourceManager.retrieveSoundas2D("Player_Walk", false, false);
	}

	else if (inputManager->getKey("Right"))
	{
		Player->SetAngle(tpCamera.GetCamAngle() - 90.f);
		Player->Update(dt, Player->GetAngle());
		tpCamera.SetStrafeLeft(2);

		dynamicSceneGraph->GetChildNode("Player")->GetGameObject()->setPosition(Vector3(
			dynamicSceneGraph->GetChildNode("Player")->GetGameObject()->getPosition().x,
			dynamicSceneGraph->GetChildNode("Player")->GetGameObject()->getPosition().y,
			dynamicSceneGraph->GetChildNode("Player")->GetGameObject()->getPosition().z));

		dynamicSceneGraph->GetChildNode("Player")->GetGameObject()->setRotation(Player->GetAngle(),
			0,
			1,
			0);
		resourceManager.retrieveSoundas2D("Player_Walk", false, false);
	}
	else
	{
		Player->SetIsMoving(false);
		Player->GetNode()->GetGameObject()->setVelocity(Vector3());
		Player->RevertLimb(tpCamera.GetAimMode(), dt);
		tpCamera.SetStrafeLeft(0);
	}

	if (inputManager->getKey("Left") == false && inputManager->getKey("Right") == false)
	{
		tpCamera.SetStrafeLeft(0);
	}

	//Update zoom
	if (inputManager->getKey("RMB"))
	{
		tpCamera.SetAimMode(true);
		Player->SetAngle(tpCamera.GetCamAngle());
		dynamicSceneGraph->GetChildNode("Player")->GetGameObject()->setRotation(Player->GetAngle(),
			0,
			1,
			0);
		Player->RotateLimb("RightHand_Joint", -90, 500, true, dt, 1, 0, 0);
	}
	//Update normal
	else
	{
		tpCamera.SetAimMode(false);
	}

	//Rotate player with camera
	if (Player->GetIsMoving() == false)
	{
		Player->SetAngle(tpCamera.GetCamAngle());
		dynamicSceneGraph->GetChildNode("Player")->GetGameObject()->setRotation(Player->GetAngle(),
			0,
			1,
			0);
	}	
}

void SceneManager_Play_L2::UpdateEnemy(double dt)
{
	spawnTimer += dt;

	//Increase spawn amount
	if (spawnTimer > Enemy_BeholderSpawnInterval)
	{
		spawnTimer = 0.f;
		SpawnEnemy();	
	}
	
	for (int i = 0; i < enemyList_Enemy_Beholder.size(); ++i)
	{
		//Only update alive enemies
		if (enemyList_Enemy_Beholder.at(i)->GetIsAlive())
		{
			if (enemyList_Enemy_Beholder.at(i)->getCurrentState() == CEnemy_Beholder::ATTACK
				|| enemyList_Enemy_Beholder.at(i)->getCurrentState() == CEnemy_Beholder::RUN)
			{
				Vector3 direction = Player->GetNode()->GetGameObject()->getPosition() - enemyList_Enemy_Beholder.at(i)->GetNode()->GetGameObject()->getPosition();
				float angleToRotate = Math::RadianToDegree(atan2(direction.x, direction.z));
				enemyList_Enemy_Beholder.at(i)->SetAngle(angleToRotate);
				enemyList_Enemy_Beholder.at(i)->Update(dt, Player->GetNode()->GetGameObject()->getPosition(), enemyList_Enemy_Beholder);
			}
		}
	}
}

bool SceneManager_Play_L2::CheckSelfCollide(SceneNode* first, SceneNode* second)
{
	if (first->GetGameObject()->getName() == "Head"
		|| first->GetGameObject()->getName() == "Player"
		|| first->GetGameObject()->getName() == "LeftHand_Joint"
		|| first->GetGameObject()->getName() == "LeftHand"
		|| first->GetGameObject()->getName() == "RightHand_Joint"
		|| first->GetGameObject()->getName() == "RightHand"
		|| first->GetGameObject()->getName() == "Weapon"
		|| first->GetGameObject()->getName() == "LeftLeg_Joint"
		|| first->GetGameObject()->getName() == "LeftLeg"
		|| first->GetGameObject()->getName() == "RightLeg_Joint"
		|| first->GetGameObject()->getName() == "RightLeg")
	{
		if (second->GetGameObject()->getName() == "Head"
			|| second->GetGameObject()->getName() == "Player"
			|| second->GetGameObject()->getName() == "LeftHand_Joint"
			|| second->GetGameObject()->getName() == "LeftHand"
			|| second->GetGameObject()->getName() == "RightHand_Joint"
			|| second->GetGameObject()->getName() == "RightHand"
			|| second->GetGameObject()->getName() == "Weapon"
			|| second->GetGameObject()->getName() == "LeftLeg_Joint"
			|| second->GetGameObject()->getName() == "LeftLeg"
			|| second->GetGameObject()->getName() == "RightLeg_Joint"
			|| second->GetGameObject()->getName() == "RightLeg")
		{
			return true;
		}
	}
	if (first->GetGameObject()->getName() == "Head"
		|| first->GetGameObject()->getName() == "Player"
		|| first->GetGameObject()->getName() == "LeftHand_Joint"
		|| first->GetGameObject()->getName() == "LeftHand"
		|| first->GetGameObject()->getName() == "RightHand_Joint"
		|| first->GetGameObject()->getName() == "RightHand"
		|| first->GetGameObject()->getName() == "Weapon"
		|| first->GetGameObject()->getName() == "LeftLeg_Joint"
		|| first->GetGameObject()->getName() == "LeftLeg"
		|| first->GetGameObject()->getName() == "RightLeg_Joint"
		|| first->GetGameObject()->getName() == "RightLeg")
	{
		if (second->GetGameObject()->getName() == "playerbullet")
		{
			return true;
		}
	}

	if (first->GetGameObject()->getName() == "playerbullet")
	{
		if (second->GetGameObject()->getName() == "Head"
			|| second->GetGameObject()->getName() == "Player"
			|| second->GetGameObject()->getName() == "LeftHand_Joint"
			|| second->GetGameObject()->getName() == "LeftHand"
			|| second->GetGameObject()->getName() == "RightHand_Joint"
			|| second->GetGameObject()->getName() == "RightHand"
			|| second->GetGameObject()->getName() == "Weapon"
			|| second->GetGameObject()->getName() == "LeftLeg_Joint"
			|| second->GetGameObject()->getName() == "LeftLeg"
			|| second->GetGameObject()->getName() == "RightLeg_Joint"
			|| second->GetGameObject()->getName() == "RightLeg")
		{
			return true;
		}
	}
	//else if (first->GetGameObject()->getName() == "Enemy_Head"
	//	|| first->GetGameObject()->getName() == "Enemy"
	//	|| first->GetGameObject()->getName() == "Enemy_LeftHand_Joint"
	//	|| first->GetGameObject()->getName() == "Enemy_LeftHand"
	//	|| first->GetGameObject()->getName() == "Enemy_RightHand_Joint"
	//	|| first->GetGameObject()->getName() == "Enemy_RightHand"
	//	|| first->GetGameObject()->getName() == "Enemy_Weapon"
	//	|| first->GetGameObject()->getName() == "Enemy_LeftLeg_Joint"
	//	|| first->GetGameObject()->getName() == "Enemy_LeftLeg"
	//	|| first->GetGameObject()->getName() == "Enemy_RightLeg_Joint"
	//	|| first->GetGameObject()->getName() == "Enemy_RightLeg")
	//{
	//	if (second->GetGameObject()->getName() == "Enemy_Head"
	//		|| second->GetGameObject()->getName() == "Enemy"
	//		|| second->GetGameObject()->getName() == "Enemy_LeftHand_Joint"
	//		|| second->GetGameObject()->getName() == "Enemy_LeftHand"
	//		|| second->GetGameObject()->getName() == "Enemy_RightHand_Joint"
	//		|| second->GetGameObject()->getName() == "Enemy_RightHand"
	//		|| second->GetGameObject()->getName() == "Enemy_Weapon"
	//		|| second->GetGameObject()->getName() == "Enemy_LeftLeg_Joint"
	//		|| second->GetGameObject()->getName() == "Enemy_LeftLeg"
	//		|| second->GetGameObject()->getName() == "Enemy_RightLeg_Joint"
	//		|| second->GetGameObject()->getName() == "Enemy_RightLeg")
	//	{
	//		return true;
	//	}
	//}
	return false;
}

void SceneManager_Play_L2::CollisionResponse(SceneNode* first, SceneNode* second, double dt)
{
	if (first->GetGameObject()->getName() == "Player")
	{
		////Dont apply response on self
		//if (second->GetGameObject()->getName() == "Head"
		//	|| second->GetGameObject()->getName() == "LeftHand_Joint"
		//	|| second->GetGameObject()->getName() == "LeftHand"
		//	|| second->GetGameObject()->getName() == "RightHand_Joint"
		//	|| second->GetGameObject()->getName() == "RightHand"
		//	|| second->GetGameObject()->getName() == "Weapon"
		//	|| second->GetGameObject()->getName() == "LeftLeg_Joint"
		//	|| second->GetGameObject()->getName() == "LeftLeg"
		//	|| second->GetGameObject()->getName() == "RightLeg_Joint"
		//	|| second->GetGameObject()->getName() == "RightLeg")
		//{
		//	return;
		//}
		if (second->GetGameObject()->getName() == "Joker")
		{
			float distSquared = (second->GetGameObject()->getPosition() - first->GetGameObject()->getPosition()).LengthSquared();
			float moveDistSquared = (second->GetGameObject()->getPosition() - (first->GetGameObject()->getPosition() + first->GetGameObject()->getVelocity() * 5)).LengthSquared();
			//std::cout << distSquared << "   " << moveDistSquared << "   " << std::endl;
			
			if (moveDistSquared > distSquared)
			{
				//first->GetGameObject()->setVelocity();
				//std::cout << "Negative vel is: " << -first->GetGameObject()->getVelocity() << std::endl;
				first->GetGameObject()->setVelocity(Vector3());
				//first->GetGameObject()->setPosition(first->GetGameObject()->getPosition() - first->GetGameObject()->getVelocity() * dt);
				//first->GetGameObject()->setVelocity(Vector3());
				
				std::cout << "collided 1" << std::endl;
			}
		}
	}
	//else if (first->GetGameObject()->getName() == "Joker")
	//{
	//	if (second->GetGameObject()->getName() == "Player")
	//	{
	//		float distSquared = (first->GetGameObject()->getPosition() - second->GetGameObject()->getPosition()).LengthSquared();
	//		float moveDistSquared = (first->GetGameObject()->getPosition() - (second->GetGameObject()->getPosition() + first->GetGameObject()->getVelocity())).LengthSquared();
	//		std::cout << distSquared << "   " << moveDistSquared << std::endl;
	//		if (moveDistSquared > distSquared)
	//		{
	//			second->GetGameObject()->setVelocity(Vector3());
	//			
	//			std::cout << "collided 2" << std::endl;
	//		}
	//	}
	//}
}

//
//bool SceneManager_Play_L2::ProcessCollision(SceneNode* first, SceneNode* second)
//{
//	string boxName = "";
//	if (first->GetGameObject()->getName() == "Head"
//		|| first->GetGameObject()->getName() == "Player"
//		|| first->GetGameObject()->getName() == "LeftHand_Joint"
//		|| first->GetGameObject()->getName() == "LeftHand"
//		|| first->GetGameObject()->getName() == "RightHand_Joint"
//		|| first->GetGameObject()->getName() == "RightHand"
//		|| first->GetGameObject()->getName() == "Weapon"
//		|| first->GetGameObject()->getName() == "LeftLeg_Joint"
//		|| first->GetGameObject()->getName() == "LeftLeg"
//		|| first->GetGameObject()->getName() == "RightLeg_Joint"
//		|| first->GetGameObject()->getName() == "RightLeg")
//	{
//		if (second->GetGameObject()->getName() == "Ammo")
//		{
//			if (check3DCollision(first->GetGameObject()->getHitbox(), second->GetGameObject()->getHitbox(), boxName))
//			{
//				return true;
//			}
//		}
//	}
//	return false;
//}

void SceneManager_Play_L2::Render()
{
	SceneManagerGameplay::ClearScreen();

	Mtx44 perspective;
	perspective.SetToPerspective(45.0f, 4.0f / 3.0f, 0.1f, 10000.0f);
	//perspective.SetToOrtho(-80, 80, -60, 60, -1000, 1000);
	projectionStack.LoadMatrix(perspective);

	// Set up the view
	viewStack.LoadIdentity();
	viewStack.LookAt(tpCamera.getPosition().x, tpCamera.getPosition().y, tpCamera.getPosition().z,
		tpCamera.getTarget().x, tpCamera.getTarget().y, tpCamera.getTarget().z,
		tpCamera.getUp().x, tpCamera.getUp().y, tpCamera.getUp().z);

	// Model matrix : an identity matrix (model will be at the origin)
	modelStack.LoadIdentity();

	RenderBG();
	RenderStaticObject();
	RenderMobileObject();
	RenderGUI();
	RenderLight(0, 0, 1, 0);
}

void SceneManager_Play_L2::Exit()
{
	for (unsigned i = 0; i < nodeList.size(); ++i)
	{
		if (nodeList[i] != NULL)
		{
			nodeList[i]->CleanUp();
			delete nodeList[i];
			nodeList[i] = NULL;
		}
	}

	if (spatialPartitionManager)
	{
		spatialPartitionManager->CleanUp();
		delete spatialPartitionManager;
		spatialPartitionManager = NULL;
	}

	nodeList.clear();

	//Clean up spawned materials
	//for (int i = 0; i < enemyList_Demon.size(); ++i)
	//{
	//	delete enemyList_Demon.at(i);
	//	enemyList_Demon.at(i) = NULL;

	//	delete enemyList_DemonSpawner.at(i);
	//	enemyList_DemonSpawner.at(i) = NULL;
	//}
	
	for (int i = 0; i < enemyList_Enemy_Beholder.size(); ++i)
	{
		if (enemyList_Enemy_Beholder.at(i) != NULL)
		{
			delete enemyList_Enemy_Beholder.at(i);
			enemyList_Enemy_Beholder.at(i) = NULL;
		}
	}

	//if (Enemy_Beholder != NULL)
	//{
	//	delete Enemy_Beholder;
	//	Enemy_Beholder = NULL;
	//}
	/*if (miniMap)
	{
		delete miniMap;
		miniMap = NULL;
	}*/

	SceneManagerGameplay::Exit();
}

void SceneManager_Play_L2::BindShaders()
{
	SceneManagerGameplay::BindShaders();
}

// Other specific init, update and render classes
void SceneManager_Play_L2::InitShader()
{
	SHADER thisShader = resourceManager.retrieveShader("Comg");
	programID = LoadShaders(thisShader.vertexShaderDirectory.c_str(), thisShader.fragmentShaderDirectory.c_str());

	this->BindShaders();
	parameters.resize(U_TOTAL);
	lights.resize(2);

	// Get a handle for our uniform
	parameters[U_MVP] = glGetUniformLocation(programID, "MVP");
	//parameters[U_MODEL] = glGetUniformLocation(programID, "M");
	//parameters[U_VIEW] = glGetUniformLocation(programID, "V");
	parameters[U_MODELVIEW] = glGetUniformLocation(programID, "MV");
	parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(programID, "MV_inverse_transpose");
	parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(programID, "material.kAmbient");
	parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(programID, "material.kDiffuse");
	parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(programID, "material.kSpecular");
	parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(programID, "material.kShininess");
	parameters[U_LIGHTENABLED] = glGetUniformLocation(programID, "lightEnabled");
	parameters[U_NUMLIGHTS] = glGetUniformLocation(programID, "numLights");

	parameters[U_LIGHT0_TYPE] = glGetUniformLocation(programID, "lights[0].type");
	parameters[U_LIGHT0_POSITION] = glGetUniformLocation(programID, "lights[0].position_cameraspace");
	parameters[U_LIGHT0_COLOR] = glGetUniformLocation(programID, "lights[0].color");
	parameters[U_LIGHT0_POWER] = glGetUniformLocation(programID, "lights[0].power");
	parameters[U_LIGHT0_KC] = glGetUniformLocation(programID, "lights[0].kC");
	parameters[U_LIGHT0_KL] = glGetUniformLocation(programID, "lights[0].kL");
	parameters[U_LIGHT0_KQ] = glGetUniformLocation(programID, "lights[0].kQ");
	parameters[U_LIGHT0_SPOTDIRECTION] = glGetUniformLocation(programID, "lights[0].spotDirection");
	parameters[U_LIGHT0_COSCUTOFF] = glGetUniformLocation(programID, "lights[0].cosCutoff");
	parameters[U_LIGHT0_COSINNER] = glGetUniformLocation(programID, "lights[0].cosInner");
	parameters[U_LIGHT0_EXPONENT] = glGetUniformLocation(programID, "lights[0].exponent");

	parameters[U_LIGHT1_TYPE] = glGetUniformLocation(programID, "lights[1].type");
	parameters[U_LIGHT1_POSITION] = glGetUniformLocation(programID, "lights[1].position_cameraspace");
	parameters[U_LIGHT1_COLOR] = glGetUniformLocation(programID, "lights[1].color");
	parameters[U_LIGHT1_POWER] = glGetUniformLocation(programID, "lights[1].power");
	parameters[U_LIGHT1_KC] = glGetUniformLocation(programID, "lights[1].kC");
	parameters[U_LIGHT1_KL] = glGetUniformLocation(programID, "lights[1].kL");
	parameters[U_LIGHT1_KQ] = glGetUniformLocation(programID, "lights[1].kQ");
	parameters[U_LIGHT1_SPOTDIRECTION] = glGetUniformLocation(programID, "lights[1].spotDirection");
	parameters[U_LIGHT1_COSCUTOFF] = glGetUniformLocation(programID, "lights[1].cosCutoff");
	parameters[U_LIGHT1_COSINNER] = glGetUniformLocation(programID, "lights[1].cosInner");
	parameters[U_LIGHT1_EXPONENT] = glGetUniformLocation(programID, "lights[1].exponent");
	// Get a handle for our "colorTexture" uniform
	parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(programID, "colorTextureEnabled");
	parameters[U_COLOR_TEXTURE] = glGetUniformLocation(programID, "colorTexture");
	// Get a handle for our "textColor" uniform
	parameters[U_TEXT_ENABLED] = glGetUniformLocation(programID, "textEnabled");
	parameters[U_TEXT_COLOR] = glGetUniformLocation(programID, "textColor");

	// Use our shader
	glUseProgram(programID);

	lights[0].type = Light::LIGHT_DIRECTIONAL;
	lights[0].position.Set(0, 0, 1);
	lights[0].color.Set(1, 1, 1);
	lights[0].power = brightness;
	lights[0].kC = 1.f;
	lights[0].kL = 0.001f;
	lights[0].kQ = 0.0001f;
	lights[0].cosCutoff = cos(Math::DegreeToRadian(45));
	lights[0].cosInner = cos(Math::DegreeToRadian(30));
	lights[0].exponent = 3.f;
	lights[0].spotDirection.Set(0.f, 0.f, 1.f);

	lights[1].type = Light::LIGHT_POINT;
	lights[1].position.Set(0, 0, 10);
	lights[1].color.Set(1, 1, 1);
	lights[1].power = 5;
	lights[1].kC = 1.f;
	lights[1].kL = 0.001f;
	lights[1].kQ = 0.0001f;
	lights[1].cosCutoff = cos(Math::DegreeToRadian(45));
	lights[1].cosInner = cos(Math::DegreeToRadian(30));
	lights[1].exponent = 3.f;
	lights[1].spotDirection.Set(0.f, 0.f, 1.f);

	glUniform1i(parameters[U_NUMLIGHTS], 2);
	glUniform1i(parameters[U_TEXT_ENABLED], 0);

	glUniform1i(parameters[U_LIGHT0_TYPE], lights[0].type);
	glUniform3fv(parameters[U_LIGHT0_COLOR], 1, &lights[0].color.r);
	glUniform1f(parameters[U_LIGHT0_POWER], lights[0].power);
	glUniform1f(parameters[U_LIGHT0_KC], lights[0].kC);
	glUniform1f(parameters[U_LIGHT0_KL], lights[0].kL);
	glUniform1f(parameters[U_LIGHT0_KQ], lights[0].kQ);
	glUniform1f(parameters[U_LIGHT0_COSCUTOFF], lights[0].cosCutoff);
	glUniform1f(parameters[U_LIGHT0_COSINNER], lights[0].cosInner);
	glUniform1f(parameters[U_LIGHT0_EXPONENT], lights[0].exponent);

	glUniform1i(parameters[U_LIGHT1_TYPE], lights[1].type);
	glUniform3fv(parameters[U_LIGHT1_COLOR], 1, &lights[1].color.r);
	glUniform1f(parameters[U_LIGHT1_POWER], lights[1].power);
	glUniform1f(parameters[U_LIGHT1_KC], lights[1].kC);
	glUniform1f(parameters[U_LIGHT1_KL], lights[1].kL);
	glUniform1f(parameters[U_LIGHT1_KQ], lights[1].kQ);
	glUniform1f(parameters[U_LIGHT1_COSCUTOFF], lights[1].cosCutoff);
	glUniform1f(parameters[U_LIGHT1_COSINNER], lights[1].cosInner);
	glUniform1f(parameters[U_LIGHT1_EXPONENT], lights[1].exponent);
}

void SceneManager_Play_L2::RenderLight(const float rotation, const float x, const float y, const float z)
{
	//Lights
	if (lights[0].type == Light::LIGHT_DIRECTIONAL)
	{
		Mtx44 rot;
		rot.SetToRotation(rotation, x, y, z);
		Vector3 lightDir(lights[0].position.x, lights[0].position.y, lights[0].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * rot * lightDir;
		glUniform3fv(parameters[U_LIGHT0_POSITION], 1, &lightDirection_cameraspace.x);
	}
	else if (lights[0].type == Light::LIGHT_SPOT)
	{
		Mtx44 rot;
		rot.SetToRotation(rotation, x, y, z);
		Position lightPosition_cameraspace = viewStack.Top() * lights[0].position;
		glUniform3fv(parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * rot * lights[0].spotDirection;
		glUniform3fv(parameters[U_LIGHT0_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else if (lights[0].type == Light::LIGHT_POINT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * lights[0].position;
		glUniform3fv(parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * lights[0].spotDirection;
		glUniform3fv(parameters[U_LIGHT0_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		Position lightPosition_cameraspace = viewStack.Top() * lights[0].position;
		glUniform3fv(parameters[U_LIGHT0_POSITION], 1, &lightPosition_cameraspace.x);
	}

	//Lights
	if (lights[1].type == Light::LIGHT_DIRECTIONAL)
	{
		Mtx44 rot;
		rot.SetToRotation(rotation, x, y, z);
		Vector3 lightDir(lights[1].position.x, lights[1].position.y, lights[1].position.z);
		Vector3 lightDirection_cameraspace = viewStack.Top() * rot * lightDir;
		glUniform3fv(parameters[U_LIGHT1_POSITION], 1, &lightDirection_cameraspace.x);
	}
	else if (lights[1].type == Light::LIGHT_SPOT)
	{
		Mtx44 rot;
		rot.SetToRotation(rotation, x, y, z);
		Position lightPosition_cameraspace = viewStack.Top() * lights[1].position;
		glUniform3fv(parameters[U_LIGHT1_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * rot * lights[1].spotDirection;
		glUniform3fv(parameters[U_LIGHT1_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else if (lights[1].type == Light::LIGHT_POINT)
	{
		Position lightPosition_cameraspace = viewStack.Top() * lights[1].position;
		glUniform3fv(parameters[U_LIGHT1_POSITION], 1, &lightPosition_cameraspace.x);
		Vector3 spotDirection_cameraspace = viewStack.Top() * lights[1].spotDirection;
		glUniform3fv(parameters[U_LIGHT1_SPOTDIRECTION], 1, &spotDirection_cameraspace.x);
	}
	else
	{
		Position lightPosition_cameraspace = viewStack.Top() * lights[1].position;
		glUniform3fv(parameters[U_LIGHT1_POSITION], 1, &lightPosition_cameraspace.x);
	}
}

void SceneManager_Play_L2::RenderBG()
{

}

void SceneManager_Play_L2::RenderStaticObject()
{
	static Mesh* debugMesh = resourceManager.retrieveMesh("DEBUG_CUBE");
	staticSceneGraph->Draw(this, debugMesh);
	Mesh* drawMesh;

	drawMesh = resourceManager.retrieveMesh("SKYPLANE2");
	modelStack.PushMatrix();
	modelStack.Translate(0, 2000, 0);
	RenderMesh(drawMesh, false);
	modelStack.PopMatrix();

	drawMesh = resourceManager.retrieveMesh("FLOOR2");

	modelStack.PushMatrix();
	modelStack.Rotate(-90, 1, 0, 0);
	modelStack.Translate(0, 0, -10);
	modelStack.Rotate(-90, 0, 0, 1);
	RenderMesh(drawMesh, false);
	modelStack.PopMatrix();

	if (debugInfo)
	{
		for (int k = 0; k < (int)spatialPartitionManager->getNumPartition().z; ++k)
		{
			for (int j = 0; j < (int)spatialPartitionManager->getNumPartition().y; ++j)
			{
				for (int i = 0; i < (int)spatialPartitionManager->getNumPartition().x; ++i)
				{
					Partition* partition = spatialPartitionManager->getPartition(Vector3((float)i, (float)j, (float)k), false);
					if (partition->nodes.size() > 0)
					{
						//std::cout << partition->nodes.size() << std::endl;
						modelStack.PushMatrix();
						modelStack.Translate(
							world3DStart.x + (i + 0.5f) * spatialPartitionManager->getParitionDimension().x,
							world3DStart.y + (j + 0.5f) * spatialPartitionManager->getParitionDimension().y,
							world3DStart.z + (k + 0.5f) * spatialPartitionManager->getParitionDimension().z);
						modelStack.Scale(spatialPartitionManager->getParitionDimension());
						RenderMesh(partition->getMesh(), false);
						modelStack.PopMatrix();
					}
				}
			}
		}
	}
}

void SceneManager_Play_L2::RenderMobileObject()
{
	static Mesh* debugMesh = resourceManager.retrieveMesh("DEBUG_CUBE");
	dynamicSceneGraph->Draw(this, debugMesh);

	//Billboard text
	//for (int i = 0; i < enemyList_Demon.size(); ++i)
	//{
	//	//Only display for active
	//	if (enemyList_Demon.at(i)->GetNode()->getActive())
	//	{

	//		Vector3 rotation = tpCamera.getPosition() - enemyList_Demon.at(i)->GetNode()->GetWorldPosition();
	//		float angleToRotate = Math::RadianToDegree(atan2(rotation.x, rotation.z));
	//		enemyList_Demon.at(i)->SetAngle(angleToRotate);
	//		ostringstream ss;
	//		ss << spatialPartitionManager->generatePartitionIndex(enemyList_Demon.at(i)->GetNode()->GetWorldPosition() * 0.005);
	//		modelStack.PushMatrix();
	//		modelStack.Translate(enemyList_Demon.at(i)->GetNode()->GetWorldPosition().x, enemyList_Demon.at(i)->GetNode()->GetWorldPosition().y + 5, enemyList_Demon.at(i)->GetNode()->GetWorldPosition().z);
	//		modelStack.Rotate(enemyList_Demon.at(i)->GetAngle(), 0, 1, 0);
	//		modelStack.Scale(20, 20, 20);
	//		RenderText(textMesh, ss.str(), Color(1, 1, 1));
	//		modelStack.PopMatrix();
	//	}
	//}
}

void SceneManager_Play_L2::RenderGUI()
{
	Mesh* drawMesh;
	static Color textCol = resourceManager.retrieveColor("Black");

	std::ostringstream ss;
	ss.precision(5);
	ss << "FPS: " << fps;
	RenderTextOnScreen(textMesh, ss.str(), Color(1, 1, 1), 50.f, sceneWidth - 200, sceneHeight - 50);

	std::ostringstream ssCurrentAmmo;
	ssCurrentAmmo << Player->GetWeapon()->getCurrentAmmo() << " / " << Player->GetWeapon()->getReservedAmmo();
	RenderTextOnScreen(textMesh, ssCurrentAmmo.str(), Color(1, 1, 1), 50, sceneWidth - 200, 100);

	if (Player->GetWeapon()->GetReloading())
	{
		std::ostringstream ssReload;
		ssReload.precision(2);
		ssReload << "Reloading Weapon " << Player->GetWeapon()->GetReloadTimer() << "s";
		RenderTextOnScreen(textMesh, ssReload.str(), Color(1, 1, 1), 50, sceneWidth * 0.4f, sceneHeight * 0.4f);
	}

	if (tpCamera.GetAimMode())
	{
		drawMesh = resourceManager.retrieveMesh("CROSSHAIR");
		Render2DMesh(drawMesh, false, Vector2(50, 50), Vector2(sceneWidth * 0.5f, sceneHeight * 0.45f));
	}

	std::ostringstream ssCurrentScore;
	ssCurrentScore << "Score: " << score;
	RenderTextOnScreen(textMesh, ssCurrentScore.str(), Color(1, 1, 1), 50, 50, sceneHeight - 100);

	if (GameOver)
	{
		RenderTextOnScreen(textMesh, "You are dead...", Color(1, 1, 1), 50, sceneWidth * 0.4f, sceneHeight * 0.65f);
		std::ostringstream ssFinalScore;
		ssFinalScore << "Your final score: " << score;
		RenderTextOnScreen(textMesh, ssFinalScore.str(), Color(1, 1, 1), 50, sceneWidth * 0.4f, sceneHeight * 0.55f);
	}

	if (newRecord)
	{
		RenderTextOnScreen(textMesh, "but you have lasted longer than the others..", Color(1, 1, 1), 50, sceneWidth * 0.4f, sceneHeight * 0.6f);
	}
}

void SceneManager_Play_L2::InitSceneGraph()
{
	// root node
	this->sceneGraph = getNode();
	this->sceneGraph->SetGameObject(NULL);

	// first child node of the scenegraph consists of static nodes / nodes that do not need constantly update
	this->staticSceneGraph = getNode();
	this->staticSceneGraph->SetGameObject(NULL);
	// second child node of the scenegraph consists of dynamic / moving nodes / nodes that need to be updated with spatial partition for collision detection
	this->dynamicSceneGraph = getNode();
	this->dynamicSceneGraph->SetGameObject(NULL);

	// add the 2 child to parent node first
	sceneGraph->AddChildNode(staticSceneGraph);
	sceneGraph->AddChildNode(dynamicSceneGraph);

	InitStaticNodes();
	InitDynamicNodes();
	//InitPlayer();

	// Rmb to init static nodes position first
	spatialPartitionManager->addNode(sceneGraph, spatialPartitionManager->type);
}

void SceneManager_Play_L2::InitStaticNodes()
{
	//All props goes here
	InitEnvironmentNodes();
}

void SceneManager_Play_L2::InitEnvironmentNodes()
{

}

void SceneManager_Play_L2::InitDynamicNodes()
{
	//All moving stuff goes here
	InitPlayer();
	//InitDemonSpawner();
	InitEnemy();
}

void SceneManager_Play_L2::InitPlayer()
{
	SceneNode* node;
	Mesh* drawMesh;

	//Init player(Body is main node)
	drawMesh = resourceManager.retrieveMesh("HUMAN_BODY");
	drawMesh->textureID = resourceManager.retrieveTexture("PLAYER");
	Player->Init(Vector3(50, 14, 200), Vector3(0, 1, 0), drawMesh);
	Player->GetNode()->GetGameObject()->setHitbox(Vector3(), 7, 12, 4, "BodyHitbox");

	GameObject3D Pistol;
	Pistol.setPosition(Vector3());
	Pistol.setMesh(resourceManager.retrieveMesh("PISTOL"));
	Pistol.setCollidable(true);
	Pistol.setName("Pistol");
	Pistol.setReflectLight(true);
	Pistol.setHitbox(Vector3(), 3, 3, 3, "Pistolhitbox");
	GameObject3D Bullet;
	CProjectile Projectile;
	Projectile.Init(Bullet, Vector3(), false, 10.f);
	Player->GetWeapon()->Init(Pistol, Projectile, 10, 120, 1.f, 0.1f);

	//Head of player
	node = getNode();
	drawMesh = resourceManager.retrieveMesh("HUMAN_HEAD");
	drawMesh->textureID = resourceManager.retrieveTexture("PLAYER");
	node->GetGameObject()->setMesh(drawMesh);
	node->GetGameObject()->setName("Head");
	Player->GetNode()->AddChildNode(node);
	Player->GetNode()->GetChildNode("Head")->GetGameObject()->setPosition(Vector3(0, 10.5, 0));
	Player->GetNode()->GetChildNode("Head")->GetGameObject()->setHitbox(Vector3(), 8.5, 9.5, 8.5, "HeadHitbox");

	//Right Hand joint
	node = getNode();
	node->GetGameObject()->setName("RightHand_Joint");
	Player->GetNode()->AddChildNode(node);
	Player->GetNode()->GetChildNode("RightHand_Joint")->GetGameObject()->setPosition(Vector3(-5, 4, 0));
	Player->GetNode()->GetChildNode("RightHand_Joint")->GetGameObject()->setHitbox(Vector3(), 1, 1, 1, "RightHandJointHitbox");

	//Right hand
	node = getNode();
	drawMesh = resourceManager.retrieveMesh("HUMAN_RIGHTHAND");
	drawMesh->textureID = resourceManager.retrieveTexture("PLAYER");
	node->GetGameObject()->setMesh(drawMesh);
	node->GetGameObject()->setName("RightHand");
	Player->GetNode()->AddChildToChildNode("RightHand_Joint", node);
	Player->GetNode()->GetChildNode("RightHand_Joint")->GetChildNode("RightHand")->GetGameObject()->setPosition(Vector3(0, -4, 0));
	Player->GetNode()->GetChildNode("RightHand_Joint")->GetChildNode("RightHand")->GetGameObject()->setHitbox(Vector3(), 3.25, 12, 3.25, "RightHandHitbox");

	//Weapon
	node = getNode();
	node->GetGameObject()->setMesh(Player->GetWeapon()->getMesh());
	node->GetGameObject()->setName("Weapon");
	Player->GetNode()->AddChildToChildNode("RightHand", node);
	Player->GetNode()->GetChildNode("RightHand_Joint")->GetChildNode("RightHand")->GetChildNode("Weapon")->GetGameObject()->setPosition(Vector3(0, -4, 0));
	Player->GetNode()->GetChildNode("RightHand_Joint")->GetChildNode("RightHand")->GetChildNode("Weapon")->GetGameObject()->setHitbox(Vector3(), 1, 1, 1, "WeaponHitbox");

	//Left Hand joint
	node = getNode();
	node->GetGameObject()->setName("LeftHand_Joint");
	Player->GetNode()->AddChildNode(node);
	Player->GetNode()->GetChildNode("LeftHand_Joint")->GetGameObject()->setPosition(Vector3(5, 4, 0));
	Player->GetNode()->GetChildNode("LeftHand_Joint")->GetGameObject()->setHitbox(Vector3(), 1, 1, 1, "LeftHandJointHitbox");

	//Left Hand
	node = getNode();
	drawMesh = resourceManager.retrieveMesh("HUMAN_LEFTHAND");
	drawMesh->textureID = resourceManager.retrieveTexture("PLAYER");
	node->GetGameObject()->setMesh(drawMesh);
	node->GetGameObject()->setName("LeftHand");
	Player->GetNode()->AddChildToChildNode("LeftHand_Joint", node);
	Player->GetNode()->GetChildNode("LeftHand_Joint")->GetChildNode("LeftHand")->GetGameObject()->setPosition(Vector3(0, -4, 0));
	Player->GetNode()->GetChildNode("LeftHand_Joint")->GetChildNode("LeftHand")->GetGameObject()->setHitbox(Vector3(), 3.25, 12, 3.25, "LeftHandHitbox");

	//Left Leg joint
	node = getNode();
	node->GetGameObject()->setName("LeftLeg_Joint");
	Player->GetNode()->AddChildNode(node);
	Player->GetNode()->GetChildNode("LeftLeg_Joint")->GetGameObject()->setPosition(Vector3(2, -2, 0));
	Player->GetNode()->GetChildNode("LeftLeg_Joint")->GetGameObject()->setHitbox(Vector3(), 1, 1, 1, "LeftLegJointHitbox");

	//Left Leg
	node = getNode();
	drawMesh = resourceManager.retrieveMesh("HUMAN_LEG");
	drawMesh->textureID = resourceManager.retrieveTexture("PLAYER");
	node->GetGameObject()->setMesh(drawMesh);
	node->GetGameObject()->setName("LeftLeg");
	Player->GetNode()->AddChildToChildNode("LeftLeg_Joint", node);
	Player->GetNode()->GetChildNode("LeftLeg_Joint")->GetChildNode("LeftLeg")->GetGameObject()->setPosition(Vector3(0, -11, 0));
	Player->GetNode()->GetChildNode("LeftLeg_Joint")->GetChildNode("LeftLeg")->GetGameObject()->setHitbox(Vector3(), 3.5, 17, 3.5, "LeftLegHitbox");

	//Right Leg joint
	node = getNode();
	node->GetGameObject()->setName("RightLeg_Joint");
	Player->GetNode()->AddChildNode(node);
	Player->GetNode()->GetChildNode("RightLeg_Joint")->GetGameObject()->setPosition(Vector3(-2, -2, 0));
	Player->GetNode()->GetChildNode("RightLeg_Joint")->GetGameObject()->setHitbox(Vector3(), 1, 1, 1, "RightLegJointHitbox");

	//Right Leg
	node = getNode();
	drawMesh = resourceManager.retrieveMesh("HUMAN_LEG");
	drawMesh->textureID = resourceManager.retrieveTexture("PLAYER");
	node->GetGameObject()->setMesh(drawMesh);
	node->GetGameObject()->setName("RightLeg");
	Player->GetNode()->AddChildToChildNode("RightLeg_Joint", node);
	Player->GetNode()->GetChildNode("RightLeg_Joint")->GetChildNode("RightLeg")->GetGameObject()->setPosition(Vector3(0, -11, 0));
	Player->GetNode()->GetChildNode("RightLeg_Joint")->GetChildNode("RightLeg")->GetGameObject()->setHitbox(Vector3(), 3.5, 17, 3.5, "RightLegHitbox");

	//Adds the player node
	dynamicSceneGraph->AddChildNode(Player->GetNode());
}

void SceneManager_Play_L2::InitEnemy()
{
	Mesh* drawMesh;
	for (int i = 0; i < 20; ++i)
	{
		CEnemy_Beholder* Enemy_Beholder = new CEnemy_Beholder();

		//Init player(Body is main node)
		drawMesh = resourceManager.retrieveMesh("EYEBALL");
		drawMesh->textureID = resourceManager.retrieveTexture("EYEBALL");
		Enemy_Beholder->Init(Vector3(rand() % 4000 + (-2000), 14, rand() % 4000 + (-2000)), Vector3(0, 1, 0), drawMesh);
		Enemy_Beholder->GetNode()->GetGameObject()->setHitbox(Vector3(), 5, 5, 5, "BodyHitbox");
		
		float distCheck = (Player->GetNode()->GetGameObject()->getPosition() - Enemy_Beholder->GetNode()->GetGameObject()->getPosition()).LengthSquared();

		while (distCheck < 4000)
		{
			Enemy_Beholder->GetNode()->GetGameObject()->setPosition(Vector3(rand() % 4000 + (-2000), 14, rand() % 4000 + (-2000)));
		}

		enemyList_Enemy_Beholder.push_back(Enemy_Beholder);
		dynamicSceneGraph->AddChildNode(Enemy_Beholder->GetNode());
	}
}

void SceneManager_Play_L2::DeleteEnemy(SceneNode* node)
{
	for (std::vector<CEnemy_Beholder*>::iterator it = enemyList_Enemy_Beholder.begin(); it != enemyList_Enemy_Beholder.end(); ++it)
	{
		CEnemy_Beholder* Enemy_Beholder = (CEnemy_Beholder *)*it;
		if (Enemy_Beholder->GetNode()->GetGameObject()->getPosition() == node->GetGameObject()->getPosition())
		{
			(*it)->SetIsAlive(false);
		}
	}
}

void SceneManager_Play_L2::SpawnEnemy(void)
{
	for (int i = 0; i < enemyList_Enemy_Beholder.size(); ++i)
	{
		if (enemyList_Enemy_Beholder.at(i)->GetIsAlive() == false)
		{
			enemyList_Enemy_Beholder.at(i)->GetNode()->GetGameObject()->setPosition(Vector3(rand() % 4000 + (-2000), 14, rand() % 4000 + (-2000)));
		}
		dynamicSceneGraph->AddChildNode(enemyList_Enemy_Beholder.at(i)->GetNode());
	}
}

void SceneManager_Play_L2::UpdateMouse()
{
	SceneManagerGameplay::UpateMouse();
}

void SceneManager_Play_L2::LoadHighScore(void)
{
	data = " ";
	ifstream inFile;
	inFile.open("Config//Highscore.txt");
	int Line = 0;
	if (inFile.good())
	{
		while (getline(inFile, data))
		{
			ScoreList[Line] = stoi(data);
			++Line;
		}
		inFile.close();
	}
	else
		std::cout << "Load score file failed" << std::endl;
}
void SceneManager_Play_L2::CompareHighScore(void)
{
	//If beat the lowest score
	if (score > ScoreList[9] && GameOver == false)
	{
		newRecord = true;
		ScoreList[9] = score;
		//Sort high score in descending order
		for (int i = 1; i < 10; ++i)
		{
			for (int j = 0; j < 9; ++j)
			{
				if (ScoreList[j] < ScoreList[j + 1])
				{
					float temp = ScoreList[j];
					ScoreList[j] = ScoreList[j + 1];
					ScoreList[j + 1] = temp;
				}
			}
		}
		WriteHighScore();
	}
	GameOver = true;
}
void SceneManager_Play_L2::WriteHighScore(void)
{
	ofstream outFile;
	outFile.open("Config//Highscore.txt");
	if (outFile.good())
	{
		for (int i = 0; i < 10; ++i)
		{
			outFile << ScoreList[i] << std::endl;
		}
		outFile.close();
	}
	else
		std::cout << "Write score file failed" << std::endl;
}

SceneNode* SceneManager_Play_L2::getNode()
{
	for (unsigned i = 0; i < nodeList.size(); ++i)
	{
		if (!nodeList[i]->getActive())
		{
			nodeList[i]->GetGameObject()->setMesh(NULL);
			nodeList[i]->GetGameObject()->setName("");
			nodeList[i]->GetGameObject()->setPosition(Vector3());
			nodeList[i]->GetGameObject()->setRotation(0, 1, 1, 1);
			nodeList[i]->GetGameObject()->setUpdate(true);
			nodeList[i]->GetGameObject()->setRender(true);
			nodeList[i]->setActive(true);
			return nodeList[i];
		}
	}

	GameObject3D* object;
	SceneNode* node;
	for (int i = 0; i < 50; ++i)
	{
		object = new GameObject3D;
		node = new SceneNode;

		node->SetGameObject(object);
		node->setActive(false);

		nodeList.push_back(node);
	}

	SceneNode *lastnode = nodeList.back();
	lastnode->setActive(true);
	return lastnode;
}