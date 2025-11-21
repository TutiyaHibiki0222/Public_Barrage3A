#include "BulletManager.h"
#include "MyBullet.h"
#include "Collider2D.h"
#include "BulletType.h"

void BulletManager::AddBullet(const std::shared_ptr<BulletBase>& obj) {
	bullets.push_back(obj);
}

void BulletManager::RemoveBullet(const std::shared_ptr<BulletBase>& obj) {
	bullets.erase(
		std::remove(bullets.begin(), bullets.end(), obj),
		bullets.end()
	);
}

void BulletManager::AllDestroyLayer(Layer layer) {
	for(const auto& bullet : bullets) {
		if (bullet->GetGameObject()->GetLayer() == layer) {
			bullet->Destroy();
		}
	}
}
