#ifndef VIEWER_H
#define VIEWER_H

class SceneDescription;
class IntersectKdTree;

void run_viewer(int argc, char** argv,
                SceneDescription & FileScene,
                IntersectKdTree & intersect_kd_tree);

#endif // VIEWER_H