#pragma once

#include <vector>
#include <DxLib.h>
#include "../../Object/UnitBase.h"

class Collision
{
public:
	Collision();
	~Collision();

	void AddObject(UnitBase* obj) { player.push_back(obj); }
	void AddObject(std::vector<UnitBase*>obj) { for (auto& o : obj) { AddObject(o); } }

	void AddStage(UnitBase* obj) { stageObject_.push_back(obj); }
	void AddStage(std::vector<UnitBase*>obj) { for (auto& o : obj) { AddStage(o); } }

	void AddEnemy(UnitBase* obj) { enemy.push_back(obj); }
	void AddEnemy(std::vector<UnitBase*>obj) { for (auto& o : obj) { AddEnemy(o); } }
	void Check();
	void Clear() { player.clear(); stageObject_.clear(); enemy.clear(); }

private:
	std::vector<UnitBase*> player;
	std::vector<UnitBase*> stageObject_;
	std::vector<UnitBase*> enemy;

	// 衝突関数振り分け
	bool IsHit(const Base& a, const Base& b);

	// 衝突関数
	bool SphereSphere(const Base& a, const Base& b) const;
	bool ObbObb(const Base& a, const Base& b) const;
	bool CapsuleCapsule(const Base& a, const Base& b) const;
	bool SphereObb(const Base& sphere, const Base& obb) const;
	bool SphereCapsule(const Base& sphere, const Base& capsule) const;
	bool CapsuleObb(const Base& capsule, const Base& obb) const;


#pragma region 補助

	// OBB情報
	struct Obb { VECTOR center; VECTOR half; VECTOR axis[3]; };

	// OBB作成
	Obb MakeObb(const Base& box) const;

	// 点と線分の距離の2乗
	float SegmentSegmentDistSq(const VECTOR& p1, const VECTOR& q1, const VECTOR& p2, const VECTOR& q2) const;
	//　上のSegmentSegmentDistSq()の特殊ケースわずかに高速化(ほとんど誤差)
	float PointSegmentDistSq(const VECTOR& p, const VECTOR& a, const VECTOR& b) const;


	// 線分とAABBの距離の2乗（ローカル座標系）
	float SegmentAabbDistSq_Local(const VECTOR& p0, const VECTOR& p1, const VECTOR& half) const;

#pragma endregion
};