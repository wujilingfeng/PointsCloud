#include<zkhy_tools/zkhy_tools_common.h>
#include<points_cloud_algorithm.h>
#ifndef ZKHY_TOOLS_FIND_WALL_H_
#define ZKHY_TOOLS_FIND_WALL_H_


//在xy平面建立网格，以此作为参数化区域
//
static Node*** find_wall_construct_xy_region_cells(Mesh *m)
{

    Node*** re=(Node***)malloc(sizeof(Node**)*1000);
    for(int i=0;i<1000;i++)
    {
        re[i]=(Node**)malloc(sizeof(Node*)*100);
        memset(re[i],0,sizeof(Node*)*100);
    }
    for(auto vit=m->vertices.begin();vit!=m->vertices.end();vit++)
    {

        int i=(int)vit->second->point[1]/200;
        int j=(int)vit->second->point[0]/400;
        i=i*SIGN(i);
        j=j*SIGN(j);
        //printf("i %d j%d\n",i,j);
        re[i][j]=node_overlying(re[i][j],vit->second);
    }
    for(int i=0;i<1000;i++)
    {
        for(int j=0;j<100;j++)
        {
            if(re[i][j]!=NULL)
            {
                double * p=(double*)malloc(sizeof(double));
                *p=0;
                re[i][j]->traits=p;
                int sum=0;
                for(Node* nit=re[i][j];nit!=NULL;nit=(Node*)(nit->Next))
                {
                    template_v* v=(template_v*)(nit->value);
                    *p=*p+v->point[2];
                    sum++; 

                } 
                *p=*p/(double)sum;
            }

        }

    }
    return re;
}
//寻找直线并返回y坐标
static Node* find_wall_straight_line_y_coordinate(Node***frame )
{
    Node* re=NULL;
    for(int i=0;i<1000;i++ )
    {
        int sum=0;
        for(int j=0;j<84;j++)
        {
           
            if(frame[i][j]==NULL||frame[i][j+1]==NULL)
            {
                continue;
            }
            double t1=*((double*)(frame[i][j]->traits)),t2=*((double*)(frame[i][j+1]->traits));
            if(fabs(t1-t2)<60)
            {
                sum++;
            } 

        } 
        if(sum>30)
        {
            int * l=(int*)malloc(sizeof(int));
            *l=i;
            re=node_overlying(re,l);
        }
    }

    return re;
}
//返回直线的平均高度
static double find_wall_straight_line_z(Node** l)
{
    double re=0;
    double sum=0;
    for(int i=0;i<85;i++)
    {
    
        for(Node* nit=l[i];nit!=NULL;nit=(Node*)(nit->Next))
        {

            template_v* v=(template_v*)(nit->value);
            re+=v->point[2];

            sum=sum+1; 

        }
    }

    if(sum==0)
    {
        return 0;
    }
    re=re/sum;

    return re;

}
//在直线中找出墙的边界
static Node* find_wall_boundary_from_straight_lines(Node* indexs,Node*** frame)
{

    Node* re=NULL;
    for(Node* nit=indexs;nit!=NULL;nit=(Node*)(nit->Next))
    {
        int ii=*((int*)(nit->value));
        int ci[4]={ii-2,ii-1,ii+1,ii+2};

        int flag=0;
        double z1=find_wall_straight_line_z(frame[ii]);
        for(int i=0;i<4;i++)
        {
            if(ci[i]<0||ci[i]>999)
            {
                continue;
            }
            double z2=find_wall_straight_line_z(frame[ci[i]]);
            if(fabs(z1-z2)>1500)
            {
                flag=1;
                break;
            }
        } 
        if(flag==1)
        {
            int*p=(int*)malloc(sizeof(int));
            *p=ii; 
            re=node_overlying(re,p);
        }
    } 
    
    double zz=0,sum=0;
    for(Node* nit=re;nit!=NULL;nit=(Node*)(nit->Next))
    {
        zz+=find_wall_straight_line_z(frame[*((int*)(nit->value))]);
        //printf("%d\n",*((int*)(nit->value)));
        sum+=1;

    }
    zz/=sum;
   int  flag=1;
    while(flag==1)
    {
        flag=0;
        for(Node* nit=re;nit!=NULL;nit=(Node*)(nit->Next))
        {
            double temp=find_wall_straight_line_z(frame[*((int*)(nit->value))]);
            if(fabs(temp-zz)>2000)
            {
                flag=1;
                void * ttt=nit->value;
                re=node_delete_value(re,nit->value);

                free(ttt);
                break;
            }
        }
    }

    return re;
}
static bool find_wall_node_find(Node* indexs,int i)
{

    for(Node* nit=indexs;nit!=NULL;nit=(Node*)(nit->Next))
    {
        if(*((int*)(nit->value))==i)
        {
            return true;
        }

    } 

    return false;
}
static int find_wall_neighborhood(Node* indexs,int i)
{

    int re=0;
    for(Node* nit=indexs;nit!=NULL;nit=(Node*)(nit->Next))
    {
        int l=*((int*)(nit->value));
        if(fabs(l-i)<7)
        {   

            re++;
        } 

    }
    return re;
}
//补全墙的所有直线
static Node* _find_wall_(Node* indexs,Node*** frame)
{
    int flag=1;
    Node* re=indexs;
     double zz=0,sum=0;
    for(Node* nit=re;nit!=NULL;nit=(Node*)(nit->Next))
    {
            zz+=find_wall_straight_line_z(frame[*((int*)(nit->value))]);
        //printf("%d\n",*((int*)(nit->value)));
            sum+=1;

    }

        zz/=sum;
    while(flag==1)
    {
        flag=0;
        for(Node* nit=re;nit!=NULL;nit=(Node*)(nit->Next))
        {
            int ii=*((int*)(nit->value));
            //double z1=find_wall_straight_line_z(frame[ii]);
            int ci[2]={ii-1,ii+1};
            for(int i=0;i<2;i++)
            {
                if(ci[i]<0||ci[i]>999)
                {
                    continue;
                }

               if(find_wall_node_find(re,ci[i]))
               {
                    continue;
               } 
               if(find_wall_neighborhood(re,ci[i])>5)
               {

                    continue;
               }
                double z2=find_wall_straight_line_z(frame[ci[i]]);
                if(fabs(zz-z2)<600)
                {

                    int*p=(int*)malloc(sizeof(int));
                    *p=ci[i]; 
                    re=node_overlying(re,p);
                    flag=1;
                }

            }  
            if(flag==1)
            {
                break;
            }

        }
    }
    return re;
}



