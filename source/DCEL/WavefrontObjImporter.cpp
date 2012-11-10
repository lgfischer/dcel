///////////////////////////////////////////////////////////////////////////////
//
//  Copyright Leonardo Fischer 2011 - http://coderender.blogspot.com
//
//  Distributed under the licence available in the accompanying file 
//  LICENCE.txt. Please read it before use this code.
//
///////////////////////////////////////////////////////////////////////////////

#include "WavefrontObjImporter.h"
#include <fstream>
#include <sstream>

void WavefrontObjLoader::load( const std::string& objFile )
{
    vertices.clear();
    verticeCount=0;
    faces.clear();
    faceCount=0;

    std::ifstream stream;
    try
    {
        stream.open( objFile.c_str(), std::ios::in );
        if( stream )
        {
            // load the obj
            while( stream.good() )
            {
                char buffer[512];
                stream.getline(buffer, sizeof(buffer));
                std::stringstream str(buffer);
                std::string name;
                str >> name;
                if( name=="v" )
                {
                    Vector3f position;
                    str >> position.x >> position.y >> position.z;
                    vertices.push_back( position );
                    verticeCount++;
                }
                else if( name=="f" )
                {
                    unsigned int a, b, c;
                    str >> a >> b >> c;
                    faces.push_back(a-1);
                    faces.push_back(b-1);
                    faces.push_back(c-1);
                    faceCount++;
                }
            }
            stream.close();
        }
        else
        {
            throw cpp::Exception("Can't open the file '"+objFile+"'");
        }
    }
    catch( const std::exception& e )
    {
        if( stream.is_open() )
            stream.close();
        std::cerr << e.what();
        throw;
    }
}
