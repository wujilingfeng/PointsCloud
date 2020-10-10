#include<stdio.h>
#include "include/Arcroll.h"
#define Matrix4x4 Viewer_Matrix4x4_
#include<Mesh_IO/Mesh_IO.h>
#include<tool/libcell_tools_view.h>
#include <tool/tools_formats.h>
#include<zkhy_tools/zkhy_find_wall.h>
#include<points_cloud_algorithm.h>

void set_default_voi(Viewer_Opengl_Interpreter* voi)
{
    Viewer_World* vw=voi->world;
    char camera[]="Camera";
    Node* n=vw->create_something(vw,camera);
    Viewer_Something*ms=(Viewer_Something*)(n->value);
    Viewer_Camera*mc=(Viewer_Camera*)(ms->evolution);
    mc->is_using=1;
    Matrix4x4 *p=Projection(M_PI/3.0f,(float)(vw->g_info->resolution[0])/(float)(vw->g_info->resolution[1]),0.5f,200.0f);
    p->copy_data(mc->Proj,p);
    Matrix4x4_free(p);

    free_node(n);

    char edges[]="Edges";
//**********************************

    n=vw->create_something(vw,edges);
    auto vs=(Viewer_Something*)(n->value);
  //  vs->disappear=1;
    auto ve=(Viewer_Edges*)(vs->evolution);
    ve->Data_rows=4;
    ve->Data_index_rows=3;
    ve->Data=(float*)malloc(sizeof(float)*3*ve->Data_rows);
    ve->Data_index=(unsigned int*)malloc(sizeof(unsigned int)*2*ve->Data_index_rows);     
    ve->Data[0]=0;ve->Data[1]=0;ve->Data[2]=0;
    ve->Data[3]=1;ve->Data[4]=0;ve->Data[5]=0;
    ve->Data[6]=0;ve->Data[7]=1;ve->Data[8]=0; 
    ve->Data[9]=0;ve->Data[10]=0;ve->Data[11]=1;
   

    ve->Data_index[0]=0;ve->Data_index[1]=1;
    ve->Data_index[2]=0;ve->Data_index[3]=2; 
    ve->Data_index[4]=0;ve->Data_index[5]=3;


    ve->color_rows=ve->Data_index_rows;
    ve->color=(float*)malloc(sizeof(float)*4*ve->color_rows);
    memset(ve->color,0,sizeof(float)*4*ve->color_rows);

    ve->color[0]=1;ve->color[1]=0; ve->color[2]=0;ve->color[3]=1;
    ve->color[4]=0;ve->color[5]=1; ve->color[6]=0;ve->color[7]=1;
    ve->color[8]=0;ve->color[9]=0; ve->color[10]=1;ve->color[11]=1;
    free_node(n); 
//***
    Viewer_Arcroll*ma=(Viewer_Arcroll*)malloc(sizeof(Viewer_Arcroll));
    Viewer_Arcroll_init(ma);
    ma->mc=mc;
    ma->vw=vw;
    ma->voi=voi;
    
    ma->vs=vs;
//****
    char intera[]="Intera";
    n=vw->create_something(vw,intera);
    ms=(Viewer_Something*)(n->value);

    Viewer_Intera* mi=(Viewer_Intera*)(ms->evolution);
    mi->representation=(void*)ma;
    mi->cursor_position_callback=viewer_Arcroll_cursor_position_callback;
    mi->scroll_callback=viewer_Arcroll_scroll_callback;
    mi->mouse_button_callback=viewer_Arcroll_mouse_button_callback;
    mi->drop_callback=viewer_Arcroll_drop_callback;
    mi->key_callback=viewer_Arcroll_key_callback;
    free_node(n);

}

