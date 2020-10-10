#include<points_cloud_algorithm.h>

void Points_Cloud_Piexel_init(Points_Cloud_Piexel* pcp)
{
    //pcp->loc[0]=0;pcp->loc[1]=0;pcp->loc[2]=0;
    //pcp->r=0;
    pcp->n=NULL;
}

void points_cloud_piexel_free(Points_Cloud_Piexel* pcp)
{
    if(pcp==NULL)
    {
        return ;
    }
    free_node(pcp->n);
    free(pcp);
}
void Points_Cloud_Image_init(Points_Cloud_Image* pci)
{
    pci->a1=0;pci->a2=0;pci->a3=0;
    pci->data=NULL;
    pci->loc[0]=0;pci->loc[1]=0;pci->loc[2]=0;
    pci->r=1.0/1000.0;


}

void points_cloud_image_free(Points_Cloud_Image* pci)
{
    if(pci==NULL)
    {
        return;
    }

    if(pci->data!=NULL)
    {
        for(int i=0;i<pci->a1;i++)
        {
            for(int j=0;j<pci->a2;j++)
            {
                for(int k=0;k<pci->a3;k++)
                {
                    free_node(pci->data[i][j][k].n);
                }
                free(pci->data[i][j]);
            }
            free(pci->data[i]);
        } 
    }
    free(pci->data);
    free(pci);
}

static Points_Cloud_Piexel*** create_points_cloud_piexels(int a1,int a2,int a3)
{
    Points_Cloud_Piexel*** re=(Points_Cloud_Piexel***)malloc(sizeof(Points_Cloud_Piexel**)*a1);
    for(int i=0;i<a1;i++)
    {
        re[i]=(Points_Cloud_Piexel**)malloc(sizeof(Points_Cloud_Piexel*)*a2);
        for(int j=0;j<a2;j++)
        {
            re[i][j]=(Points_Cloud_Piexel*)malloc(sizeof(Points_Cloud_Piexel)*a3);

            for(int k=0;k<a3;k++)
            {   
                Points_Cloud_Piexel_init(&re[i][j][k]);

            } 
        }
    } 
    return re;
} 


/*
static void free_points_cloud_piexel(Points_Cloud_Piexel*** pcp)
{

}*/
//using namespace std;


void Points_Cloud_Analyzer_init_(Points_Cloud_Analyzer* pca)
{
    pca->xs[0]=0;pca->xs[1]=0;
    pca->ys[0]=0;pca->ys[1]=0;
    pca->zs[0]=0;pca->zs[1]=0;
    pca->centroid[0]=0;pca->centroid[1]=0;pca->centroid[2]=0;

    //***************
    pca->tree=NULL;
    //****************
    pca->median_distance=0;
    //pca->dense_density=0;
   // pca->sparse_density=0;
}
void points_cloud_analyzer_free(Points_Cloud_Analyzer*pca)
{
    if(pca==NULL)
    {
        return ;
    }
    free(pca);
}

