#ifndef VERTEX_H
#define VERTEX_H

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <boost/algorithm/string.hpp>

using namespace boost::algorithm;

struct Vertex
{
    Vertex() {}
    Vertex(GLfloat x = 0.0, GLfloat y = 0.0, GLfloat z = 0.0) : x(x), y(y), z(z) {}
    Vertex(std::vector<GLfloat> v) : x(v[0]), y(v[1]), z(v[2]) {}

    GLfloat x, y, z;
};
bool operator ==(Vertex a, Vertex b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}
// TODO - implement Normal by inheriting from Vertex
struct Normal
{
    Normal() {}
    Normal(GLfloat x = 0.0, GLfloat y = 0.0, GLfloat z = 0.0) : x(x), y(y), z(z) {}
    Normal(std::vector<GLfloat> v) : x(v[0]), y(v[1]), z(v[2]) {}
    glm::vec3 getVec()
    {
        return glm::vec3(x, y, z);
    }

    GLfloat x, y, z;
};

struct Triangle
{
    Vertex vert_1, vert_2, vert_3;
    Normal norm;
    Triangle(Vertex vertTmp_1, Vertex vertTmp_2, Vertex vertTmp_3, Normal normTmp) 
        :vert_1(vertTmp_1), vert_2(vertTmp_2), vert_3(vertTmp_3), norm(normTmp)
    {
    }
    void setNorm(Normal normTmp) { norm = normTmp; }
    void setVerts(Vertex vertTmp_1, Vertex vertTmp_2, Vertex vertTmp_3)
    {
        vert_1 = vertTmp_1;
        vert_2 = vertTmp_2;
        vert_3 = vertTmp_3;
    }
    glm::vec3 getNormVec()
    {
        return norm.getVec();
    }
};

struct Plane
{
    Vertex vert_1, vert_2, vert_3;
    Plane(Vertex vertTmp_1, Vertex vertTmp_2, Vertex vertTmp_3) : vert_1(vertTmp_1), vert_2(vertTmp_2), vert_3(vertTmp_3)
    {}
    glm::vec3 getNormal()
    {
        glm::vec3 vec1(vert_1.x - vert_2.x, vert_1.y - vert_2.y, vert_1.z - vert_2.z);
        glm::vec3 vec2(vert_3.x - vert_2.x, vert_3.y - vert_2.y, vert_3.z - vert_2.z);
        return glm::normalize(glm::cross(vec1, vec2));
    }

};
void createTriangles(std::vector<Triangle>& triangles, std::vector<Vertex>& vertices, std::vector<Normal>& normals)
{

    for (size_t i = 0; i < normals.size(); ++i)
    {
        Triangle tmpTriangle(vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2], normals[i]);

        triangles.push_back(tmpTriangle);
    }
}

void getVerticesForIntersectionTriangles(std::vector<Triangle>& Triangles, std::vector<Vertex>& verts)
{
    for (size_t i = 0; i < Triangles.size(); ++i)
    {
        verts.push_back(Vertex(Triangles[i].vert_1.x,
                               Triangles[i].vert_1.y,
                               Triangles[i].vert_1.z));
        verts.push_back(Vertex(Triangles[i].vert_2.x,
                               Triangles[i].vert_2.y,
                               Triangles[i].vert_2.z));
        verts.push_back(Vertex(Triangles[i].vert_3.x,
                               Triangles[i].vert_3.y,
                               Triangles[i].vert_3.z));
    }
}



