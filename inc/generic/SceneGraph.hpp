
#ifndef CS354_GENERIC_SCENE_GRAPH_HPP
#define CS354_GENERIC_SCENE_GRAPH_HPP

#include "Model.hpp"

#include <vector>

namespace cs354 {
    struct SceneNode {
        double tx, ty, tz;
        double rx, ry, rz;
        double sx, sy, sz;
        Model *model; /*< Models at this level of the scene graph */
        std::vector<SceneNode> children;
    };
    
    /* Recursive */
    class SceneGraph {
        SceneNode *head;
    };
}

#endif
