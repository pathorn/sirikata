#include <arpa/inet.h>
#include "Utility.hpp"
#include "asyncUtil.hpp"
#include "CraqEntry.hpp"
#include "Base64.hpp"

namespace Sirikata {
void CraqEntry::deserialize(unsigned char xinput[CRAQ_SERVER_SIZE]) {
    unsigned char input[12]={0};
    input[11]='=';
    input[10]='=';
    memcpy(input,xinput,CRAQ_SERVER_SIZE);
    unsigned char base256osegData[10]={0};
    decode12Base64(base256osegData+1,input);          
    uint32_t radiusbe=0;
    uint32_t serverbe=0;
    memcpy(&serverbe,base256osegData,4);
    memcpy(&radiusbe,base256osegData+4,4);
    uint32_t radiusle=ntohl(radiusbe);
    mServer=ntohl(serverbe);
    char radiusArray[sizeof(float)];
    memcpy(radiusArray,&radiusle,sizeof(float));
    memcpy(&mRadius,radiusArray,sizeof(float));
}
void CraqEntry::serialize(unsigned char output[CRAQ_SERVER_SIZE]) const{
  char radiusle[sizeof(float)];
  memcpy(radiusle,&mRadius,sizeof(float));
  uint32_t radiusbe;
  memcpy(&radiusbe,radiusle,sizeof(uint32_t));
  radiusbe=htonl(radiusbe);
  unsigned int inputDatabe=htonl(mServer);
  unsigned char base256osegData[10]={0};
  memcpy(base256osegData,&inputDatabe,4);
  memcpy(base256osegData+4,&radiusbe,4);
  
  unsigned char xoutput[12];
  encode9Base64(base256osegData+1,xoutput);
  memcpy(output,xoutput,CRAQ_SERVER_SIZE);
}
static bool craqSerializationUnitTest(){
    CraqEntry input(123589,15985.129);
    unsigned char output[CRAQ_SERVER_SIZE+1];
    output [CRAQ_SERVER_SIZE]='\0';
    input.serialize(output);
    CraqEntry result(output);
    bool failed=false;
    if (input.server()!=result.server()) {
        fprintf (stderr,"%d != %d serialized:%s\n",input.server(),result.server(),output);
    }
    if (input.radius()!=result.radius()) {
        fprintf (stderr,"%f != %f  serialized:%s\n",input.radius(),result.radius(),output);
    }

    assert(input.server()==result.server());
    assert(input.radius()==result.radius());
    output[0]=';';
    output[1]=':';
    output[2]='_';
    output[3]='-';
    output[4]='0';
    CraqEntry hacked(output);
    unsigned char hackedoutput[CRAQ_SERVER_SIZE];
    hacked.serialize(hackedoutput);
    assert(memcmp(hackedoutput,output,CRAQ_SERVER_SIZE)==0);
    if (memcmp(hackedoutput,output,CRAQ_SERVER_SIZE)!=0){
        printf("CRAQ SERIALIZATION failure with : and ;\n");
    }
    if (input.server()!=result.server()||input.radius()!=result.radius()) {
        while(true) {
            static bool test=true;
            if (test) {
                printf("failure\n");
            }
            test=false;
        }
    }
    return true;
}
bool didthiswork=craqSerializationUnitTest();
}
