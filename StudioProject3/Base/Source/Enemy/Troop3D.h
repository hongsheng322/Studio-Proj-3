#pragma once
#include "../GenericEntity.h"
#include "../GroundEntity.h"
#include "../Projectile/Projectile.h"

#include <vector>
using namespace std;

class Mesh;

class CTroop3D :
	public GenericEntity
{
protected:
	enum CTROOP3D_TYPE
	{
		NONE = 0,
		TROOPER,
		ARCHER,
		TANKER,
		TOTAL
	}type;

	Mesh* modelMesh;

	Vector3 defaultPosition, defaultTarget, defaultUp;
	Vector3 target, up, rotate;
	Vector3 maxBoundary, minBoundary;
	Vector3 destination, finalDestination; //Destination: for when troops are avoiding turrets. //Final destination: where troops will eventually end up at
	GroundEntity* m_pTerrain;

	double m_dSpeed;
	double m_dAcceleration;
	bool m_bChangeDir; // Direction changing 
	bool m_bFireProjectile; // If true, can fire. Else, cannot
public:
	CTroop3D() {};
	CTroop3D(Mesh* _modelMesh);
	virtual ~CTroop3D();

	void Init(void);
	// Reset this player instance to default
	void Reset(void);

	//Reset game
	void ResetGame();

	// Set position
	void SetPos(const Vector3& pos);
	// Set target
	void SetTarget(const Vector3& target);
	// Set Up
	void SetUp(const Vector3& up);
	// Set the boundary 
	void SetBoundary(Vector3 max, Vector3 min);
	// Set the terrain 
	void SetTerrain(GroundEntity* m_pTerrain);
	// Set type
	void SetType(int type);
	// Set destination/final destination of troops
	void SetDestination(Vector3 destination);
	// Set whether can fire
	void SetFire(bool fire) { m_bFireProjectile = fire; };
	bool GetFire() { return m_bFireProjectile; };

	// Get position
	Vector3 GetPos(void) const;
	// Get target
	Vector3 GetTarget(void) const;
	// Get Up
	Vector3 GetUp(void) const;
	// Get the terrain 
	GroundEntity* GetTerrain(void);
	//Get final destination of troops
	Vector3 GetFinalDestination() { return finalDestination; };
	// Get type of enemy
	int GetType() { return type; };

	// Update
	void Update(double dt = 0.0333f);

	// Constrain the position within the borders
	void Constrain(void);
	// Render
	void Render(void);

	void SetMesh(Mesh* mesh);
	float m_fElapsedTimeBeforeUpdate;
};

namespace Create
{
	CTroop3D* Troop3D(const std::string& _meshName,
		const Vector3& _position,
		const Vector3& _scale = Vector3(1.0f, 1.0f, 1.0f),
		const int type = 1);
};
