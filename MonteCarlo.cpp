#include <iostream>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <random>


//Integracion de funciones escalares f:Rn->R
using namespace std;

typedef struct MCintegrate{
    //Elementos
    unsigned int n,ndim;//Longitud del vector aleatorio y dimension del dominio de integracion
    vector<double> xlo,xhi; //Límites inferiores y superiores del hipercubo
    vector<vector<double>> X;//Vector Aleatorio
    double I,err;//Integral y error
    double vol;//Volumen de la region de integracion
    double(*func)(vector<double>); //Funcion a integrar
    bool (*inregion)(vector<double>);
    //Funciones miembros
    MCintegrate(vector<double> xlow,vector<double> xhigh,double funcs(vector<double>),bool inregiop(vector<double>));//Constructor
    void integrate(); //Calcula I y err
    void step(unsigned int nstep); //Genera el vector aleatorio X de longitud nstep

}MCintegrate;

//Definicion de las funciones miembro
MCintegrate::MCintegrate(vector<double> xlow,vector<double> xhigh,double funcs(vector<double>),bool inregiop(vector<double>)){
    ndim= static_cast<unsigned int>(xlow.size());
    xlo=xlow;
    xhi=xhigh;
    func=funcs;
    inregion=inregiop;
    double aux=1.0;
    for (unsigned int i = 0; i< xlow.size(); i++)
        aux=aux*(xlow.at(i)-xhigh.at(i));
    vol=fabs(aux);
}

void MCintegrate::integrate(){
    double aux=0.0;
    double aux1=0.0;
    for(unsigned int i=0;i<n;i++){
        if(inregion(X.at(i))){
           aux+=func(X.at(i));
           aux1+=func(X.at(i))*func(X.at(i));
        }
    }
    aux=aux/n; aux1=aux1/n;
    I=vol*aux;
    err=vol*sqrt((aux1-aux*aux)/n);
}
void MCintegrate::step(unsigned int nstep){
    n=nstep;
    default_random_engine generator;
    for(unsigned int i=0;i<nstep;i++){
        vector<double> aux;
        for(unsigned int j=0;j<ndim;j++){
            uniform_real_distribution<double> distribution(xlo.at(j),xhi.at(j));
            aux.push_back(distribution(generator));
        }
        X.push_back(aux);
    }
}
//Para el ejemplo definimos las funciones a integrar y la region del toro
//Integraremos la función 1
double den(vector<double> x){
    return 1.0;
}
//La region de la esfera unitaria es la siguiente
bool sphereregion(vector<double> x){
    double aux=sqrt(x[0]*x[0]+x[1]*x[1]+x[2]*x[2]);
    return (aux<=1.0);
}
//La región de el toro es la siguiente
bool torusregion(vector <double> x){
    double aux=pow(sqrt(x[0]*x[0]+x[1]*x[1])-2,2)+x[2]*x[2];
    return (aux<=1.0);
}
//----------------------------------------------------------------------------
//Funciones a integrar
double func1(vector<double> x){
    return sqrt(1.0-x[0]*x[0]);
}
double func2(vector<double> x){
    return 1.0/x[0];
}
double func3(vector<double> x){
    return tan(x[0]);
}
//Intervalos de integracion
bool reg1(vector<double> x){
    return(-1.0<= x[0] && x[0]<=1.0);
}
bool reg2(vector<double> x){
    return (1.0<= x[0] && x[0]<=M_E);
}
bool reg3(vector<double>x){
    return (0.0<= x[0] && x[0]<=M_PI/4.0);
}
int main()
{
    //Esfera unitaria S^2
    vector<double>xlow1,xhigh1;
    for(unsigned int i=0;i<3;i++){
        xlow1.push_back(-1.0);
        xhigh1.push_back(1.0);
    }
    MCintegrate mymc1(xlow1,xhigh1,den,sphereregion);
    mymc1.step(1000000);
    mymc1.integrate();
    cout<<"El volumen de la esfera unitaria es: "<<mymc1.I<<endl;
    cout<<"El error es: "<<mymc1.err<<endl;
    //Toro
    vector<double>xlow2,xhigh2;
    for(unsigned int i=0;i<3;i++){
        xlow2.push_back(-3.0);
        xhigh2.push_back(3.0);
    }
    MCintegrate mymc2(xlow2,xhigh2,den,torusregion);
    mymc2.step(1000000);
    mymc2.integrate();
    cout<<"El volumen del toro de radio mayor 2 y radio menor 1 es: "<<mymc2.I<<endl;
    cout<<"El error es: "<<mymc2.err<<endl;

    //Integrar funcione de una variable para comparacion
    vector<double>a,b;
    a.push_back(-2.0); b.push_back(4.0);
    MCintegrate f1(a,b,func1,reg1);
    f1.step(1000000);
    f1.integrate();
    MCintegrate f2(a,b,func2,reg2);
    f2.step(1000000);
    f2.integrate();
    MCintegrate f3(a,b,func3,reg3);
    f3.step(1000000);
    f3.integrate();
    cout<<"Al estimar las integrales de una variable los resultados fueron: "<<endl;
    cout<<"El valor de pi/2 es: "<<f1.I<<endl;
    cout<<"El error en la estimacion es: "<<fabs(f1.I-M_PI/2.0)<<endl;
    cout<<"El valor de ln(e) es: "<<f2.I<<endl;
    cout<<"El error en la estimacion es: "<<fabs(f2.I-1.0)<<endl;
    cout<<"El valor de ln(2)/2 es: "<<f3.I<<endl;
    cout<<"El error en la estimacion fue de: "<<fabs(f3.I-log(2.0)/2.0)<<endl;
    return 0;
}
