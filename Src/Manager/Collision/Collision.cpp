#include "Collision.h"
#include <algorithm>
#include <cmath>

#include"../../Utility/Utility.h"

Collision::Collision() : player(),stageObject_(),enemy() {}
Collision::~Collision() {}

void Collision::Check()
{
	// ステージオブジェクトとオブジェクトの当たり判定
	for (auto& s : stageObject_) {
		for (auto& o : player) {
			const Base& us = s->GetUnit();
			const Base& uo = o->GetUnit();

			if ((us.aliveCollision_ && !us.isAlive_) || (uo.aliveCollision_ && !uo.isAlive_)) continue;

			if (IsHit(us, uo)) {
				s->OnCollision(o);
				o->OnCollision(s);
			}
		}
	}

	for (auto& s : stageObject_) {
		for (auto& e : enemy) {
			const Base& us = s->GetUnit();
			const Base& ue = e->GetUnit();

			if ((us.aliveCollision_ && !us.isAlive_) || (ue.aliveCollision_ && !ue.isAlive_)) continue;

			if (IsHit(us, ue)) {
				s->OnCollision(e);
				e->OnCollision(s);
			}
		}
	}

	for (auto& o : player) {
		for (auto& e : enemy) {
			const Base& uo = o->GetUnit();
			const Base& ue = e->GetUnit();

			if ((uo.aliveCollision_ && !uo.isAlive_) || (ue.aliveCollision_ && !ue.isAlive_)) continue;

			if (IsHit(uo, ue)) {
				o->OnCollision(e);
				e->OnCollision(o);
			}
		}
	}
}

bool Collision::IsHit(const Base& a, const Base& b)
{
	Base A = a; A.pos_ = VAdd(A.pos_, A.para_.center);
	Base B = b; B.pos_ = VAdd(B.pos_, B.para_.center);

	auto sA = A.para_.colliShape;
	auto sB = B.para_.colliShape;
	if (sA == CollisionShape::NON || sB == CollisionShape::NON) return false;

	// 同種
	if (sA == CollisionShape::SPHERE && sB == CollisionShape::SPHERE)    return SphereSphere(A, B);
	if (sA == CollisionShape::OBB && sB == CollisionShape::OBB) return ObbObb(A, B);
	if (sA == CollisionShape::CAPSULE && sB == CollisionShape::CAPSULE)   return CapsuleCapsule(A, B);

	// 混合
	if (sA == CollisionShape::SPHERE && sB == CollisionShape::OBB) return SphereObb(A, B);
	if (sA == CollisionShape::OBB && sB == CollisionShape::SPHERE)    return SphereObb(B, A);

	if (sA == CollisionShape::SPHERE && sB == CollisionShape::CAPSULE)   return SphereCapsule(A, B);
	if (sA == CollisionShape::CAPSULE && sB == CollisionShape::SPHERE)    return SphereCapsule(B, A);

	if (sA == CollisionShape::CAPSULE && sB == CollisionShape::OBB) return CapsuleObb(A, B);
	if (sA == CollisionShape::OBB && sB == CollisionShape::CAPSULE)   return CapsuleObb(B, A);

	return false;
}

Collision::Obb Collision::MakeObb(const Base& box) const
{
	Obb r;

	// ワールドでの中心
	r.center = VAdd(box.pos_, box.para_.center);

	// 半分サイズ
	r.half = VScale(box.para_.size, 0.5f);

	// 単位基底を回転して取得（正規直交）
	r.axis[0] = VTransform({ 1.0f, 0.0f, 0.0f }, Utility::MatrixAllMultXYZ({ box.angle_ }));
	r.axis[1] = VTransform({ 0.0f, 1.0f, 0.0f }, Utility::MatrixAllMultXYZ({ box.angle_ }));
	r.axis[2] = VTransform({ 0.0f, 0.0f, 1.0f }, Utility::MatrixAllMultXYZ({ box.angle_ }));

	return r;
}

bool Collision::SphereSphere(const Base& a, const Base& b) const
{
	VECTOR d = VSub(a.pos_, b.pos_);
	float rr = (a.para_.radius + b.para_.radius);
	return VDot(d, d) <= rr * rr;
}

bool Collision::ObbObb(const Base& a, const Base& b) const
{
	Obb A = MakeObb(a);
	Obb B = MakeObb(b);

	// 回転行列 R_ij = Ai・Bj
	float R[3][3], AbsR[3][3];
	const float EPS = 1e-6f;

	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			R[i][j] = VDot(A.axis[i], B.axis[j]);
			AbsR[i][j] = std::fabs(R[i][j]) + EPS;
		}
	}

	// t を A のローカル基底へ
	VECTOR t3 = VSub(B.center, A.center);
	float t[3] = { VDot(t3, A.axis[0]), VDot(t3, A.axis[1]), VDot(t3, A.axis[2]) };