Points_Cloud_Analyzer* points_cloud_analyze_points_from_mesh(Mesh* m)
{

    if(m->num_v(m)<=0)
    {
        return NULL;
    } 
    Points_Cloud_Analyzer*pca=(Points_Cloud_Analyzer*)malloc(sizeof(Points_Cloud_Analyzer));
    Points_Cloud_Analyzer_init_(pca);
    template_v* v=NULL;
    v=m->vertices.begin()->second;

    pca->xs[0]=v->point[0];pca->xs[1]=v->point[0];
    pca->ys[0]=v->point[1];pca->ys[1]=v->point[1];
    pca->zs[0]=v->point[2];pca->zs[1]=v->point[2];
    int sum=0;
    for(auto vit=m->vertices.begin();vit!=m->vertices.end();vit++)
    {
        v=vit->second;
        pca->centroid[0]+=v->point[0];pca->centroid[1]+=v->point[1];pca->centroid[2]+=v->point[2];
        if(v->point[0]<pca->xs[0])
        {
            pca->xs[0]=v->point[0];
        }
        else if(v->point[0]>pca->xs[1])
        {
            pca->xs[1]=v->point[0];
        }

        if(v->point[1]<pca->ys[0])
        {
            pca->ys[0]=v->point[1];
        }
        else if(v->point[1]>pca->ys[1])
        {
            pca->ys[1]=v->point[1];
        }
        if(v->point[2]<pca->zs[0])
        {
            pca->zs[0]=v->point[2];
        }
        else if(v->point[2]>pca->zs[1])
        {
            pca->zs[1]=v->point[2];
        }
        sum++;

    } 
    pca->centroid[0]/=(double)sum; pca->centroid[1]/=(double)sum; pca->centroid[2]/=(double)sum; 

    pca->tree=(OC_Node*)malloc(sizeof(OC_Node));
    OC_Node_init(pca->tree);
    OC_Node* ocn=pca->tree;
    ocn->loc_min[0]=pca->xs[0];ocn->loc_min[1]=pca->ys[0];ocn->loc_min[2]=pca->zs[0];
    double max=pca->xs[1]-pca->xs[0];
    if(pca->ys[1]-pca->ys[0]>max)
    {
        max=pca->ys[1]-pca->ys[0];
    }
    if(pca->zs[1]-pca->zs[0]>max)
    {
        max=pca->zs[1]-pca->zs[0];
    }

    ocn->loc_max[0]=max+ocn->loc_min[0];ocn->loc_max[1]=max+ocn->loc_min[1];ocn->loc_max[2]=max+ocn->loc_min[2];
    printf("%lf %lf %lf %lf %lf %lf\n",ocn->loc_min[0],ocn->loc_min[1],ocn->loc_min[2],ocn->loc_max[0],ocn->loc_max[1],ocn->loc_max[2] );
    for(auto vit=m->vertices.begin();vit!=m->vertices.end();vit++)
    {
        ocn->value=node_overlying(ocn->value,vit->second);
    }
   // printf("max:%lf\n",max );

    int num1=1,num2=1,depth=0;
    do
     {
        num1=num2;
        num2=oc_node_divide_all_leaves(ocn);
    
    //    printf("%d \n",num2 );
        depth++;
     } while(num2>1.5*num1); 
    /*for(int i=0;i<11;i++)
    {
        oc_node_divide_all_leaves(ocn); 
        printf("%d\n",oc_node_voxel_size(ocn) );
    }*/
    //oc_node_divide_all_leaves(ocn);
    
    
    
    pca->median_distance=max/pow(2,depth-1); 
    //printf("sum:%d %d %d %lf \n",sum,m->num_v(m) ,oc_node_voxel_size(ocn),max/pow(2,depth-1));
    oc_node_free(ocn);
    pca->tree=NULL;



    return pca;
  /* 
    for(auto oit=octree_begin(ocn);oit!=octree_end(ocn);oit++)
    {
        if(node_size(oit.it->value)>1)
        {
            printf("%d\n",node_size(oit.it->value));    
            printf("%lf \n",oit.it->loc_max[1]-oit.it->loc_min[1]); 
            break;   
        }
    } 
*/

}

