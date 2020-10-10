#ifndef ZKHY_TOOLS_COMMON_H_
#define ZKHY_TOOLS_COMMON_H_
#include<tool/libcell_tools_kd_node.h>
#ifdef __cplusplus
extern "C" {
#endif

static void points_obj2off(char const * filename)
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
    int v_rows=0,f_rows=0;
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
    while(!feof(infile))
    {
        double v_p;
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
    
    printf("v_rows: %u,f_rows: %u here\n",v_rows,f_rows);
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
   // printf("%c\n",filename[i] );
    if(filename[i]=='\0'||filename[i+1]=='\0'||filename[i+2]=='\0')
    {return;} 
    char fn[200];
    strcpy(fn,filename);
    if(filename[i+1]=='o'&&filename[i+2]=='b'&&filename[i+3]=='j') 
    {
        fn[i+1]='o';fn[i+2]='f';fn[i+3]='f';
        points_obj2off(filename);
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
#ifdef __cplusplus
}
#endif
#endif
