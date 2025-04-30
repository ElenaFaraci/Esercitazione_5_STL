#include "Utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>


namespace PolygonalLibrary
{
bool ImportMesh(PolygonalMesh& mesh)
{
    if(!ImportCell0Ds(mesh))
        return false;

    if(!ImportCell1Ds(mesh))
        return false;

    if(!ImportCell2Ds(mesh))
        return false;

    return true;
}
// ***************************************************************************
bool ImportCell0Ds(PolygonalMesh& mesh)
{
    ifstream file("./Cell0Ds.csv");

    if(file.fail())
        return false;

    list<string> listLines;

    string line;
    while (getline(file, line))
        listLines.push_back(line);

    file.close();

    // remove header
    listLines.pop_front();

    mesh.NumCell0Ds = listLines.size();

    if (mesh.NumCell0Ds == 0)
    {
        cerr << "There is no cell 0D" << endl;
        return false;
    }

    mesh.Cell0DsId.reserve(mesh.NumCell0Ds);
    mesh.Cell0DsCoordinates = Eigen::MatrixXd::Zero(3, mesh.NumCell0Ds);

    for (const string& line : listLines)
    {
        istringstream converter(line);

        unsigned int id;
        unsigned int marker;
        char delimiter;
        Vector2d coord;

        converter >>  id >> delimiter >> marker >> delimiter >> mesh.Cell0DsCoordinates(0, id) >> delimiter >> mesh.Cell0DsCoordinates(1, id);

        mesh.Cell0DsId.push_back(id);

        /// Memorizza i marker
        if(marker != 0)
        {
            const auto it = mesh.MarkerCell0Ds.find(marker);
            if(it == mesh.MarkerCell0Ds.end())
            {
                mesh.MarkerCell0Ds.insert({marker, {id}});
            }
            else
            {
                // mesh.MarkerCell0Ds[marker].push_back(id);
                it->second.push_back(id);
            }
        }

    }

    return true;
}
// ***************************************************************************
bool ImportCell1Ds(PolygonalMesh& mesh)
{
    ifstream file("./Cell1Ds.csv");

    if(file.fail())
        return false;

    list<string> listLines;
    string line;
    while (getline(file, line))
        listLines.push_back(line);

    file.close();

    // remove header
    listLines.pop_front();

    mesh.NumCell1Ds = listLines.size();

    if (mesh.NumCell1Ds == 0)
    {
        cerr << "There is no cell 1D" << endl;
        return false;
    }

    mesh.Cell1DsId.reserve(mesh.NumCell1Ds);
    mesh.Cell1DsExtrema = Eigen::MatrixXi(2, mesh.NumCell1Ds);

    for (const string& line : listLines)
    {
        istringstream converter(line);

        unsigned int id;
        unsigned int marker;
        Vector2i vertices;
        char delimiter;

        converter >>  id >> delimiter >> marker >> delimiter >>  mesh.Cell1DsExtrema(0, id) >> delimiter >>  mesh.Cell1DsExtrema(1, id);
        mesh.Cell1DsId.push_back(id);

        /// Memorizza i marker
        if(marker != 0)
        {
            const auto it = mesh.MarkerCell1Ds.find(marker);
            if(it == mesh.MarkerCell1Ds.end())
            {
                mesh.MarkerCell1Ds.insert({marker, {id}});
            }
            else
            {
                // mesh.MarkerCell1Ds[marker].push_back(id);
                it->second.push_back(id);
            }
        }
    }

    return true;
}
// ***************************************************************************
bool ImportCell2Ds(PolygonalMesh& mesh)
{
    ifstream file;
    file.open("./Cell2Ds.csv");

    if(file.fail())
        return false;

    list<string> listLines;
    string line;
    while (getline(file, line))
        listLines.push_back(line);

    file.close();

    // remove header
    listLines.pop_front();

    mesh.NumCell2Ds = listLines.size();

    if (mesh.NumCell2Ds == 0)
    {
        cerr << "There is no cell 2D" << endl;
        return false;
    }

    mesh.Cell2DsId.reserve(mesh.NumCell2Ds);
    mesh.Cell2DsVertices_Num.reserve(mesh.NumCell2Ds);
    mesh.Cell2DsEdges_Num.reserve(mesh.NumCell2Ds);
    mesh.Cell2DsVertices.reserve(mesh.NumCell2Ds);
    mesh.Cell2DsEdges.reserve(mesh.NumCell2Ds);

    for (const string& line : listLines)
    {
        istringstream converter(line);
        unsigned int id;
        unsigned int marker;
        unsigned int vertices_num;
        unsigned int edges_num;
        
        
        char delimiter;

        converter >> id >> delimiter >> marker >> delimiter >> vertices_num;
        vector<unsigned int> vertices(vertices_num);
        for(unsigned int i = 0; i < vertices_num; i++)
        	converter >> delimiter >> vertices[i];
	
        converter >> delimiter >> edges_num;
        vector<unsigned int> edges(edges_num);
        for(unsigned int i = 0; i < edges_num; i++)
        	converter >> delimiter >> edges[i];

        mesh.Cell2DsId.push_back(id);
        mesh.Cell2DsVertices_Num.push_back(vertices_num);
        mesh.Cell2DsEdges_Num.push_back(edges_num);
        mesh.Cell2DsVertices.push_back(vertices);
        mesh.Cell2DsEdges.push_back(edges);

        /// Memorizza i marker
        if(marker != 0)
        {
            const auto it = mesh.MarkerCell2Ds.find(marker);
            if(it == mesh.MarkerCell2Ds.end())
            {
                mesh.MarkerCell2Ds.insert({marker, {id}});
            }
            else
            {
                // mesh.MarkerCell2Ds[marker].push_back(id);
                it->second.push_back(id);
            }
        }
    }

    return true;
}

// ***************************************************************************

bool EdgeNonZero(PolygonalMesh& mesh)
{
	const double tol = 1e-8;
	for (unsigned int i = 0; i < mesh.NumCell0Ds; i++)
	{
		unsigned int origin = mesh.Cell1DsExtrema(0,i);
		unsigned int end = mesh.Cell1DsExtrema(1,i);
		
		double x1 = mesh.Cell0DsCoordinates(0, origin);
		double y1 = mesh.Cell0DsCoordinates(1, origin);
		double x2 = mesh.Cell0DsCoordinates(0, end);
		double y2 = mesh.Cell0DsCoordinates(1, end);
		
		double distance = sqrt(pow((x2-x1),2)+pow((y2-y1),2));
		if (distance < tol)
			cerr << "Edge " << i << "has lenght zero." << endl;
			return false;
		
		}

    return true;

}

// ***************************************************************************

bool PolygonAreaNonZero (PolygonalMesh& mesh)
{
	const double tol = 1e-16;
	for (unsigned int i = 0; i < mesh.NumCell2Ds; ++i) {
        const auto& vertices = mesh.Cell2DsVertices[i];
        double area = 0.0;

        for (size_t j = 0; j < vertices.size(); ++j) {
            unsigned int current = vertices[j];
            unsigned int next = vertices[(j + 1) % vertices.size()];

            double x1 = mesh.Cell0DsCoordinates(0, current);
            double y1 = mesh.Cell0DsCoordinates(1, current);
            double x2 = mesh.Cell0DsCoordinates(0, next);
            double y2 = mesh.Cell0DsCoordinates(1, next);

            area += x1 * y2 - x2 * y1;
        }

        area = std::fabs(area) / 2.0;
        if (area <= tol) 
            return false;
      
    }
    return true;

}

}