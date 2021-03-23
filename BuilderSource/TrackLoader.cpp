#include "TrackLoader.h"
#include "FromFile.h"
#include "Track.h"
#include <iostream>
#include <fstream>
#include <vector>

TrackLoader::TrackLoader() 
{

}

//  Extract track data from a text file.
bool TrackLoader::LoadTrack(char file_name[], Track* track)
{
    track->EraseTrack();

    std::ifstream file(file_name);
    if (file.is_open())
    {  
        std::vector<float> data;
        
        int track_piece_count = 0;
        file >> track_piece_count;

        for (int i = 0; i < track_piece_count; i++)
        {
            TrackPiece* piece = new FromFile();
            SL::Vector point;
            float x, y, z, tension, roll_target, length = 0.0f;

            file >> x >> y >> z;
            point.Set(x, y, z);
            piece->SetControlPoint(0, point);

            file >> x >> y >> z;
            point.Set(x, y, z);
            piece->SetControlPoint(1, point);

            file >> x >> y >> z;
            point.Set(x, y, z);
            piece->SetControlPoint(2, point);

            file >> x >> y >> z;
            point.Set(x, y, z);
            piece->SetControlPoint(3, point);

            file >> tension;
            piece->SetTension(tension);

            file >> roll_target;
            piece->SetRollTarget(roll_target);

            file >> length;
            piece->SetLength(length);

            piece->CalculateSpline();

            track->AddTrackPieceFromFile(piece);
        }
       
        file.close();

        track->LoadTrack();
       

        return true;
    }
    return false;
}

//  Input track data into a text file.
bool TrackLoader::SaveTrack(char file_name[], Track* track)
{
    std::ofstream file(file_name);
    if (file.is_open())
    {
        file << track->GetTrackPieceCount() << std::endl;

        //  Loop through each track piece and input track data to file.
        for (int i = 0; i < track->GetTrackPieceCount(); i++)
        {
            TrackPiece* piece = track->GetTrackPiece(i);
            SL::Vector point = piece->GetControlPoint(0);
            file << point.X() << " " << point.Y()<< " " << point.Z() << std::endl;
            point = piece->GetControlPoint(1);
            file << point.X() << " " << point.Y() << " " << point.Z() << std::endl;
            point = piece->GetControlPoint(2);
            file << point.X() << " " << point.Y() << " " << point.Z() << std::endl;
            point = piece->GetControlPoint(3);
            file << point.X() << " " << point.Y() << " " << point.Z() << std::endl;
            file << piece->GetTension() << std::endl;
            file << piece->GetRollTarget() << std::endl;
            file << piece->GetLength() << std::endl;
        }

        file.close();

        return true;
    }
    return false;
}