//进一步筛选提出出墙的点
static Node* _find_wall_points_(Node*index,Node***frame)
{
    double zz=0,sum=0;
    for(Node* nit=index;nit!=NULL;nit=(Node*)(nit->Next))
    {
        zz+=find_wall_straight_line_z(frame[*((int*)(nit->value))]);
        //printf("%d\n",*((int*)(nit->value)));
        sum+=1;

    }

    zz/=sum;
    Node* re=NULL;
    void* p=malloc(sizeof(int));
    for(Node* nit=index;nit!=NULL;nit=(Node*)(nit->Next))
    {
         int ii=*((int*)(nit->value));
        //double z1=find_wall_straight_line_z(frame[ii]);
        int ci[3]={ii-1,ii,ii+1};
        for(int j=0;j<3;j++)
        {
            if(ci[j]<0||ci[j]>999)
            {
                    continue;
            }
            Node**l=frame[ci[j]];
            for(int i=0;i<100;i++)
            {
                for(Node* nit=l[i];nit!=NULL;nit=(Node*)(nit->Next))
                {
                    template_v* v=(template_v*)(nit->value);
                    if(v->prop!=NULL)
                    {continue;}
                    if(fabs(zz-v->point[2])<700||v->point[2]>zz)
                    {
                        v->prop=p;
                        re=node_overlying(re,v);
                    }  
                } 
            }
        }
    }
    free(p);
    for(Node* nit=re;nit!=NULL;nit=(Node*)(nit->Next))
    {
        template_v* v=(template_v*)(nit->value);
        v->prop=NULL;
    } 
 
    return re;
}
static int find_wall_index_cmp(void* a,void* b)
{
    int i=*((int*)a),j=*((int*)b);
    if(i>j)
    {
        return 1;
    }
    return -1; 
}
//找出墙并删除后，开始分离料堆,找出分界线
static Node* find_wall_find_limits(Mesh* m,Node* indexs)
{
    int size=node_size(indexs);
    for(int i=0;i<size;i++)
    {
        node_bub_sort(indexs,find_wall_index_cmp);
    }
    int * values=(int*)malloc(sizeof(int)*size); 
    int i=0;
    for(Node* nit=indexs;nit!=NULL;nit=(Node*)(nit->Next))
    {

        values[i]=*((int*)(nit->value));
        i++;
    } 
  //  printf("*****************************************\n");
    i=0; 
    Node* limits=NULL;
    while(i<size)
    {
        limits=node_overlying(limits,&values[i]);
        int j=i+1;
        for(;j<size;j++)
        {

            if(fabs(values[j]-values[i])>17)
            {

                break;
            }
        } 
        i=j;
    }

    return limits;
}
//返回分离的料堆点集
static Node** find_wall_divide_piles(Mesh* m,Node* limits)
{

    int size=node_size(limits);
    Node** re=(Node**)malloc(sizeof(Node*)*(size+1));
    memset(re,0,sizeof(Node*)*(size+1));
    int * lim=(int*)malloc(sizeof(int)*size);
    int i=0;
    for(Node*nit=limits;nit!=NULL;nit=(Node*)(nit->Next))
    {
        lim[i]=*((int*)(nit->value));
        printf("lim%d\n",lim[i] );
        i++; 
    } 

    for(auto vit=m->vertices.begin();vit!=m->vertices.end();vit++)
    {
        i=(int)vit->second->point[1]/200;
      //  printf("%d\n",i );
        int j=0;
        for(;j<size;j++)
        {
            if(lim[j]>i)
            {
                break;
            }
        }
        
        re[j]=node_overlying(re[j],vit->second); 
    } 

    free(lim);

    return re;
}


