#pragma once

#include "Header/EnemyBase.h"
#include "Enemy.h"
#include "GameObject.h"
#include <memory>
#include <vector>
#include <algorithm>

// --------------------------------------------------
// ◆ EnemyManager（敵の一括管理）
// --------------------------------------------------

class EnemyManager {
private:
	EnemyManager();

	struct SpawnData {
		std::string type;
		std::string scriptType; // スクリプト名
		int spawnTime = 0;
		Vector2D position;
	};

	std::vector<std::string> csvFiles;
	int currentCsvIndex = 0;      // 現在読み込んでいるCSVのインデックス
	int currentListIndex = 0;     // CSV内のWaveインデックス
	int repeatCount = 0;          // 同じWaveを繰り返している回数
	int maxRepeatCount = 3;       // Wave繰り返し上限（2回繰り返す）
	int frameCnt = 0;

	std::vector<SpawnData> spawnList;                  // 今実行中のWave
	std::vector<std::vector<SpawnData>> spawnLists;    // CSVからロードしたWave群
	std::vector<SpawnData> originalSpawnList;

	std::vector<std::shared_ptr<Enemy>> enemys;
	std::unordered_map<std::string, std::function<std::shared_ptr<EnemyBase>(float, const Vector2D&)>> factories;

	float SafeStof(const std::string& s);
	int SafeStoi(const std::string& s);
public:
	static EnemyManager& GetInstance();

	void Init();
	void AddCSVList(const std::vector<std::string>& files);
	void LoadCurrentCSV();
	void LoadCSV(const std::string& path);
	void Update();
	void Spawn();
	void AddEnemyObj(const std::shared_ptr<Enemy>& enemy);
	void RemoveEnemyObj(const std::shared_ptr<Enemy>& enemy);
	void AddFactory(const std::string& type, std::function<std::shared_ptr<EnemyBase>(float, const Vector2D&)> factory);

	const std::vector<std::shared_ptr<Enemy>>& GetEnemys() const;
	std::shared_ptr<GameObject> GetEnemyTarget(const Vector2D& pos);
	void AllEnemyDamage(int damage);
};