int main(int argc,char**argv)
{
//objtooff("qinbei.obj");
    Mesh mesh;
    Mesh_init(&mesh);
    _Read_(&mesh,"0831.obj");
   /* KD_Node*kn=create_kd_tree_from_mesh(&mesh);
    int sum=0;
    for(auto kit=kd_tree_begin(kn);kit!=kd_tree_end(kn);kit++)
    {
        sum++;
    }
    printf("%d %d\n",sum,mesh.num_v(&mesh) );*/
  //  myobj2off("qinbei.obj");
    //_Read_(&mesh,"qinbei.obj"); 
    //printf("%d\n",mesh.num_c(&mesh) );

    //find_piels("09031.obj");
   // Points_Cloud_Analyzer pca;

    //Points_Cloud_Analyzer_init_(&pca);
    //Points_Cloud_Analyzer *pca=points_cloud_analyze_points_from_mesh(&mesh); 
    std::map<int,int> mp1;std::map<int,Node*> mp2;
    Mesh* mesh1=points_cloud_simplify_points_from_mesh(&mesh,10,mp1,mp2);
    printf("numv:%d\n",mesh1->num_v(mesh1) );
    Points_Cloud_Analyzer* pca=points_cloud_analyze_points_from_mesh(mesh1);
    //printf("%lf\n",pca->median_distance );
    Node* node=points_cloud_classify_points_from_mesh(mesh1,1.5*pca->median_distance);
    printf("%d\n",node_size(node) );
    Node* nit=node;
    for(;nit!=NULL;nit=(Node*)(nit->Next))
    {
        printf("%d\n",node_size((Node*)(nit->value)) );
    }
    nit=node;
    //KD_Node* kn= create_kd_tree_from_mesh(&mesh);

   /* points_cloud_analyze_points(kn,&pca);
    printf("%lf %lf\n",pca.xs[0],pca.xs[1] );
    printf("%lf %lf\n",pca.ys[0],pca.ys[1] );
    printf("%lf %lf\n",pca.zs[0],pca.zs[1] );
*/
    //****************
    Viewer_World vw;
    Viewer_World_init(&vw);

    Viewer_Opengl_Interpreter voi;
    Viewer_Opengl_Interpreter_init(&voi);
    voi.world=&vw;
    
    char points[]="Points";
    Node* n=vw.create_something(&vw,points);
    auto vs=(Viewer_Something*)(n->value);
    auto vp=(Viewer_Points*)(vs->evolution);
    vp->Data_rows= mesh1->num_v(mesh1);
    //vs->disappear=1;
   // vp->Data_rows=node_size((Node*)(nit->value));
    vp->pointsize=5.0;
    vp->Data=(float*)malloc(sizeof(float)*3*vp->Data_rows);
    int i=0;
    for(auto vit=mesh1->vertices.begin();vit!=mesh1->vertices.end();vit++)
    {
        vp->Data[i]=vit->second->point[0];
        i++;
        vp->Data[i]=vit->second->point[1];
        i++;
        vp->Data[i]=vit->second->point[2];
        i++;
    }
    float color[4]={0.9,0.2,0.5,1.0};
    vp->set_color(vp,color);

    //**************************
    /*
    n=vw.create_something(&vw,points);
    vs=(Viewer_Something*)(n->value);
    vp=(Viewer_Points*)(vs->evolution);
    vp->Data_rows=node_size((Node*)(nit->value));
    printf("node_size:%d\n",vp->Data_rows );
    vp->pointsize=7.0;
    vp->Data=(float*)malloc(sizeof(float)*3*vp->Data_rows);
    i=0;
    for(Node* nnit=(Node*)(nit->value);nnit!=NULL;nnit=(Node*)(nnit->Next))
    {
        template_v* v=(template_v*)(nnit->value);
        vp->Data[i]=v->point[0];
        i++;
        vp->Data[i]=v->point[1];
        i++;
        vp->Data[i]=v->point[2];
        i++;
    }
    color[0]=0.1;color[1]=0.8;color[2]=0.2;
    
    vp->set_color(vp,color);*/
    /*
    char edges[]="Edges";
    Node* n=vw.create_something(&vw,edges);
    auto vs=(Viewer_Something*)(n->value);
    auto ve=(Viewer_Edges*)(vs->evolution);
    ve->Data_rows=8;
    ve->Data_index_rows=12;
    ve->color_rows=ve->Data_index_rows;
    ve->Data=(float*)malloc(sizeof(float)*3*ve->Data_rows); 
    ve->Data_index=(unsigned int*)malloc(sizeof(unsigned int)*2*ve->Data_index_rows);

    int l=0;
    for(int i=0;i<2;i++)
    {
        for(int j=0;j<2;j++)
        {
            for(int k=0;k<2;k++)
            {

                ve->Data[l*3+0]=pca->xs[i];
                ve->Data[l*3+1]=pca->ys[j];
                ve->Data[l*3+2]=pca->zs[k];
                l++;
            }
        }
    }
    ve->Data_index[0]=0;ve->Data_index[1]=1;
    ve->Data_index[2]=2;ve->Data_index[3]=3;
    ve->Data_index[4]=0;ve->Data_index[5]=2;
    ve->Data_index[6]=1;ve->Data_index[7]=3; 

    ve->Data_index[8]=4;ve->Data_index[9]=5;
    ve->Data_index[10]=6;ve->Data_index[11]=7;
    ve->Data_index[12]=4;ve->Data_index[13]=6;
    ve->Data_index[14]=5;ve->Data_index[15]=7;
    

    ve->Data_index[16]=0;ve->Data_index[17]=4;
    ve->Data_index[18]=1;ve->Data_index[19]=5;   
    ve->Data_index[20]=2;ve->Data_index[21]=6;
    ve->Data_index[22]=3;ve->Data_index[23]=7;
    free_node(node_reverse(n));
*/
/*
    char points[]="Points";
    n=vw.create_something(&vw,points);
    vs=(Viewer_Something*)(n->value);
    auto vp=(Viewer_Points*)(vs->evolution);
    OC_Node* ocn=pca.tree;
    Node* node=NULL;
    for(auto oit=octree_begin(ocn);oit!=octree_end(ocn);oit++)
    {
        if(node_size(oit.it->value)>10)
        {
            node=oit.it->value;
            break;
        }

    }
    vp->Data_rows=node_size(node);
    vp->pointsize=6.0;
    vp->Data=(float*)malloc(sizeof(float)*3*vp->Data_rows);
    int i=0;
    for(Node* nit=node;nit!=NULL;nit=(Node*)(nit->Next))
    {
        template_v* vv=(template_v*)(nit->value);
        vp->Data[i]=vv->point[0];
        i++;
        vp->Data[i]=vv->point[1];
        i++;
        vp->Data[i]=vv->point[2];
        i++;
    } 
    float color[4]={0.9,0.3,0.8,1.0};

    vp->set_color(vp,color); 
    free_node(node_reverse(n));
    
    */
   // vp->color_rows=vp->Data_rows;
     
    //printf("kn node size:%d\n",kd_node_size(kn) );
    set_default_voi(&voi);
    voi.interpreter(&voi);
    return 0;
}
