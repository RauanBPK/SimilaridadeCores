#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>
#include <numeric> 

// Rauan Pires - 14103318

// Programa para segmentar imagens.
// Baseado em 4 algoritmos:
// L1-Norm - Cubo no RGB
// L2-Norm - Esfera no RGB
// Distancia de Mahalanobis
// K-Vizinhos mais proximos

// Existe muita coisa sendo calculada mais que o necessario. Redundancia
// Possiveis bugs no menu devido a forma como o terminal trabalha processando as entradas
// Nem todo valor de retorno de funcao esta sendo checado
// A inversa esta sendo calculada pelo OpenCV. Tive problemas para calcula-la usando determinante (numerico/computacional)
// Nao existe rastro na imagem indicando pontos selecionados (desnecessario)
// Algoritmos calculados sobre pontos pre selecionados (pelo usuario). Nao calcula em tempo de amostragem.
// Porem.... TODOS FUNCIONAM :)
 
using namespace std;
using namespace cv;

vector<int> amostraR;
vector<int> amostraG;
vector<int> amostraB;

unsigned char* ReadPPM(const char* filename, unsigned int &W, unsigned int &H, char* type)
{
    FILE* fp = fopen(filename, "r");
    if(!fp) return NULL;

     fscanf(fp, "%c", &type[0]);   
     fscanf(fp, "%c", &type[1]);
     unsigned int maxVal=0;
     
     unsigned char* data;

     if(type[0] == 'P' && type[1] == '3')
     {
            fscanf(fp, "%d %d %d", &W, &H, &maxVal);
            data = (unsigned char*) calloc(W*H*3, sizeof(unsigned int)); 
            
            for(int i=0; i<W*H*3; i++)
            {
                    fscanf(fp, "%d ", &data[i]);
            }            
    }
    fclose(fp);
    return data;
}

bool SavePPM(const char* filename, unsigned char *data, unsigned int W, unsigned int H)
{
    FILE* fp = fopen(filename, "w");
    if(!fp) return false;
        
    fprintf(fp, "P3\n%d %d 255\n", W, H);
     
    for(int i=0; i<W*H*3; i++)
    {
            fprintf(fp, "%d ", data[i]);
    }
    fclose(fp);
    return data;
}

bool isBetween (unsigned int valor,int thN, int thP){


    if( valor > thN && valor < thP){
        return true;
    }else{
        return false;
    }
}

void CallBackFunc(int event, int x, int y, int flags, void* param)
{

    Mat *rgb =  (Mat*) param;
    if ( event == EVENT_MOUSEMOVE && flags == EVENT_FLAG_LBUTTON )
    {

        printf(" R %d, G %d, B %d\n", 
        (int)(*rgb).at<Vec3b>(y, x)[2], 
        (int)(*rgb).at<Vec3b>(y, x)[1], 
        (int)(*rgb).at<Vec3b>(y, x)[0]); 

        // Adiciona RGB lido nos vetores de amostras
        amostraR.push_back((int)(*rgb).at<Vec3b>(y, x)[2]);
        amostraG.push_back((int)(*rgb).at<Vec3b>(y, x)[1]);
        amostraB.push_back((int)(*rgb).at<Vec3b>(y, x)[0]);
    }    
}

int maxOf3( int a, int b, int c )
{
   int max = ( a < b ) ? b : a;
   return ( ( max < c ) ? c : max );
}

int minOf3( int a, int b, int c )
{
   int min = ( a > b ) ? b : a;
   return ( ( min > c ) ? c : min );
}

