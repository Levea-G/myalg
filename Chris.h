namespace Chris{
template<class key>
class Chris{
    typedef int ll;
private:
    class node{
    public:
        node():rb(0),ct(0),act(0),s{0,0},f(0){}
        node(ll f,key v):rb(1),ct(1),act(1),s{0,0},f(f),_key(v){}
        bool rb;
        ll ct,act,s[2],f;
        key _key;
    };
    ll size=1,*bin,top=1;
    node*tr;
    bool disp,uni;
    double s_thresh;
    bool __id__(ll x){return tr[tr[x].f].s[1]==x;}
    void __update__(ll x){tr[x].act=tr[x].ct+tr[tr[x].s[0]].act+tr[tr[x].s[1]].act;}
    void __rotate__(ll x){
        bool j=__id__(x);ll y=tr[x].f,z=tr[y].f,w=tr[x].s[j^1];
        tr[x].f=z;tr[z].s[__id__(y)]=x;
        tr[w].f=y;tr[y].s[j]=w;
        tr[y].f=x;tr[x].s[j^1]=y;
        __update__(y);__update__(x);
    }
    bool __scale__(){
        node*temp=new(std::nothrow)node[size<<1|1];
        ll*tbin=new(std::nothrow)ll[size<<1|1],cap=size;
        if(temp==NULL||tbin==NULL){
            temp=new(std::nothrow)node[size+10];
            tbin=new(std::nothrow)ll[size+10];
            if(temp==NULL||tbin==NULL)return 1;
            size+=10;
        }
        else size<<=1;
        for(ll i=0;i<=cap;i++)temp[i]=tr[i];
        for(ll i=cap+1;i<=size;i++)tbin[++top]=i;
        delete[] tr;delete[] bin;
        tr=temp;bin=tbin;
        if(disp)std::cout<<size<<std::endl;
        return 0;
    }
    void __shrink__(){
        node*temp=new(std::nothrow)node[(size>>1)+1];
        ll*tbin=new(std::nothrow)ll[(size>>1)+1],x=tr[0].s[1],nct=0;
        bool j=0;
        if(temp==NULL||tbin==NULL)return;
        if(x){//postorder
            l1:while(tr[x].s[0])x=tr[x].s[0];
            l2:if(tr[x].s[1]){x=tr[x].s[1];goto l1;}
            j=__id__(x);
            temp[++nct]=tr[x];
            temp[tr[x].s[0]].f=temp[tr[x].s[1]].f=tr[tr[x].f].s[j]=nct;
            while(tr[x].f){
                x=tr[x].f;if(!j)goto l2;
                j=__id__(x);
                temp[++nct]=tr[x];
                temp[tr[x].s[0]].f=temp[tr[x].s[1]].f=tr[tr[x].f].s[j]=nct;
            }
            temp->s[1]=nct;
        }
        size>>=1,top=0;
        for(ll i=nct+1;i<=size;i++)tbin[++top]=i;
        delete[] tr;delete[] bin;
        tr=temp;bin=tbin;
        if(disp)std::cout<<size<<std::endl;
    }
    bool __insert__(key v){
        if(!tr[0].s[1]){
            ll n=bin[top--];
            tr[0].s[1]=n;tr[n]=node(0,v);
            tr[n].rb=0;
            return 0;
        }
        if(!top&&__scale__())return 1;
        ll x=tr[0].s[1];
        while(1){
            tr[x].act++;
            if(v==tr[x]._key){
                if(uni)for(;x;x=tr[x].f){tr[x].act--;return 1;}
                else {tr[x].ct++;return 0;}
            }
            bool j=v>tr[x]._key;
            if(!tr[x].s[j]){
                ll n=bin[top--];
                tr[x].s[j]=n;tr[n]=node(x,v);
                x=n;break;
            }
            x=tr[x].s[j];
        }
        for(ll y=tr[x].f;y&&tr[x].rb&&tr[y].rb;){
            bool j=__id__(y);ll z=tr[y].f,w=tr[z].s[j^1];
            if(!tr[w].rb){
                if(j!=__id__(x))__rotate__(x),y=x;
                tr[y].rb=0,tr[z].rb=1;
                __rotate__(y);return 0;
            }
            tr[y].rb=tr[w].rb=0,tr[z].rb=1;
            x=z,y=tr[z].f;
        }
        tr[tr[0].s[1]].rb=0;
        return 0;
    }
    void __delete__(key v){
        ll x=tr[0].s[1];
        while(1){
            if(!x)return;
            if(v==tr[x]._key)break;
            x=tr[x].s[v>tr[x]._key];
        }
        for(ll temp=x;temp;temp=tr[temp].f)tr[temp].act--;
        if(tr[x].ct>1){tr[x].ct--;return;}
        if(tr[x].s[0]&&tr[x].s[1]){
            ll y=x;x=tr[x].s[1];
            while(tr[x].s[0])x=tr[x].s[0];
            tr[y].ct=tr[x].ct;tr[y]._key=tr[x]._key;
            for(ll temp=tr[x].f;temp!=y;temp=tr[temp].f)tr[temp].act-=tr[x].ct;
        }
        ll y=tr[x].f,temp=tr[x].s[0]|tr[x].s[1];
        tr[y].s[__id__(x)]=temp;tr[temp].f=y;
        tr[temp].rb=0;
        bin[++top]=x;
        if(temp||tr[x].rb)y=0;
        for(bool j=__id__(x);y;j=__id__(x)){
            ll w=tr[y].s[j^1];
            if(tr[w].rb){tr[y].rb=1,tr[w].rb=0;__rotate__(w);w=tr[y].s[j^1];}
            ll l=tr[w].s[j],r=tr[w].s[j^1];
            if(tr[r].rb){tr[w].rb=tr[y].rb,tr[y].rb=tr[r].rb=0;__rotate__(w);break;}
            if(tr[l].rb){tr[l].rb=tr[y].rb,tr[y].rb=0;__rotate__(l),__rotate__(l);break;}
            if(w)tr[w].rb=1;
            if(tr[y].rb){tr[y].rb=0;break;}
            x=y,y=tr[y].f;
        }
        if((double)top/size>s_thresh&&size>1)__shrink__();
    }
    bool __clear__(bool do_shrink){
        tr[0].s[1]=0;top=0;
        for(ll i=1;i<=size;i++)bin[++top]=i;
        if(!do_shrink)return 0;
        node*temp=new(std::nothrow)node[2];
        ll*tbin=new(std::nothrow)ll[2]{0,1};
        if(temp==NULL||tbin==NULL)return 1;
        delete tr;delete bin;
        tr=temp;bin=tbin;size=1;top=1;
        if(disp)std::cout<<size<<std::endl;
        return 0;
    }
    key*__kth__(ll k){
        ll x=tr[0].s[1];
        while(x){
            if(k<=tr[tr[x].s[0]].act){x=tr[x].s[0];continue;}
            k-=tr[tr[x].s[0]].act;
            if(k<=tr[x].ct)return &(tr[x]._key);
            k-=tr[x].ct;x=tr[x].s[1];
        }
        return NULL;
    }
    ll __rank__(key v){
        ll x=tr[0].s[1],otc=0;
        while(x){
            if(tr[x]._key>v){x=tr[x].s[0];continue;}
            otc+=tr[tr[x].s[0]].act;
            if(v==tr[x]._key)return otc;
            otc+=tr[x].ct;
            x=tr[x].s[1];
        }
        return -1;
    }
    key*__pre__(key v){
        ll x=tr[0].s[1];
        key*otc=NULL;
        while(x){
            if(v>tr[x]._key){if(otc==NULL||tr[x]._key>*otc)otc=&(tr[x]._key);x=tr[x].s[1];}
            else x=tr[x].s[0];
        }
        return otc;
    }
    key*__next__(key v){
        ll x=tr[0].s[1];
        key*otc=NULL;
        while(x){
            if(tr[x]._key>v){if(otc==NULL||*otc>tr[x]._key)otc=&(tr[x]._key);x=tr[x].s[0];}
            else x=tr[x].s[1];
        }
        return otc;
    }
    void __view__(const char*sep,const char*end,void(*print)(key v)){
        ll x=tr[0].s[1];if(!x)return;
        if(print!=NULL){
            l1:while(tr[x].s[0])x=tr[x].s[0];
            print(tr[x]._key);std::cout<<sep;
            l2:if(tr[x].s[1]){x=tr[x].s[1];goto l1;}
            while(tr[x].f){
                bool j=__id__(x);x=tr[x].f;
                if(!j){print(tr[x]._key);std::cout<<sep;goto l2;}
            }
            std::cout<<end;
        }
        else{
            l3:while(tr[x].s[0])x=tr[x].s[0];
            std::cout<<tr[x]._key<<sep;
            l4:if(tr[x].s[1]){x=tr[x].s[1];goto l3;}
            while(tr[x].f){
                bool j=__id__(x);x=tr[x].f;
                if(!j){std::cout<<tr[x]._key<<sep;goto l4;}
            }
            std::cout<<end;
        }
    }
public:
    Chris(bool display=1,double shrink_threshold=0.67,bool unique=0){
        tr=new node[2];
        bin=new ll[2]{0,1};
        disp=display;uni=unique;
        if(shrink_threshold>0.6&&shrink_threshold<1)s_thresh=shrink_threshold;
        else s_thresh=0.67;
        if(disp)std::cout<<"tree built"<<std::endl;
    }
    ~Chris(){
        delete[] tr;
        delete[] bin;
        if(disp)std::cout<<"tree deleted"<<std::endl;
    }
    bool insert(key v){return __insert__(v);}
    void remove(key v){__delete__(v);}
    void clear(bool do_shrink=1){__clear__(do_shrink);}
    ll len(){return tr[tr[0].s[1]].act;}
    ll find(key v){return __rank__(v);}
    key*operator [](ll k){return __kth__(k);}
    key*upper_bound(key v){return __next__(v);}
    key*lower_bound(key v){return __pre__(v);}
    void view(const char*sep="",const char*end="",void(*print)(key v)=NULL){__view__(sep,end,print);}
};
}