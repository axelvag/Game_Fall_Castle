// apps/LIFAMI/main_ProjetFinal.cpp

//Le projet FALL CASTEL est inspire d'Angry bird
//Il y aura une page d'acceuil
//Une page de selection de niveau
//Puis soit le niveau 1 soit le niveau Boss
//On lancera un dragon qui devra toucher les chevaliers derriere les murs pour gagner (les murs auront des collisions avec le dragon)
//image a mettre dans data/lifami_ProjetFinal/fond1.jpg  :  https://thumbs.dreamstime.com/b/paysage-de-fond-de-jeu-de-champs-50038340.jpg

#include<Grapic.h>
#include <math.h>
#include <iostream>

using namespace grapic;
using namespace std;

const int DIMW=1000;
const int DIMH=700;
const int MAX=100;

struct Vec2
{
    float x,y;
};

float toRad(float a)
{
    return a*M_PI/180.0;
}

float toDeg(float a)
{
    return a*180.0/M_PI;
}

Vec2 make_Vec2(float x,float y)
{
    Vec2 z;
    z.x=x;
    z.y=y;
    return z;
}

Vec2 make_Vec2_exp(float r,float th)
{
    Vec2 z;
    z.x=r*cos(toRad(th));
    z.y=r*sin(toRad(th));
    return z;
}

Vec2 operator+(Vec2 opg,Vec2 opd)
{
    Vec2 res;
    res.x=opg.x+opd.x;
    res.y=opg.y+opd.y;
    return res;
}

Vec2 operator-(Vec2 opg,Vec2 opd)
{
    Vec2 res;
    res.x=opg.x-opd.x;
    res.y=opg.y-opd.y;
    return res;
}

Vec2 operator*(float opg, Vec2 opd)
{
    return make_Vec2(opg*opd.x,opg*opd.y);
}

Vec2 operator*(Vec2 opg, float opd)
{
    return opd*opg;
}

Vec2 operator/(Vec2 opg, float opd)
{
    return opg * (1.0 / opd);
}

Vec2 Scale(Vec2 p,float cx,float cy,float lambda)
{
    Vec2 res;
    Vec2 reff=make_Vec2(cx,cy);
    res=lambda*(p-reff+reff);
    return res;
}

Vec2 operator*(Vec2 opg,Vec2 opd)
{
    Vec2 res;
    res.x=opg.x*opd.x-opg.y*opd.y;
    res.y=opg.x*opd.y+opg.y*opd.x;
    return res;
}

struct Particule
{
    Vec2 sommeForces;
    Vec2 acceleration;
    Vec2 vitesse;
    Vec2 position;
    float masse;
    Image dragon;
};
//=======================================================================================
struct Spring
{
    float l;
    float k;
    unsigned int P1;
    unsigned int P2;
};

struct World
{
    int nbp;
    Particule tab[MAX];
    int nbr;
    Spring tabr[MAX];
};
//==============================================================
struct Mur
{
    Vec2 positionmin;
    Vec2 positionmax;
    Image brique;
};

struct Chateau
{
    Mur tabm[20];
    Image fond;
    int nbMur=6;
};
//================================================================================
struct Lettre
{
    Image titre;
};
struct Titre   //FALL CASTLE
{
    Lettre tabm[20];
};
//=======================================================================================
void partInit(Particule & part,Vec2 pos,Vec2 vit,float m)
{
    part.sommeForces=make_Vec2(0,0);
    part.acceleration=make_Vec2(0,0);
    part.position=pos;
    part.vitesse=vit;
    part.masse=m;
    //part.dragon=image("../data/lifami_ProjetFinal/dragon.png");
}

void partAddForce(Particule & part,Vec2 force)
{
    part.sommeForces=part.sommeForces+force;
}