Mesh* points_cloud_simplify_points_from_mesh(Mesh* m,double fac,std::map<int,int> &mp1,std::map<int,Node*> &mp2)
{

    if(fac<1||m->num_v(m)<=0)
    {
        return NULL;
    } 
    int num1=(int)(m->num_v(m)/fac);
    Points_Cloud_Analyzer *pca=NULL;
    //Points_Cloud_Analyzer_init_(&pca); 
    pca=points_cloud_analyze_points_from_mesh(m);

    OC_Node* ocn=(OC_Node*)malloc(sizeof(OC_Node));
    OC_Node_init(ocn);
    ocn->loc_min[0]=pca->xs[0];ocn->loc_min[1]=pca->ys[0];ocn->loc_min[2]=pca->zs[0];
    double max=pca->xs[1]-pca->xs[0];
    if(pca->ys[1]-pca->ys[0]>max)
    {
        max=pca->ys[1]-pca->ys[0];
    }
    if(pca->zs[1]-pca->zs[0]>max)
    {
        max=pca->zs[1]-pca->zs[0];
    }

    ocn->loc_max[0]=max+ocn->loc_min[0];ocn->loc_max[1]=max+ocn->loc_min[1];ocn->loc_max[2]=max+ocn->loc_min[2];
    for(auto vit=m->vertices.begin();vit!=m->vertices.end();vit++)
    {
        ocn->value=node_overlying(ocn->value,vit->second);
    }

    while(oc_node_divide_all_leaves(ocn)<num1)
    {
    } 

//    printf("%d\n",oc_node_voxel_size(ocn) );
    Mesh* re=new Mesh;
    Mesh_init(re); 
    double p[3]={0,0,0};
    template_v* v1=NULL,*v2=NULL;
    int size=0;
    Node* n=NULL;
    for(auto oit=octree_begin(ocn);oit!=octree_end(ocn);oit++)
    {
        if(oit.it->value!=NULL)
        {

            v2=re->create_vertexv(re,p,3);
            p[0]=0;p[1]=0;p[2]=0;size=0;
            n=NULL;
            for(Node* nit=oit.it->value;nit!=NULL;nit=(Node*)(nit->Next))
            {
                v1=(template_v*)(nit->value);
                p[0]+=v1->point[0];p[1]+=v1->point[1];p[2]+=v1->point[2];
                size++;
                mp1[v1->id]=v2->id;
                n=node_overlying(n,v1);  
            } 
            p[0]/=(double)(size);p[1]/=(double)(size);p[2]/=(double)(size);
            v2->point[0]=p[0];v2->point[1]=p[1];v2->point[2]=p[2];
            mp2[v2->id]=n;
            
        }

    }
    oc_node_free(ocn);
    points_cloud_analyzer_free(pca);

    return re;

}
void points_cloud_analyze_points(Node* n,Points_Cloud_Analyzer*pca)
{
    if(n==NULL)
    {
        return ;
    }
    Node* nit=n;
    template_v* v=NULL;v=(template_v*)(nit->value); 
    pca->xs[0]=v->point[0];pca->xs[1]=v->point[0];
    pca->ys[0]=v->point[1];pca->ys[1]=v->point[1];  
    pca->zs[0]=v->point[2];pca->zs[1]=v->point[2];
    int sum=0;
    for(;nit!=NULL;nit=(Node*)(nit->Next))
    {
        v=(template_v*)(nit->value);
        pca->centroid[0]+=v->point[0];pca->centroid[1]+=v->point[1];pca->centroid[2]+=v->point[2];
        if(v->point[0]<pca->xs[0])
        {
            pca->xs[0]=v->point[0];
        }
        else if(v->point[0]>pca->xs[1])
        {
            pca->xs[1]=v->point[0];
        }

        if(v->point[1]<pca->ys[0])
        {
            pca->ys[0]=v->point[1];
        }
        else if(v->point[1]>pca->ys[1])
        {
            pca->ys[1]=v->point[1];
        }
        if(v->point[2]<pca->zs[0])
        {
            pca->zs[0]=v->point[2];
        }
        else if(v->point[2]>pca->zs[1])
        {
            pca->zs[1]=v->point[2];
        }
        sum++;
    }
    pca->centroid[0]/=(double)sum; pca->centroid[1]/=(double)sum; pca->centroid[2]/=(double)sum; 

    pca->tree=(OC_Node*)malloc(sizeof(OC_Node));
    OC_Node_init(pca->tree);
    OC_Node* ocn=pca->tree;
    for(nit=n;nit!=NULL;nit=(Node*)(nit->Next))
    {
        ocn->value=node_overlying(ocn->value,nit->value);
    }


/*
    Points_Cloud_Image* pci=(Points_Cloud_Image*)malloc(sizeof(Points_Cloud_Image));
    points_cloud_image_init(pci);
    double len=pca->xs[1]-pca->xs[0];
    if(pca->ys[1]-pca->ys[0]>len)
    {
        len=pca->ys[1]-pca->ys[0];
    }
    if(pca->zs[1]-pca->zs[0]>len)
    {
        len=pca->zs[1]-pca->zs[0];
    }
*/  
/*    double num_cuts=1000;

    pci->r=len/num_cuts;
    pci->a1=(int)((pca->xs[1]-pca->xs[0])/pci->r)+1;
    pci->a2=(int)((pca->ys[1]-pca->ys[0])/pci->r)+1;
    pci->a3=(int)((pca->zs[1]-pca->zs[0])/pci->r)+1;
    printf("%d %d %d sum:%d\n",(int)(-1.8),pci->a2,pci->a3,sum );

    pci->data=create_points_cloud_piexels(pci->a1,pci->a2,pci->a3);

    pci->loc[0]=pca->xs[0];pci->loc[1]=pca->ys[0];pci->loc[2]=pca->zs[0];

    sum=0;
    int i=0,j=0,k=0;
    for(auto kit=kd_tree_begin(kn);kit!=kd_tree_end(kn);kit++)
    {
        i=(int)((kit.it->value->point[0]-pci->loc[0])/pci->r);
        j=(int)((kit.it->value->point[1]-pci->loc[1])/pci->r);
        k=(int)((kit.it->value->point[2]-pci->loc[2])/pci->r);
        
        Node* n=pci->data[i][j][k].n;
        
        pci->data[i][j][k].n=node_overlying(n,kit.it->value);

        //kit.it->value->
    } 
    sum=0;*/

  /*  for(i=0;i<pci->a1;i++)
    {
        for(j=0;j<pci->a2;j++)
        {
            for(k=0;k<pci->a3;k++)
            {
                if(pci->)
                {

                }
            }

        }

    }*/
   // printf("%d\n",sum );
   // points_cloud_image_free(pci);
//    pca->pci=pci;
}

