#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct color{
    unsigned char r;
    unsigned char g;
    unsigned char b;
};
int main(int argc,char *argv[])
{

    //default filenames will be used if not provided in command line
    char source[100];
    if(argc>1)
    {
        strcpy(source,argv[1]);
    }
    else
    {
        strcpy(source,"render.txt");
    }
    FILE *fp;
    fp=fopen(source,"r");
    if(fp==NULL)
    {
        printf("File not found\n");
        return 0;
    }
    
    char destination[100];
    if(argc>2)
    {
        strcpy(destination,argv[2]);
    }
    else
    {
        strcpy(destination,"output.ppm");
    }
    FILE *fp1;
    fp1=fopen(destination,"w");
    if(fp1==NULL)
    {
        printf("File not found\n");
        return 0;
    }
    //Reading image width and height
    int width,height;
    fscanf(fp,"%d %d",&width,&height);

    //Allocating memory for the image
    struct color * image = (struct color *) malloc(width*height*sizeof(struct color));

    //Reading the image data into the allocated memory
    while(feof(fp)==0)
    {
        char ch;
        fscanf(fp," %c",&ch);
        
        if(ch == 'c')
        {
            int x,y,radius,r,b,g;

            //Reading the circle data
            fscanf(fp,"%d %d %d %d %d %d",&x,&y,&radius,&r,&g,&b);

            int i,j;

            //Making the filled circle
            for(i=x-radius;i<=x+radius;i++)
            {
                if(i<0||i>=width)
                    continue;
                for(j=y-radius;j<=y+radius;j++)
                {
                    if(j<0||j>=height)
                        continue;
                    if((i-x)*(i-x)+(j-y)*(j-y)<=radius*radius)
                    {
                        image[j*width+i].r=r;
                        image[j*width+i].g=g;
                        image[j*width+i].b=b;
                    }
                }
            }
        }
        else if(ch == 'r')
        {
            int x,y,wid,hei,r,g,b;

            //Reading the rectangle data
            fscanf(fp,"%d %d %d %d %d %d %d",&x,&y,&wid,&hei,&r,&g,&b);
            int i,j;

            //Making the filled rectangle
            for(i=x-wid/2;i<=x+wid/2;i++)
            {
                if(i<0||i>=width)
                    continue;
                for(j=y-hei/2;j<=y+hei/2;j++)
                {
                    if(j<0||j>=height)
                        continue;
                    image[j*width+i].r=r;
                    image[j*width+i].g=g;
                    image[j*width+i].b=b;
                    
                }
            }
        }
    }

    //ppm p3 header part
    fprintf(fp1,"P3 ");
    fprintf(fp1,"%d %d ",width,height);
    fprintf(fp1,"255\n");

    //pixel data to file 
    int i,j;
    for(j=0;j<width;j++)
    {
        for(i=0;i<height;i++)
        {
            fprintf(fp1,"%d %d %d ",(int)image[j*width+i].r,(int)image[j*width+i].g,(int)image[j*width+i].b);
        }
        fprintf(fp1,"\n");
    }

    free(image);
    fclose(fp);
    fclose(fp1);
    return 0;
}