void partUpdatePV(Particule & part,float dtlance)           //Update des particules et colisions avec la fen�tre
{
    float FRICTION=0.6;
    part.acceleration=part.sommeForces/part.masse;
    part.vitesse=part.vitesse+part.acceleration*dtlance;
    part.position=part.position+part.vitesse*dtlance;
    part.sommeForces=make_Vec2(0,0);
    if(part.position.x<0 && part.position.x > -75)
    {
        part.position.x=part.position.x*(-1);
        part.vitesse.x=part.vitesse.x*(-1);
        part.vitesse=FRICTION*part.vitesse;
    }
    if(part.position.y<0)
    {
        part.position.y=part.position.y*(-1);
        part.vitesse.y=part.vitesse.y*(-1);
        part.vitesse=FRICTION*part.vitesse;
    }
    if(part.position.x>DIMW)
    {
        part.position.x=DIMW-(part.position.x-DIMW);
        part.vitesse.x=part.vitesse.x*(-1);
        part.vitesse=FRICTION*part.vitesse;
    }
    if(part.position.y>DIMH)
    {
        part.position.y=DIMH-(part.position.y-DIMH);
        part.vitesse.y=part.vitesse.y*(-1);
        part.vitesse=FRICTION*part.vitesse;
    }
}

void updateMur(Particule & P,Chateau & C,float dtlance)     //collisions avec murs
{

    for(int i=0;i<C.nbMur;i++)
    {
        if((P.position.x>=C.tabm[i].positionmin.x)&&(P.position.x<=C.tabm[i].positionmax.x)
        &&(P.position.y>=C.tabm[i].positionmin.y)&&(P.position.y<=C.tabm[i].positionmax.y))
        {
            if(((P.position.x>=C.tabm[i].positionmin.x)&&(P.position.x<=C.tabm[i].positionmin.x+5))||
               ((P.position.x<=C.tabm[i].positionmax.x)&&(P.position.x>=C.tabm[i].positionmax.x-5)))
            {
                P.vitesse.x=P.vitesse.x*(-1);
                //P.vitesse=FRICTION*P.vitesse;
            }
            if(((P.position.y>=C.tabm[i].positionmin.y)&&(P.position.y<=C.tabm[i].positionmin.y+5))||
               ((P.position.y<=C.tabm[i].positionmax.y)&&(P.position.y>=C.tabm[i].positionmax.y-5)))
            {
                P.vitesse.y=P.vitesse.y*(-1);
            }
        }
    }
}

void partDraw(Particule & P)                        //Particule rouge du dragon
{
    color(245,50,50);
    circleFill(P.position.x,P.position.y,10);
}

void partDrawDragon(Particule & P,float deg,int direction)                        //DRAGON
{
    if (direction==0)
    {
        image_draw(P.dragon,P.position.x-20,P.position.y-20,40,40,-deg);
    }
    else
    {
    image_draw(P.dragon,P.position.x-20,P.position.y-20,40,40,-deg,SDL_FLIP_VERTICAL);   //SDL_FLIP_VERTICAL | SDL_FLIP_HORIZONTAL
    }
}

void partDrawEnnemiZone(Particule & ennemi)          //affiche le centre de la hitbox des ennemis
{
    color(0,0,0);
    circleFill(ennemi.position.x,ennemi.position.y,3);
}

float calculdeg(Particule & P)    //calcule le degr� pour faire tourner le dragon
{
    return toDeg(atan2(P.vitesse.y,P.vitesse.x));
}

//================================================================================================================
void updatePartPV(Particule & P,float dt)           //gravit�
{
    if(P.masse>0)
    {
        P.acceleration=P.sommeForces/P.masse;
        P.vitesse=P.vitesse+P.acceleration*dt;
        P.position=P.position+P.vitesse*dt;
    }
    P.sommeForces=make_Vec2(0,0);
}

float distance(Vec2 A,Vec2 B)
{
    Vec2 d=B-A;
    float d2=d.x*d.x+d.y*d.y;
    return sqrtf(d2);
}

void initRessort(Spring &r,float k,unsigned int P1,unsigned int P2,World W)
{
    r.l=distance(W.tab[P1].position,W.tab[P2].position);
    r.k=k;
    r.P1=P1;
    r.P2=P2;
}

