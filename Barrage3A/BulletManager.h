#pragma once

#include "GameObject.h"
#include "BulletBase.h"

class BulletManager {
private:

	std::vector<std::shared_ptr<BulletBase>> bullets;

	BulletManager() {}
public:
	static BulletManager& Instance() {
		static BulletManager instance;
		return instance;
	}

	void AddBullet(const std::shared_ptr<BulletBase>&);
	void RemoveBullet(const std::shared_ptr<BulletBase>&);

	void AllDestroyLayer(Layer layer);
};