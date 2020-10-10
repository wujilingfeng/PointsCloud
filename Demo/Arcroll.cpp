#include "include/Arcroll.h" 
#define Matrix4x4 Viewer_Matrix4x4_
#include<Mesh_IO/Mesh_IO.h>
#include<tool/libcell_tools_view.h>
#include <tool/tools_formats.h>
void Viewer_Arcroll_init(Viewer_Arcroll*ma)
{
    ma->old_mouse_coord=(float*)malloc(sizeof(float)*2);
    ma->vs=NULL;
    ma->mc=0;
    ma->vw=NULL;
    ma->voi=NULL;
    ma->disappear_somethings=NULL;
}
void viewer_Arcroll_cursor_position_callback(Viewer_Intera* mi)
{
    Interactor_GlobalInfo* g_info=mi->g_info;
    Viewer_Arcroll* ma=(Viewer_Arcroll*)(mi->representation);
    Viewer_Camera* mc=(Viewer_Camera*)(ma->mc);
    if(g_info->mouse_button==VIEWER_MOUSE_BUTTON_LEFT&&g_info->mouse_action==VIEWER_PRESS&&g_info->key_action==0)
    {
        float tempx=g_info->mouse_coord[0]-ma->old_mouse_coord[0];
        float tempy=g_info->mouse_coord[1]-ma->old_mouse_coord[1];
        tempx=-tempx*0.005;tempy=tempy*0.005;
        Matrix4x4 *temp_m=(Matrix4x4*)malloc(sizeof(Matrix4x4));
        Matrix4x4_init_float(temp_m);
        Matrix4x4 *MV=(Matrix4x4*)malloc(sizeof(Matrix4x4));
        Matrix4x4_init_float(MV);
        float* data=(float*)(MV->data);
        data[2*4+3]=mc->focal_distance;
        Matrix4x4 *MV_inverse=MV->inverse(MV);
        data=(float*)(temp_m->data);
        data[0*4+0]=cos(tempx);data[0*4+1]=-sin(tempx)*sin(tempy);
        data[0*4+2]=sin(tempx)*cos(tempy);
        data[1*4+1]=cos(tempy);
        data[1*4+2]=sin(tempy);
        data[2*4+0]=-sin(tempx);
        data[2*4+1]=-cos(tempx)*sin(tempy);
        data[2*4+2]=cos(tempx)*cos(tempy);
        MV->mult(MV,temp_m);
        MV->mult(MV,MV_inverse);
        MV->mult(MV,mc->matrix_inverse);
        mc->matrix_inverse->copy_data(mc->matrix_inverse,MV);
        Matrix4x4_free(mc->matrix);
        mc->matrix=mc->matrix_inverse->inverse(mc->matrix_inverse);
        Matrix4x4_free(temp_m);
        Matrix4x4_free(MV);
        Matrix4x4_free(MV_inverse);
    }
    else if(g_info->mouse_button==VIEWER_MOUSE_BUTTON_RIGHT&&g_info->mouse_action==VIEWER_PRESS)
    {
        float tempx=g_info->mouse_coord[0]-ma->old_mouse_coord[0];
        float tempy=g_info->mouse_coord[1]-ma->old_mouse_coord[1];
        tempx=-tempx*0.005;tempy=tempy*0.005;
        float*data=(float*)mc->matrix_inverse->data;
        data[0*4+3]-=tempx;
        data[1*4+3]-=tempy;
        Matrix4x4_free(mc->matrix);
        mc->matrix=mc->matrix_inverse->inverse(mc->matrix_inverse);
    }

    ma->old_mouse_coord[0]=g_info->mouse_coord[0];
    ma->old_mouse_coord[1]=g_info->mouse_coord[1];
}
void viewer_Arcroll_scroll_callback(Viewer_Intera*mi,double x,double y)
{
    Interactor_GlobalInfo* g_info=mi->g_info;
    Viewer_Arcroll* ma=(Viewer_Arcroll*)(mi->representation);

    Viewer_World* vw=ma->vw;
    Viewer_Camera* mc=(Viewer_Camera*)(ma->mc);
    //Viewer_World* vw=(Viewer_World*)(mi->representation);
    //printf("scroll key_mods:%d ,key_action:%d\n",g_info->key_mods,g_info->key_action);
    if((g_info->key==VIEWER_KEY_CONTROL&&g_info->key_action==1))
    {

        //printf("here\n");
        mc->focal_distance+=(float)y*0.05;
    }
    else if(g_info->key==VIEWER_KEY_ALT&&g_info->key_action==1)
    {
        char points[]="Points";
        Node* names_id=vw->find_species(vw,points);
        RB_int rbt,*rbt1=NULL;
        rbt.key=*((int*)(names_id->value));
        SAFE_FREE(names_id->value);
        free_node(names_id);
        rbt1=(RB_int*)vw->species2somethings->find(vw->species2somethings,&rbt);
        RB_Tree* tree=NULL;
        RB_Trav* iter1=NULL;
        if(rbt1!=NULL)
        {
            tree=((RB_Tree*)(rbt1->value));
            iter1=tree->begin(tree);
            for(;iter1->it!=NULL;iter1->next(iter1))
            {
                Viewer_Something* vs=(Viewer_Something*)(iter1->second(iter1));
                Viewer_Points* vp=(Viewer_Points*)(vs->evolution);      
                float * data=(float*)(vp->mat->data);
                data[0*4+0]*=(1-(float)y*0.05);
                data[1*4+1]*=(1-(float)y*0.05);
                data[2*4+2]*=(1-(float)y*0.05);          
            }
            SAFE_FREE(iter1);
        }
        char faces[]="Faces";
        names_id=vw->find_species(vw,faces);
        rbt.key=*((int*)(names_id->value));
        SAFE_FREE(names_id->value);
        free_node(names_id);
        rbt1=(RB_int*)vw->species2somethings->find(vw->species2somethings,&rbt);
        if(rbt1!=NULL)
        {

            tree=(RB_Tree*)(rbt1->value);
            iter1=tree->begin(tree);
            for(;iter1->it!=NULL;iter1->next(iter1))
            {
                Viewer_Something* vs=(Viewer_Something*)(iter1->second(iter1));
                Viewer_Faces* vf=(Viewer_Faces*)(vs->evolution);      
                float * data=(float*)(vf->mat->data);
                data[0*4+0]*=(1-(float)y*0.05);
                data[1*4+1]*=(1-(float)y*0.05);
                data[2*4+2]*=(1-(float)y*0.05);          
            }
            SAFE_FREE(iter1);

        }
        char edges[]="Edges";
        names_id=vw->find_species(vw,edges);
        rbt.key=*((int*)(names_id->value));
        SAFE_FREE(names_id->value);
        free_node(names_id);
        rbt1=(RB_int*)vw->species2somethings->find(vw->species2somethings,&rbt);
        if(rbt1!=NULL)
        {

            tree=(RB_Tree*)(rbt1->value);
            iter1=tree->begin(tree);
            for(;iter1->it!=NULL;iter1->next(iter1))
            {
                Viewer_Something* vs=(Viewer_Something*)(iter1->second(iter1));
                if(vs==ma->vs)
                {continue;}
                Viewer_Edges* ve=(Viewer_Edges*)(vs->evolution);      
                float * data=(float*)(ve->mat->data);
                data[0*4+0]*=(1-(float)y*0.05);
                data[1*4+1]*=(1-(float)y*0.05);
                data[2*4+2]*=(1-(float)y*0.05);          
            }
            SAFE_FREE(iter1);
        }
    }
    else if(g_info->key_action==0)
    { 
        if(g_info->pick_something!=0)
        {
            Viewer_Something* ms=(Viewer_Something*)(g_info->pick_something);
            if(ms!=ma->vs)
            {

                float * data=NULL;
                if(strcmp("Faces",ms->name)==0)
                {
                    Viewer_Faces* mf=(Viewer_Faces*)(ms->evolution);

                    data=(float*)(mf->mat->data);
                
                //printf("%lf\n",(1-(float)y*0.05));
                }
                else if(strcmp("Edges",ms->name)==0)
                {
                    Viewer_Edges* ve=(Viewer_Edges*)(ms->evolution);
                    data=(float*)(ve->mat->data);

                }
                else if(strcmp("Points",ms->name)==0)
                {
                    Viewer_Points* vp=(Viewer_Points*)(ms->evolution);
                    data=(float*)(vp->mat->data);
                }
                if(data!=NULL)
                {
                    data[0*4+0]*=(1-(float)y*0.05);
                    data[1*4+1]*=(1-(float)y*0.05);
                    data[2*4+2]*=(1-(float)y*0.05);
                }
                  
            }

        }
        else
        {
            float *data=(float*)(mc->matrix_inverse->data);
            data[2*4+3]+=(float)y*0.05;
        }

    }
}
void viewer_Arcroll_mouse_button_callback(Viewer_Intera* mi)
{
    Interactor_GlobalInfo* g_info=mi->g_info;
    
    //printf("%d\n",g_info->mouse_button);
    if(g_info->pick_something!=NULL)
    {
        Viewer_Something*vs=(Viewer_Something*)(g_info->pick_something);

        if(strcmp(vs->name,(char*)"Faces")==0)
        {
            //printf("\n");    
        }
       // printf("pick something id:%d\n",vs->id);
    }
    if(g_info->key==VIEWER_KEY_CONTROL&&g_info->key_action==1)
    {
    //    int id=g_info->readpixelcolor[0]*255*255+g_info->readpixelcolor[1]*255+g_info->readpixelcolor[2];
        //char faces[]="faces";
        //Node* names_id=Mesh_viewer_world_find_species()

    
    } 
}
void viewer_Arcroll_key_callback(Viewer_Intera*vi)
{

   // Viewer_Opengl_Interpreter* voi=upd->voi;
   // voi->update_data(voi);
    Interactor_GlobalInfo* g_info=vi->g_info;
    Viewer_Arcroll* va=(Viewer_Arcroll*)(vi->representation);
  
    if(g_info->key_action==VIEWER_PRESS&&g_info->key==VIEWER_KEY_D)
    {

        if(g_info->pick_something==va->vs||g_info->pick_something==NULL)
        {
            return ;
        }
        auto  vs=(Viewer_Something*)(g_info->pick_something);
        if(vs->disappear==1)
        {
            return;
        }
        va->disappear_somethings=node_overlying(va->disappear_somethings,vs);
        vs->disappear=1;  

    }
    else if(g_info->key_action==VIEWER_PRESS&&g_info->key==VIEWER_KEY_B)
    {
//    auto  vs=(Viewer_Something*)(g_info->pick_something);
  //      va->disappear_somethings=node_overlying(va->disappear_somethings,vs);
    //    vs->disappear=1;  
        if(va->disappear_somethings!=NULL)
        {
            Node* temp_n=va->disappear_somethings;va->disappear_somethings=(Node*)(va->disappear_somethings->Next);
            auto vs=(Viewer_Something*)(temp_n->value);

            vs->disappear=0; 

            free(temp_n);
        }
    }



    //Viewer_Something* vs=upd->vs;


}