void initWorld(World &W)                    //position cube ennemi 1
{
    W.nbp=4;
    partInit(W.tab[0],make_Vec2(780,200),make_Vec2(0,0),1);
    partInit(W.tab[1],make_Vec2(870,200),make_Vec2(0,0),1);
    partInit(W.tab[2],make_Vec2(780,270),make_Vec2(0,0),1);
    partInit(W.tab[3],make_Vec2(870,270),make_Vec2(0,0),1);
    W.nbr=6;
    initRessort(W.tabr[0],3,0,1,W);
    initRessort(W.tabr[1],3,1,2,W);
    initRessort(W.tabr[2],3,2,3,W);
    initRessort(W.tabr[3],3,3,0,W);
    initRessort(W.tabr[4],3,0,2,W);
    initRessort(W.tabr[5],3,1,3,W);
    for (int i;i<2;i++)
    {
        partDrawEnnemiZone(W.tab[i]);
    }
}

void initWorld2(World &W2)                    //position triangle ennemi 2
{
    W2.nbp=4;
    partInit(W2.tab[0],make_Vec2(440,500),make_Vec2(0,0),1);
    partInit(W2.tab[1],make_Vec2(445,500),make_Vec2(0,0),1);
    partInit(W2.tab[2],make_Vec2(450,500),make_Vec2(0,0),1);
    partInit(W2.tab[3],make_Vec2(445,550),make_Vec2(0,0),1);
    W2.nbr=5;
    initRessort(W2.tabr[0],3,0,1,W2);
    initRessort(W2.tabr[1],3,1,2,W2);
    initRessort(W2.tabr[2],3,2,3,W2);
    initRessort(W2.tabr[3],3,3,0,W2);
    initRessort(W2.tabr[4],3,1,3,W2);
    for (int i;i<2;i++)
    {
        partDrawEnnemiZone(W2.tab[i]);
    }
}

void initWorld3(World &W3)                    //position triangle ennemi 2
{
    W3.nbp=6;
    partInit(W3.tab[0],make_Vec2(810,400),make_Vec2(0,0),1);
    partInit(W3.tab[1],make_Vec2(830,390),make_Vec2(0,0),1);
    partInit(W3.tab[2],make_Vec2(850,400),make_Vec2(0,0),1);
    partInit(W3.tab[3],make_Vec2(850,420),make_Vec2(0,0),1);
    partInit(W3.tab[4],make_Vec2(830,440),make_Vec2(0,0),1);
    partInit(W3.tab[5],make_Vec2(810,420),make_Vec2(0,0),1);
    W3.nbr=9;
    initRessort(W3.tabr[0],30,0,1,W3);
    initRessort(W3.tabr[1],30,1,2,W3);
    initRessort(W3.tabr[2],30,2,3,W3);
    initRessort(W3.tabr[3],30,3,4,W3);
    initRessort(W3.tabr[4],30,4,5,W3);
    initRessort(W3.tabr[5],30,5,0,W3);
    initRessort(W3.tabr[6],30,1,4,W3);
    initRessort(W3.tabr[7],150,0,3,W3);
    initRessort(W3.tabr[8],150,2,5,W3);
    for (int i;i<2;i++)
    {
        partDrawEnnemiZone(W3.tab[i]);
    }
}

Vec2 normaliser(Vec2 v)
{
    float n=sqrtf(v.x*v.x+v.y*v.y);
    return v*(1.0/n);
}

void update(World &W,float dt)             //update des enemis
{
    float G=9.81;
    int i;
    for(i=0;i<W.nbp;i++)
    {
        partAddForce(W.tab[i],G*W.tab[i].masse*make_Vec2(0,-1));
    }
    for(i=0;i<W.nbr;i++)
    {
        Vec2 P1=W.tab[W.tabr[i].P1].position;
        Vec2 P2=W.tab[W.tabr[i].P2].position;

        float l=distance(P1,P2);
        Vec2 lr=normaliser(P2-P1);
        float dl=l-W.tabr[i].l;
        partAddForce(W.tab[W.tabr[i].P1],    lr*dl*W.tabr[i].k);
        partAddForce(W.tab[W.tabr[i].P2],-1* lr*dl*W.tabr[i].k);
    }
    for(i=0;i<W.nbp;i++)
    {
        partUpdatePV(W.tab[i],dt);
    }
}

