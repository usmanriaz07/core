#include <gmi_mesh.h>
#include <gmi_null.h>
#include <apfMDS.h>
#include <apfMesh2.h>
#include <apfConvert.h>
#include <apfConvertTags.h>
#include <apf.h>
#include <PCU.h>
#include <lionPrint.h>
#include <pcu_util.h>
#include <cstdlib>
#include <string.h>
#include <cassert>
#include <algorithm>
#include <apfBox.h>

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <regex>

/* from https://github.com/SCOREC/core/issues/205
0=fully interior of the volume
1-6 =classified on face (not edge or vertex)
11-22 = classified on model edge (not end points which are model vertices)
31-38 = classified on a model vertex.
*/

/* tags on vertices */
#define INTERIORTAG  0
#define FACE 1
#define FACE_LAST 6
#define EDGE 11
#define EDGE_LAST 22
#define VERTEX 31
#define VERTEX_LAST 38

/* model entity ids */
#define INTERIOR_REGION 0

apf::ModelEntity* getMdlRgn(gmi_model* model) {
  apf::ModelEntity* rgn = reinterpret_cast<apf::ModelEntity*>(
      gmi_find(model, 3, INTERIOR_REGION));
  PCU_ALWAYS_ASSERT(rgn);
  return rgn;
}

apf::ModelEntity* getMdlFace(apf::Mesh2* mesh, int tag) {
  apf::ModelEntity* face = mesh->findModelEntity(2,tag);
  PCU_ALWAYS_ASSERT(face);
  return face;
}

apf::ModelEntity* getMdlEdge(apf::Mesh2* mesh, int tag) {
  apf::ModelEntity* edge = mesh->findModelEntity(1,tag);
  PCU_ALWAYS_ASSERT(edge);
  return edge;
}

apf::ModelEntity* getMdlVtx(apf::Mesh2* mesh, int tag) {
  apf::ModelEntity* vertex = mesh->findModelEntity(0,tag);
  PCU_ALWAYS_ASSERT(vertex);
  return vertex;
}

void setVtxClassification(gmi_model* model, apf::Mesh2* mesh, apf::MeshTag* vtxClass) {
  (void)model;
  (void)mesh;
  (void)vtxClass;
  apf::MeshIterator* it = mesh->begin(0);
  apf::MeshEntity* v;
  //apf::Vector3 vCoord;
  int c;
  //int count=0,cint=0,cface=0,cedge=0,cvtx=0;
  while( (v = mesh->iterate(it)) ) {
    //mesh->getPoint(v, 0, vCoord);
    //std::cout<<"Coordinates: "<<vCoord[0]<<" "<<vCoord[1]<<" "<<vCoord[2]<<std::endl;
    mesh->getIntTag(v,vtxClass,&c);
    //std::cout<<"Returned tag is c= "<<c<<std::endl;
    //std::cout<<" "<<std::endl;
    //count++;
    if (c == INTERIORTAG) {
       mesh->setModelEntity(v,getMdlRgn(model));
       //cint++;
    } else if (c >= FACE && c <= FACE_LAST) {
       if (c == 1) { //face tag 1 corresponds to model face 0
          mesh->setModelEntity(v,getMdlFace(mesh,0));
       } else if (c == 2) { //face tag 2 corresponds to model face 1
          mesh->setModelEntity(v,getMdlFace(mesh,1));
       } else if (c == 3) { //face tag 3 corresponds to model face 3
          mesh->setModelEntity(v,getMdlFace(mesh,3));
       } else if (c == 4) { //face tag 4 corresponds to model face 4
          mesh->setModelEntity(v,getMdlFace(mesh,4));
       } else if (c == 5) { //face tag 5 corresponds to model face 2
          mesh->setModelEntity(v,getMdlFace(mesh,2));
       } else if (c == 6) { //face tag 6 corresponds to model face 5
          mesh->setModelEntity(v,getMdlFace(mesh,5));
       }
       //cface++;
    } else if (c >= EDGE && c <= EDGE_LAST) {
       if (c == 11) { //edge tag 11 corresponds to model edge 0
          mesh->setModelEntity(v,getMdlEdge(mesh,0));
       } else if (c == 12) {
          mesh->setModelEntity(v,getMdlEdge(mesh,2));
       } else if (c == 13) {
          mesh->setModelEntity(v,getMdlEdge(mesh,3));
       } else if (c == 14) {
          mesh->setModelEntity(v,getMdlEdge(mesh,1));
       } else if (c == 15) {
          mesh->setModelEntity(v,getMdlEdge(mesh,4));
       } else if (c == 16) {
          mesh->setModelEntity(v,getMdlEdge(mesh,5));
       } else if (c == 17) {
          mesh->setModelEntity(v,getMdlEdge(mesh,7));
       } else if (c == 18) {
          mesh->setModelEntity(v,getMdlEdge(mesh,6));
       } else if (c == 19) {
          mesh->setModelEntity(v,getMdlEdge(mesh,8));
       } else if (c == 20) {
          mesh->setModelEntity(v,getMdlEdge(mesh,10));
       } else if (c == 21) {
          mesh->setModelEntity(v,getMdlEdge(mesh,11));
       } else if (c == 22) {
          mesh->setModelEntity(v,getMdlEdge(mesh,9));
       }
       //cedge++;
    } else if (c >= VERTEX && c <= VERTEX_LAST) {
       if (c == 31) { //vertex tag 31 corresponds to model vertex 0
          mesh->setModelEntity(v,getMdlVtx(mesh,0));
       } else if (c == 32) { //vertex tag 32 corresponds to model vertex 1
          mesh->setModelEntity(v,getMdlVtx(mesh,1));
       } else if (c == 33) { //vertex tag 33 corresponds to model vertex 3
          mesh->setModelEntity(v,getMdlVtx(mesh,3));
       } else if (c == 34) { //vertex tag 34 corresponds to model vertex 2
          mesh->setModelEntity(v,getMdlVtx(mesh,2));
       } else if (c == 35) { //vertex tag 35 corresponds to model vertex 4
          mesh->setModelEntity(v,getMdlVtx(mesh,4));
       } else if (c == 36) { //vertex tag 36 corresponds to model vertex 5
          mesh->setModelEntity(v,getMdlVtx(mesh,5));
       } else if (c == 37) { //vertex tag 37 corresponds to model vertex 7
          mesh->setModelEntity(v,getMdlVtx(mesh,7));
       } else if (c == 38) { //vertex tag 38 corresponds to model vertex 6
          mesh->setModelEntity(v,getMdlVtx(mesh,6));
       }
       //cvtx++;
    }
  }
  //std::cout<<"count is "<<cvtx<<std::endl;
  mesh->end(it);
}