float avgOf3(int a, int b, int c){
    return (a+b+c)/3;
}

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
            printf("Input arguments error. Input should be <executable {input}.ppm {output}.ppm>\n");
            return 1;
    }
 
    unsigned int H, W; 
    H=W=0;
    char type[2];
    int option = 0;
 
    int done = 0;
    do{

    // Zera vetores de amostras. A cada iteracao permite selecionar pontos novamente.
    amostraR = {0};
    amostraG = {0};
    amostraB = {0};
    unsigned char *data = ReadPPM(argv[1], W, H, type);
    if(data==NULL)
    {
            printf("I found a problem, help me !\n");
            return 1;       
    }  
    Mat img = imread(argv[1]);
 
    //if fail to read the image
    if ( img.empty() ) 
    { 
        cout << "Error loading the image" << endl;
        return -1; 
    }

    printf("\nSelecione os pontos na imagem com o botao esquerdo do mouse!\n");

    namedWindow("ImageDisplay", 1);
 
    //set the callback function for any mouse event
    setMouseCallback("ImageDisplay", CallBackFunc, &img);
 
    //show the image
    imshow("ImageDisplay", img);
 
    // Wait until user press some key
    waitKey(0);

     

    int maxR = *max_element(amostraR.begin(), amostraR.end());
    int maxG = *max_element(amostraG.begin(), amostraG.end());
    int maxB = *max_element(amostraB.begin(), amostraB.end());
    int minR = *min_element(amostraR.begin(), amostraR.end());
    int minG = *min_element(amostraG.begin(), amostraG.end());
    int minB = *min_element(amostraB.begin(), amostraB.end());

    // averageX => media dos valores do vetor de amostra
    float averageR = accumulate( amostraR.begin(), amostraR.end(), 0.0)/amostraR.size();
    float averageG = accumulate( amostraG.begin(), amostraG.end(), 0.0)/amostraG.size();
    float averageB = accumulate( amostraB.begin(), amostraB.end(), 0.0)/amostraB.size();

    // medX => valor intermediario do vetor. 
    float medR = (maxR - minR)/2 + minR;
    float medG = (maxG - minG)/2 + minG;
    float medB = (maxB - minB)/2 + minB;

    printf("averageR %d\n", (int)averageR );
    printf("averageG %d\n", (int)averageG );
    printf("averageB %d\n", (int)averageB );
    printf("maxR %d\n", maxR );
    printf("maxG %d\n", maxG );
    printf("maxB %d\n", maxB );
    printf("minR %d\n", minR );
    printf("minG %d\n", minG );
    printf("minB %d\n", minB );
    

    
    printf("\n------------------------------------\n");
    printf("Escolha o algoritmo a ser usado.");
    printf("\n1 - Volume  RGB");
    printf("\n2 - Esfera  RGB");
    printf("\n3 - Mahalanobis  RGB");
    printf("\n4 - K-vizinhos  RGB\n> ");

    
    scanf("%d", &option);

    switch(option){

//-------------------------CUBO/VOLUME--------------------------//
        case 1:
            for(int i=0; i<H; i++)
                for(int j=0; j<W; j++)
                {            
                    int r = data[i*W*3 + j*3];
                    int g = data[i*W*3 + j*3 + 1];
                    int b = data[i*W*3 + j*3 + 2];
                    
                    // Se alguma das coordenadas esta fora do limite min-max, o ponto fica de fora
                    if(!isBetween(r,minR,maxR) || !isBetween(g, minG, maxG) || !isBetween(b, minB, maxB))
                    {
                        data[i*W*3 + j*3] = 0;
                        data[i*W*3 + j*3 + 1] = 0;
                        data[i*W*3 + j*3 + 2] = 0;
                    }
                }
            break;
//------------------------ESFERA---------------------------------//
        case 2:
            for(int i=0; i<H; i++)
                for(int j=0; j<W; j++)
                {            
                    int r = data[i*W*3 + j*3];
                    int g = data[i*W*3 + j*3 + 1];
                    int b = data[i*W*3 + j*3 + 2];
                   
                    float DE = (r - averageR)*(r - averageR) + (g - averageG)*(g - averageG) + (b - averageB)*(b - averageB);
                    
                    // Threshold calculado a partir de dados da amostra. Media do Ponto max - ponto medio de cada componente
                    if(sqrt(DE) > avgOf3(maxR - medR, maxG - medG, maxB - medB))
                    {
                                data[i*W*3 + j*3] = 0;
                                data[i*W*3 + j*3 + 1] = 0;
                                data[i*W*3 + j*3 + 2] = 0;                        
                    }
                }
            break;
//-----------------------MAHALANOBIS------------------------------//
        case 3:
        {
            //     R     G    B
           
            // R [0-0]  [0-1]  [0-2]

            // G [1-0]  [1-1]  [1-2]

            // B [2-0]  [2-1]  [2-2]

            //double covarMat[3][3];

            Mat covarMat(3,3,CV_64F);
            Mat covInv(3,3,CV_64F);
            

            // COVAR RxR
            for(int i = 0; i < amostraR.size(); i++)
            {
                covarMat.at<double>(0,0) += pow((amostraR[i] - averageR),2);
            }
            covarMat.at<double>(0,0) /= amostraR.size();

            // COVAR GXG
            for(int i = 0; i < amostraG.size(); i++)
            {
                covarMat.at<double>(1,1) += pow((amostraG[i] - averageG),2);
            }
            covarMat.at<double>(1,1) /= amostraG.size();

            // COVAR BXB
            for(int i = 0; i < amostraB.size(); i++)
            {
                covarMat.at<double>(2,2) += pow((amostraB[i] - averageB),2);
            }
            covarMat.at<double>(2,2) /= amostraB.size();

            // COVAR RxG
            for(int i = 0; i < amostraR.size(); i++)
            {
                covarMat.at<double>(0,1) += (amostraR[i] - averageR) * (amostraG[i] - averageG);
            }
            covarMat.at<double>(0,1) /= amostraR.size();
            covarMat.at<double>(1,0) = covarMat.at<double>(0,1); // Simetria

            // COVAR RxB
            for(int i = 0; i < amostraR.size(); i++)
            {
                covarMat.at<double>(0,2) += (amostraR[i] - averageR) * (amostraB[i] - averageB);
            }
            covarMat.at<double>(0,2) /= amostraR.size();
            covarMat.at<double>(2,0) = covarMat.at<double>(0,2); // Simetria

            // COVAR GxB
            for(int i = 0; i < amostraG.size(); i++)
            {
                covarMat.at<double>(1,2) += (amostraG[i] - averageG) * (amostraB[i] - averageB);
            }
            covarMat.at<double>(1,2) /= amostraG.size();
            covarMat.at<double>(2,1) = covarMat.at<double>(1,2); // Simetria


            // Fiz a inversa pelo opencv pois pelo metodo comum, usando determinante, encotrei problemas numericos/computacionais
            // O determinante fica muito alto e por consequencia o calculo da inversa nao e preciso.
            covInv = covarMat.inv();          
            
            // [r-rm, g-gm, b-bm]* [covarMat(3x3)] = [l,m,n]

            // [l,m,n] * |r-rm| = [K]  dMahalanobis = sqrt(K)
            //           |g-gm|
            //           |b-bm|
           
            double l,m,n,K;
            double dMahalanobis;

            //Threshold mahalanobis - quanto maior, mais permissivo fica o algoritmo
            float thMahalanobis;
            printf("Digite o threshold de Mahalanobis: (normalmente entre 1 e 4)\n");
            scanf("%f", &thMahalanobis);

            for(int i=0; i<H; i++)
                for(int j=0; j<W; j++)
                {            
                    int r = data[i*W*3 + j*3];
                    int g = data[i*W*3 + j*3 + 1];
                    int b = data[i*W*3 + j*3 + 2];

                    l = ((r - averageR) * covInv.at<double>(0,0)) + ((g - averageG) * covInv.at<double>(1,0)) + ((b - averageB) * covInv.at<double>(2,0));
                    m = ((r - averageR) * covInv.at<double>(0,1)) + ((g - averageG) * covInv.at<double>(1,1)) + ((b - averageB) * covInv.at<double>(2,1));
                    n = ((r - averageR) * covInv.at<double>(0,2)) + ((g - averageG) * covInv.at<double>(1,2)) + ((b - averageB) * covInv.at<double>(2,2));

                    K = (l * (r - averageR)) + (m * (g - averageG)) + (n * (b - averageB));

                    dMahalanobis = sqrt(K);
                    //printf("%f\n", K);

                    // Threshold escolhido pelo usuario. (Normalmente funciona bem entre 2 e 4)
                    if(dMahalanobis > thMahalanobis){
                          
                        data[i*W*3 + j*3] = 0;
                        data[i*W*3 + j*3 + 1] = 0;
                        data[i*W*3 + j*3 + 2] = 0; 

                    }
                   

                }
        }
            break;
//----------------------K-VIZINHOS--------------------------------//
        case 4:
            {
                int numEsf, threshold;
                float tamSeg;
                printf("Digite o numero de esferas para fazer a segmentacao:\n");
                scanf("%d", &numEsf);
                if(numEsf > amostraR.size()){
                    printf("Impossivel dividir em tantas esferas!\n");
                    break;
                }

                // Cada esfera tem o mesmo raio
                printf("Digite o raio de cada esfera\n");
                scanf("%d",&threshold);
                tamSeg = amostraR.size()/numEsf;

                // Zera dados originais (bem feio, mas ok)
                for(int i=0; i<H; i++)
                for(int j=0; j<W; j++)
                { 
                    data[i*W*3 + j*3] = 0;
                    data[i*W*3 + j*3 + 1] = 0;
                    data[i*W*3 + j*3 + 2] = 0;
                }

                //Matriz auxiliar. Abre imagem novamente (poderia copiar? sim, mas posso abrir denovo tambem)
                unsigned char *data2 = ReadPPM(argv[1], W, H, type);

                for(int i = 1; i <= numEsf; i++){

                    //Valor medio de cada segmento
                    int ravg = accumulate( &amostraR[int(i*tamSeg - tamSeg)], &amostraR[int(i*tamSeg)], 0.0)/tamSeg;
                    int gavg = accumulate( &amostraG[int(i*tamSeg - tamSeg)], &amostraG[int(i*tamSeg)], 0.0)/tamSeg;
                    int bavg = accumulate( &amostraB[int(i*tamSeg - tamSeg)], &amostraB[int(i*tamSeg)], 0.0)/tamSeg;

                    for(int i=0; i<H; i++)
                    for(int j=0; j<W; j++)
                    {            
                        int r = data2[i*W*3 + j*3];
                        int g = data2[i*W*3 + j*3 + 1];
                        int b = data2[i*W*3 + j*3 + 2];
                        float DE = (r - ravg)*(r - ravg) + (g - gavg)*(g - gavg) + (b - bavg)*(b - bavg);
                        if(sqrt(DE) < threshold)
                        {
                            // Se o ponto este dentro de alguma esfera, pintar da cor da matriz auxiliar na mesma posicao
                            data[i*W*3 + j*3] = data2[i*W*3 + j*3];
                            data[i*W*3 + j*3 + 1] = data2[i*W*3 + j*3 + 1];
                            data[i*W*3 + j*3 + 2] = data2[i*W*3 + j*3 + 2];                        
                        }

                    }
                }
            break;
            }
        default:
            printf("Invalid option!\n");
            break;
    }
        

    SavePPM(argv[2], data, W, H);

    Mat imgRes = imread(argv[2]);
    namedWindow("ImageDisplay2", 1);
    imshow("ImageDisplay2", imgRes);
    waitKey(0);
    printf("\nGostou do resultado?\n");
    printf("1 - SIM\n* - NAO\n> ");
    scanf("%d", &done);
    free(data);
}while(done != 1);   
    
    printf("\n\nShow!");
    return 0;
}
