#include <functional>
#include <iostream>
#include <random>
#include <cmath>
using namespace std;


//Funciones a integrar
double func1(double x){
    return sqrt(1-x*x);
}
double func2(double x){
    return 1.0/x;
}
double func3(double x){
    return tan(x);
}

//VEGAS para funciones escalares f:Rn->R
double vegas_algorithm(double (*f)(double), double a,double b,int K,int T,int S)
{
    std::default_random_engine generator; //Generador de numeros aleatorios
    std::uniform_real_distribution<double> dist(0.0,1.0); //Distribucion uniforme en (0,1)
    vector<double> g(K,0.0);//Vector de tamaño K
    //Muestreo Estratificado
    for(int t=0;t<T;t++)  //Se hacen T estimaciones
    {
        for(int k=0;k<K;k++)
        {
            double x = (b-a)*(dist(generator)+k)/K+a;//Numero aleatorio uniforme en el intervalo (a,b)
            g[k] += fabs(f(x))/T;//Estimador en cada intervalo de la forma [(b-a)*k/K+a,(b-a)*(k+1)/K+a] de tamaño T
        }

    }

    double s_g = 0.0;
    for(int k=0;k<K;k++) s_g += g[k];   //Se suman todos lo estimadores en cada intervalo
    for(int k=0;k<K;k++) g[k] /= s_g;   //Se promedian

    //Muestreo por importancia
    std::discrete_distribution<int> bin_dist(g.begin(),g.end()); //Distribucion que da mayor probabilidad donde la funcion aporta mas a la integral
    double I = 0.0;
    for(int s=0;s<S;s++)   //Estimacion de la integral
    {
        int z = bin_dist(generator);//Variable con la distribucion descrita por el vector g
        double x = (b-a)*(dist(generator)+z)/K+a; //Numero aleatorio en el intervalo [(b-a)*z/K+a,(b-a)*(z+1)/K+a] donde se concentra el valor de la integral
        I += (b-a)*f(x)/(g[z]*K*S); //Muestreo por importancia
    }
    return I;
}
int main()
{
    cout.precision(5);
    double a1=-1.0; double b1=1.0;
    //Funcion 1
    double integral1=vegas_algorithm(&func1,a1,b1,256,1000,100000);
    cout<<"El valor de pi/2: "<<integral1<<endl;
    //Error en la estimacion
    cout<<"El error en la estimación fue:"<<fabs(integral1-M_PI/2.0)<<endl;
    //Funcion 2
    double a2=1.0; double b2=M_E;
    double integral2=vegas_algorithm(&func2,a2,b2,256,1000,100000);
    cout<<"El valor de ln(e) es: "<<integral2<<endl;
    cout<<"El error en la estimación fue: "<<fabs(integral2-1.0)<<endl;
    //Funcion 3
    double a3=0.0; double b3=M_PI/4.0;
    double integral3=vegas_algorithm(&func3,a3,b3,256,1000,100000);
    cout<<"El valor de ln(2)/2 es: "<<integral3<<endl;
    cout<<"El error en la estimación fue: "<<fabs((log(2.0)/2.0)-integral3)<<endl;
    return 0;
}
