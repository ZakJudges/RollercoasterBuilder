#pragma once

#include "TrackPiece.h"
#include <vector>
#include <directxmath.h>

class SplineMesh;
class TrackMesh;

namespace SL
{
	class CRSplineController;
}

class Track
{
public:
	Track(const int resolution, TrackMesh* track_mesh);
	void AddTrackPiece(TrackPiece::Tag tag);
	void AddTrackPieceFromFile(TrackPiece* track_piece);
	void LoadTrack();
	void UpdateSimulation(float t);
	void GenerateMesh();
	void Reset();
	void EraseTrack();
	float GetTrackLength();
	float RecalculateTrackLength();
	int GetTrackPieceCount();
	void RemoveBack();
	void CalculateEndOfSimulation();
	void UpdateBack(TrackPiece* track_piece);
	inline unsigned int GetMaxTrackPieceCount() { return max_segments_; }
	TrackPiece* GetBack();
	TrackMesh* GetTrackMesh();
	TrackPiece* GetTrackPiece(int index);
	DirectX::XMVECTOR GetCameraEye();
	DirectX::XMVECTOR GetCameraLookAt();
	DirectX::XMVECTOR GetCameraUp();
	DirectX::XMFLOAT3 GetPoint();
	DirectX::XMFLOAT3 GetPointAtDistance(float d);
	DirectX::XMFLOAT3 GetPointAtTime(float t);
	DirectX::XMFLOAT3 GetForward();
	DirectX::XMFLOAT3 GetUp();
	DirectX::XMFLOAT3 GetRight();
	SL::Vector GetTangent();
	SL::Vector GetForwardStore();
	SL::Vector GetUpStore();
	SL::Vector GetRightStore();
	inline float GetTargetRollStore() { return target_roll_store_; }
	inline float GetRollStore() { return roll_store_; }
	void CalculatePieceBoundaries();
	void StoreMeshData();
	void GenerateSupportStructures();
	~Track();

private:
	void StoreSimulationValues();
	std::vector<SL::Vector> GetBoundingSphereCentres(int sphere_count);
	int GetActiveTrackPiece();
	float Lerpf(float f0, float f1, float t);

private:
	unsigned int max_segments_;
	std::vector<TrackPiece*> track_pieces_;
	SL::CRSplineController* spline_controller_;
	TrackMesh* track_mesh_;
	int resolution_;
	float t_;
	SL::Vector initial_forward_;
	SL::Vector forward_;
	SL::Vector right_;
	SL::Vector initial_right_;
	SL::Vector up_;
	SL::Vector initial_up_;
	float roll_;	
	float roll_store_;
	float target_roll_store_;
	SL::Vector forward_store_;
	SL::Vector right_store_;
	SL::Vector up_store_;
	bool preview_active_;
	float min_height_;
};