///////////////////////////////////////////////////////////////////////////////
//
//  Copyright Leonardo Fischer 2011 - http://coderender.blogspot.com
//
//  Distributed under the licence available in the accompanying file 
//  LICENCE.txt. Please read it before use this code.
//
///////////////////////////////////////////////////////////////////////////////

#include "PlyImporter.h"
#include "../rply/rply.h"

/*
    This code uses the RPly 1.01 library, from 
    http://w3.impa.br/~diego/software/rply/.

    The code for this lib can be downloaded from 
    http://w3.impa.br/~diego/software/rply/rply-1.01.tar.gz. There is a copy of
    the RPly 1.01 available with this file.
*/

int vertex_cb(p_ply_argument argument)
{
    PlyLoader* loader;
    long coordinate;

    ply_get_argument_user_data(argument, (void**)&loader, &coordinate);
    float position = (float)ply_get_argument_value(argument);
    switch(coordinate)
    {
    case 0:
        loader->vertices.push_back( Vector3f(position,0,0) );
        break;
    case 1:
        loader->vertices.back().y = position;
        break;
    case 2:
        loader->vertices.back().z = position;
    }
    return 1;
}

int face_cb(p_ply_argument argument)
{
    long length, value_index;
    ply_get_argument_property(argument, NULL, &length, &value_index);

    if( length!=3 ) //ensures only triangles are read
    {
        throw cpp::Exception("This file contains other things than triangles (1)!'");
    }
    unsigned int val = (unsigned int)ply_get_argument_value(argument);
    if( value_index<0 )
    {
        if( val!=3 ) //ensures only triangles are read
        {
            throw cpp::Exception("This file contains other things than triangles (2)!'");
        }
        return 1; // just returns, this is not usefull data (its the size of the list that will follow)
    }

    PlyLoader* loader;
    ply_get_argument_user_data(argument, (void**)&loader, NULL);
    loader->faces.push_back( val );
    
    return 1;
}

void error_cb(const char *message)
{
    std::cerr << message << std::endl;
}

void PlyLoader::load( const std::string& plyFilename )
{
    this->vertices.clear();
    this->verticeCount = 0;
    this->faces.clear();
    this->faceCount = 0;

    p_ply ply = ply_open(plyFilename.c_str(), error_cb);
    if( !ply )
    {
        throw cpp::Exception(std::string("Cannot open the file '")+plyFilename+"'");
    }

    if(!ply_read_header(ply) ) 
    {
        throw cpp::Exception(std::string("Error loading the header of the file '")+plyFilename+"'");
    }

    this->verticeCount = ply_set_read_cb(ply, "vertex", "x", vertex_cb, this, 0);
    ply_set_read_cb(ply, "vertex", "y", vertex_cb, this, 1);
    ply_set_read_cb(ply, "vertex", "z", vertex_cb, this, 2);

    this->faceCount = ply_set_read_cb(ply, "face", "vertex_indices", face_cb, this, 0);
    if (!ply_read(ply))
    {
        throw cpp::Exception(std::string("Error loading the the whole file '")+plyFilename+"'");
    }
    ply_close(ply);
}