void setEdgeClassification(gmi_model* model, apf::Mesh2* mesh,apf::MeshTag* vtxClass) {
  (void)model;
  (void)mesh;
  (void)vtxClass;
  apf::MeshIterator* it = mesh->begin(1);
  apf::MeshEntity* e;
  //apf::Vector3 vCoord;
  int c;
  //int count=0;
  apf::Adjacent verts;
  while( (e = mesh->iterate(it)) ) {
    //std::cout<<"Edge number "<<count++<<" with nodes"<<std::endl;
    mesh->getAdjacent(e, 0, verts);
    int cmin=100;
    for(size_t i=0; i<verts.size(); i++) {
      mesh->getIntTag(verts[i],vtxClass,&c);
      //mesh->getPoint(verts[i], 0, vCoord);
      //std::cout<<vCoord[0]<<" "<<vCoord[1]<<" "<<vCoord[2]<<std::endl;
      cmin=std::min(cmin,c);
    }
    //std::cout<<"has classification "<<cmin<<std::endl;
    //std::cout<<" "<<std::endl;
    if (cmin == INTERIORTAG) {
       mesh->setModelEntity(e,getMdlRgn(model));
       //cint++;
    } else if (cmin >= FACE && cmin <= FACE_LAST) {
       if (cmin == 1) { //face tag 1 corresponds to model face 0
          mesh->setModelEntity(e,getMdlFace(mesh,0));
       } else if (cmin == 2) { //face tag 2 corresponds to model face 1
          mesh->setModelEntity(e,getMdlFace(mesh,1));
       } else if (cmin == 3) { //face tag 3 corresponds to model face 3
          mesh->setModelEntity(e,getMdlFace(mesh,3));
       } else if (cmin == 4) { //face tag 4 corresponds to model face 4
          mesh->setModelEntity(e,getMdlFace(mesh,4));
       } else if (cmin == 5) { //face tag 5 corresponds to model face 2
          mesh->setModelEntity(e,getMdlFace(mesh,2));
       } else if (cmin == 6) { //face tag 6 corresponds to model face 5
          mesh->setModelEntity(e,getMdlFace(mesh,5));
       }
       //cface++;
    } else if (cmin >= EDGE && cmin <= EDGE_LAST) {
       if (cmin == 11) { //edge tag 11 corresponds to model edge 0
          mesh->setModelEntity(e,getMdlEdge(mesh,0));
       } else if (cmin == 12) {
          mesh->setModelEntity(e,getMdlEdge(mesh,2));
       } else if (cmin == 13) {
          mesh->setModelEntity(e,getMdlEdge(mesh,3));
       } else if (cmin == 14) {
          mesh->setModelEntity(e,getMdlEdge(mesh,1));
       } else if (cmin == 15) {
          mesh->setModelEntity(e,getMdlEdge(mesh,4));
       } else if (cmin == 16) {
          mesh->setModelEntity(e,getMdlEdge(mesh,5));
       } else if (cmin == 17) {
          mesh->setModelEntity(e,getMdlEdge(mesh,7));
       } else if (cmin == 18) {
          mesh->setModelEntity(e,getMdlEdge(mesh,6));
       } else if (cmin == 19) {
          mesh->setModelEntity(e,getMdlEdge(mesh,8));
       } else if (cmin == 20) {
          mesh->setModelEntity(e,getMdlEdge(mesh,10));
       } else if (cmin == 21) {
          mesh->setModelEntity(e,getMdlEdge(mesh,11));
       } else if (cmin == 22) {
          mesh->setModelEntity(e,getMdlEdge(mesh,9));
       }
       //cedge++;
    } else if (cmin >= VERTEX && cmin <= VERTEX_LAST) {
       if (cmin == 31) { //vertex tag 31 corresponds to model vertex 0
          mesh->setModelEntity(e,getMdlVtx(mesh,0));
       } else if (cmin == 32) { //vertex tag 32 corresponds to model vertex 1
          mesh->setModelEntity(e,getMdlVtx(mesh,1));
       } else if (cmin == 33) { //vertex tag 33 corresponds to model vertex 3
          mesh->setModelEntity(e,getMdlVtx(mesh,3));
       } else if (cmin == 34) { //vertex tag 34 corresponds to model vertex 2
          mesh->setModelEntity(e,getMdlVtx(mesh,2));
       } else if (cmin == 35) { //vertex tag 35 corresponds to model vertex 4
          mesh->setModelEntity(e,getMdlVtx(mesh,4));
       } else if (cmin == 36) { //vertex tag 36 corresponds to model vertex 5
          mesh->setModelEntity(e,getMdlVtx(mesh,5));
       } else if (cmin == 37) { //vertex tag 37 corresponds to model vertex 7
          mesh->setModelEntity(e,getMdlVtx(mesh,7));
       } else if (cmin == 38) { //vertex tag 38 corresponds to model vertex 6
          mesh->setModelEntity(e,getMdlVtx(mesh,6));
       }
       //cvtx++;
    }
  }
  mesh->end(it);
}