void find_wall_write_pile_points1(Node* b,char const * filename)
{
#ifdef WIN32
    FILE *outfile;
    fopen_s(&outfile,filename,"w");
#else
    FILE *outfile=fopen(filename,"w");
#endif
   if(!outfile)
   {
        printf("cant open this file\n");
        return ;
   }

    int size=node_size(b);

    fprintf(outfile, "# Vertices: %d\n",size);
    template_v* v=NULL;
    for(Node*nit=b;nit!=NULL;nit=(Node*)(nit->Next))
    {
        v=(template_v*)(nit->value);
//        outfile<<v->id<<endl;
        fprintf(outfile, " %d\n",v->id );

    }  
    fclose(outfile);

}

static Node*find_wall_find_connected_vertices_from_mesh(Mesh* m,KD_Node* kn,double dis)
{
    Node* re=NULL;
    if(m->num_v(m)==0)
    {
        return re;
    }
    template_v* vb=m->vertices.begin()->second;

    re=points_cloud_find_connected_vertices(vb, kn,dis);


    for(auto vit=m->vertices.begin();vit!=m->vertices.end();vit++)
    {
        if(vit->second->prop!=NULL)
        {
            printf("L错误\n");
        }
    }
    return re;
}
bool find_piels_denoising_pile(Node* n,char const* filename)
{
    Mesh mesh;
    Mesh_init(&mesh);
    for(Node* nit=n;nit!=NULL;nit=(Node*)(nit->Next))
    {
        template_v* v1=(template_v*)(nit->value);
        //printf("%lf %lf %lf\n",v->point[0],v->point[1],v->point[2] );
       // template_v* v1= mesh.create_vertexv(&mesh,v->point,3);
       //template_v* v=(template_v*)malloc(sizeof(template_v));
        template_v* v=(template_v*)malloc(sizeof(template_v));
        Vertex_init_(v);
        v->id=v1->id;
        mesh.vertices[v->id]=v;
        v->point_size=3;
        v->point=(double*)malloc(sizeof(double)*3);
        v->point[0]=v1->point[0];v->point[1]=v1->point[1];v->point[2]=v1->point[2]; 
      
       // v1->id=v->id;
    }
    printf("numv:%d \n",mesh.num_v(&mesh) );
    KD_Node* kn=NULL;Node* connected_points=NULL;

    do
    {
        for(Node* nit=connected_points;nit!=NULL;nit=(Node*)(nit->Next))
        {
            template_v* v=(template_v*)(nit->value);
            mesh.delete_vertex(&mesh,*v,true); 
        }
        free_node(connected_points);


        kn=create_kd_tree_from_mesh(&mesh);
        connected_points= find_wall_find_connected_vertices_from_mesh(&mesh,kn,700);
      //  connected_points= points_cloud_find_connected_vertices(&mesh, kn,700);

         
        printf("node size:%d\n",node_size(connected_points) );
        free_kdnode(kn);
    }while(node_size(connected_points)!=0&&node_size(connected_points)<4000);



    printf("size:%d\n",node_size(connected_points));
    bool re=false;
    if(node_size(connected_points)!=0)
    {

        find_wall_write_pile_points1(connected_points,filename);

        re=true;
    }

    free_node(connected_points);
    Mesh_free(&mesh);
    return re;

}