static void myobj2off(char const * filename)
{
    int filenamesize=strlen(filename);
    char *outfilename=(char*)malloc(sizeof(char)*(filenamesize+2));
    strcpy(outfilename,filename);
    strcpy(&(outfilename[filenamesize-3]),"off");
    printf("%s\n",outfilename);
#ifdef WIN32
    FILE *infile,*outfile;
    fopen_s(&infile,filename,"r");
    fopen_s(&outfile,outfilename,"w");
#else
    FILE * infile=fopen(filename,"r");
    FILE *outfile=fopen(outfilename,"w");
#endif
    if(!infile)
    {
        printf("can't open this file %s\n",filename);
        return;
    }
    if(!outfile)
    {
        printf("can't open this file %s\n",filename);
        return;
    }
    char a;
    int v_rows=0;
    while(!feof(infile))
    {
         fscanf(infile,"%c",&a);
        if(a=='\r')
        {
             fscanf(infile,"%c",&a);
        }
        if(a=='#')
        {
            myfgets(infile); 
        }
        else if(a=='v')
        {
            fscanf(infile,"%c",&a);
            if(a==' ')
            {
               v_rows++;
            }
            
            myfgets(infile);
            
        }
        else if(a=='\n')
        {
        }
        else
        {
            
           myfgets(infile);
        } 
    }
    fseek(outfile,0,SEEK_SET);
    fprintf(outfile,"OFF\n");
    fprintf(outfile,"%u %u %d\n",v_rows,0,0);

    fseek(infile,0,SEEK_SET);
    unsigned int f_ids[10];
    while(!feof(infile))
    {
        double v_p;
        unsigned int f_id;
        fscanf(infile,"%c",&a);
        if(a=='\r')
        {
             fscanf(infile,"%c",&a);
        }
        if(a=='#')
        {
            myfgets(infile); 
        }
        else if(a=='v')
        {
            fscanf(infile,"%c",&a);
            if(a==' ')
            {
                for(int i=0;i<3;i++)
                {
                
                    fscanf(infile,"%lf ",&v_p);
                    //printf("%lf ",v_p);
                    fprintf(outfile,"%lf ",v_p);
                }
              //  v_rows++;
                fprintf(outfile,"\n");
            }
            else
            {
            
                myfgets(infile);
            }
            
        }
        else if(a=='\n')
        {
        }
        else
        {
            
           myfgets(infile);
        }
    
    }
    
    printf("v_rows: %u here\n",v_rows);
   free(outfilename); 
    fclose(infile);
    fclose(outfile); 
}

