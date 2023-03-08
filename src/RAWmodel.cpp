#include <stdio.h>
#include <iostream>
#include <assert.h>
#include <string.h>
#include <GLFW/glfw3.h>
#include <iomanip>
#include "RAWmodel.h"

RAWmodel::RAWmodel(){

}

RAWmodel::~RAWmodel(){
    // free(bounderVoxelData);
    for(int i = 0; i < infdata.resolution[0]; i++){
        for(int j = 0; j < infdata.resolution[1]; j++){
            free(voxelData[i][j]);
        }
    }
    for(int i = 0; i < infdata.resolution[0]; i++){
        free(voxelData[i]);
    }
    free(voxelData);
}
void RAWmodel::LoadFile(const char* infFileName,const char* rawFileName){
    LoadINFfile(infFileName);
    CreateVoxel();
    LoadRAWfile(rawFileName);
    
}

bool RAWmodel::LoadINFfile(const char* infFileName){
    FILE *file = NULL;
    errno_t err;                    //record error
    file = fopen(infFileName, "r"); // err = 0 success, err != fail
    if(file == NULL){
        std::cout << "Failed to open rawfile" << std::endl;
        return false;
    }


    char buffer[133];
    int lineno = 0; // record line number // total 12 line
    char c[10];
    // start reading
    fgets(buffer, sizeof(buffer), file); // raw-file=XXX.raw
    lineno++;
    fgets(buffer, sizeof(buffer), file);// resolution=XX*XX*XX
    sscanf(buffer, "resolution=%dx%dx%d", &infdata.resolution[0], &infdata.resolution[1], &infdata.resolution[2]);
    std::cout <<  "resolution : "<<infdata.resolution[0] << ", " << infdata.resolution[1] << ", " << infdata.resolution[2] << std::endl;
    
    fgets(buffer, sizeof(buffer), file);// sample-type=XXXXXX
    sscanf(buffer, "sample-type=%s", c);
    if(SetSampleType(c) == false){
        std::cout << "Failed to set sample type" << std::endl;  
        return false;;
    }
    // std::cout << infdata.sampleType << std::endl;

    fgets(buffer, sizeof(buffer), file);//voxel-size=XXX:XXX:XXX
    sscanf(buffer, "voxel-size=%f:%f:%f", &infdata.voxelSize[0], &infdata.voxelSize[1], &infdata.voxelSize[2]);
    // std::cout << "voxel-size : "<<infdata.voxelSize[0] << ", " << infdata.voxelSize[1] << ", " << infdata.voxelSize[2] << std::endl;
    
    fgets(buffer, sizeof(buffer), file);//endian=XXX
    sscanf(buffer, "endian=%s",infdata.endian);
    // std::cout << infdata.endian << std::endl;

    fgets(buffer, sizeof(buffer), file);//Min=XXX:XXX:XXX
    sscanf(buffer, "Min=%f:%f:%f",&infdata.min[0],&infdata.min[1],&infdata.min[2]);
    std::cout << "min : "<<infdata.min[0] << ", " << infdata.min[1] << ", " << infdata.min[2] << std::endl;

    fgets(buffer, sizeof(buffer), file);//Max=XXX:XXX:XXX
    sscanf(buffer, "Max=%f:%f:%f",&infdata.max[0],&infdata.max[1],&infdata.max[2]);
    std::cout <<"max : "<< infdata.max[0] << ", " << infdata.max[1] << ", " << infdata.max[2] << std::endl;


    if (feof(file))
    {
        std::cout << "End of file reached!" << std::endl;
    }
    else if (ferror(file))
    {
        std::cout << "Encountered an error while reading the file!" << std::endl;
    }

    fclose(file);

    return true;

}