//根据点云构造图像，需要将点云放在单位立方体内




static Node* points_cloud_dyeing_and_get_connected_points(Node*nvs,KD_Node*kn,double dis,void* p)
{
  //  printf("heree\n");
    Node* re=NULL;
    for(Node* nit=nvs;nit!=NULL;nit=(Node*)(nit->Next))
    {
        template_v* v=(template_v*)(nit->value);
        v->prop=p;

        Node* U=kd_tree_find_nearest_sphere(v,dis,kn);

        for(Node*nnit=U;nnit!=NULL;nnit=(Node*)(nnit->Next))
        {
            template_v* v1=(template_v*)(nnit->value);
            if(v1->prop==NULL)
            {
                v1->prop=p;
                re=node_overlying(re,v1);
            } 

        }

        free_node(U);
    }

    return re;
}



Node* points_cloud_find_connected_vertices(template_v* vb,KD_Node* kn,double dis)
{
    Node* re=NULL;
    if(vb==NULL)
    {
        return re;
    }

    //template_v* vb=v;


    void* p=malloc(sizeof(int));

    Node* n1=NULL,*n2=NULL;
    n1=node_overlying(n1,vb);
    vb->prop=p;
    while(n1!=NULL)
    {
       n2=points_cloud_dyeing_and_get_connected_points(n1,kn,dis,p);
       free_node(n1);
       n1=n2; 
    }

    for(auto kit=kd_tree_begin(kn);kit!=kd_tree_end(kn);kit++)
    {
        if(kit.it->value->prop==p)
        {
            re=node_overlying(re,kit.it->value);
            kit.it->value->prop=NULL; 
        } 
    }
    free(p);
    return re; 
}
Node* points_cloud_classify_points_from_mesh(Mesh* m,double dis)
{
    if(m->num_v(m)<=0)
    {
        return NULL;
    }

    Node* re=NULL;
    template_v* vb=NULL,*v=NULL;
    KD_Node* kn=NULL;
    Node* n1=NULL;
    std::map<int,template_v*> mp;
    for(auto vit=m->vertices.begin();vit!=m->vertices.end();vit++)
    {
        mp[vit->second->id]=vit->second;
    }

    do{
        vb=mp.begin()->second;

        free_kdnode(kn);kn=NULL;
        kn=create_kd_tree_from_RBTree(mp);
        n1=NULL;
        n1=points_cloud_find_connected_vertices(vb, kn,dis);
        re=node_overlying(re,n1);
        for(Node* nit=n1;nit!=NULL;nit=(Node*)(nit->Next))
        {
            v=(template_v*)(nit->value);
            mp.erase(v->id);
        } 
        
    }while(mp.size()>0);


    return re;  

}

/*

Node* points_cloud_find_connected_vertices1(template_v* vb,KD_Node* kn,double dis)
{
    Node* re=NULL;
    if(vb==NULL)
    {
        return re;
    }
    
    void* p=malloc(sizeof(int));

    Node* n1=NULL,*n2=NULL;
    n1=node_overlying(n1,vb);
    vb->prop=p;
    while(n1!=NULL)
    {

       n2=points_cloud_dyeing_and_get_connected_points(n1,kn,dis,p);
       free_node(n1);
       n1=n2; 

    }

    for(auto kit=kd_tree_begin(kn);kit!=kd_tree_end(kn);kit++)
    {
       if(kit.it->value->prop==p)
       {
            re=node_overlying(re,kit.it->value);
            kit.it->value->prop=NULL; 
       } 

    }

    free(p);
    return re;    
}*/