void find_piels(char const * filename)
{
    Mesh mesh;
    Mesh_init(&mesh);
    _Read_(&mesh,filename);
    Node*** frame=find_wall_construct_xy_region_cells(&mesh);
    Node* indexs=find_wall_straight_line_y_coordinate(frame );
    Node*indexs2=find_wall_boundary_from_straight_lines(indexs,frame);
    free_node(indexs);

    indexs2=_find_wall_(indexs2,frame); 
    Node* vsss=_find_wall_points_(indexs2,frame);

    for(auto vit=mesh.vertices.begin();vit!=mesh.vertices.end();vit++)
    {
        if(vit->second->prop!=NULL)
        {
            printf("错误\n");
            vit->second->prop=NULL;
        } 
    }
    template_v* v2=NULL;

    
    for(Node* nit=vsss;nit!=NULL;nit=(Node*)(nit->Next))
    {
        v2=(template_v*)(nit->value);
       mesh.delete_vertex(&mesh,*v2,true); 

    }
   Node* limits= find_wall_find_limits(&mesh,indexs2);
   Node**piles=find_wall_divide_piles(&mesh,limits);
   int size=node_size(limits);

    char str[25]="piles";int jj=0;
    for(int i=0;i<=size;i++)
    {
        snprintf(&str[5],12,"%d",jj);
        if(!find_piels_denoising_pile(piles[i],str))
        {
            jj--;
        }
        jj++;
    } 
    free_node(indexs2);
    free_node(vsss);
    free_node(limits);
    for(int i=0;i<=size;i++)
    {
        free_node(piles[i]);
    }
    free(piles);

}

Node* reade_isolate_points_(Mesh* m,char const* filename)
{

#ifdef WIN32
    FILE *infile;
    fopen_s(&infile,filename,"r");
#else
    FILE *infile=fopen(filename,"r");
#endif
    if(!infile)
    {
        printf("cant't open this file\r\n");
        return NULL;
    }
    fseek(infile,0,SEEK_SET);

    char temp_c[20];
    fscanf(infile,"%c",temp_c);
    fscanf(infile,"%s ",temp_c);


    int size=0;


    fscanf(infile,"%d",&size);
    printf("file size:%d\n", size);
    Node* re=NULL;
    
    int id=0;
    template_v* v=NULL;

    for(int i=0;i<size;i++)
    {
        fscanf(infile,"%d ",&id);
        auto it=m->vertices.find(id);
        v=it->second;

        re=node_overlying(re,v);

    }

    fclose(infile);
    return re;
}

#endif