/* if any of four vertices are classified on region -> region
 * else on model face and it is impossible to have more than one face in the 4
 * vertices classification
 * */
void setFaceClassification(gmi_model* model, apf::Mesh2* mesh, apf::MeshTag* vtxClass) {
  (void)model;
  (void)mesh;
  (void)vtxClass;
  apf::MeshIterator* it = mesh->begin(2);
  apf::MeshEntity* f;
  int c;
  int TofM[6] = {0, 1, 3, 4, 2, 5}; // tags of discrete model are NOT same as model faces
  // int EofF[6][4];
  // int VofE[12][2];
  int FofE[12][2];
  FofE[0][0]=0;
  FofE[0][1]=1;
  FofE[1][0]=0;
  FofE[1][1]=2;
  FofE[2][0]=0;
  FofE[2][1]=3;
  FofE[3][0]=0;
  FofE[3][1]=4;
  FofE[4][0]=1;
  FofE[4][1]=4;
  FofE[5][0]=1;
  FofE[5][1]=2;
  FofE[6][0]=2;
  FofE[6][1]=3;
  FofE[7][0]=3;
  FofE[7][1]=4;
  FofE[8][0]=1;
  FofE[8][1]=5;
  FofE[9][0]=2;
  FofE[9][1]=5;
  FofE[10][0]=3;
  FofE[10][1]=5;
  FofE[11][0]=4;
  FofE[11][1]=5;
/*
  VofE[0][0]=0;
  VofE[0][1]=1;
  VofE[1][0]=1;
  VofE[1][1]=2;
  VofE[2][0]=2;
  VofE[2][1]=3;
  VofE[3][0]=3;
  VofE[3][1]=0;
  VofE[4][0]=0;
  VofE[4][1]=4;
  VofE[5][0]=1;
  VofE[5][1]=5;
  VofE[6][0]=2;
  VofE[6][1]=6;
  VofE[7][0]=3;
  VofE[7][1]=7;
  VofE[8][0]=4;
  VofE[8][1]=5;
  VofE[9][0]=5;
  VofE[9][1]=6;
  VofE[10][0]=6;
  VofE[10][1]=7;
  VofE[11][0]=7;
  VofE[11][1]=4;
  EofF[0][0]=0;
  EofF[0][1]=1;
  EofF[0][2]=2;
  EofF[0][3]=3;
  EofF[1][0]=4;
  EofF[1][1]=8;
  EofF[1][2]=5;
  EofF[1][3]=0;
  EofF[2][0]=1;
  EofF[2][1]=6;
  EofF[2][2]=9;
  EofF[2][3]=5;
  EofF[3][0]=2;
  EofF[3][1]=6;
  EofF[3][2]=10;
  EofF[3][3]=7;
  EofF[4][0]=3;
  EofF[4][1]=7;
  EofF[4][2]=11;
  EofF[4][3]=4;
  EofF[5][0]=8;
  EofF[5][1]=11;
  EofF[5][2]=10;
  EofF[5][3]=9;
*/

  apf::Adjacent verts;

  double distFromDebug1, distFromDebug2;
//  apf::Vector3 xd1(-0.54864, 7.44015e-06, 0.0397148 );
  apf::Vector3 xd1(-0.306845, 0.443585, 0.0291);
  apf::Vector3 xd2(0.914478, 0.0145401, 0.04 );
  apf::Vector3 dx1;
  apf::Vector3 dx2;
  apf::Vector3 tmp;
  apf::Vector3 Centroid;

  while( (f = mesh->iterate(it)) ) {
    mesh->getAdjacent(f, 0, verts);
    size_t nverts = verts.size();
 
    Centroid=apf::getLinearCentroid(mesh,f);
    dx1=xd1-Centroid;
    dx2=xd2-Centroid;
    distFromDebug1=dx1[0]*dx1[0]
                  +dx1[1]*dx1[1]
                  +dx1[2]*dx1[2];
    distFromDebug2=dx2[0]*dx2[0]
                  +dx2[1]*dx2[1]
                  +dx2[2]*dx2[2];

    int cmin=100;
    int cmax=-100;
    int cmid=-100;
    int ctri[4];  // up to 4 points on a face
    int f1, f2, f1x, f2x, f1d, f2d;
    int emax,emin,F0max,F0min,F1max,F1min;
    for(size_t i=0; i<verts.size(); i++) {
      mesh->getIntTag(verts[i],vtxClass,&c);
      cmin=std::min(cmin,c);
      cmax=std::max(cmax,c);
      ctri[i]=c;
    }
    int imax=0;  
    int imin=0;  
    for(size_t i=0; i<nverts; i++) {
      if(ctri[i]==cmax) imax++;
      if(ctri[i]==cmin) imin++;
    }
    if(cmax==cmin) { 
       cmid= cmin;  // all nverts on same face
    } else if (imax>=2) { // >=2 but not all with cmax
       cmid=cmax;
    } else if (imin>=2 ) { // >=2 but not all with min
       cmid=cmin;
    } else { // not 2 of either so mid is distinct
      for(size_t i=0; i<nverts; i++) { // find it
        if((ctri[i] != cmin) && (ctri[i] != cmax)) cmid=std::max(cmid,ctri[i]);  // max is to catch lowest dim/highest code for quads....actually not necessary since either of the other two will follow switches as noted below.
      }
    }

    if(std::min(distFromDebug1,distFromDebug2) < 1e-12) {
         fprintf(stderr, "%d %d %d %.15e %.15E %.15E \n", cmin, cmid, cmax, Centroid[0], Centroid[1], Centroid[2]);
         for (size_t i=0; i < nverts; i++) {
            mesh->getPoint(verts[i],0,tmp);
//            fprintf(stderr, "%d %.15e %.15E %.15E \n", i , tmp[0], tmp[1], tmp[2]);
         }
    }
     
    if (cmin == INTERIORTAG) {  // no brainer since a point on the interior always classifies interior
       mesh->setModelEntity(f,getMdlRgn(model));
       //cint++;
    } else  if(cmax <= FACE_LAST && cmin >= FACE) { // all nodes on  model face(s?)
         if(cmax != cmin) { // all on faces but not all on same so classified on interior
             mesh->setModelEntity(f,getMdlRgn(model));
         } else { // all on same face so classify on that one
          mesh->setModelEntity(f,getMdlFace(mesh,TofM[cmin-1]));
         }
    } else if (cmid <= FACE_LAST )   {  // two points on face(s?)
         if( cmin==cmid) {   // happens if 2 on SAME face
             if( cmax > EDGE_LAST) {   // happens if third is a model vert which is ALWAYS in the same plane
               mesh->setModelEntity(f,getMdlFace(mesh,TofM[cmin-1])); // classified on cmin face either way
             } else {  // third on a model edge but might not be in closure of the face
               f1=1+FofE[(cmax-11)][0];
               f2=1+FofE[(cmax-11)][1];
               if(f1==cmin || f2==cmin) { // if either face usage of this edge is cmin (or cmid) then class on face
                   mesh->setModelEntity(f,getMdlFace(mesh,TofM[cmin-1]));  
               } else mesh->setModelEntity(f,getMdlRgn(model));  // edge not in face closure so class on region
             }
          } else mesh->setModelEntity(f,getMdlRgn(model)); // cmin != cmid means on different faces so clas on region
    } else if (cmin <=FACE_LAST) { // since 3, and 2 face class handled already this is only 1 pt class on face
         if (cmax > EDGE_LAST ){ // never 2 on verts so  1face, 1edge and 1 vert  check to see if edge in closure of face
               f1=1+FofE[(cmid-11)][0];  // get two uses of the edge which is cmid
               f2=1+FofE[(cmid-11)][1];
               if(f1==cmin || f2==cmin) { // one of the usages is on cmin face and vertex is always so class on face
                   mesh->setModelEntity(f,getMdlFace(mesh,TofM[cmin-1]));  
               } else mesh->setModelEntity(f,getMdlRgn(model));  // edge not in closor so classify on region
         } else { // getting here only if 1face and 2 edges...most complicated case only face if  both edges in closure
               f1x=1+FofE[(cmax-11)][0]; // first two find 2 face usages of max edge
               f2x=1+FofE[(cmax-11)][1];
               f1d=1+FofE[(cmid-11)][0]; // next two find 2 face usages of mid edge
               f2d=1+FofE[(cmid-11)][1];
               if(   (f1x==cmin || f2x==cmin)   // cmax edge is in cmin closure
                  && (f1d==cmin || f2d==cmin) ) { // cmid edge is in cmin closure -- must have both to class on face
                     mesh->setModelEntity(f,getMdlFace(mesh,TofM[cmin-1]));  
                  } else mesh->setModelEntity(f,getMdlRgn(model)); // at least one of the edges not in face closure so clssify on region
         }
    } else if (cmax <= EDGE_LAST){ 
         if( cmax==cmid || cmin== cmid) { // two on same edge one on other so this is on a face--  find which one
                emax=cmax-11;
                emin=cmin-11;
   		F0max=FofE[emax][0];
 		F0min=FofE[emin][0];
  		F1max=FofE[emax][1];
  		F1min=FofE[emin][1];
                if(F0max==F0min) mesh->setModelEntity(f,getMdlFace(mesh,F0max)); // don't need map TofM[f1d-1]));
                if(F0max==F1min) mesh->setModelEntity(f,getMdlFace(mesh,F0max)); 
                if(F1max==F0min) mesh->setModelEntity(f,getMdlFace(mesh,F1max)); 
                if(F1max==F1min) mesh->setModelEntity(f,getMdlFace(mesh,F1max)); 
         } else  {// catch all now correct?? was not before
 // since none on face (3, 2, and 1 done above) and none on verts, 3 edges-> interior 
               mesh->setModelEntity(f,getMdlRgn(model));
         }
    } else { // getting here only if none of the face verts are interior or on model face so must be 2 e and 1 v but
        f1x=1+FofE[(cmin-11)][0]; // first two find 2 face usages of min edge
        f2x=1+FofE[(cmin-11)][1];
        f1d=1+FofE[(cmid-11)][0]; // next two find 2 face usages of mid edge
        f2d=1+FofE[(cmid-11)][1];
        if (f1x==f1d || f2x==f1d) {  // these two check if ether face using cmin match first face using cmid
              mesh->setModelEntity(f,getMdlFace(mesh,TofM[f1d-1]));  
        } else if ( f1x==f2d || f2x==f2d) {  // these two check if ether face using cmin match second face using cmid
              mesh->setModelEntity(f,getMdlFace(mesh,TofM[f2d-1]));  
        } else {
            fprintf(stderr, "face classification of these vert classification failed %d %d  %d \n", cmin, cmid, cmax);
        }
        if (0) {   
// above is cleaner but below was first idea
// none of these nodal classifications know directly the face to classify this face on.  HARD CODE for now to our case
         
         int iface=0;
         if(cmax==31) {
            if( cmin== 11) {
              if( cmid==14) iface=1; else iface=2;
           }
           else iface=5;
         } else if(cmax==32) {
            if( cmin== 11) {
              if( cmid==12) iface=1; else iface=2;
            }
            else iface=3;
         } else if(cmax==33) {
            if( cmin== 12) {
              if( cmid==13) iface=1; else iface=3;
            }
            else iface=4;
         } else if(cmax==34) {
            if( cmin== 13) {
              if( cmid==14) iface=1; else iface=4;
            }
            else iface=5;
         } else if(cmax==35) {
            if( cmin== 15) {
              if( cmid==19) iface=2; else iface=5;
            }
            else iface=6;
         } else if(cmax==36) {
            if( cmin== 16) {
              if( cmid==19) iface=2; else iface=3;
            }
            else iface=6;
         } else if(cmax==37) {
            if( cmin== 17) {
              if( cmid==20) iface=3; else iface=4;
            }
            else iface=6;
         } else if(cmax==38) {
            if( cmin== 18) {
              if( cmid==21) iface=4; else iface=5;
            }
            else iface=6;
         }
         if(iface==0){
            fprintf(stderr, "%d %d  %d \n", cmin, cmid, cmax);
         }
         PCU_ALWAYS_ASSERT(iface != 0);
         mesh->setModelEntity(f,getMdlFace(mesh,TofM[iface-1])); // classified on chosen face 
        }
    }
  }
  mesh->end(it);
}

