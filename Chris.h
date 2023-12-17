#include<iostream>
namespace _Chris{
typedef int ll;
template<class key>
key max(key a,key b){return a>b?a:b;}
template<class key>
key min(key a,key b){return a<b?a:b;}
template<class key>
ll __default_extern__(const key&a,const key&b){
    if(a==b)return 2;
    if(a>b)return 1;
    return 0;
}
template<class key>
class rbtree{
public:
    class iterator{
    private:
        ll x;const rbtree*t;
        bool __forward__(){
            if(!x)return 1;
            if(t->tr[x].s[1]){
                x=t->tr[x].s[1];
                while(t->tr[x].s[0])x=t->tr[x].s[0];
                return 0;
            }
            ll temp=x;
            while(t->__id__(temp)&&temp)temp=t->tr[temp].f;
            if(temp){x=t->tr[temp].f;return 0;}
            return 1;
        }
        bool __backward__(){
            if(!x)return 1;
            if(t->tr[x].s[0]){
                x=t->tr[x].s[0];
                while(t->tr[x].s[1])x=t->tr[x].s[1];
                return 0;
            }
            ll temp=x;
            while(!t->__id__(temp)&&temp)temp=t->tr[temp].f;
            if(t->tr[temp].f){x=t->tr[temp].f;return 0;}
            return 1;
        }
    public:
        iterator(const rbtree*t,ll x):t(t),x(x){}
        bool forward(){return __forward__();}
        bool backward(){return __backward__();}
        key get_key(){return t->tr[x]._key;}
        ll get_count(){return t->tr[x].ct;}
        void operator++(ll){__forward__();}
        void operator--(ll){__backward__();}
        iterator operator+(ll k){
            iterator temp(t,x);
            for(ll i=0;i<k;i++)temp.forward();
            return temp;
        }
        iterator operator-(ll k){
            iterator temp(t,x);
            for(ll i=0;i<k;i++)temp.backward();
            return temp;
        }
        void operator+=(ll k){for(ll i=0;i<k;i++)__forward__();}
        void operator-=(ll k){for(ll i=0;i<k;i++)__backward__();}
        bool operator==(iterator _I){return t==_I.t&&x==_I.x;}
        bool operator!=(iterator _I){return t!=_I.t||x!=_I.x;}
        bool empty(){return x==0;}
    };
    rbtree(double shrink_threshold=0.67,bool unique=0,ll(*compare)(const key&,const key&)=&__default_extern__){
        tr=new node[2];
        bin=new ll[2]{0,1};
        uni=unique;
        __cmp__=compare;
        if(shrink_threshold>0.6&&shrink_threshold<1)s_thresh=shrink_threshold;
        else s_thresh=0.67;
    }
    rbtree(const rbtree&b){
        s_thresh=b.s_thresh,uni=b.uni,__cmp__=b.__cmp__,size=b.size,top=b.top;
        tr=new node[size+1];bin=new ll[size+1];
        for(ll i=0;i<=size;i++)tr[i]=b.tr[i];
        for(ll i=1;i<=top;i++)bin[i]=b.bin[i];
    }
    ~rbtree(){
        delete[] tr;delete[] bin;
    }
    bool insert(const key&v){return __insert__(v);}
    bool remove(const key&v){return __delete__(v);}
    bool clear(bool do_shrink=1){return __clear__(do_shrink);}
    ll len(){return tr[tr[0].s[1]].act;}
    ll find(const key&v){return __rank__(v);}
    key*operator[](ll k){return __kth__(k+1);}
    key*upper_bound(const key&v){return __upperbound__(v);}
    key*lower_bound(const key&v){return __lowerbound__(v);}
    void view(const char*sep="",const char*end="",void(*print)(key)=NULL){__view__(sep,end,print);}
    iterator begin()const{return __begin__();}
    iterator end()const{return __end__();}
    iterator start_from(ll k)const{return __startfrom__(k);}
    bool operator<(const rbtree&b)const{
        iterator ita=begin(),itb=b.begin();bool m=ita.empty(),n=itb.empty(); 
        while((m|n)==0){
            ll j=__cmp__(ita.get_key(),itb.get_key());
            if(j==2){m=ita.forward(),n=itb.forward();continue;}
            return !(bool)j;
        }
        if(m&n)return 0;
        return m;
    }
    bool operator>(const rbtree&b)const{
        iterator ita=begin(),itb=b.begin();bool m=ita.empty(),n=itb.empty(); 
        while((m|n)==0){
            ll j=__cmp__(ita.get_key(),itb.get_key());
            if(j==2){m=ita.forward(),n=itb.forward();continue;}
            return (bool)j;
        }
        if(m&n)return 0;
        return n;
    }
    bool operator==(const rbtree&b)const{
        iterator ita=begin(),itb=b.begin();bool m=ita.empty(),n=itb.empty(); 
        while((m|n)==0){
            ll j=__cmp__(ita.get_key(),itb.get_key());
            if(j==2){m=ita.forward(),n=itb.forward();continue;}
            return 0;
        }
        if(m&n)return 1;
        return 0;
    }
    bool operator!=(const rbtree&b)const{
        iterator ita=begin(),itb=b.begin();bool m=ita.empty(),n=itb.empty(); 
        while((m|n)==0){
            ll j=__cmp__(ita.get_key(),itb.get_key());
            if(j==2){m=ita.forward(),n=itb.forward();continue;}
            return 1;
        }
        if(m&n)return 0;
        return 1;
    }
    void operator=(const rbtree&b){
        node*newtr=new(std::nothrow)node[b.size+1];
        if(newtr==NULL)return;
        ll*newbin=new(std::nothrow)ll[b.size+1];
        if(newbin==NULL){delete[] newtr;return;}
        s_thresh=b.s_thresh,uni=b.uni,__cmp__=b.__cmp__,size=b.size,top=b.top;
        delete[] tr;delete[] bin;
        tr=newtr;bin=newbin;
        for(ll i=0;i<=size;i++)tr[i]=b.tr[i];
        for(ll i=1;i<=top;i++)bin[i]=b.bin[i];
    }
    ll key_compare(const key&a,const key&b)const{return __cmp__(a,b);}
    friend std::ostream&operator<<(std::ostream&outstr,const rbtree&a){
        ll x=a.tr[0].s[1];if(!x)return outstr;
        outstr1:while(a.tr[x].s[0])x=a.tr[x].s[0];
        outstr<<a.tr[x]._key<<' '<<a.tr[x].ct<<'\n';
        outstr2:if(a.tr[x].s[1]){x=a.tr[x].s[1];goto outstr1;}
        while(a.tr[x].f){
            bool j=a.__id__(x);x=a.tr[x].f;
            if(!j){outstr<<a.tr[x]._key<<' '<<a.tr[x].ct<<'\n';goto outstr2;}
        }
        return outstr;
    }
private:
    class node{
    public:
        node():rb(0),ct(0),act(0),s{0,0},f(0){}
        node(ll f,const key&v):rb(1),ct(1),act(1),s{0,0},f(f),_key(v){}
        bool rb;
        ll ct,act,s[2],f;
        key _key;
    };
    ll size=1,*bin,top=1;
    node*tr;
    bool uni;
    double s_thresh;
    bool __id__(ll x)const{return tr[tr[x].f].s[1]==x;}
    void __update__(ll x){tr[x].act=tr[x].ct+tr[tr[x].s[0]].act+tr[tr[x].s[1]].act;}
    void __rotate__(ll x){
        bool j=__id__(x);ll y=tr[x].f,z=tr[y].f,w=tr[x].s[j^1];
        tr[x].f=z;tr[z].s[__id__(y)]=x;
        tr[w].f=y;tr[y].s[j]=w;
        tr[y].f=x;tr[x].s[j^1]=y;
        __update__(y);__update__(x);
    }
    bool __scale__(){
        node*newtr=new(std::nothrow)node[size<<1|1];
        if(newtr==NULL)return 1;
        ll*newbin=new(std::nothrow)ll[size<<1|1],psize=size;
        if(newbin==NULL){delete[] newtr;return 1;}
        size<<=1;
        for(ll i=0;i<=psize;i++)newtr[i]=tr[i];
        for(ll i=size;i>psize;i--)newbin[++top]=i;
        delete[] tr;delete[] bin;
        tr=newtr;bin=newbin;
        return 0;
    }
    void __shrink__(){
        node*newtr=new(std::nothrow)node[(size>>1)+1];
        if(newtr==NULL)return;
        ll*newbin=new(std::nothrow)ll[(size>>1)+1],x=tr[0].s[1],nct=0;
        if(newbin==NULL){delete[] newtr;return;}
        bool j=0;
        if(x){//postorder
            l1:while(tr[x].s[0])x=tr[x].s[0];
            l2:if(tr[x].s[1]){x=tr[x].s[1];goto l1;}
            j=__id__(x);
            newtr[++nct]=tr[x];
            newtr[tr[x].s[0]].f=newtr[tr[x].s[1]].f=tr[tr[x].f].s[j]=nct;
            while(tr[x].f){
                x=tr[x].f;if(!j)goto l2;
                j=__id__(x);
                newtr[++nct]=tr[x];
                newtr[tr[x].s[0]].f=newtr[tr[x].s[1]].f=tr[tr[x].f].s[j]=nct;
            }
            newtr->s[1]=nct;
        }
        size>>=1,top=0;
        for(ll i=size;i>nct;i--)newbin[++top]=i;
        delete[] tr;delete[] bin;
        tr=newtr;bin=newbin;
    }
    bool __insert__(const key&v){
        if(!tr[0].s[1]){
            ll n=bin[top--];
            tr[0].s[1]=n;
            tr[n]=node(0,v);
            tr[n].rb=0;
            return 0;
        }
        if(!top&&__scale__())return 1;
        ll x=tr[0].s[1];
        while(1){
            tr[x].act++;
            ll j=__cmp__(v,tr[x]._key);
            if(j==2){
                if(uni)for(;x;x=tr[x].f){tr[x].act--;return 1;}
                else {tr[x].ct++;return 0;}
            }
            if(!tr[x].s[j]){
                ll n=bin[top--];
                tr[x].s[j]=n;
                tr[n]=node(x,v);
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
    bool __delete__(const key&v){
        ll x=tr[0].s[1];
        while(1){
            if(!x)return 1;
            ll j=__cmp__(v,tr[x]._key);
            if(j==2)break;
            x=tr[x].s[j];
        }
        for(ll temp=x;temp;temp=tr[temp].f)tr[temp].act--;
        if(tr[x].ct>1){tr[x].ct--;return 0;}
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
        return 0;
    }
    bool __clear__(bool do_shrink){
        tr[0].s[1]=0;top=0;
        for(ll i=1;i<=size;i++)bin[++top]=i;
        if(!do_shrink)return 0;
        node*newtr=new(std::nothrow)node[2];
        if(newtr==NULL)return 1;
        ll*newbin=new(std::nothrow)ll[2]{0,1};
        if(newbin==NULL){delete[] newtr;return 1;}
        delete[] tr;delete[] bin;
        tr=newtr;bin=newbin;
        size=top=1;
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
    ll __rank__(const key&v){
        ll x=tr[0].s[1],otc=0;
        while(x){
            ll j=__cmp__(v,tr[x]._key);
            if(!j){x=tr[x].s[0];continue;}
            otc+=tr[tr[x].s[0]].act;
            if(j==2)return otc;
            otc+=tr[x].ct;
            x=tr[x].s[1];
        }
        return -1;
    }
    key*__lowerbound__(const key&v){
        ll x=tr[0].s[1];
        key*otc=NULL;
        while(x){
            if(v>tr[x]._key){if(otc==NULL||tr[x]._key>*otc)otc=&(tr[x]._key);x=tr[x].s[1];}
            else x=tr[x].s[0];
        }
        return otc;
    }
    key*__upperbound__(const key&v){
        ll x=tr[0].s[1];
        key*otc=NULL;
        while(x){
            if(tr[x]._key>v){if(otc==NULL||*otc>tr[x]._key)otc=&(tr[x]._key);x=tr[x].s[0];}
            else x=tr[x].s[1];
        }
        return otc;
    }
    void __view__(const char*sep,const char*end,void(*print)(key)){
        ll x=tr[0].s[1];if(!x){std::cout<<end;return;}
        if(print!=NULL){
            view1:while(tr[x].s[0])x=tr[x].s[0];
            print(tr[x]._key);std::cout<<sep;
            view2:if(tr[x].s[1]){x=tr[x].s[1];goto view1;}
            while(tr[x].f){
                bool j=__id__(x);x=tr[x].f;
                if(!j){print(tr[x]._key);std::cout<<sep;goto view2;}
            }
            std::cout<<end;
        }
        else{
            view3:while(tr[x].s[0])x=tr[x].s[0];
            std::cout<<tr[x]._key<<sep;
            view4:if(tr[x].s[1]){x=tr[x].s[1];goto view3;}
            while(tr[x].f){
                bool j=__id__(x);x=tr[x].f;
                if(!j){std::cout<<tr[x]._key<<sep;goto view4;}
            }
            std::cout<<end;
        }
    }
    iterator __begin__()const{
        ll x=tr[0].s[1];
        while(tr[x].s[0])x=tr[x].s[0];
        return iterator(this,x);
    }
    iterator __end__()const{
        ll x=tr[0].s[1];
        while(tr[x].s[1])x=tr[x].s[1];
        return iterator(this,x);
    }
    iterator __startfrom__(ll k)const{
        ll x=tr[0].s[1];
        while(x){
            if(k<=tr[tr[x].s[0]].act){x=tr[x].s[0];continue;}
            k-=tr[tr[x].s[0]].act;
            if(k<=tr[x].ct)return iterator(this,x);
            k-=tr[x].ct;x=tr[x].s[1];
        }
        return iterator(this,0);
    }
    ll(*__cmp__)(const key&,const key&);
};
template<class key>
class vector{
public:
    class iterator{
    private:
        ll x;const vector*t;
        bool __forward__(){
            if(x<t->cap){x++;return 0;}
            return 1;
        }
        bool __backward__(){
            if(x>0){x--;return 0;}
            return 1;
        }
    public:
        iterator(const vector*t,ll x):t(t),x(x){}
        bool forward(){return __forward__();}
        bool backward(){return __backward__();}
        key get_key(){return t->vec[x]._key;}
        void operator++(ll){__forward__();}
        void operator--(ll){__backward__();}
        iterator operator+(ll k){
            iterator temp(t,x);
            for(ll i=0;i<k;i++)temp.forward();
            return temp;
        }
        iterator operator-(ll k){
            iterator temp(t,x);
            for(ll i=0;i<k;i++)temp.backward();
            return temp;
        }
        void operator+=(ll k){x=min(x+k,t->cap-1);}
        void operator-=(ll k){x=max(0,x-k);}
        void operator==(iterator _I){return t==_I.t&&x==_I.x;}
        void operator!=(iterator _I){return t!=_I.t||x!=_I.x;}
        bool empty(){return x==-1;}
    };
    vector(ll(*compare)(const key&,const key&)=&__default_extern__){
        vec=new node[1];
        __cmp__=compare;
    }
    vector(const vector&b){
        size=b.size;cap=b.cap;__cmp__=b.__cmp__;
        vec=new node[size];
        for(ll i=0;i<=cap;i++)vec[i]=b.vec[i];
    }
    ~vector(){
        delete[] vec;
    }
    bool insert(const key&v,ll position){return __insert__(v,position);}
    bool append(const key&v){return __append__(v);}
    bool remove(const key&v){return __delete__(v);}
    bool pop(ll position){return __pop__(position);}
    bool clear(bool do_shrink=1){return __clear__(do_shrink);}
    ll len(){return cap;}
    key&operator[](ll k){
        return vec[k]._key;
    }
    void view(const char*sep="",const char*end="",void(*print)(key)=NULL){__view__(sep,end,print);}
    iterator begin()const{return __begin__();}
    iterator end()const{return __end__();}
    iterator start_from(ll k)const{return __startfrom__(k);}
    bool operator<(const vector&b)const{
        iterator ita=begin(),itb=b.begin();bool m=ita.empty(),n=itb.empty();
        while((m|n)==0){
            ll j=__cmp__(ita.get_key(),itb.get_key());
            if(j==2){m=ita.forward(),n=itb.forward();continue;}
            return !(bool)j;
        }
        if(m&n)return 0;
        return m;
    }
    bool operator>(const vector&b)const{
        iterator ita=begin(),itb=b.begin();bool m=ita.empty(),n=itb.empty();
        while((m|n)==0){
            ll j=__cmp__(ita.get_key(),itb.get_key());
            if(j==2){m=ita.forward(),n=itb.forward();continue;}
            return (bool)j;
        }
        if(m&n)return 0;
        return n;
    }
    bool operator==(const vector&b)const{
        iterator ita=begin(),itb=b.begin();bool m=ita.empty(),n=itb.empty();
        while((m|n)==0){
            ll j=__cmp__(ita.get_key(),itb.get_key());
            if(j==2){m=ita.forward(),n=itb.forward();continue;}
            return 0;
        }
        if(m&n)return 1;
        return 0;
    }
    bool operator!=(const vector&b)const{
        iterator ita=begin(),itb=b.begin();bool m=ita.empty(),n=itb.empty();
        while((m|n)==0){
            ll j=__cmp__(ita.get_key(),itb.get_key());
            if(j==2){m=ita.forward(),n=itb.forward();continue;}
            return 1;
        }
        if(m&n)return 0;
        return 1;
    }
    void operator=(const vector&b){
        node*newvec=new node[b.size];
        if(newvec==NULL)return;
        size=b.size;cap=b.cap;__cmp__=b.__cmp__;
        delete[] vec;vec=newvec;
        for(ll i=0;i<=cap;i++)vec[i]=b.vec[i];
    }
    ll key_compare(const key&a,const key&b)const{return __cmp__(a,b);}
    friend std::ostream&operator<<(std::ostream&outstr,const vector&a){
        for(ll i=0;i<a.cap;i++)outstr<<a.vec[i]._key<<' ';
        return outstr;
    }
private:
    class node{
    public:
        key _key;
    };
    node*vec;
    ll size=1,cap=0;
    bool __scale__(){
        ll _scale=size<65536?2:1;
        node*newvec=new(std::nothrow)node[size<<_scale];
        if(newvec==NULL){
            newvec=new(std::nothrow)node[size+10];
            if(newvec==NULL)return 1;
            size+=10;
        }
        else size<<=_scale;
        for(ll i=0;i<cap;i++)newvec[i]=vec[i];
        delete[] vec;vec=newvec;
        return 0;
    }
    void __shrink__(){
        ll _scale=size>3?2:1;
        node*newvec=new(std::nothrow)node[size>>_scale];
        if(newvec==NULL)return;
        size>>=_scale;
        for(ll i=0;i<cap;i++)newvec[i]=vec[i];
        delete[] vec;vec=newvec;
    }
    bool __insert__(const key&v,ll position){
        if((double)cap>=0.8*size&&__scale__())return 1;
        if(position<0)position=0;
        else if(position>cap)position=cap;
        cap++;
        for(ll i=cap-1;i>position;i--)vec[i]._key=vec[i-1]._key;
        vec[position]._key=v;
        return 0;
    }
    bool __append__(const key&v){
        return __insert__(v,cap);
    }
    bool __pop__(ll position){
        if(position<0||position>=cap)return 1;
        cap--;
        for(ll i=position;i<cap;i++)vec[i]=vec[i+1];
        if((double)(size-cap)>0.9*size&&size>1)__shrink__();
        return 0;
    }
    bool __delete__(const key&v){
        for(ll i=0;i<cap;i++)if(__cmp__(v,vec[i]._key)==2)return __pop__(i);
        return 1;
    }
    bool __clear__(bool do_shrink){
        cap=0;
        if(!do_shrink)return 0;
        node*newvec=new(std::nothrow)node[1];
        if(newvec==NULL)return 1;
        delete[] vec;vec=newvec;
        size=1;
        return 0;
    }
    void __view__(const char*sep,const char*end,void(*print)(key)){
        if(!cap){std::cout<<end;return;}
        if(print!=NULL)for(ll i=0;i<cap;i++)print(vec[i]._key),std::cout<<sep;
        else for(ll i=0;i<cap;i++)std::cout<<vec[i]._key<<sep;
        std::cout<<end;
    }
    iterator __begin__()const{
        if(!cap)return iterator(this,-1);
        return iterator(this,0);
    }
    iterator __end__()const{return iterator(this,cap-1);}
    iterator __startfrom__(ll k)const{
        if(k<0||k>=cap)return iterator(this,-1);
        return iterator(this,k);
    }
    ll(*__cmp__)(const key&,const key&);
};
template<class key>
class hashset{
public:
    hashset(ll(*hash)(const key&)=NULL){
        set=new node[mask+1];
        __hash__=hash;
    }
    hashset(const hashset&b){
        mask=b.mask;used=b.used;__hash__=b.__hash__;
        set=new node[mask+1];
        for(ll i=0;i<=mask;i++)set[i]=b.set[i];
    }
    ~hashset(){
        delete[] set;
    }
    bool sethash(ll(*hash)(const key&)){return __sethash__(hash);}
    bool insert(const key&v){return __insert__(v,__hash__(v));}
    bool remove(const key&v){return __delete__(v,__hash__(v));}
    bool clear(bool do_shrink=1){return __clear__(do_shrink);}
    bool has(const key&v){return __has__(v,__hash__(v));}
    hashset intersect(const hashset<key>&b)const{return __intersect__(b);}
    //
    void view(const char*sep="",const char*end="",void(*print)(key)=NULL){
        //
    }
    ll len(){return used;}
    void operator=(const hashset&b){
        node*newset=new(std::nothrow)node[b.mask+1];
        if(newset==NULL)return;
        mask=b.mask;used=b.used;__hash__=b.__hash__;
        delete[] set;set=newset;
        for(ll i=0;i<=mask;i++)set[i]=b.set[i];
    }
    friend std::ostream&operator<<(std::ostream&outstr,const hashset&a){
        //
        return outstr;
    }
private:
    class node{
    public:
        ll hash=-1;//-1 empty, -2 dummy
        key _key;
    };
    node*set;
    ll mask=7,used=0,fill=0;
    bool __sethash__(ll(*hash)(const key&)){
        if(__hash__==NULL){__hash__=hash;return 0;}
        return 1;
    }
    bool __resize__(){
        ll pmask=mask,tmask=used<50000?used<<2:used<<1;
        for(mask=8;mask<=tmask;mask<<=1);mask--;
        node*newset=new(std::nothrow)node[mask+1];
        if(newset==NULL){mask=pmask;return 1;}
        for(ll sp=0;sp<=pmask;sp++){
            if(set[sp].hash<0)continue;
            ll hvalue=set[sp].hash;key v=set[sp]._key;
            ll i=hvalue&mask,step=hvalue;
            while(1){
                for(ll slot=i,obst=i+((i+9<=mask)?9:0);slot<=obst;slot++){
                    if(newset[slot].hash<0){
                        newset[slot]._key=v;
                        newset[slot].hash=hvalue;
                        goto newinserted;
                    }
                }
                step>>=5;
                i=(i*5+1+step)&mask;
            }
            newinserted:continue;
        }
        fill=used;
        delete[] set;set=newset;
        return 0;
    }
    bool __insert__(const key&v,ll hvalue){
        if(5*fill>=mask*3&&__resize__())return 1;
        ll i=hvalue&mask,step=hvalue;
        while(1){
            for(ll slot=i,obst=i+((i+9<=mask)?9:0);slot<=obst;slot++){
                if(set[slot].hash<0){
                    if(set[slot].hash==-1)fill++;
                    set[slot]._key=v;
                    set[slot].hash=hvalue;
                    goto inserted;
                }
                if(v==set[i]._key)return 1;
            }
            step>>=5;
            i=(i*5+1+step)&mask;
        }
        inserted:used++;
        return 0;
    }
    bool __delete__(const key&v,ll hvalue){
        ll i=hvalue&mask,step=hvalue,slot=-1;
        while(1){
            for(ll slot=i,obst=i+((i+9<=mask)?9:0);slot<=obst;slot++){
                if(set[i].hash==-1)return 1;
                if(set[i].hash>=0&&v==set[i]._key){set[slot].hash=-2;goto deleted;}
            }
            step>>=5;
            i=(i*5+1+step)&mask;
        }
        deleted:used--;
        if(used*10<mask)__resize__();
        return 0;
    }
    bool __has__(const key&v,ll hvalue)const{
        ll i=hvalue&mask,step=hvalue;
        while(1){
            for(ll slot=i,obst=i+((i+9<=mask)?9:0);slot<=obst;slot++){
                if(set[slot].hash==-1)return 0;
                if(set[slot].hash>=0&&v==set[slot]._key)return 1;
            }
            step>>=5;
            i=(i*5+1+step)&mask;
        }
    }
    bool __clear__(bool do_shrink){
        for(ll i=0;i<=mask;i++)set[i].hash=-1;
        used=fill=0;
        if(!do_shrink)return 0;
        node*newset=new(std::nothrow)node[8];
        if(newset==NULL)return 1;
        mask=7;
        delete[] set;set=newset;
        return 0;
    }
    hashset __intersect__(const hashset&b)const{
        hashset<key>t(__hash__);
        if(used<b.used)
            for(ll i=0;i<=mask;i++){
                ll hvalue=set[i].hash;
                if(hvalue<0)continue;
                key tkey=set[i]._key;
                if(b.__has__(tkey,hvalue))t.__insert__(tkey,hvalue);
            }
        else
            for(ll i=0;i<=b.mask;i++){
                ll hvalue=b.set[i].hash;
                if(hvalue<0)continue;
                key tkey=b.set[i]._key;
                if(__has__(tkey,hvalue))t.__insert__(tkey,hvalue);
            }
        return t;
    }
    ll(*__hash__)(const key&);
};
}