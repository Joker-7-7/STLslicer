#include "readBINARY.h"

// -------------------------------------------------------------------------------------------------------------------
// this function opens the STL file specified by the user and reads the contents

int read_binary_STL_file(std::string STL_filename, std::vector<Triangle>& facet)
{
    // specify the location of STL files on this computer

    // declare an (input) file object
    std::ifstream binaryInputFile;

    // open the STL file by using the full path and the name
    // specify that the file is opened in "read-only" mode
    // specify that the file is opened in binary format
    binaryInputFile.open((STL_filename).c_str(), std::ifstream::in | std::ifstream::binary);

    // check whether the file was opened successfully
    // if yes then continue otherwise terminate program execution
    if (binaryInputFile.fail())
    {
        std::cout << "ERROR: Input STL file could not be opened!" << std::endl;
        return (1);
    }

    // position the pointer to byte number 80
    binaryInputFile.seekg(80);

    // read the number of facets (triangles) in the STL geometry
    int numberOfTriangles;
    binaryInputFile.read((char*)&numberOfTriangles, sizeof(int));

    // declare an object "tri" of type triangle (see main.h for the definition of the triangle class)
    Triangle tri(Vertex(0.0, 0.0, 0.0), Vertex(0.0, 0.0, 0.0), Vertex(0.0, 0.0, 0.0), Normal(0.0, 0.0, 0.0));

    // storage space for the "unused bytes" 
    char unused_bytes[2];


    // temporary floating point variable
    float temp_float_var;

    for (int count = 0; count < numberOfTriangles; count++)
    {
        // read the three components of the normal vector
        binaryInputFile.read((char*)&temp_float_var, 4); tri.norm.x = (double)temp_float_var;
        binaryInputFile.read((char*)&temp_float_var, 4); tri.norm.y = (double)temp_float_var;
        binaryInputFile.read((char*)&temp_float_var, 4); tri.norm.z = (double)temp_float_var;

        // read the three coordinates of vertex 1 
        binaryInputFile.read((char*)&temp_float_var, 4); tri.vert_1.x = (double)temp_float_var;
        binaryInputFile.read((char*)&temp_float_var, 4); tri.vert_1.y = (double)temp_float_var;
        binaryInputFile.read((char*)&temp_float_var, 4); tri.vert_1.z = (double)temp_float_var;

        // read the three coordinates of vertex 2 
        binaryInputFile.read((char*)&temp_float_var, 4); tri.vert_2.x = (double)temp_float_var;
        binaryInputFile.read((char*)&temp_float_var, 4); tri.vert_2.y = (double)temp_float_var;
        binaryInputFile.read((char*)&temp_float_var, 4); tri.vert_2.z = (double)temp_float_var;

        // read the three coordinates of vertex 3
        binaryInputFile.read((char*)&temp_float_var, 4); tri.vert_3.x = (double)temp_float_var;
        binaryInputFile.read((char*)&temp_float_var, 4); tri.vert_3.y = (double)temp_float_var;
        binaryInputFile.read((char*)&temp_float_var, 4); tri.vert_3.z = (double)temp_float_var;

        // read the 2 unused bytes
        binaryInputFile.read(unused_bytes, 2);

        // add data for this triangle to the "facet" vector
        facet.push_back(tri);
    }

    // explicitly close the connection to the input STL file
    binaryInputFile.close();

    return (0);  // all is well
}