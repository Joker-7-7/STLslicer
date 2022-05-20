#include "readASCII.h"

// -------------------------------------------------------------------------------------------------------------------
// this function opens the STL file specified by the user and reads the contents

int read_ascii_STL_file(std::string STL_filename, std::vector<Triangle>& facet)
{
    // specify the location of STL files on this computer
    //std::string STL_files_path = "../stl/";

    // declare a (input) file object
    std::ifstream asciiInputFile;

    // open the STL file by using the full path and the name
    // specify that the file is opened in "read-only" mode
    asciiInputFile.open((/*STL_files_path  +*/STL_filename).c_str(), std::ifstream::in);

    // check whether the file was opened successfully
    // if yes then continue otherwise terminate program execution
    if (asciiInputFile.fail())
    {
        std::cout << "ERROR: Input STL file could not be opened!" << std::endl;
        return(1); // error
    }

    // read in the contents line by line until the file ends

    // initialize counter for counting the number of lines in this file
    int triangle_number = 0;

    // declare an object "tri" of type triangle (see above for the definition of the triangle struct)
    Triangle tri(Vertex(0.0, 0.0, 0.0), Vertex(0.0, 0.0, 0.0), Vertex(0.0, 0.0, 0.0), Normal(0.0, 0.0, 0.0));

    // declare some string objects
    std::string junk;
    std::string string1, string2;

    // read in the first line (until the /n delimiter) and store it in the string object "line"
    getline(asciiInputFile, junk);

    // begin loop to read the rest of the file until the file ends
    while (true)
    {
        // read the components of the normal vector
        asciiInputFile >> string1 >> string2 >> tri.norm.x >> tri.norm.y >> tri.norm.z;        //  1
        // continue reading this line until the \n delimiter
        getline(asciiInputFile, junk);                                                                //  1

        // read the next line until the \n delimiter
        getline(asciiInputFile, junk);                                                                //  2

        // read the (x,y,z) coordinates of vertex 1            
        asciiInputFile >> string1 >> tri.vert_1.x >> tri.vert_1.y >> tri.vert_1.z;             //  3
        getline(asciiInputFile, junk);                                                                //  3

        // read the (x,y,z) coordinates of vertex 2            
        asciiInputFile >> string1 >> tri.vert_2.x >> tri.vert_2.y >> tri.vert_2.z;             //  4
        getline(asciiInputFile, junk);                                                                //  4

        // read the (x,y,z) coordinates of vertex 3            
        asciiInputFile >> string1 >> tri.vert_3.x >> tri.vert_3.y >> tri.vert_3.z;             //  5
        getline(asciiInputFile, junk);                                                                //  5

        // read some more junk
        getline(asciiInputFile, junk);                                                                //  6
        getline(asciiInputFile, junk);                                                                //  7

        // break out of the while loop if "end-of-file" becomes true
        if (asciiInputFile.eof()) break;

        // increment the triangle number
        triangle_number++;

        // add data for this triangle to the "facet" vector
        facet.push_back(tri);
    }
    // end while loop

    // explicitly close the output file
    asciiInputFile.close();

    return (0);   // all is well
}