void differentiationTriangles(std::vector<Triangle>& intersectionTriangles, std::vector<Triangle>& newTriangles, Plane& plane, int revert = 1)
{

    glm::vec3 planeNormal = -plane.getNormal();
    planeNormal *= revert;
    Vertex pointOnPlane = plane.vert_1;
    std::vector <Vertex> intersectionPoints;
    for (size_t i = 0; i < intersectionTriangles.size(); ++i)
    {
        glm::vec3 dP1(pointOnPlane.x - intersectionTriangles[i].vert_1.x,
                        pointOnPlane.y - intersectionTriangles[i].vert_1.y,
                        pointOnPlane.z - intersectionTriangles[i].vert_1.z);
        glm::vec3 dP2(pointOnPlane.x - intersectionTriangles[i].vert_2.x,
                        pointOnPlane.y - intersectionTriangles[i].vert_2.y,
                        pointOnPlane.z - intersectionTriangles[i].vert_2.z);
        glm::vec3 dP3(pointOnPlane.x - intersectionTriangles[i].vert_3.x,
                        pointOnPlane.y - intersectionTriangles[i].vert_3.y,
                        pointOnPlane.z - intersectionTriangles[i].vert_3.z);

        glm::vec3 p1(intersectionTriangles[i].vert_2.x - intersectionTriangles[i].vert_1.x,
                     intersectionTriangles[i].vert_2.y - intersectionTriangles[i].vert_1.y,
                     intersectionTriangles[i].vert_2.z - intersectionTriangles[i].vert_1.z);
        glm::vec3 p2(intersectionTriangles[i].vert_3.x - intersectionTriangles[i].vert_2.x,
                     intersectionTriangles[i].vert_3.y - intersectionTriangles[i].vert_2.y,
                     intersectionTriangles[i].vert_3.z - intersectionTriangles[i].vert_2.z);
        glm::vec3 p3(intersectionTriangles[i].vert_3.x - intersectionTriangles[i].vert_1.x,
                     intersectionTriangles[i].vert_3.y - intersectionTriangles[i].vert_1.y,
                     intersectionTriangles[i].vert_3.z - intersectionTriangles[i].vert_1.z);

        GLfloat t1 = (glm::dot(planeNormal, dP1)) / (glm::dot(planeNormal, p1));
        GLfloat t2 = (glm::dot(planeNormal, dP2)) / (glm::dot(planeNormal, p2));
        GLfloat t3 = (glm::dot(planeNormal, dP3)) / (glm::dot(planeNormal, p3));

        std::vector<Vertex> o;
        o.push_back(Vertex(t1 * p1.x + intersectionTriangles[i].vert_1.x,
            t1 * p1.y + intersectionTriangles[i].vert_1.y,
            t1 * p1.z + intersectionTriangles[i].vert_1.z));
        o.push_back(Vertex(t2 * p2.x + intersectionTriangles[i].vert_2.x,
            t2 * p2.y + intersectionTriangles[i].vert_2.y,
            t2 * p2.z + intersectionTriangles[i].vert_2.z));
        o.push_back(Vertex(t3 * p3.x + intersectionTriangles[i].vert_3.x,
            t3 * p3.y + intersectionTriangles[i].vert_3.y,
            t3 * p3.z + intersectionTriangles[i].vert_3.z));



        std::vector <Vertex> deletePoints;
        if (glm::dot(planeNormal, dP1) > 0)
            deletePoints.push_back(intersectionTriangles[i].vert_1);
        if (glm::dot(planeNormal, dP2) > 0)
            deletePoints.push_back(intersectionTriangles[i].vert_2);
        if (glm::dot(planeNormal, dP3) > 0)
            deletePoints.push_back(intersectionTriangles[i].vert_3);

        if (deletePoints.size() == 1)
        {
            if (deletePoints[0] == intersectionTriangles[i].vert_1)
            {
                newTriangles.push_back(Triangle(o[2], intersectionTriangles[i].vert_3, o[0], Normal(0.0, 0.0, 0.0)));
                newTriangles.push_back(Triangle(intersectionTriangles[i].vert_3, intersectionTriangles[i].vert_2, o[0], Normal(0.0, 0.0, 0.0)));
                intersectionPoints.push_back(o[0]);
                intersectionPoints.push_back(o[2]);
            }
            else if (deletePoints[0] == intersectionTriangles[i].vert_2)
            {
                newTriangles.push_back(Triangle(o[1], intersectionTriangles[i].vert_3, o[0], Normal(0.0, 0.0, 0.0)));
                newTriangles.push_back(Triangle(intersectionTriangles[i].vert_3, intersectionTriangles[i].vert_1,o[0], Normal(0.0, 0.0, 0.0)));
                intersectionPoints.push_back(o[0]);
                intersectionPoints.push_back(o[1]);
            }
            else if (deletePoints[0] == intersectionTriangles[i].vert_3)
            {
                newTriangles.push_back(Triangle(o[2], intersectionTriangles[i].vert_1,o[1],  Normal(0.0, 0.0, 0.0)));
                newTriangles.push_back(Triangle(intersectionTriangles[i].vert_1, intersectionTriangles[i].vert_2, o[1], Normal(0.0, 0.0, 0.0)));
                intersectionPoints.push_back(o[1]);
                intersectionPoints.push_back(o[2]);
            }
        }
        else if (deletePoints.size() == 2)
        {
            if (deletePoints[0] == intersectionTriangles[i].vert_2 && deletePoints[1] == intersectionTriangles[i].vert_3)
            {
                newTriangles.push_back(Triangle(o[2], intersectionTriangles[i].vert_1, o[0], Normal(0.0, 0.0, 0.0)));
                intersectionPoints.push_back(o[0]);
                intersectionPoints.push_back(o[2]);
            }
            else if (deletePoints[0] == intersectionTriangles[i].vert_1 && deletePoints[1] == intersectionTriangles[i].vert_3)
            {
                newTriangles.push_back(Triangle(o[1], intersectionTriangles[i].vert_2, o[0], Normal(0.0, 0.0, 0.0)));
                intersectionPoints.push_back(o[0]);
                intersectionPoints.push_back(o[1]);
            }
            else if (deletePoints[0] == intersectionTriangles[i].vert_1 && deletePoints[1] == intersectionTriangles[i].vert_2)
            {
                newTriangles.push_back(Triangle(o[1], intersectionTriangles[i].vert_3, o[2], Normal(0.0, 0.0, 0.0)));
                intersectionPoints.push_back(o[1]);
                intersectionPoints.push_back(o[2]);
            }
        }

        //newTriangles.push_back(o[0]);
        //newTriangles.push_back(o[1]);
        //newTriangles.push_back(o[2]);
    }
    if (true)
    {
        int t = 4;
    }

}

