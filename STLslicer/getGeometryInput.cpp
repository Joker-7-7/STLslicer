// include the header file

#include "getGeometryInput.h"

// this function handles the task of reading in input geometry files in STL format 
// and finding out the required size of the environmental box

int getGeometryInput(std::string STL_filename,
    std::vector<Triangle>& facet)
{
    // open the STL file and read its contents

    // specify the location of STL files on this computer
    //std::string STL_files_path = "../stl/";

    // BEGIN algorithm from ParaView to detect whether the STL file is ASCII or BINARY

    // open the STL file specified by the user as "read-only" and in "binary" mode
    // and store the pointer to the file buffer in "fp"
#pragma warning(suppress : 4996)
    FILE* fp = fopen((STL_filename).c_str(), "rb");

    // specify how many bytes of the file will be read to calculate the percentage of characters
    // that are non-ASCII
    unsigned long length = 256;

    // dynamically allocate a buffer for storing the contents of the file
    // based on the length specified above
    unsigned char* buffer = new unsigned char[length];

    // read an array of "length" elements from the STL file, where each element is 1 byte in size
    // store the result in the buffer defined above
    // fread returns the number of bytes successfully read and this is stored in "read_length" 
    size_t read_length = fread(buffer, 1, length, fp);

    // close the STL file
    fclose(fp);

    // loop over contents and count

    size_t text_count = 0;

    const unsigned char* ptr = buffer;
    const unsigned char* buffer_end = buffer + read_length;

    // loop over the entire buffer, starting at the memory location pointed to by "ptr"
    while (ptr != buffer_end)
    {
        if ((*ptr >= 0x20 && *ptr <= 0x7F) || *ptr == '\n' || *ptr == '\r' || *ptr == '\t')
        {
            text_count++;  // Yay! we have an ASCII character!
        }
        ptr++;    // move on to the next "byte"
    }

    // free memory
    delete[] buffer;

    // calculate the fraction of characters(bytes) in part of the STL file that are BINARY
    double current_percent_bin = (static_cast<double> (read_length - text_count) / static_cast<double> (read_length));

    // anything above the cut-off value below means that this STL file is of type BINARY
    double percent_bin = 0.05;

    std::string STL_filetype;

    if (current_percent_bin >= percent_bin)
    {
        STL_filetype = "binary";
    }
    else
    {
        STL_filetype = "ascii";
    }

    // END algorithm from ParaView to detect whether the STL file is ASCII or BINARY

    std::cout << "Input Geometry File = " << STL_filename << " (" << STL_filetype << ")" << std::endl;

    // function call to open the ascii file and read the content
    if (STL_filetype == "ascii")
    {
        // call the function to read the ASCII file
        // if all goes well, this function will return a value of 0
        int error = read_ascii_STL_file(STL_filename, facet);

        // if something goes wrong, the above function will return a value of 1
        if (error == 1)
        {
            return(1);       // exit returning an error
        }
    }

    // function call to open the binary file and read the content
    if (STL_filetype == "binary")
    {
        // call the function to read the BINARY file
        // if all goes well, this function will return a value of 0
        //int error = read_binary_STL_file(STL_filename, facet);

        // if something goes wrong, the above function will return a value of 1
        //if (error == 1)
        //{
        //    return(1);       // exit returning an error
        //}
    }

    if (STL_filetype != "ascii" && STL_filetype != "binary")
    {
        std::cout << "ERROR: Unable to find out whether the STL file is ascii or binary" << std::endl;
        return(1);
    }

    // print the number of triangles
    std::cout << "The number of triangles in the STL file = " << facet.size() << std::endl;

    return (0);  // all is well
}

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