void draw(World W)          //draw les ennemis avec nbp leur particules et nbr leur ressorts
{
    int i;
    for(i=0;i<W.nbp;i++)
    {
        color(0,0,0);
        circleFill(W.tab[i].position.x,W.tab[i].position.y,3);
        Vec2 v=normaliser(W.tab[i].vitesse)*10;
        color(0,0,0);
        //line(W.tab[i].position.x,W.tab[i].position.y,W.tab[i].position.x+v.x,W.tab[i].position.y+v.y);  //vecteurs de forces
    }
    for(i=0;i<W.nbr;i++)
    {

        Vec2 P1=W.tab[W.tabr[i].P1].position;
        Vec2 P2=W.tab[W.tabr[i].P2].position;
        color(0,0,0);
        line(P1.x,P1.y,P2.x,P2.y);
        color(0,0,0);

        float l=distance(P1,P2);
        float dl=W.tabr[i].l-l;
        //line(P1.x,P1.y,P1.x+dl*normaliser(P2-P1).x,P1.y+dl*normaliser(P2-P1).y);   //vecteurs de forces
    }
}

//=========================================================DRAGON=======================================

void initChateau(Chateau & C)              //MURS
{
    C.tabm[0].positionmin.x=50; //pilier
    C.tabm[0].positionmin.y=0;
    C.tabm[0].positionmax.x=100;
    C.tabm[0].positionmax.y=290;
    C.tabm[1].positionmin.x=350;  //murs
    C.tabm[1].positionmin.y=50;
    C.tabm[1].positionmax.x=400;
    C.tabm[1].positionmax.y=500;
    C.tabm[2].positionmin.x=400;  //balecon
    C.tabm[2].positionmin.y=350;
    C.tabm[2].positionmax.x=500;
    C.tabm[2].positionmax.y=370;
    C.tabm[3].positionmin.x=640;  //plaquevolante
    C.tabm[3].positionmin.y=300;
    C.tabm[3].positionmax.x=900;
    C.tabm[3].positionmax.y=320;
    C.tabm[4].positionmin.x=650;  //minimur
    C.tabm[4].positionmin.y=0;
    C.tabm[4].positionmax.x=670;
    C.tabm[4].positionmax.y=120;
    C.tabm[5].positionmin.x=780;  //mursurplaquevolante
    C.tabm[5].positionmin.y=320;
    C.tabm[5].positionmax.x=790;
    C.tabm[5].positionmax.y=400;
}

void initDragon(Particule & P)   //balle rouge
{
    partInit(P,make_Vec2(100,300),make_Vec2(0,0),10);
    partDrawDragon(P,0,0);
}

/*void initEnnemiZone(Particule & E)    //ancienne zone de collision ennemi 1 cube
{
    partInit(E,make_Vec2(820,30),make_Vec2(0,0),10);
    //partDrawEnnemiZone(E);         //affiche centre zone
}
void initEnnemiZone2(Particule & E2)    //ancienne zone de collision ennemi 2 triangle
{
    partInit(E2,make_Vec2(445,500),make_Vec2(0,0),10);
    //partDrawEnnemiZone(E2);       //affiche centre zone
}
void initEnnemiZone3(Particule & E3)    //ancienne zone de collision ennemi 3 boule
{
    partInit(E3,make_Vec2(800,400),make_Vec2(0,0),10);
    //partDrawEnnemiZone(E3);       //affiche centre zone
}*/

void initEnnemi(World & W,float dt)  //cube ressort
{
    update(W,dt);
    draw(W);
}



void initMonde1(Particule & P, World & W,World & W2,World & W3, Chateau & C,float dt,int & i)  //init tout les perso
{
    initDragon(P);
    initChateau(C);
    if(i==0)
    {
        initWorld(W);  //place ennemi 1
        initEnnemi(W,dt);
        initWorld2(W2);  //place ennemi 2
        initEnnemi(W2,dt);
        initWorld3(W3);  //place ennemi 3
        initEnnemi(W3,dt);
        i=1;
    }
    update(W,dt);
    draw(W);
    update(W2,dt);
    draw(W2);
    update(W3,dt);
    draw(W3);
}

