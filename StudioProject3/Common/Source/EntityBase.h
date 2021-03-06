#ifndef ENTITY_BASE_H
#define ENTITY_BASE_H

#include "Vector3.h"
#include <string>
using std::string;

class Mesh;

class EntityBase
{
public:
	EntityBase();
	virtual ~EntityBase();

	virtual void Update(double _dt);
	virtual void Render();
	virtual void RenderUI();

	inline void SetPosition(const Vector3& _value){ position = _value; };
	inline Vector3 GetPosition(){ return position; };

	inline void SetScale(const Vector3& _value){ scale = _value; };
	inline Vector3 GetScale(){ return scale; };

	inline void SetMeshName(const string& _value) { meshName = _value; };
	inline string GetMeshName() { return meshName; };

	bool IsDone();
	void SetIsDone(const bool _value);

	// Check if this entity has a collider class parent
	virtual bool HasCollider(void) const;
	// Set the flag to indicate if this entity has a collider class parent
	virtual void SetCollider(const bool _value);
	//Set new mesh
	virtual void SetMesh(Mesh* mesh);

	//Set collision with player
	virtual void SetCollide(const bool _value) { m_bCollide = _value; };
	virtual bool GetCollide() { return m_bCollide; };

	virtual void SetActionDone(const bool _value) { m_bActionDone = _value; };
	virtual bool GetActionDone() { return m_bActionDone; };

	virtual void SetAvoidPos(Vector3 _value) { objAvoidPos = _value; };
	virtual Vector3 GetAvoidPos() { return objAvoidPos; };

	virtual void SetBuffer(float _value) { m_fBuffer = _value; };
	virtual float GetBuffer() { return m_fBuffer; };

	virtual void SetFireDestination(Vector3 destination) { fireDestination = destination; };
	virtual Vector3 GetFireDestination() { return fireDestination; };

	virtual void SetSelected(const bool _value) { m_bSelected = _value; };
	virtual bool GetSelected() { return m_bSelected; };

	virtual void SetHealth(const int _value) { m_iHealth = _value; };
	virtual int GetHealth() { return m_iHealth; };

	virtual void SetRange(const int _value) { m_iFireRange = _value; };
	virtual int GetRange() { return m_iFireRange; };

	virtual void SetDestination(Vector3 destination) { this->destination = destination; };
	//Reset 
	virtual void ResetGame();

protected:
	Vector3 position;
	Vector3 scale;
	string meshName;
	Vector3 objAvoidPos;
	Vector3 fireDestination;
	Vector3 destination;
	//Vector3 objAvoidScale;

	bool isDone;
	bool m_bCollider; //Has CCollider as parent class
	bool m_bCollide; //Has collided with an object
	bool m_bActionDone; //Action has been done
	bool m_bSelected;
	int m_iHealth;
	int m_iFireRange; //Range of how far can shoot
	float m_fBuffer;

	Mesh* modelMesh;
};

#endif // ENTITY_BASE_H