/** \brief set the mesh region classification
  \details hacked to set the classification to the same geometric model region
*/
void setRgnClassification(gmi_model* model, apf::Mesh2* mesh) {
  apf::ModelEntity* mdlRgn = getMdlRgn(model);
  apf::MeshIterator* it = mesh->begin(3);
  apf::MeshEntity* rgn;
  while( (rgn = mesh->iterate(it)) )
    mesh->setModelEntity(rgn,mdlRgn);
  mesh->end(it);
}

void setClassification(gmi_model* model, apf::Mesh2* mesh, apf::MeshTag* t) {
  setRgnClassification(model,mesh);
  setFaceClassification(model,mesh,t);
  setEdgeClassification(model,mesh,t);
  setVtxClassification(model,mesh,t);
  mesh->acceptChanges();
}



void getLocalRange(apf::Gid total, int& local,
    apf::Gid& first, apf::Gid& last) {
  const int self = PCU_Comm_Self();
  const int peers = PCU_Comm_Peers();
  local = total/peers; 
  if( self == peers-1 ) {  //last rank
    apf::Gid lp=local*peers;
    if( lp < total ){
      apf::Gid lpd;
      lpd= total - lp;
      local += lpd;
    }
  }
  first = PCU_Exscan_Long(local);
  last = first+local;
}

