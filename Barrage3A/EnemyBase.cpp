#include "Header/EnemyBase.h"
#include "EnemyManager.h"

void EnemyBase::Awake() {
	//EnemyManager::GetInstance().AddEnemyObj(shared_from_this());
}

void EnemyBase::OnDestroy() {
	//EnemyManager::GetInstance().RemoveEnemyObj(shared_from_this());
}