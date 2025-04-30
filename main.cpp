#include <iostream>
#include "PolygonalMesh.hpp"
#include "Utils.hpp"
#include "UCDUtilities.hpp"

using namespace std;
using namespace Eigen;
using namespace PolygonalLibrary;


int main()
{
    PolygonalMesh mesh;


    if(!ImportMesh(mesh))
    {
        cerr << "file not found" << endl;
        return 1;
    }


    /// Per visualizzare online le mesh:
    /// 1. Convertire i file .inp in file .vtu con https://meshconverter.it/it
    /// 2. Caricare il file .vtu su https://kitware.github.io/glance/app/

    Gedim::UCDUtilities utilities;
    {
        vector<Gedim::UCDProperty<double>> cell0Ds_properties(1);

        cell0Ds_properties[0].Label = "Marker";
        cell0Ds_properties[0].UnitLabel = "-";
        cell0Ds_properties[0].NumComponents = 1;

        vector<double> cell0Ds_marker(mesh.NumCell0Ds, 0.0);
        for(const auto &m : mesh.MarkerCell0Ds)
            for(const unsigned int id: m.second)
                cell0Ds_marker.at(id) = m.first;

        cell0Ds_properties[0].Data = cell0Ds_marker.data();

        utilities.ExportPoints("./Cell0Ds.inp",
                               mesh.Cell0DsCoordinates,
                               cell0Ds_properties);

    }

    {

        vector<Gedim::UCDProperty<double>> cell1Ds_properties(1);

        cell1Ds_properties[0].Label = "Marker";
        cell1Ds_properties[0].UnitLabel = "-";
        cell1Ds_properties[0].NumComponents = 1;

        vector<double> cell1Ds_marker(mesh.NumCell1Ds, 0.0);
        for(const auto &m : mesh.MarkerCell1Ds)
            for(const unsigned int id: m.second)
                cell1Ds_marker.at(id) = m.first;

        cell1Ds_properties[0].Data = cell1Ds_marker.data();

        utilities.ExportSegments("./Cell1Ds.inp",
                                 mesh.Cell0DsCoordinates,
                                 mesh.Cell1DsExtrema,
                                 {},
                                 cell1Ds_properties);
    }


    {


        vector<Gedim::UCDProperty<double>> cell2Ds_properties(1);

        cell2Ds_properties[0].Label = "Marker";
        cell2Ds_properties[0].UnitLabel = "-";
        cell2Ds_properties[0].NumComponents = 1;

        vector<double> cell2Ds_marker(mesh.NumCell2Ds, 0.0);
        for(const auto &m : mesh.MarkerCell2Ds)
            for(const unsigned int id: m.second)
                cell2Ds_marker.at(id) = m.first;

        cell2Ds_properties[0].Data = cell2Ds_marker.data();

    }

//all markers are correctly stored:
    map<unsigned int, list<unsigned int>> MarkerCell0Ds_correct = {
    {1, {0}},
    {2, {1}},
    {3, {2}},
    {4, {3}},
    {5, {6, 16, 24}},
    {6, {7, 17, 22, 78}},
    {7, {8, 20, 23, 52, 59}},
    {8, {5, 15, 21, 26, 92}}
    };
    
    if ( mesh.MarkerCell0Ds != MarkerCell0Ds_correct) {
        cerr << "Errore nei marker celle 0D" << endl;
    }
    
    map<unsigned int, list<unsigned int>> MarkerCell1Ds_correct = {
	{5, {8,19,22,28}},
    {6, {6, 23, 26,126,127}},
    {7, {14,17,24,79,92, 93}},
    {8, {11,25,29,30,159,160}},
    };
    
    if ( mesh.MarkerCell1Ds != MarkerCell1Ds_correct) {
        cerr << "Errore nei marker celle 1D" << endl;
    }
    map<unsigned int, list<unsigned int>> MarkerCell2Ds_correct = {};
    
    if ( mesh.MarkerCell2Ds != MarkerCell2Ds_correct) {
        cerr << "Errore nei marker celle 2D" << endl;
    }

    return 0;
}