void printElmTypeError(int dim, int numVtxPerElm) {
  fprintf(stderr, "unknown element type for"
      "dim %d and numVtxPerElm %d in %s\n",
      dim, numVtxPerElm, __func__);
}

unsigned getElmType(int dim, int numVtxPerElm) {
  if (dim == 2) {
    if (numVtxPerElm == 3)
      return apf::Mesh::TRIANGLE;
    if (numVtxPerElm == 4)
      return apf::Mesh::QUAD;
    else {
      printElmTypeError(dim, numVtxPerElm);
      exit(EXIT_FAILURE);
    }
  } else if (dim == 3) {
    if (numVtxPerElm == 4)
      return apf::Mesh::TET;
    else if (numVtxPerElm == 6)
      return apf::Mesh::PRISM;
    else if (numVtxPerElm == 8)
      return apf::Mesh::HEX;
    else {
      printElmTypeError(dim, numVtxPerElm);
      exit(EXIT_FAILURE);
    }
  } else {
    printElmTypeError(dim, numVtxPerElm);
    exit(EXIT_FAILURE);
  }
}

bool skipLine(char* line) {
  // lines that start with either a '#' or a single white space
  // are skipped
  return (line[0] == '#' || line[0] == ' ' );
}

void getNumVerts(FILE* f, apf::Gid& verts) {
  rewind(f);
  gmi_fscanf(f, 1, "%ld",  &verts);
}

