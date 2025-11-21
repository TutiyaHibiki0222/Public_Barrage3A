#include "EnemyManager.h"
#include"EnemyScriptManager.h"
#include "CsvReader.hpp"
#include "GameObjectMgr.h"
#include "Enemy02.h"
#include "Enemy03.h"
#include "Enemy04.h"
#include "GameManager.h"
#define ENEMY_LIST_MAX (2)

EnemyManager::EnemyManager() {}

EnemyManager& EnemyManager::GetInstance() {
	static EnemyManager instance;
	return instance;
}


void EnemyManager::Init() {
	// 初期化処理などがあればここに
	frameCnt = 0;
}

void EnemyManager::AddCSVList(const std::vector<std::string>& files) {
	csvFiles = files;
	currentCsvIndex = 0;
	repeatCount = 0;
	if (!csvFiles.empty()) {
		LoadCSV(csvFiles[0]);
	}
}

void EnemyManager::LoadCurrentCSV() {
	if (csvFiles.empty()) return;

	// safety: clamp currentCsvIndex into valid range
	if (currentCsvIndex < 0) currentCsvIndex = 0;
	if (currentCsvIndex >= static_cast<int>(csvFiles.size())) {
		currentCsvIndex = static_cast<int>(csvFiles.size()) - 1;
	}

	LoadCSV(csvFiles[currentCsvIndex]);
}

void EnemyManager::LoadCSV(const std::string& path) {
	auto csv = System::IO::CsvReader::ReadCsvFile(path);
	if (csv.empty()) return;

	// --- 前回のリストをクリア ---
	spawnLists.clear();          // もし残ってるならクリア（使ってないなら無害）
	originalSpawnList.clear();
	spawnList.clear();

	for (size_t i = 1; i < csv.size(); ++i) {
		const auto& row = csv[i];
		if (row.size() < 5) continue;

		SpawnData meta;
		meta.spawnTime = SafeStoi(row[0]);
		meta.position.x = SafeStof(row[1]);
		meta.position.y = SafeStof(row[2]);
		meta.type = row[3];
		meta.scriptType = row[4];

		// マスターコピーに追加
		originalSpawnList.push_back(meta);
	}

	// --- safety: original が空なら何もしない ---
	if (originalSpawnList.empty()) {
		Debug::WarningLog("LoadCSV: originalSpawnList is empty: " + path);
		return;
	}

	// spawnList を original のコピーで初期化（ここが重要）
	spawnList = originalSpawnList;

	// --- 初期化 ---
	currentListIndex = 0; // もし header 側で使っているなら残しておく（今回のロジックでは実際は不要）
	repeatCount = 0;
	frameCnt = 0;

#if _DEBUG
	for (size_t i = 0; i < originalSpawnList.size(); ++i) {
		const auto& s = originalSpawnList[i];
		std::cout << "[EnemyManager] row " << i
			<< " type: " << s.type
			<< ", time: " << s.spawnTime
			<< ", pos: (" << s.position.x << "," << s.position.y << ")\n";
	}
#endif
}


float EnemyManager::SafeStof(const std::string& s) {
	try { return s.empty() ? 0.0f : std::stof(s); }
	catch (...) { return 0.0f; }
}

int EnemyManager::SafeStoi(const std::string& s) {
	try { return s.empty() ? 0 : std::stoi(s); }
	catch (...) { return 0; }
}

void EnemyManager::AddEnemyObj(const std::shared_ptr<Enemy>& enemy) {
	enemys.push_back(enemy);
}

void EnemyManager::RemoveEnemyObj(const std::shared_ptr<Enemy>& enemy) {
	enemys.erase(
		std::remove(enemys.begin(), enemys.end(), enemy),
		enemys.end()
	);
}

void EnemyManager::Update() {
	if (GameManager::GetInstance().IsPause()) return;
	Spawn();
}

// 敵の出現処理
void EnemyManager::Spawn() {
	if (spawnList.empty()) return;

	frameCnt++;

	auto it = spawnList.begin();
	while (it != spawnList.end()) {
		if (frameCnt >= it->spawnTime) {
			const std::string& prefabKey = it->type;

			// --- End はループ / CSV 切替のトリガー ---
			if (prefabKey == "End") {
				// 今のリストを1周した
				repeatCount++;
				if (repeatCount < maxRepeatCount) {
					// もう1回同じCSVを最初から（original から復元）
					spawnList = originalSpawnList;
					frameCnt = 0;
					Debug::Log("Enemy Spawn: End → 同じCSVを再スタート (repeat=" + std::to_string(repeatCount) + ")");
				}
				else {
					// リピート回数到達 -> 次のCSVに移るか、無ければ現在のCSVをループ
					repeatCount = 0;

					if (currentCsvIndex + 1 < static_cast<int>(csvFiles.size())) {
						// 次のCSVがある -> インクリメントしてロード
						currentCsvIndex++;
						LoadCurrentCSV(); // LoadCSV 内で originalSpawnList と spawnList をセットする
						Debug::Log("Enemy Spawn: 次のCSVをロード (index=" + std::to_string(currentCsvIndex) + ")");
					}
					else {
						// 次のCSVが無い -> 現在のCSVを再スタート（最後のCSVを繰り返す）
						spawnList = originalSpawnList;
						frameCnt = 0;
						Debug::Log("Enemy Spawn: 次CSV無し -> 現在のCSVをループ継続");
					}
				}
				return; // End を処理したら一旦抜ける（次フレームから再開）
			}

			// --- Prefab & Script処理 ---
			const std::string& scriptKey = it->scriptType;
			auto script = EnemyScriptManager::GetInstance().CloneEnemyScript(scriptKey);
			if (!script) {
				Debug::WarningLog("Scriptが見つかりません: " + scriptKey);
				it = spawnList.erase(it);
				continue;
			}

			if (!PrefabMgr.HasPrefab(prefabKey)) {
				Debug::WarningLog("Prefabが見つかりません: " + prefabKey);
				it = spawnList.erase(it);
				continue;
			}

			auto list = PrefabMgr.Instantiate(prefabKey);
			if (list.empty()) {
				Debug::WarningLog("Prefabリストは空です: " + prefabKey);
				it = spawnList.erase(it);
				continue;
			}

			auto enemyObj = list[0];
			enemyObj->transform->position = it->position;

			auto enemy = enemyObj->AddAppBase<Enemy>();
			if (enemy) {
				enemy->SetScript(script);
				Debug::Log("Spawn完了: " + prefabKey);
			}

			it = spawnList.erase(it);
		}
		else {
			++it;
		}
	}
}





void EnemyManager::AddFactory(const std::string& type, std::function<std::shared_ptr<EnemyBase>(float, const Vector2D&)> factory) {
	factories[type] = factory;
}

const std::vector<std::shared_ptr<Enemy>>& EnemyManager::GetEnemys() const {
	return enemys;
}

std::shared_ptr<GameObject> EnemyManager::GetEnemyTarget(const Vector2D& pos) {
	std::shared_ptr<GameObject> nearest = nullptr;
	float nearestDistSq = (std::numeric_limits<float>::max)();

	for (const auto& enemy : enemys) {
		if (!enemy) continue;

		auto obj = enemy->GetGameObject();
		if (!obj || !obj->IsActive()) continue;

		float distSq = (obj->transform->position - pos).LengthSquared();
		if (distSq < nearestDistSq) {
			nearestDistSq = distSq;
			nearest = obj;
		}
	}
	return nearest;
}

void EnemyManager::AllEnemyDamage(int damage) {
	for (const auto& enemy : enemys) {
		if (enemy) {
			enemy->Damage(damage);
		}
	}
}