#define VCOMP(v,i) ((i)==0 ? (v).x : ((i)==1 ? (v).y : (v).z))

	// 1) A の軸
	for (int i = 0; i < 3; ++i) {
		float ra = VCOMP(A.half, i);
		float rb = B.half.x * AbsR[i][0] + B.half.y * AbsR[i][1] + B.half.z * AbsR[i][2];
		if (std::fabs(t[i]) > ra + rb) return false;
	}

	// 2) B の軸
	for (int i = 0; i < 3; ++i) {
		float ra = A.half.x * AbsR[0][i] + A.half.y * AbsR[1][i] + A.half.z * AbsR[2][i];
		float rb = VCOMP(B.half, i);
		float tj = std::fabs(VDot(t3, B.axis[i]));
		if (tj > ra + rb) return false;
	}

	// 3) 交差軸 9本（Ai x Bj）
	// 関数内で安全に各成分を取得
	auto comp = [](const VECTOR& v, int i) -> float {
		if (i == 0) return v.x;
		if (i == 1) return v.y;
		return v.z;
		};

	for (int i = 0; i < 3; ++i) for (int j = 0; j < 3; ++j) {
		float ra = A.half.y * AbsR[(i + 2) % 3][j] + A.half.z * AbsR[(i + 1) % 3][j];
		float rb = B.half.y * AbsR[i][(j + 1) % 3] + B.half.z * AbsR[i][(j + 2) % 3];

		// |t・(Ai×Bj)| を計算。Ai×Bj は正規直交系の外積なので、tをB基底へ写して近似
		// 簡易で | t.y * R[(i+2)%3][j] - t.z * R[(i+1)%3][j] | のように成分を組む方法が知られている
		float tval = std::fabs(
			t[(i + 1) % 3] * R[(i + 2) % 3][j] - t[(i + 2) % 3] * R[(i + 1) % 3][j]
		);

		if (tval > ra + rb) return false;
	}

	return true;
}

bool Collision::CapsuleCapsule(const Base& a, const Base& b) const 
{
	// 中心線方向（Y軸を回転）
	VECTOR aDir = VTransform({ 0, 1, 0 }, Utility::MatrixAllMultXYZ({ a.angle_ }));
	VECTOR bDir = VTransform({ 0, 1, 0 }, Utility::MatrixAllMultXYZ({ b.angle_ }));

	VECTOR a1 = VAdd(a.pos_, VScale(aDir, a.para_.capsuleHalfLen));
	VECTOR a2 = VSub(a.pos_, VScale(aDir, a.para_.capsuleHalfLen));
	VECTOR b1 = VAdd(b.pos_, VScale(bDir, b.para_.capsuleHalfLen));
	VECTOR b2 = VSub(b.pos_, VScale(bDir, b.para_.capsuleHalfLen));

	float distSq = SegmentSegmentDistSq(a1, a2, b1, b2);
	float rr = a.para_.radius + b.para_.radius;
	return distSq <= rr * rr;
}

bool Collision::SphereObb(const Base& sphere, const Base& obb) const
{
	Obb B = MakeObb(obb);

	// 球中心→OBB ローカルへ
	VECTOR d = VSub(sphere.pos_, B.center);
	float local[3] = { VDot(d, B.axis[0]), VDot(d, B.axis[1]), VDot(d, B.axis[2]) };

	// クリップして最近点
	float qx = (local[0] < -B.half.x) ? -B.half.x : (local[0] > B.half.x ? B.half.x : local[0]);
	float qy = (local[1] < -B.half.y) ? -B.half.y : (local[1] > B.half.y ? B.half.y : local[1]);
	float qz = (local[2] < -B.half.z) ? -B.half.z : (local[2] > B.half.z ? B.half.z : local[2]);

	// 最近点をワールドへ戻す
	VECTOR closest = VAdd(B.center,
		VAdd(VScale(B.axis[0], qx),
			VAdd(VScale(B.axis[1], qy),
				VScale(B.axis[2], qz))));

	VECTOR diff = VSub(sphere.pos_, closest);
	return VDot(diff, diff) <= sphere.para_.radius * sphere.para_.radius;
}

bool Collision::SphereCapsule(const Base& sphere, const Base& capsule) const {
	// カプセルの中心線方向（Y軸ベースを angle_ で回転）
	VECTOR dir = VTransform({ 0.0f, 1.0f, 0.0f }, Utility::MatrixAllMultXYZ({ capsule.angle_ }));

	// 中心線の端点（center は IsHit で pos_ に既に加算済み）
	VECTOR p1 = VAdd(capsule.pos_, VScale(dir, capsule.para_.capsuleHalfLen));
	VECTOR p2 = VSub(capsule.pos_, VScale(dir, capsule.para_.capsuleHalfLen));

	// 球中心から中心線（線分）までの最近距離
	float dist2 = PointSegmentDistSq(sphere.pos_, p1, p2);

	// 半径の和と比較（カプセル半径＋球半径）
	float rr = sphere.para_.radius + capsule.para_.radius;
	return dist2 <= rr * rr;
}