void mergeTriangles(std::vector<Triangle>& firstArrayTriangles, std::vector<Triangle>& secondArrayTriangles)
{
    for (size_t i = 0; i < firstArrayTriangles.size(); ++i)
    {
        secondArrayTriangles.push_back(firstArrayTriangles[i]);
    }
}
void findTrianglesOneSide(std::vector<Triangle>& objectTriangles, std::vector<Triangle>& intersectionTriangles, Plane& plane)
{
    glm::vec3 planeNormal = -plane.getNormal();
    Vertex pointOnPlane = plane.vert_1;

    for (size_t i = 0; i < objectTriangles.size(); ++i)
    {
        glm::vec3 tmpVec1(objectTriangles[i].vert_1.x - pointOnPlane.x, objectTriangles[i].vert_1.y - pointOnPlane.y, objectTriangles[i].vert_1.z - pointOnPlane.z);
        glm::vec3 tmpVec2(objectTriangles[i].vert_2.x - pointOnPlane.x, objectTriangles[i].vert_2.y - pointOnPlane.y, objectTriangles[i].vert_2.z - pointOnPlane.z);
        glm::vec3 tmpVec3(objectTriangles[i].vert_3.x - pointOnPlane.x, objectTriangles[i].vert_3.y - pointOnPlane.y, objectTriangles[i].vert_3.z - pointOnPlane.z);

        if ((glm::dot(tmpVec1, planeNormal) > 0
            && glm::dot(tmpVec2, planeNormal) > 0
            && glm::dot(tmpVec3, planeNormal) > 0))
            intersectionTriangles.push_back(objectTriangles[i]);

    }


}

