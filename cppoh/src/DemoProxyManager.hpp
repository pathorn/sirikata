/*  Sirikata
 *  main.cpp
 *
 *  Copyright (c) 2008, Daniel Reiter Horn
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of Sirikata nor the names of its contributors may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _DEMO_PROXY_MANAGER_HPP_
#define _DEMO_PROXY_MANAGER_HPP_
#include <oh/ProxyManager.hpp>
#include <oh/ProxyCameraObject.hpp>
#include <oh/ProxyLightObject.hpp>
#include <oh/ProxyMeshObject.hpp>
#include <oh/ProxyWebViewObject.hpp>
#include <oh/BulletSystem.hpp>

namespace Sirikata {

using namespace std;

#define DEG2RAD 0.0174532925

class DemoProxyManager :public ProxyManager {
    std::tr1::shared_ptr<ProxyCameraObject> mCamera;
    typedef std::map<SpaceObjectReference, ProxyObjectPtr > ObjectMap;
    ObjectMap mObjects;

    //noncopyable
    DemoProxyManager(const DemoProxyManager&cpy) {}

    ProxyPositionObjectPtr addMeshObject(const Transfer::URI &uri, const Location &location,
                                 const Vector3f &scale=Vector3f(1,1,1),
                                 const int mode=0, const float density=0.f, const float friction=0.f, 
                                 const float bounce=0.f,  const Vector3f &hull=Vector3f(1,1,1),
                                 const string name="", const int colMask=0, const int colMsg=0) {
        // parentheses around arguments required to resolve function/constructor ambiguity. This is ugly.
        SpaceObjectReference myId((SpaceID(UUID::null())),(ObjectReference(UUID::random())));
        std::tr1::shared_ptr<ProxyMeshObject> myObj(new ProxyMeshObject(this, myId));
        mObjects.insert(ObjectMap::value_type(myId, myObj));
        notify(&ProxyCreationListener::createProxy, myObj);
        myObj->resetPositionVelocity(Time::now(), location);
        myObj->setMesh(uri);
        myObj->setScale(scale);
        physicalParameters pp;
        pp.mode = mode;
        pp.density = density;
        pp.friction = friction;
        pp.bounce = bounce;
        pp.name = name;
        pp.colMask = colMask;
        pp.colMsg = colMsg;
        pp.hull = hull;
        myObj->setPhysical(pp);             /// always do this to ensure parameters are valid
        return myObj;
    }

    ProxyPositionObjectPtr addLightObject(const LightInfo &linfo, const Location &location) {
        // parentheses around arguments required to resolve function/constructor ambiguity. This is ugly.
        SpaceObjectReference myId((SpaceID(UUID::null())),(ObjectReference(UUID::random())));

        std::tr1::shared_ptr<ProxyLightObject> myObj(new ProxyLightObject(this, myId));
        mObjects.insert(ObjectMap::value_type(myId, myObj));
        notify(&ProxyCreationListener::createProxy, myObj);
        myObj->resetPositionVelocity(Time::now(), location);
        myObj->update(linfo);
        return myObj;
    }
    ProxyObjectPtr addWebViewObject(const std::string &url, int width=500, int height=400) {
        SpaceObjectReference myId((SpaceID(UUID::null())),(ObjectReference(UUID::random())));
        std::tr1::shared_ptr <ProxyWebViewObject> mWebView(new ProxyWebViewObject(this, SpaceObjectReference(myId)));
        mObjects.insert(ObjectMap::value_type(myId, mWebView));
        notify(&ProxyCreationListener::createProxy,mWebView);
        mWebView->resize(width, height);
        mWebView->setPosition(OverlayPosition(RP_TOPRIGHT));
        mWebView->loadURL(url);
        return mWebView;
    }

    double str2dbl(string s) {
        float f;
        if (s=="") return 0.0;
        sscanf(s.c_str(), "%f", &f);
        return (double)f;
    }

    int str2int(string s) {
        int d;
        if (s=="") return 0;
        sscanf(s.c_str(), "%d", &d);
        return d;
    }

    void parse_csv_values(string line, vector<string>& values) {
        values.clear();
        string temp("");
        for (unsigned int i=0; i<line.size(); i++) {
            char c = line[i];
            if (c!=',') {
                if ((c!=' ') && (c!='"')) {
                    temp.push_back(c);
                }
            }
            else {
                values.push_back(temp);
                temp.clear();
            }
        }
        values.push_back(temp);
    }

    vector<string> headings;

    void getline(FILE* f, string& s) {
        s.clear();
        while (true) {
            char c = fgetc(f);
            if (c<0) {
                s.clear();
                break;
            }
            if (c==0x0d && s.size()==0) continue;            /// should deal with windows \n
            if (c==0x0a || c==0x0d) {
                break;
            }
            s.push_back(c);
        }
    }

    void parse_csv_headings(FILE* fil) {
        string line;
        getline(fil, line);
        parse_csv_values(line, headings);
    }

    map<string, string>* parse_csv_line(FILE* fil) {
        string line;
        getline(fil, line);
        vector<string> values;
        map<string, string> *row;
        row = (new map<string, string>());
        if (line.size()>0) {
            parse_csv_values(line, values);
            for (unsigned int i=0; i < values.size(); i++) {
                (*row)[headings[i]] = values[i];
            }
        }
        return row;
    }

    void loadSceneObject(FILE *fp, map<string, ProxyPositionObjectPtr> &objsByName, std::map<ProxyPositionObjectPtr, string> &objsToParent) {
        Vector3d pos(0,0,0);
        Quaternion orient(Quaternion::identity());
        Vector3f scale(1,1,1);
        Vector3f hull(1,1,1);
        float density=0;
        float friction=0;
        float bounce=0;

        /// dbm new way:
        map<string, string>& row = *parse_csv_line(fp);
        if (row["objtype"][0]=='#' || row["objtype"]==string("")) {
            return;                                         /// comment or blank line
        }
        else {
            string objtype = row["objtype"];
            pos.x = str2dbl(row["pos_x"]);
            pos.y = str2dbl(row["pos_y"]);
            pos.z = str2dbl(row["pos_z"]);
            orient.x = str2dbl(row["orient_x"]);
            orient.y = str2dbl(row["orient_y"]);
            orient.z = str2dbl(row["orient_z"]);
            if (row["orient_w"].size()) {
                orient.w = str2dbl(row["orient_w"]);                /// if w is specified, assume quaternion
            }
            else {                                                  /// if no w, Euler angles
                /// we can replace this later if need be. btQuat takes yaw, pitch, roll with Z as up.
                btQuaternion bq = btQuaternion(DEG2RAD*orient.y, DEG2RAD*orient.x, DEG2RAD*orient.z);
                orient.x = bq.getX();
                orient.y = bq.getY();
                orient.z = bq.getZ();
                orient.w = bq.getW();
            }
            Location location(pos, orient, Vector3f::nil(), Vector3f::nil(), 0.);
            scale.x = str2dbl(row["scale_x"]);
            scale.y = str2dbl(row["scale_y"]);
            scale.z = str2dbl(row["scale_z"]);
            if (row["hull_x"] != string("")) {
                hull.x = str2dbl(row["hull_x"]);
                hull.y = str2dbl(row["hull_y"]);
                hull.z = str2dbl(row["hull_z"]);
            }
            ProxyPositionObjectPtr obj;
            if (objtype=="camera") {
                obj = mCamera;
                mCamera->resetPositionVelocity(Time::now(), location);
            }
            else if (objtype=="light") {
                LightInfo::LightTypes lighttype;
                if (row["subtype"]=="point") {
                    lighttype = LightInfo::POINT;
                }
                else if (row["subtype"]=="spotlight") {
                    lighttype = LightInfo::SPOTLIGHT;
                }
                else if (row["subtype"]=="directional") {
                    lighttype = LightInfo::DIRECTIONAL;
                }
                else {
                    cout << "parse csv error: unknown light subtype" << endl;
                    assert(false);
                }
                LightInfo lightInfo;
                lightInfo.setLightType(lighttype);
                float ambientPower=1, shadowPower=1;
                float x,y,z; // Light direction. Assume 0,1,0 for now.
                x = str2dbl(row["direct_x"]);
                y = str2dbl(row["direct_y"]);
                z = str2dbl(row["direct_z"]);
                string castsshadow=row["shadow"];
                lightInfo.mDiffuseColor.x = str2dbl(row["diffuse_x"]);
                lightInfo.mDiffuseColor.y = str2dbl(row["diffuse_y"]);
                lightInfo.mDiffuseColor.z = str2dbl(row["diffuse_z"]);
                ambientPower  = str2dbl(row["ambient"]);
                lightInfo.mSpecularColor.x = str2dbl(row["specular_x"]);
                lightInfo.mSpecularColor.y = str2dbl(row["specular_y"]);
                lightInfo.mSpecularColor.z = str2dbl(row["specular_z"]);
                shadowPower = str2dbl(row["shadowpower"]);
                lightInfo.mLightRange = str2dbl(row["range"]);
                lightInfo.mConstantFalloff = str2dbl(row["constfall"]);
                lightInfo.mLinearFalloff = str2dbl(row["linearfall"]);
                lightInfo.mQuadraticFalloff = str2dbl(row["quadfall"]);
                lightInfo.mConeInnerRadians = str2dbl(row["cone_in"]);
                lightInfo.mConeOuterRadians = str2dbl(row["cone_out"]);
                lightInfo.mPower = str2dbl(row["power"]);
                lightInfo.mConeFalloff = str2dbl(row["cone_fall"]);
                lightInfo.mCastsShadow = castsshadow=="true"?true:false;

                if (lightInfo.mDiffuseColor.length()&&lightInfo.mPower) {
                    lightInfo.mAmbientColor = lightInfo.mDiffuseColor*(ambientPower/lightInfo.mDiffuseColor.length())/lightInfo.mPower;
                }
                else {
                    lightInfo.mAmbientColor = Vector3f(0,0,0);
                }
                if (lightInfo.mSpecularColor.length()&&lightInfo.mPower) {
                    lightInfo.mShadowColor = lightInfo.mSpecularColor*(shadowPower/lightInfo.mSpecularColor.length())/lightInfo.mPower;
                }
                else {
                    lightInfo.mShadowColor = Vector3f(0,0,0);
                }
                lightInfo.mWhichFields = LightInfo::ALL;
                obj = addLightObject(lightInfo, location);
            }
            else if (objtype=="mesh") {
                int mode=0;
                double density=0.0;
                friction = str2dbl(row["friction"]);
                bounce = str2dbl(row["bounce"]);
                density = str2dbl(row["density"]);
                if (row["subtype"] == "staticmesh") {
                    mode=bulletObj::Static;
                }
                else if (row["subtype"] == "dynamicbox") {
                    mode=bulletObj::DynamicBox;
                }
                else if (row["subtype"] == "dynamiccylinder") {
                    mode=bulletObj::DynamicCylinder;
                }
                else if (row["subtype"] == "dynamicsphere") {
                    mode=bulletObj::DynamicSphere;
                }
                else if (row["subtype"] == "graphiconly") {
                }
                else {
                    cout << "parse csv error: unknown  mash subtype:" << row["subtype"] << endl;
                    assert(false);
                }
                string meshURI = row["meshURI"];
                if (sizeof(string)==0) {
                    cout << "parse csv error: no meshURI" << endl;
                    assert(false);
                }
                string name = row["name"];
                int colMask = str2int(row["colMask"]);
                int colMsg = str2int(row["colMsg"]);
                obj = addMeshObject(Transfer::URI(meshURI), location, scale, mode, density, friction, bounce, hull,
                              name, colMask, colMsg);
            }
            else if (!objtype.empty()) {
                cout << "parse csv error: illegal object type" << endl;
                assert(false);
            }
            if (obj) {
                if (!row["name"].empty()) {
                    if (!objsByName[row["name"]]) {
                        objsByName[row["name"]] = obj;
                    }
                }
                if (!row["parent"].empty()) {
                    objsToParent[obj] = row["parent"];
                }
            }
        }
    }

    void loadSceneObject_txt(FILE *fp) {
        Vector3d pos(0,0,0);
        Quaternion orient(Quaternion::identity());
        Vector3f scale(1,1,1);
        int mode=0;
        float density=0;
        float friction=0;
        float bounce=0;

        int ret = fscanf(fp,"(%lf %lf %lf) [%f %f %f %f] <%f %f %f> ",
                         &pos.x,&pos.y,&pos.z,&orient.w,&orient.x,&orient.y,&orient.z,&scale.x,&scale.y,&scale.z);
        Location location(pos, orient, Vector3f::nil(), Vector3f::nil(), 0.);
        // Read a line into filename.
        std::string filename;
        while (true) {
            char str[1024];
            str[0]='\0';
            char* getsptr = fgets(str, 1024, fp);
            if (getsptr == NULL) {
                // Read error or EOF, bail
                break;
            }
            std::string append(str);
            if (append.length() == 0) {
                // EOF
                break;
            }
            else if (append[append.length()-1]=='\n') {
                filename += append.substr(0, append.length()-1);
                break;
            }
            else {
                filename += append;
            }
        }
        if (ret < 7) { // not all options existed in the file.
            return;
        }
        if (sscanf(filename.c_str(),"{%d %f %f %f}", &mode, &density, &friction, &bounce)==4) {
            size_t i;
            for (i = 0; i < filename.length() && filename[i]!='}'; ++i) {
            }
            do {
                ++i;
            }
            while (i < filename.length() && isspace(filename[i]));
            filename = filename.substr(i);
        }
        std::string rest;
        std::string::size_type sppos=filename.find(' ');
        if (sppos != std::string::npos) {
            rest = filename.substr(sppos+1);
            filename = filename.substr(0, sppos);
        }
        if (filename=="CAMERA") {
            mCamera->resetPositionVelocity(Time::now(), location);
        }
        else if (filename=="point" || filename=="directional" || filename=="spotlight") {
            LightInfo::LightTypes lighttype = LightInfo::SPOTLIGHT;
            if (filename=="point") {
                lighttype = LightInfo::POINT;
            }
            else if (filename=="spotlight") {
                lighttype = LightInfo::SPOTLIGHT;
            }
            else if (filename=="directional") {
                lighttype = LightInfo::DIRECTIONAL;
            }
            LightInfo lightInfo;
            lightInfo.setLightType(lighttype);
            float ambientPower=1, shadowPower=1;
            float x,y,z; // Light direction. Assume 0,1,0 for now.
            int castsshadow=1;
            sscanf(rest.c_str(),"[%f %f %f %f] [%f %f %f %f] <%lf %f %f %f> <%f %f> [%f] %f %d <%f %f %f>",
                   &lightInfo.mDiffuseColor.x,&lightInfo.mDiffuseColor.y,&lightInfo.mDiffuseColor.z,&ambientPower,
                   &lightInfo.mSpecularColor.x,&lightInfo.mSpecularColor.y,&lightInfo.mSpecularColor.z,&shadowPower,
                   &lightInfo.mLightRange,&lightInfo.mConstantFalloff,&lightInfo.mLinearFalloff,&lightInfo.mQuadraticFalloff,
                   &lightInfo.mConeInnerRadians,&lightInfo.mConeOuterRadians,&lightInfo.mPower,&lightInfo.mConeFalloff,
                   &castsshadow,&x,&y,&z);
            lightInfo.mCastsShadow = castsshadow?true:false;

            if (lightInfo.mDiffuseColor.length()&&lightInfo.mPower) {
                lightInfo.mAmbientColor = lightInfo.mDiffuseColor*(ambientPower/lightInfo.mDiffuseColor.length())/lightInfo.mPower;
            }
            else {
                lightInfo.mAmbientColor = Vector3f(0,0,0);
            }
            if (lightInfo.mSpecularColor.length()&&lightInfo.mPower) {
                lightInfo.mShadowColor = lightInfo.mSpecularColor*(shadowPower/lightInfo.mSpecularColor.length())/lightInfo.mPower;
            }
            else {
                lightInfo.mShadowColor = Vector3f(0,0,0);
            }
            lightInfo.mWhichFields = LightInfo::ALL;
            addLightObject(lightInfo, location);
        }
        else if (filename=="NULL") {
            SpaceObjectReference myId((SpaceID(UUID::null())),(ObjectReference(UUID::random())));

            std::tr1::shared_ptr<ProxyPositionObject> myObj(new ProxyPositionObject(this, myId));
            mObjects.insert(ObjectMap::value_type(myId, myObj));
            notify(&ProxyCreationListener::createProxy, myObj);
            myObj->resetPositionVelocity(Time::now(), location);
        }
        else if (!filename.empty()) {
            addMeshObject(Transfer::URI(filename), location, scale, mode, density, friction, bounce);
        }
    }

    bool loadSceneFile(std::string filename) {
        FILE *fp;
        for (int i = 0; i < 4; ++i) {
            fp = fopen(filename.c_str(), "rt");
            if (fp) break;
            filename = std::string("../")+filename;
        }
        if (!fp) {
            return false;
        }

        int len=scenefile.size();
        if (len>=4 && scenefile[len-4]=='.' && scenefile[len-3]=='t' && scenefile[len-2]=='x' && scenefile[len-1]=='t') {
            while (!feof(fp)) {
                loadSceneObject_txt(fp);
            }
        }
        else {
            parse_csv_headings(fp);
            map<string, ProxyPositionObjectPtr> namedObjs;
            map<ProxyPositionObjectPtr, string> parentObjs;
            while (!feof(fp)) {
                loadSceneObject(fp, namedObjs, parentObjs);
            }
            for (map<ProxyPositionObjectPtr, string>::iterator iter = parentObjs.begin(); iter != parentObjs.end(); ++iter) {
                if (namedObjs[iter->second]) {
                    iter->first->setParent(namedObjs[iter->second], Time::now());
                }
            }
        }
        fclose(fp);
        return true;
    }

public:
    string scenefile;
    DemoProxyManager()
            : mCamera(new ProxyCameraObject(this, SpaceObjectReference(SpaceID(UUID::null()),ObjectReference(UUID::random())))) {
        scenefile="scene.csv";
    }

    virtual void createObject(const ProxyObjectPtr &newObj) {
        notify(&ProxyCreationListener::createProxy,newObj);
        mObjects.insert(ObjectMap::value_type(newObj->getObjectReference(), newObj));
    }

    virtual void destroyObject(const ProxyObjectPtr &newObj) {
        ObjectMap::iterator iter = mObjects.find(newObj->getObjectReference());
        if (iter != mObjects.end()) {
            newObj->destroy();
            notify(&ProxyCreationListener::destroyProxy,newObj);
            mObjects.erase(iter);
        }
    }

    void initialize() {
        notify(&ProxyCreationListener::createProxy,mCamera);
        mCamera->attach("",0,0);

        addWebViewObject("http://sirikata.com/cgi-bin/virtualchat/irc.cgi", 400, 250);

        mCamera->resetPositionVelocity(Time::now(),
                                       Location(Vector3d(0,0,50.), Quaternion::identity(),
                                                Vector3f::nil(), Vector3f::nil(), 0.));

        if (loadSceneFile(scenefile)) {
            return; // success!
        }
        // Otherwise, load fallback scene.

        LightInfo li;
        li.setLightDiffuseColor(Color(0.92,0.92,0.92));
        li.setLightAmbientColor(Color(0.0001,0.0001,0.0001));
        li.setLightSpecularColor(Color(0,0,0));
        li.setLightShadowColor(Color(0,0,0));
        li.setLightPower(0.5);
        li.setLightRange(1000);
        li.setLightFalloff(0.1,0,0);
        addLightObject(li, Location(Vector3d(0,1000.,0), Quaternion::identity(),
                                    Vector3f::nil(), Vector3f::nil(), 0.));

        addMeshObject(Transfer::URI("meru://cplatz@/arcade.mesh"),
                      Location(Vector3d(0,0,0), Quaternion::identity(),
                               Vector3f(.05,0,0), Vector3f(0,0,0),0));
        addMeshObject(Transfer::URI("meru://cplatz@/arcade.mesh"),
                      Location(Vector3d(5,0,0), Quaternion::identity(),
                               Vector3f(.05,0,0), Vector3f(0,0,0),0));
        addMeshObject(Transfer::URI("meru://cplatz@/arcade.mesh"),
                      Location(Vector3d(0,5,0), Quaternion::identity(),
                               Vector3f(.05,0,0), Vector3f(0,0,0),0));
    }
    void destroy() {
        mCamera->destroy();
        notify(&ProxyCreationListener::destroyProxy,mCamera);
        for (ObjectMap::const_iterator iter = mObjects.begin();
                iter != mObjects.end(); ++iter) {
            (*iter).second->destroy();
            notify(&ProxyCreationListener::destroyProxy,(*iter).second);
        }
        mObjects.clear();
    }
    ProxyObjectPtr getProxyObject(const SpaceObjectReference &id) const {
        if (id == mCamera->getObjectReference()) {
            return mCamera;
        }
        else {
            ObjectMap::const_iterator iter = mObjects.find(id);
            if (iter == mObjects.end()) {
                return ProxyObjectPtr();
            }
            else {
                return (*iter).second;
            }
        }
    }
};

}
#endif