void initLevel(Chateau & C,Particule & P)                      //init les images/textures
{
    P.dragon=image("../data/lifami_ProjetFinal/dragon1.png");
    C.fond=image("../data/lifami_ProjetFinal/fond1.jpg");
    C.tabm[0].brique=image("../data/lifami_ProjetFinal/tour.jpg");  //tour du dragon
    C.tabm[1].brique=image("../data/lifami_ProjetFinal/brique1.jpg");  //grand murs
    C.tabm[2].brique=image("../data/lifami_ProjetFinal/brique2.jpg");  //balecon
    C.tabm[3].brique=image("../data/lifami_ProjetFinal/brique3.jpg");  //plaque volante
    C.tabm[4].brique=image("../data/lifami_ProjetFinal/brique4.jpg");  //petit mur
    C.tabm[5].brique=image("../data/lifami_ProjetFinal/brique4.jpg");  //petit mur sur plaque volante
}

void drawLevel(Chateau C)                       //affiche fond , mur et tour
{
    image_draw(C.fond,0,0,DIMW,DIMH);
    for(int i=0;i<C.nbMur;i++)                              //dessine murs
    {
        /*color(150,150,150);
        rectangleFill(C.tabm[i].positionmin.x,C.tabm[i].positionmin.y,C.tabm[i].positionmax.x,C.tabm[i].positionmax.y);*/
        image_draw(C.tabm[i].brique,C.tabm[i].positionmin.x,C.tabm[i].positionmin.y,C.tabm[i].positionmax.x-C.tabm[i].positionmin.x,C.tabm[i].positionmax.y-C.tabm[i].positionmin.y);
    }
}

void drawAll(Particule & P, World & W,World & W2,World & W3,float dt)   //dessine dragon + ennemi
{
    int direction=0;
    float deg=calculdeg(P);       //direction t�te dragon
    if((deg>=90)||(deg<=-90))
    {
        direction=1;
        partDrawDragon(P,deg,direction);
    }
    else
    {
        direction=0;
        partDrawDragon(P,deg,direction);
    }
    update(W,dt);
    draw(W);
    update(W2,dt);
    draw(W2);
    update(W3,dt);
    draw(W3);
}

void supprime(World & W)               //supprime les ennemis en les mettant � gauche de la fen�tre
{
    W.tab[0].position.x=-100;
    W.tab[1].position.x=-100;
    W.tab[2].position.x=-100;
    W.tab[3].position.x=-100;
    W.tab[4].position.x=-100;
    W.tab[5].position.x=-100;


    W.tab[0].vitesse.x=0;
    W.tab[1].vitesse.x=0;
    W.tab[2].vitesse.x=0;
    W.tab[3].vitesse.x=0;
    W.tab[4].vitesse.x=0;
    W.tab[5].vitesse.x=0;

    W.tab[0].vitesse.y=0;
    W.tab[1].vitesse.y=0;
    W.tab[2].vitesse.y=0;
    W.tab[3].vitesse.y=0;
    W.tab[4].vitesse.y=0;
    W.tab[5].vitesse.y=0;

}