void findIntersectionTriangles(std::vector<Triangle>& objectTriangles, std::vector<Triangle>& intersectionTriangles, Plane& plane)
{
    glm::vec3 planeNormal = plane.getNormal();
    Vertex pointOnPlane = plane.vert_1;

    for (size_t i = 0; i < objectTriangles.size(); ++i)
    {
        glm::vec3 tmpVec1(objectTriangles[i].vert_1.x - pointOnPlane.x, objectTriangles[i].vert_1.y - pointOnPlane.y, objectTriangles[i].vert_1.z - pointOnPlane.z);
        glm::vec3 tmpVec2(objectTriangles[i].vert_2.x - pointOnPlane.x, objectTriangles[i].vert_2.y - pointOnPlane.y, objectTriangles[i].vert_2.z - pointOnPlane.z);
        glm::vec3 tmpVec3(objectTriangles[i].vert_3.x - pointOnPlane.x, objectTriangles[i].vert_3.y - pointOnPlane.y, objectTriangles[i].vert_3.z - pointOnPlane.z);

        if (!((glm::dot(tmpVec1, planeNormal) > 0 && glm::dot(tmpVec2, planeNormal) > 0 && glm::dot(tmpVec3, planeNormal) > 0)
            || (glm::dot(tmpVec1, planeNormal) < 0 && glm::dot(tmpVec2, planeNormal) < 0 && glm::dot(tmpVec3, planeNormal) < 0)))
            intersectionTriangles.push_back(objectTriangles[i]);

    }
}



void getVertices(std::vector<Vertex>* vertices, std::vector<Normal>* normals, const std::string stl)
{
    // read ascii stl file and load all vertices into `verts`
    std::ifstream infile(stl);
    std::string line;
    while (std::getline(infile, line))
    {
        std::string trimmed_string = trim_copy(line);

        if (starts_with(trimmed_string, "vertex"))
        {
            std::stringstream stream(trimmed_string.substr(6, trimmed_string.size()));
            std::vector<GLfloat> values(
                (std::istream_iterator<GLfloat>(stream)),
                (std::istream_iterator<GLfloat>()));

            Vertex vert(values);
            vertices->push_back(vert);
        }
        else if (starts_with(trimmed_string, "facet"))
        {
            std::stringstream stream(trimmed_string.substr(12, trimmed_string.size()));
            std::vector<GLfloat> values(
                (std::istream_iterator<GLfloat>(stream)),
                (std::istream_iterator<GLfloat>()));

            Normal norm(values);
            normals->push_back(norm);

        }
    }
}




void sliceSolid(std::vector<Triangle>& objectTriangles, Plane& planeFirst, Plane& planeSecond, std::vector <Vertex>& verts)
{
    std::vector<Triangle> intersectionTrianglesWithFirstPlane;
    findIntersectionTriangles(objectTriangles, intersectionTrianglesWithFirstPlane, planeFirst);

    //std::vector<Triangle> oneSideTriangles;
    //findTrianglesOneSide(objectTriangles, oneSideTriangles, planeFirst);


    std::vector<Triangle> firstSliceTriangles;
    //getVerticesForIntersectionTriangles(oneSideTriangles, verts);
    differentiationTriangles(intersectionTrianglesWithFirstPlane, firstSliceTriangles, planeFirst);


    std::vector<Triangle> intersectionTrianglesWithSecondPlane;
    findIntersectionTriangles(firstSliceTriangles, intersectionTrianglesWithSecondPlane, planeSecond);

    std::vector<Triangle> secondSliceTriangles;
    differentiationTriangles(intersectionTrianglesWithSecondPlane, secondSliceTriangles, planeSecond, -1);
    //mergeTriangles(firstSliceTriangles, secondSliceTriangles);
    
    getVerticesForIntersectionTriangles(secondSliceTriangles, verts);
}

#endif