static void _Read_(Mesh*mesh,char const * filename)
{
    int i=0;
    while(filename[i]!='\0')
    {
        i++;
    }
    i--;
    while(filename[i]!='.'&&filename[i]!='\0'&&i>0)
    {

        i--;
    }
    //printf("%c\n",filename[i] );
    if(filename[i]=='\0'||filename[i+1]=='\0'||filename[i+2]=='\0')
    {return;} 
    char fn[200];
    strcpy(fn,filename);
    if(filename[i+1]=='o'&&filename[i+2]=='b'&&filename[i+3]=='j') 
    {
        fn[i+1]='o';fn[i+2]='f';fn[i+3]='f';
        myobj2off(filename);
        _ReadOff_(mesh,fn,3); 
    }
    else if(filename[i+1]=='o'&&filename[i+2]=='f'&&filename[i+3]=='f')
    {
        printf("read off\n"); 
        _ReadOff_(mesh,filename,3);
    }
    else if(filename[i+1]=='c'&&filename[i+2]=='e'&&filename[i+3]=='l'&&filename[i+4]=='l')
    {
       // printf("here\n");
        _ReadCell_(mesh,filename);
    }
}
void viewer_Arcroll_drop_callback(Viewer_Intera*vi)
{
    Interactor_GlobalInfo* g_info=vi->g_info;
    Viewer_Arcroll* va=(Viewer_Arcroll*)(vi->representation);

    Viewer_World* vw=va->vw;
    Viewer_Opengl_Interpreter* voi=va->voi;
    for(int i=0;i<g_info->drop_count;i++)
    {
        //g_info->paths[i]=(char*)malloc(sizeof(char)*200);
        //strcpy(g_info->paths[i],paths[i]);
        Mesh * mesh=new Mesh;
        Mesh_init(mesh);
        _Read_(mesh,g_info->paths[i]);

        printf("%s\n",g_info->paths[i] );
        Node* n=NULL;
        if(mesh->num_c(mesh)>0)
        {
            char faces[]="Faces";
            n=vw->create_something(vw,faces);
            auto vs=(Viewer_Something*)(n->value);
            // vs->disappear=1;
            auto vf=(Viewer_Faces*)(vs->evolution);
            vf->Data_rows=mesh->num_v(mesh);
            vf->Data_index_rows=mesh->num_c(mesh);
            vf->normal_rows=vf->Data_index_rows;
            vf->color_rows=vf->Data_index_rows;
            srand((unsigned)time(NULL));
            float colors[4]={(rand()%100)/100.0,(rand()%100)/100.0,(rand()%100)/100.0,1};
            vf->is_transparent=0;
            vf->set_color(vf,colors);
            //vf->Data=(float*)malloc(sizeof(float)*vf->Data_rows*3);
            // vf->Data_index=(unsigned int*)malloc(sizeof(unsigned int)*4*vf->Data_index_rows);
    
            //vf->Data[0]=29158;vf->Data[1]=204;vf->Data[2]=6298;
            //vf->Data[3]=29066;vf->Data[4]=204;vf->Data[5]=6118;
            //vf->Data[6]=29072;vf->Data[7]=396;vf->Data[8]=6110;
            //vf->Data_index[0]=3;vf->Data_index[1]=0;vf->Data_index[2]=1;vf->Data_index[3]=2;

            get_data_from_2dim_cell(mesh,&(vf->Data),&(vf->Data_index));
            free_node(n);

        }
        else if(mesh->num_v(mesh)>0)
        {
            char points[]="Points";
            n=vw->create_something(vw,points);
            auto vs=(Viewer_Something*)(n->value);
            auto  vp=(Viewer_Points*)(vs->evolution);
            vp->Data_rows=mesh->num_v(mesh);
            //vp->color_rows=vp->Data_rows;
            vp->pointsize=5.0;
            vp->Data=(float*)malloc(sizeof(float)*3*vp->Data_rows);
            int i=0;
            for(auto vit=mesh->vertices.begin();vit!=mesh->vertices.end();vit++)
            {
                vp->Data[i]=vit->second->point[0];
                i++;
                vp->Data[i]=vit->second->point[1];
                i++;
                vp->Data[i]=vit->second->point[2];
                i++;
            }
            srand((unsigned)time(NULL));
            float colors[4]={(rand()%100)/100.0,(rand()%100)/100.0,(rand()%100)/100.0,1};
            vp->set_color(vp,colors); 
        }
   

        Mesh_free(mesh);
        delete mesh;
    }
    //printf("endfsdfs\n");
    voi->update_data(voi);

}
#undef Matrix4x4