void calculZone(World & W,World & W2,World & W3)    //calcule de la hitbox des ennemis
{
    W.tab[10].position.x=W.tab[0].position.x;   //x min
    W.tab[11].position.x=W.tab[0].position.x;   //x max
    W.tab[12].position.y=W.tab[0].position.y;   //y min
    W.tab[13].position.y=W.tab[0].position.y;   //y max
    for(int i=1;i<W.nbp;i++)
    {
        if(W.tab[10].position.x>W.tab[i].position.x)
        {
            W.tab[10].position.x=W.tab[i].position.x;  //new x min
        }
        if(W.tab[11].position.x<W.tab[i].position.x)
        {
            W.tab[11].position.x=W.tab[i].position.x;  //new x max
        }
        if(W.tab[12].position.y>W.tab[i].position.y)
        {
            W.tab[12].position.y=W.tab[i].position.y;  //new y min
        }
        if(W.tab[13].position.y<W.tab[i].position.y)
        {
            W.tab[13].position.y=W.tab[i].position.y;  //new y max
        }
    }

    W2.tab[10].position.x=W2.tab[0].position.x;   //x min
    W2.tab[11].position.x=W2.tab[0].position.x;   //x max
    W2.tab[12].position.y=W2.tab[0].position.y;   //y min
    W2.tab[13].position.y=W2.tab[0].position.y;   //y max
    for(int i=1;i<W2.nbp;i++)
    {
        if(W2.tab[10].position.x>W2.tab[i].position.x)
        {
            W2.tab[10].position.x=W2.tab[i].position.x;  //new x min
        }
        if(W2.tab[11].position.x<W2.tab[i].position.x)
        {
            W2.tab[11].position.x=W2.tab[i].position.x;  //new x max
        }
        if(W2.tab[12].position.y>W2.tab[i].position.y)
        {
            W2.tab[12].position.y=W2.tab[i].position.y;  //new y min
        }
        if(W2.tab[13].position.y<W2.tab[i].position.y)
        {
            W2.tab[13].position.y=W2.tab[i].position.y;  //new y max
        }
    }

    W3.tab[10].position.x=W3.tab[0].position.x;   //x min
    W3.tab[11].position.x=W3.tab[0].position.x;   //x max
    W3.tab[12].position.y=W3.tab[0].position.y;   //y min
    W3.tab[13].position.y=W3.tab[0].position.y;   //y max
    for(int i=1;i<W3.nbp;i++)
    {
        if(W3.tab[10].position.x>W3.tab[i].position.x)
        {
            W3.tab[10].position.x=W3.tab[i].position.x;  //new x min
        }
        if(W3.tab[11].position.x<W3.tab[i].position.x)
        {
            W3.tab[11].position.x=W3.tab[i].position.x;  //new x max
        }
        if(W3.tab[12].position.y>W3.tab[i].position.y)
        {
            W3.tab[12].position.y=W3.tab[i].position.y;  //new y min
        }
        if(W3.tab[13].position.y<W3.tab[i].position.y)
        {
            W3.tab[13].position.y=W3.tab[i].position.y;  //new y max
        }
    }
}

void collision(Particule& P, World & W, World & W2,World & W3, Chateau & C,float dt, int & nbennemi)            //si touche ennemi
{
    calculZone(W,W2,W3);
    if(((P.position.x<=W.tab[11].position.x)&&(P.position.x>=W.tab[10].position.x))&&      //zone de contact avec ennemi
       ((P.position.y<=W.tab[13].position.y)&&(P.position.y>=W.tab[12].position.y)))
    {
        supprime(W);
        nbennemi -= 1;
    }
    if(((P.position.x<=W2.tab[11].position.x)&&(P.position.x>=W2.tab[10].position.x))&&      //zone de contact avec ennemi 2
       ((P.position.y<=W2.tab[13].position.y)&&(P.position.y>=W2.tab[12].position.y)))
    {
        supprime(W2);
        nbennemi -= 1;
    }
    if(((P.position.x<=W3.tab[11].position.x)&&(P.position.x>=W3.tab[10].position.x))&&      //zone de contact avec ennemi 3
       ((P.position.y<=W3.tab[13].position.y)&&(P.position.y>=W3.tab[12].position.y)))
    {
        supprime(W3);
        nbennemi -= 1;
    }
    for(int i=0;i<W.nbp;i++)
    {
        updateMur(W.tab[i],C,dt);
    }
    for(int i=0;i<W2.nbp;i++)
    {
        updateMur(W2.tab[i],C,dt);
    }
    for(int i=0;i<W3.nbp;i++)
    {
        updateMur(W3.tab[i],C,dt);
    }
    updateMur(P,C,dt);
}

void Fin(int nbcoup,int & test)   //draw si victoire
{
    fontSize(100);
    print(300,500,"GAGNER");
    fontSize(20);
    print(480,480,"Lancer");
    print(450,480,nbcoup);
    /*if(test==0)
    {
        Image image_victoire=image("../data/lifami_ProjetFinal/dragon-victoire.png");
        test++;
    }*/
    Image image_victoire=image("../data/lifami_ProjetFinal/dragon-victoire.png");
    image_draw(image_victoire,350,180,300,300);
}

