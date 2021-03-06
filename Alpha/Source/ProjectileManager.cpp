#include "ProjectileManager.h"
#include "../Source/SceneManager/SceneManager.h"

/********************************************************************************
 Constructor
 ********************************************************************************/
ProjectileManager::ProjectileManager(void) :
	m_iActiveProjectileCount(0)
{
	//Create projectiles
	for (unsigned i = 0; i < m_iStartProjectileCount; ++i)
	{
		m_ProjectileList.push_back(new CProjectile());
	}

	Boundary_TopLeft		= Vector3( 300.0f, 300.0f, 300.0f);
	Boundary_BottomRight	= Vector3( -300.0f, 0.0f, -300.0f);
}

/********************************************************************************
 Destructor
 ********************************************************************************/
ProjectileManager::~ProjectileManager(void)
{
	CleanUp();
}

/********************************************************************************
Use a non-active projectile
********************************************************************************/
CProjectile* ProjectileManager::FetchProjectile(GameObject3D object, Vector3 direction, float speed)
{
	for (std::vector<CProjectile*>::iterator it = m_ProjectileList.begin(); it != m_ProjectileList.end(); ++it)
	{
		CProjectile* newProjectile = (CProjectile *)*it;
		//Get first non active projectile to use
		if (!newProjectile->getUpdate())
		{
			newProjectile->Init(object, direction, true, speed);
			return newProjectile;
		}
	}
	//If all existing projectiles are being used, create more projectiles
	for (unsigned i = 0; i < 10; ++i)
	{
		m_ProjectileList.push_back(new CProjectile());
	}
	//Use newly created projectile
	CProjectile* newProjectile = m_ProjectileList.back();
	newProjectile->Init(object, direction, true, speed);
	return newProjectile;
}

/********************************************************************************
 Remove Projectile
 ********************************************************************************/
void ProjectileManager::RemoveProjectile(CProjectile* projectile)
{
	projectile->setUpdate(false);
	projectile->setRender(false);
	this->m_iActiveProjectileCount--;
}
void ProjectileManager::RemoveProjectile(GameObject3D* object)
{
	for (std::vector<CProjectile*>::iterator it = m_ProjectileList.begin(); it != m_ProjectileList.end(); ++it)
	{
		CProjectile* Projectile = (CProjectile *)*it;
		if (Projectile->getUpdate())
		{
			if (Projectile->getPosition() == object->getPosition())
			{
				//Projectile->setUpdate(false);
				//Projectile->setRender(false);
				m_ProjectileList.erase(it);
				
				this->m_iActiveProjectileCount--;
				break;
			}
		}
	}
}

/********************************************************************************
 Get Number of Active Projectile
 ********************************************************************************/
int ProjectileManager::GetNumberOfActiveProjectiles(void)
{
	return this->m_iActiveProjectileCount;
}

/********************************************************************************
 Update
 ********************************************************************************/
void ProjectileManager::Update(const double dt, const float worldLength, const float worldWidth)
{
	//Iterate through vector of projectiles
	for (std::vector<CProjectile*>::iterator it = m_ProjectileList.begin(); it != m_ProjectileList.end(); ++it)
	{
		CProjectile* projectile = (CProjectile *)*it;
		
		//Only update if projectile is active
		if (projectile->getUpdate())
		{
			projectile->Update(dt, worldLength, worldWidth);
		}
	}
}

/********************************************************************************
Render projectile
********************************************************************************/
void ProjectileManager::Draw(SceneManager *sceneManager)
{
	for (std::vector<CProjectile*>::iterator it = m_ProjectileList.begin(); it != m_ProjectileList.end(); ++it)
	{
		CProjectile* projectile = (CProjectile *)*it;

		//Only render active projectiles
		if (projectile->getRender())
		{
			sceneManager->RenderPush(projectile->getProperties().modelProperties);
			sceneManager->RenderMesh(projectile->getMesh(), projectile->getReflectLight());
			sceneManager->RenderPop();
		}
	}
}

/********************************************************************************
Clean up
********************************************************************************/
void ProjectileManager::CleanUp()
{
	//Cleaning up vector
	m_ProjectileList.clear();
}