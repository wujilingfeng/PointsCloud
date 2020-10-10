#ifndef POINTS_CLOUD_ALGORITHM_H_
#define POINTS_CLOUD_ALGORITHM_H_
#include<tool/libcell_tools_kd_node.h>
#include<tool/libcell_kd_node_iterator.h>


#include<tool/libcell_tools_octree.h>
#include<tool/libcell_octree_iterator.h>

//#include<set>
#ifdef __cplusplus
extern "C"{
#endif


typedef struct Points_Cloud_Piexel{
    //double loc[3],r;
    //元素集合
    Node* n; 

}Points_Cloud_Piexel;

void Points_Cloud_Piexel_init(Points_Cloud_Piexel* pcp);
void points_cloud_piexel_free(Points_Cloud_Piexel* pcp);

typedef struct Points_Cloud_Image{
    int a1,a2,a3;
    Points_Cloud_Piexel*** data;
    //图像起始位置和像素间隔
    double loc[3],r; 

}Points_Cloud_Image;

void Points_Cloud_Image_init(Points_Cloud_Image* pci);
void points_cloud_image_free(Points_Cloud_Image* pci);



typedef struct Points_Cloud_Analyzer{
//各个坐标轴最大最小值
    double xs[2],ys[2],zs[2];
    //质心，平均位置
    double centroid[3];
    //八叉树
    OC_Node* tree; 
    //点与点的距离，最具代表性的最近距离是多少。
    double  median_distance;
    //每单位立方米含有的点数(密度)
   // double dense_density,sparse_density;

}Points_Cloud_Analyzer;

void Points_Cloud_Analyzer_init_(Points_Cloud_Analyzer* pca);
void points_cloud_analyzer_free(Points_Cloud_Analyzer*pca);


void points_cloud_analyze_points(Node* n,Points_Cloud_Analyzer*pca);
Points_Cloud_Analyzer* points_cloud_analyze_points_from_mesh(Mesh* m);
//输入点云及要简化点云倍数，输出简化的点云网格
Mesh* points_cloud_simplify_points_from_mesh(Mesh* m,double fac,std::map<int,int> &mp1,std::map<int,Node*> &mp2);
//从一个顶点找到它的所有邻域连通点
//需要顶点的prop不被占用
Node* points_cloud_find_connected_vertices(template_v*vb,KD_Node* kn,double dis);
//将点云按距离dis进行分组
//需要用到顶点的prop,确保它不被占用
Node* points_cloud_classify_points_from_mesh(Mesh* m,double dis);


//Node* points_cloud_find_connected_vertices1(template_v* v,KD_Node*kn,double dis);



#ifdef __cplusplus
}
#endif

#endif