void score(int nbcoup,Titre T,int nbennemi,int & test)         //score en haut � droite,   Titre   et  fin
{
    fontSize(20);
    print(850,650,"LANCER :");
    fontSize(32);
    print(950,645,nbcoup);
    rectangle(840,650,990,680);

    //logo FALL CASTLE
    image_draw(T.tabm[0].titre,250,650,50,50);  //F
    image_draw(T.tabm[1].titre,300,650,50,50);  //A
    image_draw(T.tabm[2].titre,350,650,50,50);  //L
    image_draw(T.tabm[2].titre,400,650,50,50);  //L
    image_draw(T.tabm[3].titre,500,650,50,50);  //C
    image_draw(T.tabm[1].titre,550,650,50,50);  //A
    image_draw(T.tabm[4].titre,600,650,50,50);  //S
    image_draw(T.tabm[5].titre,650,650,50,50);  //T
    image_draw(T.tabm[2].titre,700,650,50,50);  //L
    image_draw(T.tabm[6].titre,750,650,50,50);  //E

    //Fin gagner
    if (nbennemi<=0)
    {
        Fin(nbcoup,test);
    }
}

void initTitre(Titre & T)                 //init les lettres pour le titre en haut
{
       T.tabm[0].titre=image("../data/lifami_ProjetFinal/F.png");  //F
       T.tabm[1].titre=image("../data/lifami_ProjetFinal/A.png");  //A
       T.tabm[2].titre=image("../data/lifami_ProjetFinal/L.png");  //L
       T.tabm[3].titre=image("../data/lifami_ProjetFinal/C.png");  //C
       T.tabm[4].titre=image("../data/lifami_ProjetFinal/S.png");  //S
       T.tabm[5].titre=image("../data/lifami_ProjetFinal/T.png");  //T
       T.tabm[6].titre=image("../data/lifami_ProjetFinal/E.png");  //E
}




//=====================================================
int main(int argc, char**argv)
{
    int x,y;
    int i=0,test=0;
    int nbennemi = 3 , nbcoup = 0;
    Menu men;
    menu_add(men,"PRET");
    menu_add(men,"RELANCE");
    menu_add(men,"LANCE");

    float dtlance=0.01;   //temps pour balle
    float dt=0.025;    //temps pour ressort
    bool stop=false;
    winInit("Fall Castle",DIMW,DIMH);
    backgroundColor(220,220,220);
    Particule P;
    //Particule E; Particule E2; Particule E3;   anciennes zones de colisions
    partAddForce(P,make_Vec2(0,0));

    Titre T;
    Chateau C;
    World W;   //ennemi cube
    World W2;  //ennemi triangle
    World W3;  //ennemi rond

    initLevel(C,P);
    initTitre(T);

    while(!stop)
    {
        winClear();
        drawLevel(C);
        menu_draw(men,5,600,70,699);
        partAddForce(P,make_Vec2(0.0,-9.81*P.masse));
        partUpdatePV(P,dtlance);

        switch(menu_select(men))
        {
            case 0 :  initMonde1(P,W,W2,W3,C,dt,i);  collision(P,W,W2,W3,C,dt,nbennemi); score(nbcoup,T,nbennemi,test);        //init
            if (isMousePressed(SDL_BUTTON_LEFT))   //prend le clique pour la direction
            {

                mousePos(x,y);
                nbcoup += 1;
                P.vitesse = make_Vec2(x-P.position.x,y-P.position.y);     //vitesse selon distance entre dragon et souris
                menu_setSelect(men,2);  //lance
            }
            break;
            case 1 : partInit(P,make_Vec2(100,300),make_Vec2(0,0),10); score(nbcoup,T,nbennemi,test); break;                    //stop
            case 2 : drawAll(P,W,W2,W3,dt); collision(P,W,W2,W3,C,dt,nbennemi); score(nbcoup,T,nbennemi,test); break;           //lance
            default: partDrawDragon(P,0,0); break;
        }
        if (menu_select(men)==1)
        {
            partDrawDragon(P,0,0);
            menu_setSelect(men,0);
        }

        stop=winDisplay();
    }
winQuit();
return 0;
}
