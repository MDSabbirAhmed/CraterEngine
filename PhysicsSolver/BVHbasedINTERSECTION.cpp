

//In this implementation, BVHNode is a struct representing a node in the BVH tree, 
//with min and max defining the bounding box of the node, leftChildIdx and rightChildIdx 
//representing the indices of the left and right child nodes respectively (-1 indicates a leaf node),




#include <glm/glm.hpp>
#include <vector>

struct BVHNode {
    glm::vec3 min;
    glm::vec3 max;
    int leftChildIdx;
    int rightChildIdx;
    int primitiveIdx;
};

struct Triangle {
    glm::vec3 vertices[3];
};

bool rayBoxIntersection(const glm::vec3& origin, const glm::vec3& direction, const glm::vec3& min, const glm::vec3& max) {
    float tmin = (min.x - origin.x) / direction.x;
    float tmax = (max.x - origin.x) / direction.x;
    if (tmin > tmax) std::swap(tmin, tmax);
    
    float tymin = (min.y - origin.y) / direction.y;
    float tymax = (max.y - origin.y) / direction.y;
    if (tymin > tymax) std::swap(tymin, tymax);
    
    if ((tmin > tymax) || (tymin > tmax)) return false;
    
    if (tymin > tmin) tmin = tymin;
    if (tymax < tmax) tmax = tymax;
    
    float tzmin = (min.z - origin.z) / direction.z;
    float tzmax = (max.z - origin.z) / direction.z;
    if (tzmin > tzmax) std::swap(tzmin, tzmax);
    
    if ((tmin > tzmax) || (tzmin > tmax)) return false;
    
    return true;
}

bool intersectBVH(const glm::vec3& origin, const glm::vec3& direction, const BVHNode& node, const std::vector<Triangle>& triangles, int& intersectionIdx, float& t) {
    if (rayBoxIntersection(origin, direction, node.min, node.max)) {
        if (node.leftChildIdx == -1 && node.rightChildIdx == -1) {
            intersectionIdx = node.primitiveIdx;
            return true;
        }
        else {
            int leftIdx = node.leftChildIdx;
            int rightIdx = node.rightChildIdx;
            BVHNode leftNode = node, rightNode = node;
            if (glm::dot(direction, triangles[leftIdx].vertices[0] - origin) > glm::dot(direction, triangles[rightIdx].vertices[0] - origin)) {
                std::swap(leftIdx, rightIdx);
                std::swap(leftNode, rightNode);
            }
            float tLeft, tRight;
            bool intersectLeft = intersectBVH(origin, direction, leftNode, triangles, intersectionIdx, tLeft);
            bool intersectRight = intersectBVH(origin, direction, rightNode, triangles, intersectionIdx, tRight);
            if (intersectLeft && intersectRight) {
                if (tLeft < tRight) {
                    t = tLeft;
                }
                else {
                    t = tRight;
                }
                return true;
            }
            else if (intersectLeft) {
                t = tLeft;
                return true;
            }
            else if (intersectRight) {
                t = tRight;
                return true;
            }
        }
    }
    return false;
}





//The rayBoxIntersection function checks for intersection between a ray and a bounding box. It takes in the ray's origin and direction, as well as the minimum and maximum corner points of the bounding box. It returns true if the ray intersects the box, and false otherwise.
//
//The intersectBVH function recursively traverses the BVH tree to find the closest intersection between the input ray and the mesh. It takes in the ray's origin and direction, a BVH node, the list of triangles in the mesh, as well as intersectionIdx and t as output parameters. intersectionIdx stores the index of the closest triangle that the ray intersects, while t stores the distance along the ray to the intersection point.
//
//The function first checks if the ray intersects the bounding box of the input node. If it does, it recursively checks the left and right child nodes if they exist. If both child nodes are intersected, the function chooses the one closer to the ray origin as the next node to explore. If only one child node is intersected, the function only explores that node. If neither child node is intersected, the function terminates.
//
//When a leaf node is reached, the function updates intersectionIdx and t if the closest intersection so far is closer than the current value of t. Finally, the function returns true if an intersection is found, and false otherwise.