void RAWmodel::CreateVoxel(){
    if(infdata.type == 0){
        uc_voxelData = (BYTE*)malloc(sizeof(BYTE) * infdata.resolution[0] * infdata.resolution[1] * infdata.resolution[2]);
    }else if(infdata.type == 1){
        f_voxelData = (float*)malloc(sizeof(float)* infdata.resolution[0] * infdata.resolution[1] * infdata.resolution[2]);
    }else if(infdata.type == 2){
        d_voxelData = (double*)malloc(sizeof(double)* infdata.resolution[0] * infdata.resolution[1] * infdata.resolution[2]);
    }

    voxelData = (int***)malloc(sizeof(int**) * infdata.resolution[2]);
    for(int i = 0; i < infdata.resolution[2]; i++){
        voxelData[i] = (int**)malloc(sizeof(int*) * infdata.resolution[1]);
        for(int j = 0; j < infdata.resolution[1]; j++){
            voxelData[i][j] = (int*)malloc(sizeof(int) * infdata.resolution[0]);
            for(int k = 0; k < infdata.resolution[0]; k++){
                voxelData[i][j][k] = 0;
            }
        }
    }
}

bool RAWmodel::LoadRAWfile(const char* rawFileName){
    FILE *file = NULL;
    errno_t err;                    //record error
    file = fopen(rawFileName, "r"); // err = 0 success, err != fail
    if(file == NULL){
        std::cout << "Failed to open rawfile" << std::endl;
        return false;
    }

    //read raw to 3Darray
    if(!ReadRawFile(file)){
        std::cout << "Failed to read raw file" << std::endl;
    }
    //set 0 air, 1 bounder, 2 inside
    SetVoxelData();

    // error detect
    if (feof(file))
        std::cout << "End of file reached!" << std::endl;
    else if (ferror(file))
        std::cout << "Encountered an error while reading the file!" << std::endl;

    fclose(file);

    return true;
}

bool RAWmodel::ReadRawFile(FILE *file){
    int size = infdata.resolution[0] * infdata.resolution[1] * infdata.resolution[2];
    bounderNum = 0;
    if(infdata.type == 0){
        fread(uc_voxelData, sizeof(BYTE),size, file);
        for(int i = 0; i < infdata.resolution[2]; i++){
            for(int j = 0; j < infdata.resolution[1]; j++){
                for(int k = 0; k < infdata.resolution[0]; k++){
                    int num = k + j*infdata.resolution[0] + i*infdata.resolution[0]* infdata.resolution[1];
                    voxelData[i][j][k] = uc_voxelData[num];
                    if(voxelData[i][j][k] == 255){
                        bounderNum++;
                    }
                }
            }
        }
        return true;
    }else if(infdata.type == 1){
        fread(f_voxelData, sizeof(float),size, file);
        for(int i = 0; i < infdata.resolution[2]; i++){
            for(int j = 0; j < infdata.resolution[1]; j++){
                for(int k = 0; k < infdata.resolution[0]; k++){
                    int num = k + j*infdata.resolution[0] + i*infdata.resolution[0]* infdata.resolution[1];
                    voxelData[i][j][k] = f_voxelData[num];
                    if(voxelData[i][j][k] == 255){
                        bounderNum++;
                    }
                }
            }
        }
        return true;
    }else if(infdata.type == 2){
        fread(d_voxelData, sizeof(double),size, file);
        for(int i = 0; i < infdata.resolution[2]; i++){
            for(int j = 0; j < infdata.resolution[1]; j++){
                for(int k = 0; k < infdata.resolution[0]; k++){
                    int num = k + j*infdata.resolution[0] + i*infdata.resolution[0]* infdata.resolution[1];
                    voxelData[i][j][k] = d_voxelData[num];
                    if(voxelData[i][j][k] == 255){
                        bounderNum++;
                    }
                }
            }
        }
        return true;
    }
    return false;

}