void readClassification(FILE* f, int localNumVtx, int** classification) {
  *classification = new int[localNumVtx];
  rewind(f);
  int mdlId;
  for(int i=0; i<localNumVtx; i++) {
    gmi_fscanf(f, 1, "%d",  &mdlId);
    (*classification)[i] = mdlId;
  }
}

void readCoords(FILE* f, int& localnumvtx, double** coordinates) {
  *coordinates = new double[localnumvtx*3];
  for(int i=0; i<localnumvtx; i++) {
    double pos[3];
    gmi_fscanf(f, 3, "%lf %lf %lf", pos+0, pos+1, pos+2);
    for(unsigned j=0; j<3; j++)
        (*coordinates)[i*3+j] = pos[j];
  }
}

void readSolution(FILE* f, int& localnumvtx, double** solution) {
  *solution = new double[localnumvtx*5];
  rewind(f);
  for(int i=0; i<localnumvtx; i++) {
    double pos[5];
    pos[4]=0; //temperature
    gmi_fscanf(f, 4, "%lf %lf %lf %lf", pos+0, pos+1, pos+2, pos+3);
    for(unsigned j=0; j<5; j++)
      (*solution)[i*5+j] = pos[j];
  }
}

void readMatches(FILE* f, apf::Gid numvtx, int localnumvtx, apf::Gid** matches) {
  fprintf(stderr, "%d readMatches numvtx %ld localnumvtx %d \n",
      PCU_Comm_Self(), numvtx, localnumvtx);
  *matches = new apf::Gid[localnumvtx];
  rewind(f);
  apf::Gid matchedVtx;
  for(int i=0; i<localnumvtx; i++) {
    gmi_fscanf(f, 1, "%ld",  &matchedVtx);
    PCU_ALWAYS_ASSERT( matchedVtx == -1 ||
       ( matchedVtx >= 1 && matchedVtx <= numvtx ));
    if( matchedVtx != -1 )
        --matchedVtx;
    (*matches)[i] = matchedVtx;
  }
// I think the above will perform better than the code commented out below
//  int vidx = 0;
//  while( 1 == fscanf(f, "%ld", &matchedVtx) ) {
//    PCU_ALWAYS_ASSERT( matchedVtx == -1 ||
//       ( matchedVtx >= 1 && matchedVtx <= numvtx ));
//    if( matchedVtx != -1 )
//        --matchedVtx;
//    (*matches)[vidx] = matchedVtx;
//    vidx++;
//  }
}

//static int starts_with(char const* with, char const* s) {
//  int lw;
//  int ls;
//  lw = strlen(with);
//  ls = strlen(s);
//  if (ls < lw)
//    return 0;
//  return strncmp(with, s, lw) == 0;
//}

bool seekPart(std::ifstream& f, const std::string& marker) {
  std::stringstream ss;
  ss << "^\\s+" << marker << "$";
  std::regex partId(ss.str());
  std::string line;
  while (std::getline(f, line)) {
    if (std::regex_match(line,partId)) {
      return true;
    }
  }
  return false;
}

struct BlockInfo {
  long numElms;
  int vtxPerElm;
};

std::vector<BlockInfo> readTopoBlockInfo(std::ifstream& f) {
  std::vector<BlockInfo> blocks;
  long blockSize;
  int vtxPerElement;

  std::string line;
  while (std::getline(f, line)) {
    std::istringstream iss(line);
    if (!(iss >> blockSize >> vtxPerElement)) { break; } // error
    blocks.push_back({blockSize,vtxPerElement});
  }
  return blocks;
}