bool Collision::CapsuleObb(const Base& capsule, const Base& obb) const {

	Obb B = MakeObb(obb);

	// カプセル中心線（任意向き）
	VECTOR dir = VTransform({ 0, 1, 0 }, Utility::MatrixAllMultXYZ({ capsule.angle_ }));
	VECTOR p1 = VAdd(capsule.pos_, VScale(dir, capsule.para_.capsuleHalfLen));
	VECTOR p2 = VSub(capsule.pos_, VScale(dir, capsule.para_.capsuleHalfLen));

	// OBB ローカルへ変換（原点=box.center、基底=axis[3]）
	auto toLocal = [&](const VECTOR& p)->VECTOR {
		VECTOR d = VSub(p, B.center);
		return {
			VDot(d, B.axis[0]),
			VDot(d, B.axis[1]),
			VDot(d, B.axis[2])
		};
		};

	VECTOR lp1 = toLocal(p1);
	VECTOR lp2 = toLocal(p2);

	float distSq = SegmentAabbDistSq_Local(lp1, lp2, B.half);
	return distSq <= capsule.para_.radius * capsule.para_.radius;
}

float Collision::SegmentSegmentDistSq(const VECTOR& p1, const VECTOR& q1, const VECTOR& p2, const VECTOR& q2) const {
	VECTOR d1 = VSub(q1, p1); // 1の方向
	VECTOR d2 = VSub(q2, p2); // 2の方向
	VECTOR r = VSub(p1, p2);
	float a = VDot(d1, d1);
	float e = VDot(d2, d2);
	float f = VDot(d2, r);

	float s, t;

	if (a <= 1e-8f && e <= 1e-8f) {
		// 双方点
		return VDot(r, r);
	}
	if (a <= 1e-8f) {
		// 1が点
		s = 0.0f;
		t = std::clamp(f / e, 0.0f, 1.0f);
	}
	else {
		float c = VDot(d1, r);
		if (e <= 1e-8f) {
			// 2が点
			t = 0.0f;
			s = std::clamp(-c / a, 0.0f, 1.0f);
		}
		else {
			float b = VDot(d1, d2);
			float denom = a * e - b * b;
			if (denom != 0.0f) s = std::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
			else               s = 0.0f;
			float tnom = (b * s + f);
			t = std::clamp(tnom / e, 0.0f, 1.0f);

			// s を再調整
			float sNom = (b * t - c);
			s = std::clamp((denom != 0.0f) ? sNom / a : 0.0f, 0.0f, 1.0f);
		}
	}

	VECTOR c1 = VAdd(p1, VScale(d1, s));
	VECTOR c2 = VAdd(p2, VScale(d2, t));
	VECTOR d = VSub(c1, c2);
	return VDot(d, d);
}

float Collision::SegmentAabbDistSq_Local(const VECTOR& p0, const VECTOR& p1, const VECTOR& half) const
{
	const int steps = 10; // 必要に応じて増減
	float best = FLT_MAX;
	for (int i = 0; i <= steps; ++i) {
		float t = (float)i / steps;
		VECTOR p = VAdd(p0, VScale(VSub(p1, p0), t));
		float qx = (p.x < -half.x) ? -half.x : (p.x > half.x ? half.x : p.x);
		float qy = (p.y < -half.y) ? -half.y : (p.y > half.y ? half.y : p.y);
		float qz = (p.z < -half.z) ? -half.z : (p.z > half.z ? half.z : p.z);
		VECTOR q = { qx, qy, qz };
		VECTOR d = VSub(p, q);
		best = (std::min)(best, VDot(d, d));
	}
	return best;
}

float Collision::PointSegmentDistSq(const VECTOR& p, const VECTOR& a, const VECTOR& b) const {
	VECTOR ab = VSub(b, a);
	VECTOR ap = VSub(p, a);
	float ab2 = VDot(ab, ab);
	if (ab2 <= 1e-8f) {
		VECTOR d = VSub(p, a);
		return VDot(d, d);
	}
	float t = VDot(ap, ab) / ab2;
	if (t < 0.0f) t = 0.0f;
	else if (t > 1.0f) t = 1.0f;
	VECTOR q = VAdd(a, VScale(ab, t));
	VECTOR d = VSub(p, q);
	return VDot(d, d);
}