void RAWmodel::SetVoxelData(){
    
    int num = 0;
    for(int i = 0; i < infdata.resolution[2]; i++){
        for(int j = 0; j < infdata.resolution[1]; j++){
            for(int k = 0; k < infdata.resolution[0]; k++){
                // std::cout << voxelData[i][j][k] <<" ";
                if(voxelData[i][j][k] == 252) voxelData[i][j][k] =2;
                else if(voxelData[i][j][k] == 0) voxelData[i][j][k] = 0;
                else if(voxelData[i][j][k] == 254)voxelData[i][j][k] = 18;
                else if(voxelData[i][j][k] == 253)voxelData[i][j][k] = 26;
                else if(voxelData[i][j][k] == 255){
                    voxelData[i][j][k] = 6;
                    bounderVoxelData.push_back(VoxData_b{{j-(infdata.resolution[1]),i,k},{}});
                    // bounderVoxelData[num].bounderVoxelLocate = {j-(infdata.resolution[1]),i,k};
                    setMaxbounder(j-(infdata.resolution[1]), i, k);
                    // bounderVoxelData[num].bounderVoxelFaceAir[0] = 1;
                    SetbounderVoxelFaceAir(i,j,k, num);
                    num++;
                }
            }
        }
    }
    // std::cout << bounderMaxLocate[0] << ", " << bounderMaxLocate[1] << ", " << bounderMaxLocate[2] << std::endl;
}
void RAWmodel::setMaxbounder(int i, int j, int k){
    if(bounderMaxLocate[0] > i) bounderMaxLocate[0] = i;
    if(bounderMaxLocate[1] < j) bounderMaxLocate[1] = j;
    if(bounderMaxLocate[2] < k) bounderMaxLocate[2] = k;
}
void RAWmodel::SetbounderVoxelFaceAir(int i, int j, int k, int num){
    
    if(i+1 < infdata.resolution[2]){
        if(voxelData[i+1][j][k] != 0){
            bounderVoxelData[num].bounderVoxelFaceAir[3] = 1;
        }
    }
    
    if(i-1 >= 0){
        if(voxelData[i-1][j][k] != 0){

            bounderVoxelData[num].bounderVoxelFaceAir[2] = 1;
        }
    }
    
    if(j+1 < infdata.resolution[1]){
        if(voxelData[i][j+1][k] != 0){
            bounderVoxelData[num].bounderVoxelFaceAir[5] = 1;
            
        }
    }
    if(j-1 >= 0){

        if(voxelData[i][j-1][k] != 0){
            bounderVoxelData[num].bounderVoxelFaceAir[4] = 1;
        }
    }
    if(k+1 < infdata.resolution[0]){

        if(voxelData[i][j][k+1] != 0){
            bounderVoxelData[num].bounderVoxelFaceAir[1] = 1;
        }
    }
    if(k-1 >= 0){
        if(voxelData[i][j][k-1] != 0){
            bounderVoxelData[num].bounderVoxelFaceAir[0] = 1;
        }

    }

}
bool RAWmodel::SetSampleType(const char* type){
    if (!strcmp(type, "unsigned")){
        memset(infdata.sampleType, '\0', sizeof(infdata.sampleType));
		strcat(infdata.sampleType, "unsigned char");
        infdata.type = 0;
        return true;
    }else if(!strcmp(type, "float")){
        memset(infdata.sampleType, '\0', sizeof(infdata.sampleType));
		strcat(infdata.sampleType, "float");
        infdata.type = 1;
        return true;
    }else if(!strcmp(type, "double")){
        memset(infdata.sampleType, '\0', sizeof(infdata.sampleType));
		strcat(infdata.sampleType, "double");
        infdata.type = 2;
        return true;
    }
    return false;
}


void RAWmodel::checkComputerEndian(){
    short int a = 0x1234;
    char *p = (char *)&a;
    
    printf("p=%#hhx\n",*p);

    if(*p == 0x34)
        printf("Little endian \n");
    else if(*p == 0x12)
        printf("Big endian \n");
    else
        printf("Unknow endian \n");

}