void rewindStream(std::ifstream& f) {
  f.clear();
  f.seekg(0);
}

/**
fh = header file (there is only one for all processes), containing:
   Part0
       <numel_topo_1>   <NodesInElementTopo1>
       <nmel_topo_2>   < NodesInElementTopo2>
       ... for as many topos as are in  Part 0
   Repeat the above bock for each part.
**/
std::vector<BlockInfo> readHeader(std::ifstream& fh) {
  rewindStream(fh);
  const int self = PCU_Comm_Self();;
  bool ret = seekPart(fh, std::to_string(self));
  assert(ret);
  auto blockInfo = readTopoBlockInfo(fh);
  assert(blockInfo.size()>0);
  for(auto b : blockInfo) {
    std::cout << self << " " << b.numElms << " " << b.vtxPerElm << "\n";
  }
  return blockInfo;
}

/**
- f = part file, each part gets a file that contains a rectangular array, one for each topology
  present on that part, that provides element to vertexGlobalId connectivity in
  the order listed in the section of the header file for that part
**/
void readElements(std::ifstream& f, apf::Gid numElms,
    unsigned numVtxPerElm, apf::Gid* elements, bool rewind) {
  if(rewind) rewindStream(f);
  unsigned elmIdx = 0;
  apf::Gid* elmVtx = new apf::Gid[numVtxPerElm];
  for (int i = 0; i < numElms; i++) {
    for (unsigned j = 0; j < numVtxPerElm; j++)
      f >> elmVtx[j];
    for (unsigned j = 0; j < numVtxPerElm; j++) {
      const unsigned elmVtxIdx = elmIdx*numVtxPerElm+j;
      elements[elmVtxIdx] = --(elmVtx[j]); //export from matlab using 1-based indices
    }
    elmIdx++;
  }
  PCU_ALWAYS_ASSERT(numElms==elmIdx);
  delete [] elmVtx;
}

struct MeshInfo {
  double* coords;
  double* solution;
  std::vector<apf::Gid*> elements;
  apf::Gid* matches;
  int* classification;
  int* fathers2D;
  unsigned dim;
  std::vector<unsigned> elementType;
  apf::Gid numVerts;
  int localNumVerts;
  std::vector<apf::Gid> numElms;
  std::vector<unsigned> numVtxPerElm;
};

void readMesh(const char* meshfilename,
    const char* coordfilename,
    const char* matchfilename,
    const char* classfilename,
    const char* fathers2Dfilename,
    const char* solutionfilename,
    const char* connHeadfilename,
    MeshInfo& mesh) {

  mesh.dim = 3; //FIXME

  int self = PCU_Comm_Self();

  char filename[1024];
  sprintf(filename, "%s.%d",coordfilename,self);
    
  FILE* fc = fopen(filename , "r");
  PCU_ALWAYS_ASSERT(fc);
  getNumVerts(fc,mesh.numVerts);
  mesh.localNumVerts=mesh.numVerts;
  mesh.numVerts=PCU_Add_Long(mesh.numVerts);
  
  if(!PCU_Comm_Self())
    fprintf(stderr, "numVerts %ld\n", mesh.numVerts);
  readCoords(fc, mesh.localNumVerts, &(mesh.coords));
  fclose(fc);
 
  if(0==1) {
  sprintf(filename, "%s.%d",solutionfilename,self);
  FILE* fs = fopen(filename, "r");
  PCU_ALWAYS_ASSERT(fs);
  readSolution(fs, mesh.localNumVerts, &(mesh.solution));
  fclose(fs);
  }

  sprintf(filename, "%s.%d",classfilename,self);
  FILE* ff = fopen(filename, "r");
  PCU_ALWAYS_ASSERT(ff);
  readClassification(ff, mesh.localNumVerts, &(mesh.classification));
  fclose(ff);

  if( strcmp(fathers2Dfilename, "NULL") ) {
  //add an argument to readMesh for the fathers2D
    sprintf(filename, "%s.%d",fathers2Dfilename,self);
    FILE* fff = fopen(filename, "r");
    PCU_ALWAYS_ASSERT(fff);
    readClassification(fff, mesh.localNumVerts, &(mesh.fathers2D)); // note we re-use classification reader
    fclose(fff);
  }

  if( strcmp(matchfilename, "NULL") ) {
    sprintf(filename, "%s.%d",matchfilename,self);
    FILE* fm = fopen(filename, "r");
    PCU_ALWAYS_ASSERT(fm);
    readMatches(fm, mesh.numVerts, mesh.localNumVerts, &(mesh.matches));
    fclose(fm);
  }

  std::stringstream ss;
  ss << meshfilename << "." << self;
  std::ifstream meshConnStream(ss.str());
  PCU_ALWAYS_ASSERT(meshConnStream.is_open());
  std::ifstream connHeadStream(connHeadfilename, std::ios::in);
  PCU_ALWAYS_ASSERT(connHeadStream.is_open());
  auto blockInfo = readHeader(connHeadStream);
  connHeadStream.close();
  bool rewind = true;
  for(auto b : blockInfo) {
    mesh.numElms.push_back(b.numElms);
    mesh.numVtxPerElm.push_back(b.vtxPerElm);
    apf::Gid* elements = new apf::Gid[b.numElms*b.vtxPerElm];
    readElements(meshConnStream, b.numElms, b.vtxPerElm, elements,rewind);
    rewind=false;
    mesh.elementType.push_back(getElmType(mesh.dim, b.vtxPerElm));
    mesh.elements.push_back(elements);
  }
  meshConnStream.close();
}


int main(int argc, char** argv)
{
  MPI_Init(&argc,&argv);
  PCU_Comm_Init();
  lion_set_verbosity(1);
  int noVerify=0;    // maintain default of verifying if not explicitly requesting it off
  if( argc < 10 ) {
    if( !PCU_Comm_Self() ) {
      printf("Usage: %s <ascii mesh connectivity .cnn> "
          "<ascii vertex coordinates .crd> "
          "<ascii vertex matching flag .match> "
          "<ascii vertex classification flag .class> "
          "<ascii vertex fathers2D flag .fathers2D> "
          "<ascii solution flag .soln> "
          "<ascii conn header> "
          "<output model .dmg> <output mesh .smb>"
          "turn off verify mesh if equal 1 (on if you give nothing)\n",
          argv[0]);
    }
    return 0;
  }

  gmi_register_mesh();
  gmi_register_null();

  if( argc == 11 ) noVerify=atoi(argv[10]);

  double t0 = PCU_Time();
  MeshInfo m;
  readMesh(argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],argv[7],m);

  bool isMatched = true;
  if( !strcmp(argv[3], "NULL") )
    isMatched = false;

  if(!PCU_Comm_Self())
    fprintf(stderr, "isMatched %d\n", isMatched);

  apf::Mesh2* mesh = apf::makeMdsBox(2,2,2,1,1,1,0,isMatched);
  gmi_model* model = mesh->getModel();
  apf::GlobalToVert outMap;
  PCU_Debug_Open();
  for( size_t i=0; i< m.elements.size(); i++) {
    apf::assemble(mesh, m.elements[i], m.numElms[i], m.elementType[i], outMap);
    delete [] m.elements[i];
  }
  apf::finalise(mesh, outMap);
  apf::alignMdsRemotes(mesh);
  apf::deriveMdsModel(mesh);
  apf::setCoords(mesh, m.coords, m.localNumVerts, outMap);
  delete [] m.coords;
  if( isMatched ) {
    apf::setMatches(mesh, m.matches, m.localNumVerts, outMap);
    mesh->acceptChanges();
    delete [] m.matches;
  }
  apf::MeshTag* tc = setMappedTag(mesh, "classification", m.classification, 1,
      m.localNumVerts, outMap);
  { //debug - some verts don't have the classification tag...
    apf::MeshEntity* v;
    apf::MeshIterator* verts = mesh->begin(0);
    int i=0;
    while ((v = mesh->iterate(verts))) {
      if(!mesh->hasTag(v,tc)) {
        PCU_Debug_Print("%d missing tag\n", i);
      }
      i++;
    }
  }
  setClassification(model,mesh,tc);
  apf::removeTagFromDimension(mesh, tc, 0);
  mesh->destroyTag(tc);
 
  if( strcmp(argv[5], "NULL") ) {
    apf::MeshTag* tf = setMappedTag(mesh, "fathers2D", m.fathers2D, 1,
      m.localNumVerts, outMap);
    (void) tf;
  } else if(!PCU_Comm_Self())
    fprintf(stderr, "fathers2D not requested \n");

  //mesh->destroyTag(tf);

  if(0==1) {
  apf::MeshTag* ts = setMappedTag(mesh, "solution", m.solution, 5,
      m.localNumVerts, outMap);
  (void) ts;
  }


  /* // Print the father2D tags
  apf::MeshEntity* v;
  apf::MeshIterator* it = mesh->begin(0);
  apf::MeshTag* t = mesh->findTag("fathers2D");
  if (t==NULL) {std::cout<<"Didn't find tag"<<std::endl;}
  int tagNum; 
  int count = 0;
  while ((v = mesh->iterate(it))) { // loop over mesh vertices
    mesh->getIntTag(v,t,&tagNum);
    std::cout<<"Tag number "<<tagNum<<std::endl;
    count++;
  }
  mesh->end(it);*/

  if(!PCU_Comm_Self())
    fprintf(stderr, "seconds to create mesh %.3f\n", PCU_Time()-t0);
  if(noVerify != 1) mesh->verify();

  outMap.clear();
  gmi_write_dmg(model, argv[8]);
  apf::writeVtkFiles("rendered",mesh);
  mesh->writeNative(argv[9]);
  if(noVerify != 1) mesh->verify();

  mesh->destroyNative();
  apf::destroyMesh(mesh);
  PCU_Comm_Free();
  MPI_Finalize();
}
